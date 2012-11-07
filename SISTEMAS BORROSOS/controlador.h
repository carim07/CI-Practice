#ifndef CONTROLADOR_H
#define CONTROLADOR_H
#include "trapecio.h"
#include <vector>
using namespace std;


class controlador {
private:
	int nReglas;
	vector<vector<trapecio> > creglas; //matriz de reglas
	//Para una regla j, obtenemos centroide y area del conjunto de salida si la regla se activa
	vector<double> salidaParcial(int j,double entrada);	
public:
	controlador(int nReg);
	~controlador();
	void mapeo(trapecio entrada,trapecio salida);
	double salidaTotal(double entrada);
		
};

#endif

