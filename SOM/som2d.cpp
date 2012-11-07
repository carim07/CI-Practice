#include "som2d.h"
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




som2d::som2d(int cNeuronas,int cantEntradas,int cClases) {
	cantNeuronas=cNeuronas;
	ancho=sqrt(cantNeuronas);
	this->cantEntradas=cantEntradas;
	//inicializamos pesos aleatorios
	vector<double> aux1;	//pesos por neurona
	vector<vector<double> > aux2;	//pesos por ancho
	for(int i=0;i<ancho;i++){		
		for(int j=0;j<ancho;j++){
			for(int k=0;k<cantEntradas;k++)
				aux1.push_back(-0.5+rand()/RAND_MAX);
			aux2.push_back(aux1);
			aux1.clear();
		}
		pesos.push_back(aux2);
		aux2.clear();
	}
	//inicializamos contador de clase a cero para clasificador
	
	if(cantClases!=0){
		vector<int> auxi1;
		vector<vector<int> > auxi2;
		
		for(int i=0;i<ancho;i++){
			for(int j=0;j<ancho;j++){
				for(int k=0;k<cantClases;k++){
					auxi1.push_back(0);
				}
				auxi2.push_back(auxi1);
				auxi1.clear();                
			}
			contadorClase.push_back(auxi2);
			auxi2.clear();
		}
		//inicializamos vector de clases con todas a 0
		auxi1.clear();
		for(int i=0;i<ancho;i++){
			for(int j=0;j<ancho;j++)
				auxi1.push_back(0);
			clases.push_back(auxi1);
			auxi1.clear();
		}
	}    
}

som2d::~som2d() {
	
}


double som2d::distancia(vector<double> v1, vector<double> v2){
	double distancia=0;
	for(int i=0;i<v1.size();i++)
		distancia+=(v1[i] - v2[i])*(v1[i] - v2[i]);
	return sqrt(distancia);
}

void som2d::entrenar(vector<vector<double> > patrones, vector<vector<vector<vector<double> > > > & pesosPrueba,int e1,int e2, int e3){
	int entorno=ancho/2;
	double entornoTemporal=ancho/2;
	double eta=0.9;
	int etapa1=e1,
		etapa2=e2,
		etapa3=e3;
	int epocas=etapa1+etapa2+etapa3;
	double deltaEntorno=(entornoTemporal-1)/etapa2,
		deltaEta=(eta-0.1)/etapa2;
	
	int ganadorai;
	int ganadoraj;
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
			ganadorai=0;
			ganadoraj=0;
			distmin=9999;
			distAux=0;
			//obtenemos la ganadora
			for(int j=0;j<ancho;j++){
				for(int k=0;k<ancho;k++){
					distAux=distancia(patrones[i],pesos[j][k]);						
					if(distAux<distmin){
						distmin=distAux;
						ganadorai=j;
						ganadoraj=k;
					}
				}
			}//fin for ganadora
			//Calculamos las neuronas pertenecientes al entorno
			for(int j=-entorno;j<=entorno;j++){
				for(int k=-entorno;k<=entorno;k++){
					if((ganadorai+j)>=0 && (ganadorai+j)<ancho && (ganadoraj+k)>=0 && (ganadoraj+k)<ancho){
						//actualizamos los pesos
						for(int l=0;l<cantEntradas;l++){
							pesos[ganadorai+j][ganadoraj+k][l]+= eta*(patrones[i][l]-pesos[ganadorai+j][ganadoraj+k][l]);
							
						}
					}//fin if
							
				}
			}

		}

		pesosPrueba.push_back(pesos);			
			
	}
}

void som2d::etiquetar(vector<vector<double> > patrones){
	int cantPatrones = patrones.size();
	int ganadorai,ganadoraj;
	double distmin,distaux,suma;
	for(int i=0;i<cantPatrones;i++){
		ganadorai=0;
		ganadoraj=0;
		distmin=9999;
		distaux=0;
		//calculamos la ganadora
		for (int j=0;j<ancho;j++){
			for(int k=0;k<ancho;k++){
				distaux=distancia(patrones[i],pesos[j][k]);
				if(distaux<distmin){
					distmin=distaux;
					ganadorai=j;
					ganadoraj=k;
				}
			}
		}
		int clase=patrones[i][cantEntradas];
		contadorClase[ganadorai][ganadoraj][clase]++;
	}
	//etiquetamos
	int maxaux=0;
	int clasemax;
	for(int i=0;i<ancho;i++){
		for(int j=0;j<ancho;j++){
			for(int k=0;k<cantClases;k++){
				if(contadorClase[i][j][k]>maxaux){
					maxaux=contadorClase[i][j][k];
					clasemax=k;
				}
			}
			clases[i][j]=clasemax;
			maxaux=0;
		}
	}
	
	
}
void som2d::probar(vector<vector<double> > patrones,vector<int> & salida){
	int cantPatrones = patrones.size();
	int ganadorai,ganadoraj;
	double distmin,distaux,suma;
	for(int i=0;i<cantPatrones;i++){
		ganadorai=0;
		ganadoraj=0;
		distmin=9999;
		distaux=0;
		//calculamos la ganadora
		for (int j=0;j<ancho;j++){
			for(int k=0;k<ancho;k++){
				distaux=distancia(patrones[i],pesos[j][k]);
				if(distaux<distmin){
					distmin=distaux;
					ganadorai=j;
					ganadoraj=k;
				}
			}
		}
		salida.push_back(clases[ganadorai][ganadoraj]);
	}
}
int som2d::cantidadParametros(){
	return cantNeuronas*cantEntradas;
}
