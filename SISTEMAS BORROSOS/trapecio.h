#ifndef TRAPECIO_H
#define TRAPECIO_H

class trapecio {

private:
	
public:	
	//atributos
	double x0,x1,x2,x3;	
	//constructores
	trapecio(double xi0,double xi1,double xi2,double xi3);
	~trapecio();
	//metodos
	double area();
	double areaEscalada(double factor);
	double centroide();
	double membresia(double x);
};

#endif

