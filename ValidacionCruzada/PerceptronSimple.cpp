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
#include "PerceptronSimple.h"
#include <math.h>

PerceptronSimple::PerceptronSimple() {};
PerceptronSimple::~PerceptronSimple() {};


double PerceptronSimple::factivacion(double a){
	if (a>=0)
		return 1;
	else
		return -1;
};

void PerceptronSimple::setTasa(double tasa){
	mu = tasa;
};
void PerceptronSimple::entrenamiento(vector<vector<double> > patrones,int nEpocas,double tolerancia){
	int cantEntradas = patrones[0].size()-1;
	int cantPatrones = patrones.size();
	int salida;
	int salidaDeseada;
	int errores;
	double prodPunto;
	double porcentajeError = 100;
	
	
	
	wEpoca.clear();
	w.clear();
	
	//inicializamos pesos aleatoriamente considerando el bias (+1)
	for(int i=0; i<cantEntradas+1;i++)
	{		
		w.push_back(-0.5+ (double)(rand())/ (double)(RAND_MAX));
	}
	wEpoca.push_back(w);
	
	//comienzo del entrenamiento
	int k=0;
	while(k<nEpocas && porcentajeError>tolerancia) //while por epocas
	{
		errores = 0;
		
		for(int j=0;j<cantPatrones;j++) //for por patrones
		{	
			prodPunto=0;
			for(int i=0;i<cantEntradas;i++) //for por entradas
			{
				prodPunto = prodPunto + patrones[j][i]*w[i+1]; // w[i+1] pues w[0] corresponde al umbral
			}
			prodPunto = prodPunto - w[0]; //consideramos w[0] para la entrada x=-1
			salida = factivacion(prodPunto);
			salidaDeseada = patrones[j][cantEntradas];
			
			if ((salida - salidaDeseada)!=0)	// para determinar si actualizamos pesos o no
			{
				w[0]=w[0]-mu/2*(salida - salidaDeseada)*(-1);
				for(int i=0;i<cantEntradas;i++)
				{
					w[i+1]=w[i+1]-mu/2*(salida - salidaDeseada)*patrones[j][i];
				}
				errores++;
			}						
						
		}
		wEpoca.push_back(w);	//guardamos el vector de pesos final de cada epoca para graficar.
		//Porcentaje de aciertos
		porcentajeError = (double)errores/(double)cantPatrones;
		k++;
	}
	cout << "Termino en " << k << " epocas" << endl;
	
};

double PerceptronSimple::prueba(vector<vector<double> > patronesPrueba, vector<double> &salida){
	
	int cantEntradas = patronesPrueba[0].size()-1;
	int cantPatrones = patronesPrueba.size(); 
	int errores=0;
	double prodPunto;
	int y;
	int salidaDeseada;
	double efectividad;
	
	for(int k=0;k<cantPatrones;k++)
	{
		prodPunto=0;
		for(int i=0;i<cantEntradas;i++) //for por entradas
		{
			prodPunto = prodPunto + patronesPrueba[k][i]*w[i+1]; // w[i+1] pues w[0] corresponde al umbral
		}
		prodPunto = prodPunto - w[0]; //consideramos w[0] para la entrada x=-1
		y = factivacion(prodPunto);
		salida.push_back(y);
		salidaDeseada = patronesPrueba[k][cantEntradas];
		if(y != salidaDeseada)
			errores++;
	}
	
	efectividad = (double)(cantPatrones-errores)/(double)cantPatrones;
	return efectividad;
};

vector<vector<double> > PerceptronSimple::obtenerwEpoca(){
	return this->wEpoca;
}
