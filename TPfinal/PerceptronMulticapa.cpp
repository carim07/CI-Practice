#include "PerceptronMulticapa.h"
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

PerceptronMulticapa::PerceptronMulticapa(int cantCapas,vector<int> neuXcapa,double m,double a,double cur){
	cantidadCapas = cantCapas;
	neuronasXcapa = neuXcapa;
	mu = m;
	alfa = a;
	curvatura = cur;
	//vectores auxiliares para inicializacion de toda la red
	vector<double> aux1;	
	vector<double> aux2;
	vector<vector<double> > aux3;
	vector<vector<double> > aux4;
	
	for(int i=0;i<cantidadCapas;i++){
		for(int j=0;j<neuronasXcapa[i];j++){
			aux1.push_back(0.0);	//salida inicial de cada neurona
		}
		salidaXneurona.push_back(aux1);	//salidas iniciales de cada neuronas
		delta.push_back(aux1);		//gradientes iniciales
		aux1.clear();
	}
	
	
	//inicializamos los pesos aleatorios y la diferencia de pesos. La capa 0 no tiene pesos asociados.
	//cada neurona tiene tantos pesos como cantidad de neuronas existen en la capa anterior de la red mas el umbral
	// semilla del random
	srand((unsigned)(time(NULL)));
	
	pesos.push_back(aux3);	//capa 0 no tiene pesos
	difPesos.push_back(aux3);
	for(int i=1;i<cantidadCapas;i++){
		for(int j=0;j<neuronasXcapa[i];j++){
			for(int k=0;k<neuronasXcapa[i-1]+1;k++){			//+1 por el umbral
				aux1.push_back(-0.5+ (double)(rand())/ (double)(RAND_MAX));
				aux2.push_back(0.0);
			}//fin for cantidad de pesos de una neurona
			aux3.push_back(aux1);//pesos de cada neurona en una capa
			aux4.push_back(aux2);//dif de pesos inicial de cada neurona en una capa
			aux1.clear();
			aux2.clear();
		}//fin for por neurona
		pesos.push_back(aux3);//pesos de una capa en la matriz de pesos de la red
		difPesos.push_back(aux4);
		aux3.clear();
		aux4.clear();
	}//fin for por capa
}

PerceptronMulticapa::~PerceptronMulticapa(){

}

//funcion de activacion sigmoidea centrada en 0
double PerceptronMulticapa::factivacion(double x){
	return (double)(2.0/(1.0+exp(-x*curvatura)))-1.0;
}

//propagacion hacia adelante
void PerceptronMulticapa::propagacionAdelante(vector<double> entradas){
	double suma;
	//asignamos las entradas la capa 1 = salida capa 0
	for(int i=0;i<neuronasXcapa[0];i++){
		salidaXneurona[0][i]=entradas[i];		
	}
	//calculamos las salidas de las siguientes capas
	for(int i=1;i<cantidadCapas;i++){
		for(int j=0;j<neuronasXcapa[i];j++){
			suma=0.0;
			for(int k=0;k<neuronasXcapa[i-1];k++){
				suma+= salidaXneurona[i-1][k]*pesos[i][j][k]; //pesos[por capa][por neurona][por peso]
			}
			suma+=pesos[i][j][neuronasXcapa[i-1]];	//agregamos el umbral
			salidaXneurona[i][j]=factivacion(suma); //se calcula la funcion de activacion para cada neurona
		}//fin for por neuronas de cada capa
	}//fin for por cantidad de capas
	
	
}
bool PerceptronMulticapa::error(vector<double> salidaDeseada) 
{	// retorna true si hay error en alguna de las salidas
	for(int i=0; i<neuronasXcapa[cantidadCapas-1]; i++)
	{// vericamos si la salida y la salida deseada son distintas
		if(salidaXneurona[cantidadCapas-1][i] * salidaDeseada[i] < 0) return true;
	}
	// si no hay error, retorna false
	return false;
}

