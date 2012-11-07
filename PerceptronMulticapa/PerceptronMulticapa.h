#ifndef PERCEPTRONMULTICAPA_H
#define PERCEPTRONMULTICAPA_H

#endif

#include <vector>
using namespace std;

class PerceptronMulticapa{
	int cantidadCapas;
	vector<int> neuronasXcapa;
	vector<vector<double> > salidaXneurona;
	vector<vector<double> > gradiente;	//gradiente de error local instantaneo [capa][neurona]
	vector<vector<vector<double> > > pesos;	//vector de pesos asociado a una neurona de una capa [capa][neurona][pesos]
	vector<vector<vector<double> > > difPesos;	//diferencias de pesos entre epocas asociado a una neurona de una capa
	double mu;	//tasa de aprendizaje
	double alfa; //termino de momento
	double curvatura; //funcion sigmoid
	double factivacion(double x);	
public:
	~PerceptronMulticapa();
	PerceptronMulticapa(int cantCapas,vector<int> neuXcapa,double m,double a,double cur);
	void propagacionAdelante(vector<double> entradas);
	double errorCuadraticoTotal(vector<double> salidaDeseada);
	bool error(vector<double> salidaDeseada); 
	void propagacionAtras(vector<double> entradas, vector<double> salidaDeseada);
	double obtenerSalida(int i, int j);
	vector<double> getSalidas();
	int cantidadParametros();
	
		
	
};
