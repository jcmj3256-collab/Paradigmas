# Getters y Setters en C++

## 1. ¿Por qué existen?

En una clase, los atributos suelen ser `private`: desde fuera no se pueden leer ni modificar directamente.

```cpp
Complex z(3, 4);
z.real = 10;   //  ERROR: 'real' es privado
```

Para eso están los **getters** y **setters**: métodos públicos que sirven de "puerta" controlada a los atributos privados. A esto se le llama **encapsulamiento**.

| Método | Qué hace | Analogía |
|---|---|---|
| **Getter** | **Lee** el valor de un atributo | Mirar el marcador |
| **Setter** | **Modifica** el valor de un atributo | Cambiar el marcador |

---

## 2. Estructura

### Getter

```cpp
tipo getNombre() const
{
    return nombre;
}
```

- Devuelve el **mismo tipo** que el atributo.
- No recibe parámetros.
- Lleva `const` al final: promete **no modificar** el objeto.

### Setter

```cpp
void setNombre(tipo nuevoValor)
{
    nombre = nuevoValor;
}
```

- Devuelve `void` (no regresa nada).
- Recibe **un parámetro** con el nuevo valor.
- **No** lleva `const`, porque sí modifica el objeto.

---

## 3. Caso: clase `Complex`

```cpp
class Complex {
private:
    double real;
    double imaginary;

public:
    // Getters (leen)
    double getReal() const {
        return real;
    }
    double getImaginary() const {
        return imaginary;
    }

    // Setters (modifican)
    void setReal(double real) {
        this->real = real;
    }
    void setImaginary(double imaginary) {
        this->imaginary = imaginary;
    }
};
```

### ¿Qué es `this->real = real`?

Cuando el parámetro se llama igual que el atributo, el compilador necesita saber cuál es cuál:

| Expresión | Se refiere a... |
|---|---|
| `real` (solo) | El **parámetro** del setter |
| `this->real` | El **atributo** del objeto |

Si escribes solo `real = real;`, asignas el parámetro a sí mismo y el atributo **no cambia**.

Alternativa: ponerle otro nombre al parámetro y no usar `this`:

```cpp
void setReal(double newReal) {
    real = newReal;
}
```

---

## 4. Uso

```cpp
Complex z(3, 4);              // 3 + 4i

std::cout << z.getReal();     // 3       (leer)

z.setReal(10);                // ahora 10 + 4i  (modificar)
z.setImaginary(-2);           // ahora 10 - 2i

std::cout << z;               // 10 - 2i
```

Los setters **no** son para pedirle datos al usuario con el teclado: sirven para cambiar el valor de un objeto que **ya existe**.

---

## 5. ¿Por qué `const` en los getters y no en los setters?

```cpp
void imprimir(const Complex& c)
{
    c.getReal();      //  el getter es const, se puede usar
    c.setReal(5);     //  ERROR: no se puede modificar un objeto const
}
```

Si olvidas `const` en un getter, no podrás usarlo con objetos `const` ni con parámetros `const Complex&`.

---

## 6. Setters con validación

Una ventaja de los setters es que pueden **proteger** el objeto. Ejemplo con `Fraction`, que no acepta denominadores ≤ 0:

```cpp
void setDenominator(int newDenominator)
{
    if (newDenominator <= 0)
        throw std::invalid_argument("El denominador debe ser positivo");
    denominator = newDenominator;
    simplify();
}
```

Por eso en `Fraction` la práctica solo pide getters: los valores se validan en el constructor y los objetos quedan sin cambios después.

---

## 7. Resumen rápido

| | Getter | Setter |
|---|---|---|
| Nombre típico | `getAlgo` | `setAlgo` |
| Retorna | Tipo del atributo | `void` |
| Parámetros | Ninguno | El nuevo valor |
| `const` | ✅ Sí | ❌ No |
| Cuerpo | `return atributo;` | `atributo = nuevoValor;` |

## 8. Errores comunes

- Olvidar `const` en el getter.
- Ponerle `const` al setter (no compila: intenta modificar).
- Escribir `real = real;` sin `this->` cuando parámetro y atributo se llaman igual.
- Que el getter devuelva `void` o el setter devuelva un valor.
---
## Constructor parametrizado

Recibe los valores iniciales al crear el objeto. Así el objeto nace completo y válido, sin pasos extra.

```
 main                         Constructor                    Objeto
┌────────────────────────┐   ┌──────────────────────────┐   ┌───────────────────┐
│ CuentaBancaria a(      │   │ titular_(titular)        │   │ titular_ = "Ana"  │
│    "Ana", 101);        │──►│ numero_(numero)          │──►│ numero_  = 101    │
└────────────────────────┘   │ saldo_(0)                │   │ saldo_   = 0      │
   solo da los valores       └──────────────────────────┘   └───────────────────┘
```

### Cómo se escribe

```cpp
class CuentaBancaria {
private:
    std::string titular_;
    int numero_;
    double saldo_;

public:
    CuentaBancaria(std::string titular, int numero)
        : titular_(titular), numero_(numero), saldo_(0) {}
};

int main() {
    CuentaBancaria a("Ana", 101);     // valores directo en main
    CuentaBancaria b("Luis", 102);
}
```

### Lista de inicialización (`: atributo(valor)`)
- Se escribe después de los `)` del constructor y antes de `{`.
- Evita el problema de que el parámetro y el atributo se llamen igual (por eso los atributos llevan `_`).
- Es la forma más segura de asegurar que cada atributo reciba su valor.

