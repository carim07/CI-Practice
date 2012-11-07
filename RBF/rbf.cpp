#include "rbf.h"
#include "PerceptronSimple.h"
#include <stdlib.h>
#include <cstdlib>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h> 
#include <algorithm> 
#include <math.h>
#include <cmath>


rbf::rbf(int cantidadRbf, int cantidadPerceptrones){
	cantRbf = cantidadRbf;
	cantPerc = cantidadPerceptrones;
	
}

rbf::~rbf() {
}

double rbf::distancia(vector<double> v1, vector<double> v2){
	double distancia=0;
	for(int i=0;i<v1.size();i++)
		distancia+=((v1[i] - v2[i])*(v1[i] - v2[i]));
	return sqrt(distancia);
}
void rbf::separaEntradas(vector<vector<double> > patrones,int cantEntradas,vector<vector<double> > & entradas){
	for(int i=0;i<patrones.size();i++){
		vector<double> aux;
		for(int j=0;j<cantEntradas;j++){
			aux.push_back(patrones[i][j]);
		}
		entradas.push_back(aux);
		aux.clear();
	}
}

double rbf::evaluaGaussiana(vector<double> entrada,vector<double> mu){
	double var=1;
	//entrada y mu se supone q tienen la misma dimension . no seas pavote
	vector<double> diferencia;
	for(int i=0;i<entrada.size();i++)
		diferencia.push_back(abs(entrada[i]-mu[i]));
	double norma=0;
	for(int i=0;i<entrada.size();i++)
		norma+=diferencia[i]*diferencia[i];
	
	return exp(-norma/(2*var*var));
		
}

void rbf::salidaIntermedia(vector<vector<double> > entradas, vector<vector<double> > & salidasIntermedias){
	for(int i=0;i<entradas.size();i++){
		vector<double> aux;
		for(int j=0;j<centroides.size();j++){
			aux.push_back(evaluaGaussiana(entradas[i],centroides[j]));
		}
		salidasIntermedias.push_back(aux);
	}
	
}

void rbf::generaPatrones(vector<vector<double> > patrones,int cantEntradas,int indiceSalida, vector<vector<double> > & salidasIntermedias){
	for (int i=0;i<patrones.size();i++){
		salidasIntermedias[i].push_back(patrones[i][cantEntradas+indiceSalida]);
	}
}
	
void rbf::calcCentroide(vector<vector<double> > entradas,vector<int> elementosxconj,vector<vector<double> > &centroides){
	centroides.clear();
	vector<double> aux;
	vector<double> suma;
	int cantPatrones=entradas.size();
	//centorides en cero
	for(int j=0;j<entradas[0].size();j++)
		aux.push_back(0.0);    
	for(int i=0;i<cantRbf;i++)    
		centroides.push_back(aux);
	aux.clear();
	
	//calculamos centroides
	for(int i=0;i<cantRbf;i++){                
		for(int j=0;j<entradas[0].size();j++) //suma en cero
			suma.push_back(0.0);
		for(int j=0;j<cantPatrones;j++){
			if(conjunto[j]==i){    //el patron j pertenece al conjunto i
				for(int k=0;k<entradas[0].size();k++)
					suma[k]+=entradas[j][k];                
			}
		}
		//cout<<"elemento por conjunto "<<i<<" "<<elementosxconj[i]<<endl;
		for(int j=0;j<entradas[0].size();j++)
			aux.push_back(suma[j]/elementosxconj[i]);    
		
		centroides[i]=aux;
		aux.clear();
		suma.clear();
	}//fin calculo centroides
	
}

