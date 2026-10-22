#include <iostream>
#include <string>
#include <exception>

class DimensionInvalida : public std::exception
{
    public:
        virtual const char* what() const noexcept override
        {
            return "Dimension Fallo";
        }

};
class Figura
{

protected:

    std::string Fig;

public:
    Figura(std::string Fig) : Fig(Fig) {};

    //Destructor Virtual Obligatoria
    virtual ~Figura() {};

    //Metodo virtual puro
    virtual double area() const = 0;

    virtual std::string getNombre() const{
        return Fig;
    };

};

//Clase hija 1
class Circulo: public Figura
{
private:
    //EL parametro de entrada para la funcion area vive en la clase
    int radio;
public:
    //Constructor de la clase hija Circulo desde la clase Figura original
    Circulo(int radio) : Figura("Circulo"), radio(radio) {};

    //Implementacion de funcion de la clase figura
    //Para usar override, la firma, parametros y cualificador deben ser los mismos
    //Se agrega el const. porque tiene que ser igual a la funcion original
    double area() const override{
        double area = 3.14*radio*radio;
        return area;
    }

    //Ya hereda getNombre de la clase padre
};

//Clase hija 2
class Rectangulo : public Figura {
private:
    int base;
    int altura;
public:
    //Se establece de manera predeterminada el nombre de rectangulo.
    Rectangulo(int base, int altura) : Figura("Rectangulo"), base(base), altura(altura) {};

    double area() const override{
        double area = base*altura;
        return area;
    }
    //Ya heredan getNombre de la clase padre.
};
//Funcion global para imprimir cualquiera de las clases
void imprimir(const Figura& fig)
{
    std::cout << fig.getNombre() << " tiene un area de : " << fig.area() << std::endl;
}
int main ()
{
    Circulo c(5);
    std::cout << "Circulo: " << std::endl << std::endl;
    imprimir(c);
    Rectangulo r(5, 5);
    std::cout << "Rectangulo: " << std::endl << std::endl;
    imprimir(r);

    try
        throw DimensionInvalida();
    catch (const DimensionInvalida& e) {
        std::cout << e.what() << std::endl;
    }
}
