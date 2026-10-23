# Vector, referencias, object slicing y punteros en C++

> Apunte armado con mis propias explicaciones (técnica Feynman) y corregido paso a paso.
> Estado: puntos 1-3 listos, punto 4 a medias, `unique_ptr` pendiente.

```
[x] vector y for por rango
[x] referencias (&)
[x] object slicing
[~] punteros en vector + new/delete   <- a medias
[ ] unique_ptr
```

---

## 1. `for (int x : v)` contra `for (int& x : v)`

En cada vuelta, `x` toma el **valor** de un elemento del vector: primero `v[0]`, luego `v[1]`, etc. (No es el índice).

```
v:  [ 10 | 20 | 30 ]

for (int x : v)    -> x es una COPIA del elemento   (cambiarla NO cambia v)
for (int& x : v)   -> x es OTRO NOMBRE del elemento (cambiarla SÍ cambia v)
```

```cpp
std::vector<int> v = {10, 20, 30};

for (int& x : v) { x = 0; }          // v queda {0, 0, 0}
for (int x : v)  { std::cout << x; } // solo lee, no cambia nada
```

**Regla práctica:** `int x` para solo leer; `int& x` para modificar; `const int& x` para leer sin copiar (útil con objetos grandes).

**Ojo, error que tuve:** pensé que `x` era `v[x]`. Si `x` valiera 20, `v[x]` sería `v[20]`, ¡fuera del vector! Lo que `x` guarda es el valor del elemento, no su posición.

## 2. Referencia (`&`)

Una referencia es **otro nombre para la misma variable**.

```cpp
int a = 5;
int& r = a;     // r es el alias; a es la variable original
r = 9;
std::cout << a; // 9
std::cout << &a << " " << &r;  // misma dirección: son la misma variable
```

- El `&` va en el que **se crea como alias** (`r`), no en el original.
- Si `&` aparece en una expresión (`&a`) significa "dirección de a". Si aparece junto a un tipo (`int&`) significa "referencia".
- Con esto funciona `void imprimir(const Figura& f)`: recibe la figura **sin copiarla**.

## 3. Object slicing

Si copias un objeto hijo dentro de una variable del tipo padre, **se corta** lo que la hija agrega.

```
Circle c:   [ Fig | radius ]
                 | Figura f = c;
Figura f:   [ Fig ]            <- se perdió radius
```

Se pierden dos cosas:
1. **Los atributos propios de la hija** (`radius`).
2. **El comportamiento de la hija:** `f` es una `Figura` de verdad, así que `f.area()` ejecuta `Figura::area()`, no `Circle::area()`. Se pierde el polimorfismo.

Por eso un `std::vector<Figura>` **no sirve** para polimorfismo (y si `Figura` es abstracta, ni siquiera compila: no se puede crear una `Figura`).

```cpp
Figura f = figuras[0];     // copia -> intenta crear una Figura
Figura& f = figuras[0];    // alias -> no copia, no hay slicing
```

## 4. Guardar punteros (a medias)

Un puntero guarda una **dirección**, y una dirección mide lo mismo sin importar a qué apunte. Así no se corta nada:

```
vector<Figura*>:  [ 0x10 | 0x24 | 0x38 ]       <- solo direcciones
                     |      |      |
heap:             [Circle][Rectangle][Circle]   <- objetos completos
```

- Los objetos creados con `new` viven en el **heap** y siguen existiendo hasta que yo los destruya con `delete`.
- Si no los destruyo, quedan "flotando": es una **fuga de memoria**.

```cpp
std::vector<Figura*> figuras;
figuras.push_back(new Circle(5));
figuras.push_back(new Rectangle(4, 3));
// ... hay que hacer delete de cada uno
```

**Lo que todavía no tengo del todo claro (pendiente):**
- Cuando el vector se destruye, ¿se destruyen los objetos del heap o solo las direcciones?
- ¿Qué pasa si se lanza una excepción antes de llegar al `delete`?
- ¿Qué idea de RAII resuelve esto sin escribir `delete` a mano? -> `unique_ptr`.

## 5. Siguiente: `unique_ptr`

- [ ] Entender cómo `unique_ptr` aplica RAII para liberar el objeto solo.
- [ ] Armar `std::vector<std::unique_ptr<Figura>>` con `make_unique`.
- [ ] Por qué se recorre con `const auto&` (un `unique_ptr` no se puede copiar).
- [ ] Retomar el mini proyecto: lambdas con `sort` y `max_element`.

## Para comprobar que lo entendí (sin ver la solución)

1. Con `v = {10, 20, 30}`, escribe un `for` que sume 1 a cada elemento y otro que imprima el resultado.
2. Sin correrlo, predice qué imprime: `int a = 5; int& r = a; r = 9; std::cout << a;`
3. Si `Figura` NO fuera abstracta y `Circle` sobrescribiera `area()`, ¿qué `area()` se ejecuta en `Figura f = Circle(5); f.area();`? ¿Por qué?
4. Explica con mis palabras por qué `vector<Figura*>` no corta los objetos, pero sí obliga a hacer `delete`.