### Errores comunes
- Cuerpo vacío `{}` sin lista de inicialización: compila, pero los atributos quedan con basura.
- Declarar variables con tipo dentro del constructor (`int numero = 0;`): crea una variable local que muere al salir. Sin tipo (`numero_ = 0;`) sí asigna al atributo.
- Poner `;` después de la llave de cierre del constructor: sobra.
- Pedir el saldo inicial como parámetro cuando el enunciado dice que empieza en 0.
## Getters y setters en `main`: casos de uso

Un setter **no es para leer del teclado**: solo cambia un dato de un objeto que ya existe. De dónde viene el dato (código fijo o usuario) es un asunto aparte.

```
 ¿De dónde viene el dato?          ¿Qué se usa?
 ┌──────────────────────────┐
 │ objeto NUEVO             │ ───► constructor        CuentaBancaria b(nombre, num);
 ├──────────────────────────┤
 │ objeto que YA EXISTE     │ ───► setter             b.setTitular(nuevo);
 ├──────────────────────────┤
 │ el saldo (dinero)        │ ───► depositar/retirar  (sin setter, para no saltar validaciones)
 └──────────────────────────┘
 Si el dato lo escribe el usuario: primero cin/getline a una variable, luego constructor o setter.
```

### Código completo (compila con `g++ -std=c++17 -Wall -Wextra`)

```cpp
#include <iostream>
#include <string>
#include <iomanip>
#include <limits>

class CuentaBancaria {
private:
    std::string titular_;
    int numero_;
    double saldo_;

public:
    CuentaBancaria(std::string titular, int numero)
        : titular_(titular), numero_(numero), saldo_(0) {}

    // ---------- Getters: const, devuelven el tipo, sin parametros ----------
    std::string titular() const { return titular_; }
    int numero() const { return numero_; }
    double saldo() const { return saldo_; }

    // ---------- Setters: void, un parametro, validan ----------
    void setTitular(const std::string& titular) {
        if (!titular.empty()) titular_ = titular;   // ignora nombres vacios
    }
    void setNumero(int numero) {
        if (numero > 0) numero_ = numero;           // ignora numeros invalidos
    }
    // NO hay setSaldo: el saldo solo cambia con depositar() y retirar()

    void depositar(double monto) {
        if (monto > 0) saldo_ += monto;
    }
    bool retirar(double monto) {
        if (monto <= 0 || monto > saldo_) return false;
        saldo_ -= monto;
        return true;
    }
};

void mostrar(const CuentaBancaria& c) {   // const&: solo se pueden usar getters
    std::cout << "  Cuenta " << c.numero() << " - " << c.titular()
              << ": $" << std::fixed << std::setprecision(2) << c.saldo() << "\n";
}

int main() {
    // CASO 1: datos fijos en el codigo, via constructor
    std::cout << "Caso 1: constructor con datos fijos\n";
    CuentaBancaria a("Ana", 101);
    mostrar(a);

    // CASO 2: cambiar un dato de un objeto que ya existe, via setter
    std::cout << "Caso 2: setter sobre un objeto existente\n";
    a.setTitular("Ana Lopez");
    mostrar(a);

    // CASO 3: setter con dato invalido -> se ignora
    std::cout << "Caso 3: setter con dato invalido\n";
    a.setNumero(-5);              // no cambia
    a.setTitular("");             // no cambia
    mostrar(a);

    // CASO 4: el usuario escribe los datos y se usa el CONSTRUCTOR
    std::cout << "Caso 4: datos del usuario -> constructor\n";
    std::string nombre;
    int num;
    std::cout << "  Titular: ";
    std::getline(std::cin, nombre);          // getline: acepta espacios
    std::cout << "  Numero: ";
    std::cin >> num;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    CuentaBancaria b(nombre, num);
    mostrar(b);

    // CASO 5: el usuario escribe un dato y se usa un SETTER
    std::cout << "Caso 5: dato del usuario -> setter\n";
    std::string nuevo;
    std::cout << "  Nuevo titular: ";
    std::getline(std::cin, nuevo);
    b.setTitular(nuevo);
    mostrar(b);

    // CASO 6: el saldo se cambia con metodos, no con setter
    std::cout << "Caso 6: saldo con depositar/retirar\n";
    b.depositar(500);
    std::cout << "  retirar 100: " << (b.retirar(100) ? "ok" : "falla") << "\n";
    std::cout << "  retirar 900: " << (b.retirar(900) ? "ok" : "falla") << "\n";
    mostrar(b);

    // CASO 7: const y setters
    const CuentaBancaria c("Luis", 102);
    std::cout << "Caso 7: objeto const\n";
    std::cout << "  getter ok: " << c.titular() << "\n";
    // c.setTitular("X");   // ERROR de compilacion: un setter no es const
    return 0;
}
```

### Puntos clave

| Caso | Qué enseña |
|---|---|
| 1 | El constructor da los valores iniciales de un objeto **nuevo** |
| 2 | El setter modifica un objeto **existente** |
| 3 | El setter puede **validar** y rechazar datos malos |
| 4 y 5 | Leer del teclado es independiente: `cin` o `getline` a una variable, y luego constructor o setter |
| 6 | Datos delicados (saldo) se cambian con métodos con reglas, no con setter libre |
| 7 | Un objeto `const` solo permite getters (que son `const`) |

### Errores comunes al leer del teclado
- Mezclar `cin >> numero` con `getline` sin limpiar el buffer: `getline` se salta la lectura. Solución: `std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');` después del `cin >>`.
- Usar `cin >> nombre` para nombres con espacios: solo lee la primera palabra. Usa `std::getline`.
- Escribir `setSaldo`: permite saltarse la validación de `depositar` y `retirar`.
