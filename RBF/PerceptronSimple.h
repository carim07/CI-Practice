#ifndef PERCEPTRONSIMPLE_H
#define PERCEPTRONSIMPLE_H
#include <vector>
using namespace std;

class PerceptronSimple {
private:
// vector de pesos
	vector<double> w;
// tasa de aprendizaje
	double mu;
// vector w por epoca
	vector<vector<double> > wEpoca;
		
public:
	PerceptronSimple();
	~PerceptronSimple();
	double factivacion(double a);
	void setTasa(double tasa);
	void entrenamiento(vector<vector<double> > patrones,int nEpocas,double Tolerancia);
	double prueba(vector<vector<double> > patronesPrueba,vector <double> &salida);
	vector<vector<double> > obtenerwEpoca();	
};

#endif

