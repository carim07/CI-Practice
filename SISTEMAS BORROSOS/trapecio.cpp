#include "trapecio.h"
//CONSTRUCTORES

trapecio::trapecio(double xi0,double xi1,double xi2,double xi3)	
{
	x0=xi0;
	x1=xi1;
	x2=xi2;
	x3=xi3;
}
	


trapecio::~trapecio() {	
}


//AREA Y AREA ESCALADA
double trapecio::area()
{
	double r=1*((x3-x0)+(x2-x1))/2.0;
	return r;
}

double trapecio::areaEscalada(double factor)
{
	double r=this->area()*factor;
	return r;
}


//CENTROIDE DEL TRAPECIO
double trapecio::centroide()
{	
	double cent;
	double bM=x3-x0;
	double bm=x2-x1;
	cent= x0 + (bM*bM + bM*bm + bm*bm)/(3.0*(bM+bm));
	return cent;
}


//MEMBRESIA DE X
double trapecio::membresia(double x)
{	//ecuacion de la recta entre x0 y x1
	if (x>=x0 && x<=x1)
		return 1/(x1-x0)*(x-x0);
	//entre x1 y x2, membresia=1
	else if (x>x1 && x<=x2)
		return 1.0;
	//ecuacion de la recta entre x2 y x3
	else if (x>x2 && x<=x3)
		return (-1/(x3-x2)*(x-x2))+1;
	//si no cae dentro del trapecio, devuelve 0
	else return 0;
}
