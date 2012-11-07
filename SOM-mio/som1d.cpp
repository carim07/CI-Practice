#include "som1d.h"
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


som1d::som1d(int cNeuronas,int cantEntradas) {
	cantNeuronas=cNeuronas;
	ancho=sqrt(cantNeuronas);
	this->cantEntradas=cantEntradas;
	//inicializamos pesos aleatorios
	vector<double> aux1;	//pesos por neurona
	for(int i=0;i<cantNeuronas;i++){		
		for(int k=0;k<cantEntradas;k++)
				aux1.push_back(-0.5+(float)rand()/(float)RAND_MAX);
		pesos.push_back(aux1);
		aux1.clear();
		}		
}

som1d::~som1d() {
	
}


double som1d::distancia(vector<double> v1, vector<double> v2){
	double distancia=0;
	for(int i=0;i<v1.size();i++)
		distancia+=(v1[i] - v2[i])*(v1[i] - v2[i]);
	return sqrt(distancia);
}

void som1d::entrenar(vector<vector<double> > patrones, vector<vector<vector<double> > > & pesosPrueba){
	int entorno;
	int ganadora;
	int epocas;
	int base;
	int nodo;
	int inicio;
	int fin;
	
	double entornoTemporal;
	double eta;
	double deltaEntorno,deltaEta;
	double distmin,distAux;
	
	//Etapas de entrenamiento
	for(int e=0;e<3;e++){
		if(e==0){//ordenamiento global
			entorno=ancho/2;
			eta=0.9;
			epocas=1000;
		}
		if(e==1){//transicion
			entorno=ancho/2;
			eta=0.9;
			epocas=1000;
			deltaEntorno=(float)(((float)entorno-1)/epocas);
			deltaEta=(float)((eta-0.1)/epocas);
			entornoTemporal=entorno;
		}
		if(e==2){//convergencia
			entorno=0;
			eta=0.1;
			epocas=3000;			
		}
		for(int ep=0;ep<epocas;ep++){
			//recorremos por patrones
			for(int i=0;i<patrones.size();i++){
				ganadora=0;
				distmin=9999;
				distAux=0;
				//obtenemos la ganadora
				for(int j=0;j<cantNeuronas;j++){
					distAux=distancia(patrones[i],pesos[j]);						
					if(distAux<distmin){
						distmin=distAux;
						ganadora=j;						
					}
				}//fin for ganadora
				
				//Calculamos las neuronas pertenecientes al entorno
				for(int j=-entorno;j<=entorno;j++)
					if((ganadora+j)>=0 && (ganadora+j)<cantNeuronas)
						//actualizamos los pesos
						for(int l=0;l<cantEntradas;l++)
							pesos[ganadora+j][l]+= eta*(patrones[i][l]-pesos[ganadora+j][l]);
							
								
				if(e==1){
					entornoTemporal-=deltaEntorno;
					entorno=round(entornoTemporal);
					eta-=deltaEta;
				}
			}
			if(ep%50==0)
				pesosPrueba.push_back(pesos);			
			
		}
			
	}
}
