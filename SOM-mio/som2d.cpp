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




som2d::som2d(int cNeuronas,int cantEntradas) {
	cantNeuronas=cNeuronas;
	ancho=sqrt(cantNeuronas);
	this->cantEntradas=cantEntradas;
	//inicializamos pesos aleatorios
	vector<double> aux1;	//pesos por neurona
	
	for(int i=0;i<cantNeuronas;i++){		
		for(int j=0;j<cantEntradas;j++)
			aux1.push_back(-0.5+(float)rand()/RAND_MAX);
		pesos.push_back(aux1);
		aux1.clear();
		}
	
}

som2d::~som2d() {
	
}


double som2d::distancia(vector<double> v1, vector<double> v2){
	double distancia=0;
	for(int i=0;i<v1.size();i++)
		distancia+=(v2[i] - v1[i])*(v2[i] - v1[i]);
	return sqrt(distancia);
}
vector<double> som2d::actualiza(vector<double> v1, vector<double> v2, double mu){
	vector <double> v;
	for(int i=0;i<v1.size();i++)
		v.push_back(v2[i]+mu*(v1[i]-v2[i]));
	return v;
}

void som2d::entrenar(vector<vector<double> > patrones, vector<vector<vector<double> > > & pesosPrueba){
	
	int cantPatrones=patrones.size();
	double distaux,distmin,mu=0.5;
	int ng=0; //ganadora
	int epocas=2000;
	int e=0;
	
	while(e<epocas){
		if(e>200&e<1900)mu-=(0.4/1700);
		if(e>1900)mu=0.1;
		
		//if(e>=7000) mu=0.1;
		//Recorremos los patrones
		for(int i=0;i<cantPatrones;i++){
			distmin=9999999;
			//Obtenemos la neurona ganadora
			for(int j=0;j<cantNeuronas;j++){
				distaux=distancia(patrones[i],pesos[j]);
				if(distaux<distmin){
					distmin=distaux;
					ng=j;
				}
			}// for ganadora
			
			//Actualizamos los pesos
			pesos[ng]=actualiza(patrones[i],pesos[ng],mu);
			
		}
		//Guardamos pesos por epocas para graficar
		if(e%100==0)pesosPrueba.push_back(pesos);
		e++;
	}
	pesosPrueba.push_back(pesos);
}
