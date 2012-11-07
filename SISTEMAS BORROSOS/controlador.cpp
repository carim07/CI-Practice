#include "controlador.h"
#include <vector>
using namespace std;


controlador::controlador(int nReg){
	nReglas=nReg;
}
//}
//

controlador::~controlador(){
	
}


//mapeo vector de trapecios de entrada y salida 
void controlador::mapeo(trapecio entrada, trapecio salida){
	vector<trapecio> aux;
	aux.push_back(entrada);
	aux.push_back(salida);
	creglas.push_back(aux);
	aux.clear();
}

vector<double> controlador::salidaParcial(int regla, double entrada){
	vector<double> salida;
	double cent,area;	
	cent=creglas[regla][1].centroide();
	area=creglas[regla][1].areaEscalada(creglas[regla][0].membresia(entrada));
	salida.push_back(cent);
	salida.push_back(area);
	return salida;
}

double controlador::salidaTotal(double entrada){
	double numerador=0;
	double denominador=0;
	vector<double> salida;
	double salidatotal;
	
	//centroid-defuzzyfication	
	for(int i=0; i<nReglas; i++){ //para cada regla
		salida=salidaParcial(i,entrada);
		numerador += salida[0]*salida[1];//centroide x area escalada
		denominador += salida[1];//area escalada
	}	
	if(denominador==0) salidatotal=0; //entrada en ningun trapecio?
	else salidatotal=numerador/denominador;
	return salidatotal;
}


	