void rbf::entrenarRbf(vector<vector<double> > entradas, vector<vector<double> > & cents){
	int dimEntrada=entradas[0].size();
	int cantPatrones=entradas.size();
	bool entrena=true;
	//inicializamos los conjutos aleatorios
	
	for(int i=0;i<cantPatrones;i++){
		int aleatorio=rand()%cantRbf;
		conjunto.push_back(aleatorio);
	}
	//veo el tamanio inicial de los conjuntos
	vector<int> patronesxconj;
	for (int j=0;j<cantRbf;j++)
		patronesxconj.push_back(0);
	for(int i=0;i<cantPatrones;i++){
		for (int j=0;j<cantRbf;j++){
			if(conjunto[i]==j)
				patronesxconj[j]++;
			}
			
		}
	
	//inicializamos los centroides con 0,0,..
	vector<double> aux;
	for(int j=0;j<dimEntrada;j++)
		aux.push_back(0.0);
	for(int i=0;i<cantRbf;i++)
		centroides.push_back(aux);
	aux.clear();
	
		
	//ahora entrenamos
	int maxit = 15;
	int it=0; //cantidad de elementos de un conjunto
	vector<double> suma;
	
	
	while(entrena && it<maxit){
		//Calculamos los centroides
		calcCentroide(entradas,patronesxconj,centroides);
				
		//Reasignamos conjuntos
		entrena=false;
		
		for(int i=0;i<cantPatrones;i++){
			double distmin = 9999999;	
			int centmin;
			for(int j=0;j<cantRbf;j++){
				double d=distancia(entradas[i],centroides[j]);
				if(d<distmin){
					distmin=d;
					centmin=j;
				}
			}
			
			if(centmin!=conjunto[i] /* && patronesxconj[conjunto[i]]>0 */ ){
				entrena=true;//hubo reasignacion
				
				patronesxconj[conjunto[i]]--;
				patronesxconj[centmin]++;
				conjunto[i]=centmin;
				calcCentroide(entradas,patronesxconj,centroides);
			}
			
		}
		
		
				
		it++;
	}
	cents=centroides;
	cout<<"Patrones por conjunto final"<<endl;
	for (int i=0;i<cantRbf;i++)
		cout<<"Conjunto "<<i<<": "<<patronesxconj[i]<<endl;
}



void rbf::entrenarPesos(vector<vector<double> > patrones,int cantEpocas, int tolerancia){
	int cantSalidas = cantPerc; //solo para verlo bien
	int cantEntradas = patrones[0].size()-cantSalidas;
	vector<vector<double> > entradas;
	vector<vector<double> > salidasIntermedias;
	
	for(int i=0;i<cantPerc;i++){
		PerceptronSimple pi;
		p.push_back(pi);
	}
	//separamos las entradas para mandarselas a las gaussianas
	separaEntradas(patrones,cantEntradas,entradas);
	//obtenemos las salidas intermedias (matriz de patrones.size()xcantRbf)
	salidaIntermedia(entradas,salidasIntermedias);
	//copiamos vector para mandar a entrenar los perceptrones
	vector<vector<double> > nuevosPatrones=salidasIntermedias;
	//entrenamos cada perceptron con su correspondiente salida deseada
	for(int i=0;i<cantPerc;i++){
		generaPatrones(patrones,cantEntradas,i,nuevosPatrones);
		p[i].entrenamiento(nuevosPatrones,cantEpocas,tolerancia);
	}
	
}



double rbf::probar(vector<vector<double> > patrones, vector<vector<double> > & salidas){
	int cantEntradas = patrones[0].size()-cantPerc;
	//cargamos las entradas de cada patron
	vector<vector<double> > entradas;
	separaEntradas(patrones,cantEntradas,entradas);
	//calculamos las salidas intermedias
	vector<vector<double> > salidasIntermedias;
	salidaIntermedia(entradas,salidasIntermedias);
	//ahora probamos los perceptrones
		
	for(int i=0;i<cantPerc;i++){
		vector<double> aux;
		generaPatrones(patrones,cantEntradas,i,salidasIntermedias);
		p[i].prueba(salidasIntermedias,aux);		
		salidas.push_back(aux);		
	}
	
	//verificamos la efectividad
	int errores=0;
	bool error=false;
	for(int i=0;i<salidas[0].size();i++){
		for(int j=0;j<cantPerc;j++){
			error=false;
			if(patrones[i][cantEntradas+j]*salidas[j][i]<0)
				error=true;
		}
		if(error)
			errores++;
	}
	return 1-(float)errores/patrones.size();	//retorna efectividad
}
int rbf::cantidadParametros(){
	return cantRbf+(cantPerc*(cantRbf+1));
}
