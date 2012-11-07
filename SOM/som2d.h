#ifndef SOM2D_H
#define SOM2D_H

#include <vector>
using namespace std;

class som2d {
private:
	vector<vector<vector<double> > > pesos;
	int cantNeuronas;
	int ancho;
	int cantEntradas;
	double distancia(vector<double> v1,vector<double> v2);
	vector<vector<vector<int> > > contadorClase;
	int cantClases;
	vector<vector<int> > clases;
public:
	som2d(int cNeuronas,int cantEntradas,int cClases=0);
	~som2d();
	void entrenar(vector<vector<double> > patrones,vector<vector<vector<vector<double> > > > & pesosPrueba,int e1=100,int e2=1000, int e3=200);
	void etiquetar(vector<vector<double> > patrones);
	void probar(vector<vector<double> > patrones,vector<int> & salida);
	int cantidadParametros();
};

#endif

