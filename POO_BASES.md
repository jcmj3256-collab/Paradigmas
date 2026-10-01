# Bases de la Programación Orientada a Objetos (POO)

La idea central: en vez de separar **datos** y **funciones**, se agrupan en **objetos** que modelan cosas del problema.

```
Programación procedural          Programación orientada a objetos
┌────────┐   ┌───────────┐       ┌──────────────────────┐
│ datos  │   │ funciones │       │ objeto               │
│ sueltos│   │ sueltas   │  →    │  datos + funciones   │
└────────┘   └───────────┘       │  que los manejan     │
                                 └──────────────────────┘
```

---

## 1. Clase y objeto

- **Clase**: el molde o plano. Define qué datos y qué comportamiento tendrá algo.
- **Objeto**: una instancia concreta creada a partir de la clase.

```cpp
class Auto {
public:
    std::string marca;
    int velocidad = 0;

    void acelerar(int delta) { velocidad += delta; }
};

Auto miAuto;          // objeto (instancia de Auto)
miAuto.marca = "VW";
miAuto.acelerar(20);
```

| Término | Significado | Ejemplo |
|---|---|---|
| Atributo | Dato que guarda el objeto | `velocidad` |
| Método | Función que pertenece a la clase | `acelerar()` |
| Instancia | Objeto concreto | `miAuto` |

---

## 2. Los 4 pilares

### 2.1 Encapsulamiento

Ocultar los datos internos (`private`) y exponer solo lo necesario (`public`). Así se evita que el objeto quede en un estado inválido.

```cpp
class CuentaBancaria {
private:
    double saldo_ = 0;           // nadie lo toca directamente

public:
    void depositar(double monto) {
        if (monto > 0) saldo_ += monto;     // validación
    }
    bool retirar(double monto) {
        if (monto <= 0 || monto > saldo_) return false;
        saldo_ -= monto;
        return true;
    }
    double saldo() const { return saldo_; } // solo lectura
};
```

```
 Exterior ──► [ depositar() | retirar() | saldo() ] ──► saldo_ (protegido)
                      interfaz pública                   dato privado
```

### 2.2 Abstracción

Mostrar **qué** hace un objeto, no **cómo** lo hace. Quien usa `cuenta.retirar(50)` no necesita saber qué validaciones ocurren por dentro.

### 2.3 Herencia

Una clase reutiliza y extiende a otra. Modela una relación **"es un"**.

```cpp
class Vehiculo {
protected:
    int ruedas_;
public:
    Vehiculo(int r) : ruedas_(r) {}
    void arrancar() { std::cout << "Arrancando...\n"; }
};

class Moto : public Vehiculo {
public:
    Moto() : Vehiculo(2) {}
    void caballito() { std::cout << "Wheelie!\n"; }   // extra
};
```

```
       Vehiculo
      ┌─────────┐
      │ ruedas_ │
      │arrancar │
      └────┬────┘
      ┌────┴─────┐
    Moto       Camion
```

### 2.4 Polimorfismo

La misma llamada se comporta distinto según el **tipo real** del objeto.

```cpp
class Figura {
public:
    virtual ~Figura() = default;
    virtual double area() const = 0;      // método virtual puro
};

class Circulo : public Figura {
    double r_;
public:
    explicit Circulo(double r) : r_(r) {}
    double area() const override { return 3.14159 * r_ * r_; }
};

class Rectangulo : public Figura {
    double b_, h_;
public:
    Rectangulo(double b, double h) : b_(b), h_(h) {}
    double area() const override { return b_ * h_; }
};

void imprimir(const Figura& f) {          // referencia, no copia
    std::cout << f.area() << "\n";        // elige la versión correcta
}
```

```
 imprimir(const Figura& f)
          │
          ├── si f es Circulo     → Circulo::area()
          └── si f es Rectangulo  → Rectangulo::area()
```

> En C++ el polimorfismo necesita **`virtual`** y acceso mediante **referencia o puntero**.

---

## 3. Piezas que acompañan a las clases

| Concepto | Qué es |
|---|---|
| **Constructor** | Inicializa el objeto al crearlo (mismo nombre que la clase) |
| **Destructor** | Limpia al destruirlo; debe ser `virtual` si hay herencia polimórfica |
| **`public` / `protected` / `private`** | Niveles de acceso a los miembros |
| **`override`** | Indica que se redefine un método virtual de la base |
| **Clase abstracta** | Tiene al menos un método virtual puro (`= 0`); no se puede instanciar |
| **Interfaz** | Clase abstracta con solo métodos virtuales puros |
| **`static`** | Miembro que pertenece a la clase, no a cada objeto |
| **`this`** | Puntero al objeto actual dentro de un método |

### Niveles de acceso

| Miembro | Misma clase | Clase derivada | Exterior |
|---|:---:|:---:|:---:|
| `public` | ✅ | ✅ | ✅ |
| `protected` | ✅ | ✅ | ❌ |
| `private` | ✅ | ❌ | ❌ |

---

## 4. Herencia vs. composición

| Relación | Pregunta de prueba | Herramienta |
|---|---|---|
| **"Es un"** | ¿Una `Moto` es un `Vehiculo`? Sí | Herencia |
| **"Tiene un"** | ¿Un `Auto` tiene un `Motor`? Sí | Composición |

```cpp
class Motor { /* ... */ };

class Auto {
    Motor motor_;       // composición: Auto TIENE un Motor
};
```

**Regla práctica:** usa herencia solo cuando se cumpla de verdad "es un". Ante la duda, prefiere composición: es más flexible y acopla menos las clases.

---

## 5. Resumen rápido

```
 POO
  ├── Clase / Objeto ........ molde e instancia
  ├── Encapsulamiento ....... ocultar datos, exponer interfaz
  ├── Abstracción ........... qué hace, no cómo
  ├── Herencia .............. "es un", reutiliza código
  └── Polimorfismo .......... misma llamada, distinto comportamiento
```

## 6. Errores comunes

- Olvidar el **destructor virtual** en una clase base polimórfica.
- Usar herencia cuando lo correcto es composición.
- Hacer públicos todos los atributos (rompe el encapsulamiento).
- Pasar objetos polimórficos **por valor** (provoca *object slicing*).
