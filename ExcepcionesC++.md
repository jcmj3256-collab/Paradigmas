# Lo básico de excepciones en C++

## ¿Qué es una excepción?

Una **excepción** es una forma de avisar que ocurrió un error en tiempo de ejecución y que el código no puede continuar de forma normal. En lugar de devolver un código de error o dejar que el programa falle, el código **lanza** (`throw`) la excepción y otra parte del programa la **atrapa** (`catch`) y decide qué hacer.

Ejemplos de situaciones que las justifican:

- Dividir entre cero o crear una fracción con denominador 0.
- Abrir un archivo que no existe.
- Pedir una posición fuera de los límites de un arreglo.
- Recibir un argumento inválido en una función.

---

## Las tres palabras clave

| Palabra | Para qué sirve |
|---------|----------------|
| `try` | Encierra el código que **podría** fallar |
| `throw` | **Lanza** la excepción cuando se detecta el problema |
| `catch` | **Atrapa** la excepción y la maneja |

### Estructura general

```cpp
try {
    // código que puede lanzar una excepción
}
catch (const TipoDeExcepcion& e) {
    // qué hacer si ocurre
}
```

---

## Primer ejemplo

```cpp
#include <iostream>
#include <stdexcept>

int dividir(int a, int b)
{
    if (b == 0) {
        throw std::invalid_argument("No se puede dividir entre 0");
    }
    return a / b;
}

int main()
{
    try {
        std::cout << dividir(10, 2) << '\n';   // 5
        std::cout << dividir(10, 0) << '\n';   // lanza la excepción
        std::cout << "Esto no se ejecuta\n";
    }
    catch (const std::invalid_argument& e) {
        std::cout << "Error: " << e.what() << '\n';
    }

    std::cout << "El programa sigue funcionando\n";
    return 0;
}
```

**Salida:**

```
5
Error: No se puede dividir entre 0
El programa sigue funcionando
```

---

## Cómo fluye el programa

```
dividir(10, 0)
      |
      v
   b == 0 ?  --sí-->  throw
                        |
                        v
      se salta el resto del bloque try
                        |
                        v
      catch atrapa la excepción --> se ejecuta su código
                        |
                        v
            el programa continúa después del catch
```

Cuando se ejecuta un `throw`:

1. Se **interrumpe** el código en ese punto (lo que siga dentro del `try` no se ejecuta).
2. C++ sale de las funciones una por una buscando un `catch` que coincida (esto se llama *desenrollado de pila*).
3. Si encuentra uno, ejecuta ese `catch` y el programa continúa después de él.
4. Si **nadie** la atrapa, el programa termina de golpe.

---

## Excepciones estándar

La librería `<stdexcept>` trae varias listas para usar. Todas heredan de `std::exception`.

| Excepción | Cuándo usarla |
|-----------|---------------|
| `std::invalid_argument` | Un argumento tiene un valor inválido (ej. denominador 0) |
| `std::out_of_range` | Un índice o valor está fuera del rango permitido |
| `std::runtime_error` | Error general que solo se detecta al ejecutar |
| `std::logic_error` | Error en la lógica del programa |
| `std::exception` | Clase base de todas las anteriores |

Todas tienen el método `what()`, que devuelve el mensaje que les pasaste:

```cpp
throw std::out_of_range("Indice fuera de rango");
// e.what() -> "Indice fuera de rango"
```

---

## Buenas prácticas para el `catch`

### Atrapar por referencia constante

```cpp
catch (const std::invalid_argument& e)   // correcto
catch (std::invalid_argument e)          // evitar: copia la excepción
```

`const&` evita copiar el objeto y no permite modificarlo.

### Varios `catch`

Se evalúan **en orden**, y se ejecuta el primero que coincide. Pon los más específicos primero y los generales al final:

```cpp
try {
    // ...
}
catch (const std::invalid_argument& e) {
    std::cout << "Argumento inválido: " << e.what() << '\n';
}
catch (const std::exception& e) {
    std::cout << "Otro error: " << e.what() << '\n';
}
```

Si pones `std::exception` primero, atrapará todo y los demás `catch` nunca se ejecutarán.

### Atrapar cualquier cosa

```cpp
catch (...) {
    std::cout << "Ocurrió un error desconocido\n";
}
```

Los tres puntos atrapan cualquier tipo de excepción, pero no dan información sobre ella (no hay `e`). Úsalo como último recurso.

---

## Excepciones en un constructor

Un constructor **no puede devolver un valor** para avisar de un error, así que lanzar una excepción es la forma habitual de rechazar datos inválidos. Si el constructor lanza, el objeto **nunca llega a existir**.

Ejemplo con la clase `fraccion`:

```cpp
#include <stdexcept>

class fraccion
{
    private:
        int numerador;
        int denominador;
    public:
        fraccion(int numerador = 0, int denominador = 1)
            : numerador(numerador), denominador(denominador)
        {
            if (denominador == 0) {
                throw std::invalid_argument("El denominador no puede ser 0");
            }
        }
};
```

Uso:

```cpp
try {
    fraccion a(1, 0);   // lanza la excepción
}
catch (const std::invalid_argument& e) {
    std::cout << "Error: " << e.what() << '\n';
}
```

---

## Crear tu propia excepción

Si quieres un tipo de error propio, puedes heredar de una excepción estándar:

```cpp
#include <stdexcept>

class DenominadorCeroError : public std::runtime_error
{
    public:
        DenominadorCeroError()
            : std::runtime_error("El denominador no puede ser 0") {}
};
```

Y se usa igual:

```cpp
throw DenominadorCeroError();

// ...

catch (const DenominadorCeroError& e) {
    std::cout << e.what() << '\n';
}
```

---

## ¿Cuándo usar excepciones?

**Sí conviene** cuando:

- El error es **excepcional** y el código que lo detecta no sabe cómo resolverlo.
- Es un dato inválido que no debería existir (denominador 0, índice fuera de rango).
- Quieres separar la lógica normal del manejo de errores.

**No conviene** para el flujo normal del programa. Por ejemplo, no las uses para salir de un ciclo o para casos esperados como "el usuario escribió una opción que no existe" (para eso basta un `if`).

---

## Resumen

```
throw   ->  lanza el error
try     ->  encierra el código que puede fallar
catch   ->  atrapa y maneja el error
what()  ->  devuelve el mensaje del error
```

- Atrapa por **`const&`**.
- Ordena los `catch` de lo **específico a lo general**.
- Usa las excepciones de `<stdexcept>` cuando encajen.
- En un constructor, lanzar una excepción evita que se cree un objeto inválido.
