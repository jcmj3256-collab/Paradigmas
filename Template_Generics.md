# Template y Generics
Templates: Reemplazar tipos de datos, por uno solo generico.   
Ejemplo: T max(T a, Tb);  
template  
  Tmax(T a, T b)
  {
    if(a>=b)
      return a;
    return b; 
  }

  A nivel de metodo o a nivel de clase. 
