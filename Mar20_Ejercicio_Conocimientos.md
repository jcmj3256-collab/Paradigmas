# Guía Completa de Programación Orientada a Objetos (POO) en C++

Esta guía abarca los conceptos fundamentales y avanzados de la Programación Orientada a Objetos en C++, estructurada paso a paso.

---

## Índice
1. [Clase, Atributos y Constructor](#1-clase-atributos-y-constructor)
2. [Lista de Inicialización](#2-lista-de-inicialización)
3. [Niveles de Acceso (`private`, `protected`, `public`)](#3-niveles-de-acceso-private-protected-public)
4. [Herencia Simple (`: public`)](#4-herencia-simple--public)
5. [Constructor de la Base desde el Hijo](#5-constructor-de-la-base-desde-el-hijo)
6. [Método Virtual Puro (`= 0`) y Clase Abstracta](#6-método-virtual-puro--0-y-clase-abstracta)
7. [`virtual` y `override`](#7-virtual-y-override)
8. [Destructor Virtual](#8-destructor-virtual)
9. [Polimorfismo con Referencias y Punteros](#9-polimorfismo-con-referencias-y-punteros)
10. [`const` en Métodos](#10-const-en-métodos)
11. [`#include`, `main` y Compilar con `-Wall`](#11-include-main-y-compilar-con--wall)

---

## 1. Clase, Atributos y Constructor

Una **clase** es una plantilla para crear objetos. Agrupa **atributos** (variables de estado) y **métodos** (funciones de comportamiento). El **constructor** es una función especial que se ejecuta automáticamente al instanciar un objeto para inicializarlo.

```cpp
#include <iostream>
#include <string>

class Persona {
public:
    std::string nombre;
    int edad;

    // Constructor implícito/explícito
    Persona(std::string n, int e) {
        nombre = n;
        edad = e;
    }
};
```

---

## 2. Lista de Inicialización

La **lista de inicialización** permite asignar valores a los atributos antes de que se ejecute el cuerpo del constructor. Es obligatoria para atributos `const`, referencias (`&`) o para invocar constructores de clases base.

```cpp
class Persona {
private:
    std::string nombre;
    const int id; // Atributo constante

public:
    // Uso de la lista de inicialización : atributo(valor)
    Persona(std::string n, int i) : nombre(n), id(i) {
        // Cuerpo del constructor
    }
};
```

---

## 3. Niveles de Acceso (`private`, `protected`, `public`)

Los modificadores de acceso especifican desde dónde se pueden consultar o modificar los miembros de una clase:

*   **`private`**: Solo accesible desde dentro de la propia clase.
*   **`protected`**: Accesible dentro de la clase y sus clases derivadas (hijas).
*   **`public`**: Accesible desde cualquier parte del programa.

```cpp
class Vehiculo {
private:
    std::string numeroSerie; // Solo accesible por Vehiculo

protected:
    int velocidadMaxima;    // Accesible por Vehiculo y sus derivadas

public:
    std::string marca;       // Accesible públicamente
};
```

---

## 4. Herencia Simple (`: public`)

La herencia permite crear nuevas clases a partir de clases existentes. La sintaxis `: public Base` indica herencia pública, preservando la visibilidad `public` y `protected` de la clase base.

```cpp
class Animal {
public:
    void comer() {
        std::cout << "Comiendo..." << std::endl;
    }
};

// Perro hereda públicamente de Animal
class Perro : public Animal {
public:
    void ladrar() {
        std::cout << "¡Guau!" << std::endl;
    }
};
```

---

## 5. Constructor de la Base desde el Hijo

Cuando una clase derivada se instancia, debe inicializar primero a su clase base. Esto se hace pasando argumentos al constructor base desde la lista de inicialización del constructor derivado.

```cpp
class Vehiculo {
protected:
    std::string marca;

public:
    Vehiculo(std::string m) : marca(m) {}
};

class Coche : public Vehiculo {
private:
    int puertas;

public:
    // Se invoca el constructor de Vehiculo en la lista de inicialización
    Coche(std::string m, int p) : Vehiculo(m), puertas(p) {}
};
```

---

## 6. Método Virtual Puro (`= 0`) y Clase Abstracta

Un **método virtual puro** es una función que no tiene implementación en la clase base y se declara con `= 0`. Una clase que contenga al menos un método virtual puro se convierte en una **clase abstracta**, la cual no se puede instanciar directamente.

```cpp
class Figura { // Clase Abstracta
public:
    // Método virtual puro
    virtual double calcularArea() const = 0; 
};
```

---

## 7. `virtual` y `override`

*   **`virtual`**: Indica en la clase base que un método puede ser sobrescrito por una clase derivada.
*   **`override`**: Se usa en la clase derivada para indicar explícitamente que se está rediseñando un método virtual base. Ayuda al compilador a verificar la firma exacta.

```cpp
class Figura {
public:
    virtual void dibujar() {
        std::cout << "Dibujando figura genérica" << std::endl;
    }
};

class Circulo : public Figura {
public:
    void dibujar() override { // Garantiza la correcta sobrescritura
        std::cout << "Dibujando un círculo" << std::endl;
    }
};
```

---

## 8. Destructor Virtual

Si una clase se usará como clase base con polimorfismo, su **destructor debe ser virtual**. De lo contrario, eliminar un objeto derivado a través de un puntero a la clase base provocará comportamiento indeterminado (*undefined behavior*) y fuga de memoria.

```cpp
class Base {
public:
    Base() {}
    virtual ~Base() { // Destructor virtual obligatorio
        std::cout << "Destructor de Base" << std::endl;
    }
};

class Derivada : public Base {
public:
    ~Derivada() override {
        std::cout << "Destructor de Derivada" << std::endl;
    }
};
```

---

## 9. Polimorfismo con Referencia

El **polimorfismo** permite tratar objetos de clases derivadas a través de referencias o punteros de la clase base, ejecutando los métodos adecuados según la instancia real en tiempo de ejecución.

```cpp
void ejecutarSonido(const Animal& a) {
    a.hacerSonido(); // Llama al método según el objeto real referenciado
}

int main() {
    Perro p;
    Gato g;
    
    ejecutarSonido(p); // Llama a Perro::hacerSonido
    ejecutarSonido(g); // Llama a Gato::hacerSonido
}
```

---

## 10. `const` en Métodos

Añadir `const` al final de la firma de un método le indica al compilador que dicho método **no modificará** ningún atributo del objeto. Es obligatorio para poder invocar el método sobre instancias constantes.

```cpp
class CuentaBancaria {
private:
    double saldo;

public:
    CuentaBancaria(double s) : saldo(s) {}

    // Método de solo lectura (const)
    double getSaldo() const {
        return saldo;
    }
};
```

---

## 11. `#include`, `main` y Compilar con `-Wall`

### Estructura básica de un programa C++
Se incluyen las librerías con `#include` y se implementa la función `main`, punto de entrada de la aplicación.

```cpp
#include <iostream>

int main() {
    std::cout << "¡Hola, mundo!" << std::endl;
    return 0; // 0 indica ejecución exitosa
}
```

### Compilación desde la terminal con `-Wall`
El flag `-Wall` activa todas las advertencias (*warnings*) del compilador, lo cual es imprescindible para detectar posibles errores lógicos y mantener código de alta calidad.

```bash
# Compilar un archivo main.cpp generando el ejecutable "programa"
g++ -Wall -std=c++17 main.cpp -o programa

# Ejecutar el programa resultante
./programa
```
