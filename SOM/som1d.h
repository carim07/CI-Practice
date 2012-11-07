#ifndef SOM1D_H
#define SOM1D_H

#include <vector>
using namespace std;

class som1d {
private:
	vector<vector<double> > pesos;
	int cantNeuronas;
	int ancho;
	int cantEntradas;
	double distancia(vector<double> v1,vector<double> v2);
public:
	som1d(int cNeuronas,int cantEntradas);
	~som1d();
	void entrenar(vector<vector<double> > patrones,vector<vector<vector<double> > > & pesosPrueba,int e1=100,int e2=1000, int e3=200);
	
};

#endif

