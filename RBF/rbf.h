#ifndef RBF_H
#define RBF_H
#include "PerceptronSimple.h"
#include <vector>
using namespace std;


class rbf {
private:
	int cantRbf;
	int cantPerc;
	vector<vector<double> > centroides;
	vector<int> conjunto;
	vector<PerceptronSimple> p;
	void generaPatrones(vector<vector<double> > patrones,int cantEntradas, int indiceSalida,vector<vector<double> > & salidasIntermedias);
	void salidaIntermedia(vector<vector<double> > entradas, vector<vector<double> > & salidasIntermedias);
	void separaEntradas(vector<vector<double> > patrones,int cantEntradas,vector<vector<double> > & entradas);
	double evaluaGaussiana(vector<double> entrada,vector<double> mu);
	double distancia(vector<double> v1, vector<double> v2);
	void calcCentroide(vector<vector<double> > entradas,vector<int> elementosxconj,vector<vector<double> > &centroides);
public:
	rbf(int cantidadRbf,int cantidadPerceptrones );
	~rbf();
	void entrenarRbf(vector<vector<double> > entradas, vector<vector<double> > & cents);
	void entrenarPesos(vector<vector<double> > patrones,int cantEpocas, int tolerancia);
	double probar(vector<vector<double> > patrones,vector<vector<double> > &salidas);
	int cantidadParametros();
	
};

#endif

