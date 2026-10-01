# Object Slicing en C++

## 1. ¿Qué es?

**Object slicing** ocurre cuando copias un objeto de una clase derivada dentro de un objeto de la clase base **por valor**. El destino solo tiene espacio para la parte base, así que la parte derivada se "rebana" y se pierde.

```
 Perro rex (típ. 48 bytes)        Animal a (típ. 40 bytes)
┌─────────────────────┐          ┌─────────────────────┐
│ vptr → tabla Perro  │          │ vptr → tabla Animal │  ← el vptr se reasigna
│ nombre_ = "Rex"     │  ─────►  │ nombre_ = "Rex"     │
├─────────────────────┤          └─────────────────────┘
│ trucos_ = 5         │  ✂️ se pierde
└─────────────────────┘
```

### Consecuencias

1. **Se pierden los datos** propios de la clase derivada (`trucos_`).
2. **Se pierde el polimorfismo**: el objeto resultante es un `Animal` de verdad (su vptr apunta a la tabla de `Animal`), por lo que `sonido()` ejecuta la versión base.
3. En la asignación, puede quedar un **objeto híbrido** con estado inconsistente.

> El polimorfismo solo funciona con **referencias o punteros**, porque apuntan al objeto original sin copiarlo.

---

## 2. Código completo

```cpp
#include <iostream>
#include <string>
#include <utility>
#include <vector>

class Animal {
private:
    std::string nombre_;

public:
    explicit Animal(std::string n) : nombre_(std::move(n)) {}
    virtual ~Animal() {}

    const std::string& nombre() const { return nombre_; }
    virtual std::string sonido() const { return "..."; }
};

class Perro : public Animal {
private:
    int trucos_;  // dato extra que Animal NO tiene

public:
    Perro(std::string n, int t) : Animal(std::move(n)), trucos_(t) {}

    int trucos() const { return trucos_; }
    std::string sonido() const override {
        return "Guau (sé " + std::to_string(trucos_) + " trucos)";
    }
};

// Recibe por VALOR: se copia solo la parte Animal -> slicing
void hablar_por_valor(Animal a) {
    std::cout << "  [por valor]      " << a.nombre() << ": " << a.sonido() << "\n";
}

// Recibe por REFERENCIA: no hay copia -> polimorfismo intacto
void hablar_por_ref(const Animal& a) {
    std::cout << "  [por referencia] " << a.nombre() << ": " << a.sonido() << "\n";
}

// Recibe por PUNTERO: tampoco hay copia -> polimorfismo intacto
void hablar_por_puntero(const Animal* a) {
    std::cout << "  [por puntero]    " << a->nombre() << ": " << a->sonido() << "\n";
}

int main() {
    Perro rex("Rex", 5);

    std::cout << "1) Asignacion directa a un Animal:\n";
    Animal a = rex;  // slicing: se pierde 'trucos_' y el vptr apunta a Animal
    std::cout << "  " << a.nombre() << ": " << a.sonido() << "\n";
    std::cout << "  sizeof(Animal)=" << sizeof(Animal)
              << "  sizeof(Perro)=" << sizeof(Perro) << "\n\n";

    std::cout << "2) Paso a funcion:\n";
    hablar_por_valor(rex);    // se "rebana"
    hablar_por_ref(rex);      // correcto
    hablar_por_puntero(&rex); // correcto
    std::cout << "\n";

    std::cout << "3) vector<Animal> (contenedor por valor):\n";
    std::vector<Animal> mal;
    mal.push_back(Perro("Fido", 3));  // slicing al insertar
    for (const auto& x : mal)
        std::cout << "  " << x.nombre() << ": " << x.sonido() << "\n";
    std::cout << "\n";

    std::cout << "4) vector<Animal*> (punteros tradicionales):\n";
    std::vector<Animal*> bien;
    bien.push_back(new Perro("Fido", 3));
    bien.push_back(new Animal("Generico"));
    for (const Animal* x : bien)
        std::cout << "  " << x->nombre() << ": " << x->sonido() << "\n";

    // Con punteros crudos, la liberación es responsabilidad nuestra.
    // El destructor virtual de Animal garantiza que se destruya el Perro completo.
    for (Animal* x : bien)
        delete x;
    bien.clear();
    std::cout << "\n";

    std::cout << "5) Asignacion a traves de referencias (slicing parcial):\n";
    Perro p1("Luna", 10), p2("Toby", 1);
    Animal& r1 = p1;
    Animal& r2 = p2;
    r1 = r2;  // solo copia la parte Animal: p1 toma el nombre de p2, pero trucos_ no cambia
    std::cout << "  p1 ahora: " << p1.nombre() << ", trucos=" << p1.trucos()
              << "  -> objeto híbrido/inconsistente\n";

    return 0;
}
```

---

## 3. Salida esperada

Los valores de `sizeof` dependen de la plataforma; estos son típicos en 64 bits con libstdc++.

