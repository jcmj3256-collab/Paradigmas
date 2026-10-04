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
