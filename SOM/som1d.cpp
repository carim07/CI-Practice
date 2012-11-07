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

void som1d::entrenar(vector<vector<double> > patrones, vector<vector<vector<double> > > & pesosPrueba,int e1,int e2, int e3){
	int entorno=ancho/2;
	double entornoTemporal=ancho/2;
	int ganadora;
	int etapa1=e1,
		etapa2=e2,
		etapa3=e3;
	int epocas=etapa1+etapa2+etapa3;	
	double eta=0.9;
	double deltaEntorno=(entornoTemporal-1)/etapa2,
		deltaEta=(eta-0.1)/etapa2;
	double distmin,distAux;
	
	for(int ep=0;ep<epocas;ep++){
		if(ep<etapa1){//ordenamiento global
			entorno=ancho/2;
			eta=0.9;
		}
		if(ep>=etapa1&&ep<(etapa1+etapa2)){//transicion
			entornoTemporal=entornoTemporal-deltaEntorno;
			entorno=round(entornoTemporal);
			eta-=deltaEta;
			cout<<"Entorno: "<<entorno<<" Mu: "<<eta<<endl;
		}
		if(ep>=(etapa1+etapa2)){//convergencia
			entorno=0;
			eta=0.1;
		}
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
		}
		pesosPrueba.push_back(pesos);			
			
		
			
	}
}