```
1) Asignacion directa a un Animal:
  Rex: ...
  sizeof(Animal)=40  sizeof(Perro)=48

2) Paso a funcion:
  [por valor]      Rex: ...
  [por referencia] Rex: Guau (sé 5 trucos)
  [por puntero]    Rex: Guau (sé 5 trucos)

3) vector<Animal> (contenedor por valor):
  Fido: ...

4) vector<Animal*> (punteros tradicionales):
  Fido: Guau (sé 3 trucos)
  Generico: ...

5) Asignacion a traves de referencias (slicing parcial):
  p1 ahora: Toby, trucos=10  -> objeto híbrido/inconsistente
```

---

## 4. Análisis caso por caso

### Caso 1: `Animal a = rex;`

Se invoca el **constructor de copia de `Animal`**, que solo conoce la parte base. El resultado es un `Animal` completo y legítimo, con su vptr apuntando a la vtable de `Animal`. Por eso `sonido()` devuelve `"..."`.

`sizeof(Perro)` es mayor porque incluye el `int trucos_` más el relleno (padding) de alineación.

### Caso 2: paso a funciones

| Función | Parámetro | ¿Copia? | Resultado |
|---|---|:---:|---|
| `hablar_por_valor` | `Animal a` | Sí (slicing) | `Rex: ...` |
| `hablar_por_ref` | `const Animal&` | No | `Rex: Guau (sé 5 trucos)` |
| `hablar_por_puntero` | `const Animal*` | No | `Rex: Guau (sé 5 trucos)` |

```
 hablar_por_valor(rex)         hablar_por_ref(rex)
 ┌──────┐   copia   ┌──────┐   ┌──────┐
 │ rex  │ ───✂️───► │ a    │   │ rex  │ ◄── a (alias, mismo objeto)
 └──────┘  (Animal) └──────┘   └──────┘
```

### Caso 3: `std::vector<Animal>`

Un `vector<Animal>` guarda objetos `Animal` **exactos**, contiguos y del mismo tamaño. `push_back(Perro(...))` copia (o mueve) solo la parte `Animal` dentro del vector. No hay forma de guardar un `Perro` completo ahí.

### Caso 4: `std::vector<Animal*>`

Los punteros tienen tamaño fijo y apuntan a objetos completos en el heap, así que el polimorfismo funciona. Dos puntos importantes:

- El **destructor virtual** de `Animal` hace que `delete x` (con `x` de tipo `Animal*` apuntando a un `Perro`) destruya el `Perro` completo. Sin `virtual`, sería **comportamiento indefinido**.
- Con punteros crudos hay que hacer `delete` manualmente (o usar smart pointers).

### Caso 5: asignación mediante referencias

`r1` y `r2` son de tipo `Animal&`, aunque apuntan a objetos `Perro`. La expresión `r1 = r2` llama a `Animal::operator=`, que solo copia `nombre_`.

```
              antes                      después de r1 = r2
 p1:  nombre_="Luna"  trucos_=10    nombre_="Toby"  trucos_=10
 p2:  nombre_="Toby"  trucos_=1     nombre_="Toby"  trucos_=1
                                          ▲            ▲
                                  copiado de p2   NO se copió
                                  → p1 es un híbrido
```

Es el caso más peligroso: **compila sin errores ni advertencias** y deja el objeto en un estado que no corresponde ni a Luna ni a Toby.

---

## 5. Cómo evitar el slicing

### 5.1 Pasar por referencia o puntero

```cpp
void hablar(const Animal& a);   // en vez de void hablar(Animal a);
```

### 5.2 Usar smart pointers en contenedores polimórficos

```cpp
#include <memory>

std::vector<std::unique_ptr<Animal>> zoo;
zoo.push_back(std::make_unique<Perro>("Fido", 3));
zoo.push_back(std::make_unique<Animal>("Generico"));

for (const auto& x : zoo)
    std::cout << x->nombre() << ": " << x->sonido() << "\n";
// No hace falta delete: unique_ptr libera la memoria solo.
```

### 5.3 Impedir la copia en la clase base

```cpp
class Animal {
protected:
    Animal(const Animal&) = default;              // solo las derivadas pueden copiar
    Animal& operator=(const Animal&) = default;
public:
    explicit Animal(std::string n) : nombre_(std::move(n)) {}
    // ...
};
```

Con esto, `Animal a = rex;` **ya no compila**. Si además quieres copiar objetos a través de la jerarquía de forma segura, se usa el patrón `clone()`:

```cpp
virtual std::unique_ptr<Animal> clone() const = 0;
```

### 5.4 Clase base abstracta

Si `Animal` tuviera al menos un método virtual puro, no se podría crear un `Animal` por valor y la mayoría de los casos de slicing dejarían de compilar.

---

## 6. Resumen

| Situación | ¿Hay slicing? | Polimorfismo |
|---|:---:|:---:|
| `Animal a = rex;` | Sí | No |
| Parámetro `Animal a` | Sí | No |
| Parámetro `const Animal&` | No | Sí |
| Parámetro `const Animal*` | No | Sí |
| `vector<Animal>` | Sí | No |
| `vector<Animal*>` / `vector<unique_ptr<Animal>>` | No | Sí |
| `r1 = r2` con `Animal&` | Sí (parcial) | Objeto inconsistente |

**Regla de oro:** los objetos polimórficos se manejan siempre por **referencia o puntero**, nunca por valor.