//error cuadratico de la red
double PerceptronMulticapa::errorCuadraticoTotal(vector<double> salidaDeseada){
	double error=0.0;
	for(int i=0;i<neuronasXcapa[cantidadCapas-1];i++){
		error+=(salidaDeseada[i]-salidaXneurona[cantidadCapas-1][i])*(salidaDeseada[i]-salidaXneurona[cantidadCapas-1][i]);
	}
	return error/2.0;
}
//propagacion hacia atras
void PerceptronMulticapa::propagacionAtras(vector<double> entradas, vector<double> salidaDeseada){
	/*PASOS:
	1-Se hace propagacion hacia adelante
	2-Se calcula el gradiente de error local instantaneo hacia atras (de la ultima capa a la primera)
	3-Se aplica el termino de momento para cada peso
	4-Se actualizan los pesos hacia adelante
	*/
	
	double suma;
	//propagacion hacia adelante
	propagacionAdelante(entradas);
	
	//calculamos el gradiente(delta) de la ultima capa
	for(int i=0;i<neuronasXcapa[cantidadCapas-1];i++){
		//el gradiente de la ultima capa es el error*la derivada de la funcion sigmoidea
		delta[cantidadCapas-1][i]=0.5*(salidaDeseada[i]-salidaXneurona[cantidadCapas-1][i])*(1+salidaXneurona[cantidadCapas-1][i])*(1-salidaXneurona[cantidadCapas-1][i]);
	}
	//calculamos el gradiente para las capas anteriores (ocultas)
	for(int i=cantidadCapas-2;i>0;i--){
		//para cada neurona de la capa actual
		for(int j=0;j<neuronasXcapa[i];j++){
			suma=0.0;
			//para cada neurona de la capa siguiente
			for(int k=0;k<neuronasXcapa[i+1];k++){
				suma+=delta[i+1][k]*pesos[i+1][k][j];	//producto de gradiente y pesos de la capa siguiente asociados a la neurona j
			}
			delta[i][j]=suma*(1+salidaXneurona[i][j])*(1-salidaXneurona[i][j]);
		}
	}
	//aplicamos termino de momento
	for(int i=1;i<cantidadCapas;i++){
		for(int j=0;j<neuronasXcapa[i];j++){
			for(int k=0;k<neuronasXcapa[i-1];k++){
				pesos[i][j][k]+=alfa*difPesos[i][j][k];
			}
			pesos[i][j][neuronasXcapa[i-1]]+=alfa*difPesos[i][j][neuronasXcapa[i-1]];
		}
	}
	//ahora actualizamos los pesos
	for(int i=1;i<cantidadCapas;i++){	//para cada capa
		for(int j=0;j<neuronasXcapa[i];j++){	//para cada neurona
			for(int k=0;k<neuronasXcapa[i-1];k++){	//para cada peso asociados a cada neurona
				difPesos[i][j][k]=(1-alfa)*mu*delta[i][j]*salidaXneurona[i-1][k];
				pesos[i][j][k]+=difPesos[i][j][k];
			}
			//pesos del umbral(no tiene entradas)
			difPesos[i][j][neuronasXcapa[i-1]]=(1-alfa)*mu*delta[i][j];
			pesos[i][j][neuronasXcapa[i-1]]+=difPesos[i][j][neuronasXcapa[i-1]];
		}
	}
}
	
//Devuelve la salida de la neurona j de la capa i
double PerceptronMulticapa::obtenerSalida(int i,int j){
	return salidaXneurona[i][j];
}
vector<double> PerceptronMulticapa::getSalidas(){
	return salidaXneurona[cantidadCapas-1];
}

int PerceptronMulticapa::cantidadParametros(){
	int p=0;
	for(int i=1;i<cantidadCapas;i++)
		p+=(neuronasXcapa[i]*(neuronasXcapa[i-1]+1));
	return p;
}
