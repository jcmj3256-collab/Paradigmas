# Sobrecarga de operadores en C++: clase `fraccion`

Programa en C++ que suma fracciones (y fracciones con enteros) usando **sobrecarga de operadores**. Sirve para practicar tres ideas: sobrecarga, expresiones unarias e inmutabilidad.

## Estructura

```
fraccion.hpp   -> la clase fraccion
main.cpp       -> programa de prueba
```

Se compila con:

```
g++ main.cpp -o programa
```

## Código

### fraccion.hpp

```cpp
#ifndef FRACCION_HPP
#define FRACCION_HPP

class fraccion
{
    private:
        int numerador;
        int denominador;
    public:
        // Constructor de la clase fraccion
        fraccion(int numerador = 0, int denominador = 1)
            : numerador(numerador), denominador(denominador)
        {}

        // Getters
        int getNumerador() const {
            return numerador;
        }
        int getDenominador() const {
            return denominador;
        }

        // Suma de fracciones
        fraccion operator+(const fraccion& other) const
        {
            int newNumerador   = numerador * other.denominador + denominador * other.numerador;
            int newDenominador = denominador * other.denominador;

            return fraccion(newNumerador, newDenominador);
        }
};

#endif
```

### main.cpp

```cpp
#include <iostream>
#include "fraccion.hpp"

int main()
{
    fraccion a(1, 2);   // 1/2
    fraccion b(1, 3);   // 1/3
    fraccion c = a + b;

    std::cout << a.getNumerador() << "/" << a.getDenominador() << " + "
              << b.getNumerador() << "/" << b.getDenominador() << " = "
              << c.getNumerador() << "/" << c.getDenominador() << std::endl;

    // Al sumar un entero, el compilador convierte el 3 en una fraccion:
    // fraccion(3) se construye como 3/1
    fraccion d = a + 3;
    std::cout << "1/2 + 3 = "
              << d.getNumerador() << "/" << d.getDenominador() << std::endl;

    return 0;
}
```

### Salida

```
1/2 + 1/3 = 5/6
1/2 + 3 = 7/2
```

---

## 1. Sobrecarga

**Sobrecargar** es darle a un mismo nombre (o símbolo) varios comportamientos según los tipos con los que se use. En C++ el operador `+` ya funciona con `int` y `double`, pero no sabe sumar `fraccion`. Con la **sobrecarga de operadores** le enseñamos cómo hacerlo:

```cpp
fraccion operator+(const fraccion& other) const
```

Así, `a + b` se lee como `a.operator+(b)`, y la expresión con fracciones queda tan natural como con números normales.

La suma sigue la regla matemática:

```
a/b + c/d = (a*d + b*c) / (b*d)
```

### Sobrecarga vs. argumentos por defecto

El constructor **no** está sobrecargado. Es un solo constructor con **argumentos por defecto**:

```cpp
fraccion(int numerador = 0, int denominador = 1)
```

Esto permite crear objetos de varias formas:

```cpp
fraccion x;        // 0/1
fraccion y(5);     // 5/1
fraccion z(1, 2);  // 1/2
```

Si fuera sobrecarga real, habría **varios constructores** con el mismo nombre y distintos parámetros.

```
Argumentos por defecto -> 1 función, el compilador rellena lo que falta
Sobrecarga             -> N funciones, el compilador elige la que coincide
```

### Conversión implícita: por qué `a + 3` funciona

`operator+` espera una `fraccion`, pero recibe un `int`. Antes de marcar error, el compilador busca si puede convertir el `3` en una `fraccion`, y sí puede, porque el constructor acepta un solo argumento:

```
a + 3
a + fraccion(3)     <- conversión implícita
a + 3/1
```

Limitación: `3 + a` **no** funciona, porque el operador es un método de la clase y el lado izquierdo debe ser una `fraccion`. Para soportarlo habría que definir `operator+` como función `friend` fuera de la clase. Si no se quiere la conversión implícita, se usa `explicit` en el constructor.

---

## 2. Expresiones unarias

Un operador **unario** trabaja con **un solo operando**, a diferencia de los **binarios**, que usan dos.

| Tipo | Ejemplo | Operandos |
|------|---------|-----------|
| Binario | `a + b` | 2 |
| Unario | `-a`, `!a`, `++a` | 1 |

En este programa, `+` es **binario**: recibe `this` (el lado izquierdo) y `other` (el derecho). El código de arriba no incluye ningún operador unario, pero se pueden sobrecargar igual. Por ejemplo, el **negativo** (`-a`) de una fracción:

```cpp
// Dentro de la clase fraccion
fraccion operator-() const
{
    return fraccion(-numerador, denominador);
}
```

Nota que **no recibe parámetros**: solo usa el objeto sobre el que se llama.

```cpp
fraccion a(1, 2);
fraccion n = -a;    // n = -1/2
```

```
Binario:  a + b   ->  a.operator+(b)   (1 parámetro: other)
Unario:   -a      ->  a.operator-()    (0 parámetros)
```

---

## 3. Expresiones inmutables

Una operación es **inmutable** cuando **no modifica** los objetos originales: calcula un valor y devuelve uno **nuevo**. En `operator+` esto se logra con tres cosas:

```cpp
fraccion operator+(const fraccion& other) const
//                 ^^^^^ (1)                ^^^^^ (2)
{
    ...
    return fraccion(newNumerador, newDenominador);   // (3)
}
```

1. **`const fraccion& other`**: el parámetro se recibe por referencia (sin copiar) y es de solo lectura, así que la función no puede alterar el objeto de la derecha.
2. **`const` al final del método**: promete que el método no modifica al objeto de la izquierda (`this`). Por eso `getNumerador()` y `getDenominador()` también llevan `const`.
3. **`return fraccion(...)`**: el resultado es un objeto **nuevo**, no se sobrescribe ninguno de los operandos.

Por eso, después de `fraccion c = a + b;`, `a` sigue siendo `1/2` y `b` sigue siendo `1/3`.

```
a (1/2) --+
          +--> operator+ --> c (5/6)   (objeto nuevo)
b (1/3) --+

a y b quedan intactos
```

### Contraste: una versión que muta

Un operador como `+=` sí **modifica** el objeto de la izquierda, por lo que **no** puede ser `const`:

```cpp
fraccion& operator+=(const fraccion& other)
{
    numerador   = numerador * other.denominador + denominador * other.numerador;
    denominador = denominador * other.denominador;
    return *this;
}
```

Aquí `a += b` cambia a `a`. Las expresiones inmutables son más seguras porque evitan efectos secundarios inesperados y facilitan razonar sobre el código.

---

## Limitaciones conocidas

- No simplifica el resultado (por ejemplo, `1/2 + 1/2` da `4/4`).
- No valida que el denominador sea distinto de cero.
- `3 + a` no compila (ver la sección de conversión implícita).
