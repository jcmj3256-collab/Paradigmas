# Smart Pointers en C++

> Un **smart pointer** es un objeto que *envuelve* un puntero crudo y libera la memoria solo, usando **RAII**: el destructor del smart pointer hace el `delete`.
> Todos viven en `<memory>`.

## 1. El problema

```cpp
void f() {
    int* p = new int(5);
    // ... si aquí hay un return o una excepción ...
    delete p;   // nunca se ejecuta -> fuga de memoria
}
```

Con un smart pointer:

```cpp
#include <memory>

void f() {
    auto p = std::make_unique<int>(5);
    // ... return o excepción ...
}   // p se destruye aquí y libera el int automáticamente
```

**Regla práctica:** en C++ moderno casi nunca escribas `new` y `delete` a mano.

---

## 2. Los tres tipos

| Tipo | Dueño | ¿Se puede copiar? | Cuándo usarlo |
|---|---|---|---|
| `std::unique_ptr<T>` | **Uno solo** | No (solo se *mueve*) | Opción por defecto |
| `std::shared_ptr<T>` | **Varios** | Sí (cuenta referencias) | Cuando de verdad hay dueños compartidos |
| `std::weak_ptr<T>` | **Ninguno** (solo observa) | Sí | Romper ciclos, observar sin ser dueño |

```
unique_ptr         shared_ptr (contador = 3)        weak_ptr
 [A] ──► obj        [A] ─┐                           [W] - - ► obj
                    [B] ─┼──► obj                    (no suma al contador)
                    [C] ─┘
```

---

## 3. `unique_ptr`: un solo dueño

```cpp
auto a = std::make_unique<int>(10);

// auto b = a;              // ERROR: no se puede copiar
auto b = std::move(a);      // OK: la propiedad pasa a b
// ahora a == nullptr, b apunta al int
```

Funciones útiles:

| Función | Qué hace |
|---|---|
| `*p`, `p->` | Acceder al objeto (igual que un puntero normal) |
| `p.get()` | Devuelve el puntero crudo (sin ceder la propiedad) |
| `p.reset()` | Libera el objeto y deja `p` en `nullptr` |
| `p.reset(nuevo)` | Libera el actual y toma uno nuevo |
| `p.release()` | Suelta la propiedad **sin** hacer delete (devuelve el crudo; ojo con fugas) |
| `if (p)` | `true` si no es `nullptr` |

**Arreglos:** `std::unique_ptr<int[]> arr = std::make_unique<int[]>(5);` (usa `delete[]` por dentro).

---

## 4. `shared_ptr`: dueños compartidos

Lleva un **contador de referencias**. Cuando llega a 0, se libera el objeto.

```cpp
auto a = std::make_shared<int>(42);
std::cout << a.use_count();   // 1

{
    auto b = a;                       // copia: ahora hay 2 dueños
    std::cout << a.use_count();       // 2
}                                     // b muere
std::cout << a.use_count();           // 1
// al terminar el programa a muere -> contador 0 -> se libera el int
```

- Siempre prefiere `std::make_shared<T>(...)` sobre `shared_ptr<T>(new T(...))`: es más seguro y más eficiente.
- Es más pesado que `unique_ptr` (guarda un bloque de control con el contador). **No lo uses "por si acaso".**

---

## 5. `weak_ptr`: observar sin ser dueño

Sirve para evitar el **ciclo de referencias**, donde dos objetos se apuntan con `shared_ptr` y nunca se liberan:

```cpp
struct B;

struct A {
    std::shared_ptr<B> b;
    ~A() { std::cout << "A muere\n"; }
};

struct B {
    std::weak_ptr<A> a;     // weak_ptr rompe el ciclo
    ~B() { std::cout << "B muere\n"; }
};

int main() {
    auto a = std::make_shared<A>();
    auto b = std::make_shared<B>();
    a->b = b;
    b->a = a;
}   // con weak_ptr en B, ambos se destruyen. Con shared_ptr en los dos, ¡nunca!
```

Para usar el objeto observado hay que convertirlo con `lock()`:

```cpp
if (auto sp = b->a.lock()) {     // sp es un shared_ptr si el objeto sigue vivo
    // usar sp
} else {
    // el objeto ya fue destruido
}
```

| Función | Qué hace |
|---|---|
| `w.lock()` | Devuelve un `shared_ptr` (vacío si el objeto murió) |
| `w.expired()` | `true` si el objeto ya no existe |

---

## 6. Con herencia y polimorfismo (conecta con `Figura`)

```cpp
std::vector<std::unique_ptr<Figura>> figuras;
figuras.push_back(std::make_unique<Circulo>(5));
figuras.push_back(std::make_unique<Rectangulo>(4, 3));

for (const auto& f : figuras)
    imprimir(*f);            // polimorfismo + liberación automática
```

- Se usa `push_back(std::make_unique<...>(...))` porque `unique_ptr` no se copia: se *mueve* al vector.
- Se recorre con `const auto&` para **no copiar** (un `unique_ptr` no se puede copiar).
- Gracias al **destructor virtual** de `Figura`, al destruirse cada `unique_ptr<Figura>` se llama el destructor correcto de la hija.

---

## 7. Cómo pasarlos a funciones

| Quiero... | Firma |
|---|---|
| Solo **usar** el objeto | `void f(const Figura& x)` ← lo más común |
| **Quedarme** con la propiedad | `void f(std::unique_ptr<Figura> x)` y llamar con `std::move(p)` |
| Compartir la propiedad | `void f(std::shared_ptr<Figura> x)` |

Si la función solo necesita usar el objeto, recibe una referencia o puntero crudo, **no** un smart pointer.

---

## 8. Errores comunes

- Crear dos smart pointers desde el **mismo puntero crudo** → doble `delete`:
  ```cpp
  int* raw = new int(1);
  std::unique_ptr<int> a(raw);
  std::unique_ptr<int> b(raw);   // MAL
  ```
- Usar un `unique_ptr` después de `std::move` (queda en `nullptr`).
- Usar `shared_ptr` en todo (costo innecesario y riesgo de ciclos).
- Llamar `delete` sobre el resultado de `.get()`.
- Olvidar el **destructor virtual** en la clase base cuando guardas hijas en `unique_ptr<Base>`.

---

## 9. Resumen rápido

1. ¿Un solo dueño? → `unique_ptr` + `make_unique`.
2. ¿Dueños compartidos de verdad? → `shared_ptr` + `make_shared`.
3. ¿Solo observar sin poseer, o hay riesgo de ciclo? → `weak_ptr`.
4. Nada de `new`/`delete` manuales.

---

## 10. Ejercicios sugeridos (sin ver la solución)

- [ ] Crear un `unique_ptr<int>`, moverlo a otro y comprobar que el primero queda en `nullptr`.
- [ ] Hacer un `shared_ptr`, copiarlo en un bloque `{ }` e imprimir `use_count()` antes, dentro y después.
- [ ] Reimplementar la clase `Caja` (`new[]`/`delete[]`) usando `unique_ptr<int[]>`.
- [ ] Guardar `Circulo`, `Rectangulo` y `Triangulo` en un `vector<unique_ptr<Figura>>` e imprimir sus áreas.
- [ ] Provocar un ciclo con dos `shared_ptr` y arreglarlo con `weak_ptr` (verifica con prints en los destructores).
