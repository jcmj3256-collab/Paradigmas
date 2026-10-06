#  Apunte: RAII con Stack y Heap

## 1. Stack (pila)

- Memoria **automática**: se reserva y se libera sola.
- Cada llamada a función crea un **stackframe** (con sus parámetros y variables locales).
- Al terminar la función, su stackframe se elimina.

```cpp
int sumar(int a, int b)
{
  int resultado = a + b;
  return resultado;
}

int main()
{
  int x = 10;
  int y = 20;
  int z = sumar(x, y);
}
```

Así se ve el stack mientras corre `sumar`:

```
   STACK (crece hacia arriba)
┌──────────────────────────┐
│ stackframe de sumar      │ ← se crea al llamarla
│   a = 10, b = 20         │
│   resultado = 30         │
├──────────────────────────┤
│ stackframe de main       │
│   x = 10, y = 20, z = ?  │
└──────────────────────────┘
   Al terminar sumar → su frame desaparece y z = 30
```

## 2. Heap (montículo)

- Se usa cuando **no sabes cuánto espacio necesitas** (o necesitas que el dato viva más allá de la función).
- La memoria **no se libera sola**: tú debes liberarla (`delete` / `delete[]`).
- Si olvidas liberarla → **fuga de memoria** (memory leak).

```cpp
int* p = new int[100];   // pide memoria en el heap
// ... usar p ...
delete[] p;              // hay que liberarla manualmente
```

```
 STACK                    HEAP
┌─────────┐            ┌───────────────┐
│ p ──────┼──────────► │ int[100]      │
└─────────┘            └───────────────┘
 (puntero, automático)  (hay que liberarlo)
```

| | Stack | Heap |
|---|---|---|
| Liberación | Automática | Manual |
| Tamaño | Conocido en compilación | Dinámico |
| Velocidad | Muy rápida | Más lenta |
| Riesgo | Desbordamiento (overflow) | Fugas de memoria |

### Ejemplo: variable en stack vs. dato en heap

```cpp
void funcion()
{
  int x = 10;             // stack: vive dentro de funcion
  int* p = new int(20);   // p (puntero) en el stack, el int 20 en el heap
}                         // nadie hace delete p → fuga de memoria

int main()
{
  int x = 10;
  int* p = new int(20);   // un solo int en el heap, inicializado en 20
}
```

```
 STACK (funcion)            HEAP
┌─────────────────┐      ┌────────┐
│ x = 10          │      │   20   │
│ p ──────────────┼────► │        │
└─────────────────┘      └────────┘
 al terminar funcion: x y p desaparecen,
 pero el 20 se queda en el heap sin dueño  (leak)
```

> **Ojo con la equivalencia en C:** `new int(20)` reserva **un solo** `int` con valor 20, es como `malloc(sizeof(int))`.
> Para reservar **20 enteros** sería `new int[20]`, que equivale a `malloc(sizeof(int) * 20)`.

Para evitar la fuga hay que hacer `delete p;` (o, mejor aún, usar RAII).

>  **No mezcles:** lo reservado con `new` se libera con `delete` (y `new[]` con `delete[]`); lo reservado con `malloc` se libera con `free`.

## 3. Scope, memory leak y Garbage Collector

### Scope (alcance)

Una variable **existe solo dentro del bloque `{ }` donde se declara**. Fuera de él ya no existe.

```cpp
int func()
{
  for (int x = 10; x > 0; x--)      // x existe solo dentro de este for
  {
    for (int i = 0; i < 5; i++)     // i existe solo dentro del for interno
    {
      int r = x * 10;               // r existe solo dentro de este bloque
    }
  }                                 // aquí x, i y r ya no existen

  int y = x;                        // error: x ya salió de su scope
  return 0;
}
```

```
 { func
   { for (x)
     { for (i)
       { r }  ← r muere aquí
     }        ← i muere aquí
   }          ← x muere aquí
 }
```

### Memory leak (fuga de memoria)

Pasa cuando se pierde el acceso a memoria del heap sin haberla liberado. Un caso típico: **una excepción impide que se ejecute el `delete`**.

```cpp
#include <stdexcept>

void f()
{
  int* p = new int(20);
  throw std::runtime_error("error");   // salta de f aquí
  delete p;                            // nunca se ejecuta → leak
}
```

### Garbage Collector vs. Smart Pointers

- **Garbage Collector (Java, Python, etc.):** el lenguaje libera los datos que ya nadie usa, pero **no necesariamente al salir de la función**. Java lo hace cuando el GC decide correr; Python usa sobre todo conteo de referencias (más un GC para ciclos).
- **C++ no tiene GC.** En su lugar están los **smart pointers**: cuando el *scope* de la variable termina, eliminan automáticamente el dato del heap.
- Ojo: no funcionan igual por dentro. El GC decide *cuándo* limpiar en tiempo de ejecución; los smart pointers limpian **justo al salir del scope**, gracias al destructor (¡eso es RAII!).

```cpp
#include <memory>
#include <stdexcept>

void f()
{
  auto p = std::make_unique<int>(20);   // smart pointer (vive en el stack)
  throw std::runtime_error("error");    // aunque salte la excepción...
}                                       // ...~unique_ptr libera el int del heap 
```

## 4. RAII: Resource Acquisition Is Initialization

**Idea clave:** atar la vida de un recurso del heap (memoria, archivos, conexiones) a un **objeto en el stack**.

- **Constructor** → adquiere el recurso.
- **Destructor** → lo libera.
- Como los objetos del stack se destruyen solos al salir del alcance, **el recurso se libera automáticamente**, incluso si ocurre una excepción.

```cpp
class ArregloSeguro {
  int* datos;
public:
  ArregloSeguro(int n) { datos = new int[n]; }   // adquiere (heap)
  ~ArregloSeguro()     { delete[] datos; }       // libera

  // Sin esto, copiar el objeto copiaría el puntero y ambos harían delete[] (doble liberación )
  ArregloSeguro(const ArregloSeguro&) = delete;
  ArregloSeguro& operator=(const ArregloSeguro&) = delete;
};

void f()
{
  ArregloSeguro a(100);   // objeto en el stack
  // ... usar a ...
}                         // sale de f → se llama ~ArregloSeguro() → se libera el heap
```

```
 STACK                         HEAP
┌──────────────────┐        ┌──────────┐
│ a (objeto)       │        │ int[100] │
│   datos ─────────┼──────► │          │
└──────────────────┘        └──────────┘
 al salir de f: ~a() → delete[] → heap liberado 
```

>  Si una clase administra un recurso, hay que decidir qué pasa al **copiarla** (por eso se borran las copias arriba). Esto se conoce como la *regla de tres/cinco*. En la práctica, lo más seguro es usar `std::vector` o smart pointers en vez de escribir esta clase a mano.

##  Resumen rápido

- Stack = automático, organizado en **stackframes**.
- Heap = flexible, pero **tú liberas**.
- RAII = el **destructor** de un objeto en el stack libera el recurso del heap → sin fugas.
- Ejemplos reales de RAII en C++: `std::vector`, `std::string`, `std::unique_ptr`, `std::shared_ptr`.
