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
using namespace std;

//Funcion para parsear archivo CSV a un vector de vectores de double.
vector<vector<double> > parsearCSV(string nomArchivo){
	vector<vector<double> > salida;
	ifstream archi(nomArchivo.c_str());
	string line;
	while (getline(archi,line))
	{   vector<double> vAux;
	istringstream linestream(line);
	string dato;
	while (getline(linestream, dato, ' ')){
		double a= atof(dato.c_str());
		vAux.push_back(a);
	}
	salida.push_back(vAux);
	}
	archi.close();
	return salida;
};
void desordenaVector(vector<vector<double> > v,  vector<vector<double> > &vd){
	vector<int> indicesAleatorios(v.size());
	for(int i=0;i<v.size();i++) 
		indicesAleatorios[i]=i;
	//generamos los indices aleatorios
	random_shuffle(indicesAleatorios.begin(),indicesAleatorios.end());
	for (int j=0;j<v.size();j++)
		vd.push_back(v[indicesAleatorios[j]]);
}

vector<string> generarParticiones(string nomArchivo,int cantidadParticiones,double porcentajeEntrenamiento){
	vector<vector<double> > patrones = parsearCSV(nomArchivo);
	
	int cantPatrones = patrones.size()/2; // patron = entrada + salida (2 lineas) 
	int cantEntrenamiento = (int) (porcentajeEntrenamiento*cantPatrones);
	
	vector<vector<double> > patronesEntrenamiento;
	vector<vector<double> > patronesPrueba;
	vector<string> nombres;
	
	vector<int> indicesAleatorios(cantPatrones);
	for(int i=0;i<cantPatrones;i++) 
		indicesAleatorios[i]=i;
	
	for(int i=0;i<cantidadParticiones;i++){
		//generamos los indices aleatorios
		random_shuffle(indicesAleatorios.begin(),indicesAleatorios.end());
		
		for (int j=0;j<cantEntrenamiento;j++){
			patronesEntrenamiento.push_back(patrones[indicesAleatorios[j]*2]);
			patronesEntrenamiento.push_back(patrones[indicesAleatorios[j]*2+1]);
		}
			
			
		for (int j=cantEntrenamiento;j<cantPatrones;j++){
			patronesPrueba.push_back(patrones[indicesAleatorios[j]*2]);
			patronesPrueba.push_back(patrones[indicesAleatorios[j]*2+1]);
		}
			
		
		//Guardamos el vector con los patrones aleatorios en formato csv.
		stringstream num;
		num<<i;
		
		string nomEntrenamiento = "entrenamiento"+ num.str(); 
		nombres.push_back(nomEntrenamiento);
		string nomPrueba = "prueba"+ num.str();
		nombres.push_back(nomPrueba);
		//patrones de entrenamiento
		FILE *salidaE = fopen(nomEntrenamiento.c_str(),"w");
		int j;
		for(int k=0;k<patronesEntrenamiento.size();k++){
			for(j=0;j<patronesEntrenamiento[k].size()-1;j++)
				fprintf(salidaE,"%f ",patronesEntrenamiento[k][j]);
			fprintf(salidaE,"%f\n",patronesEntrenamiento[k][j]);
		}
		fclose(salidaE);
		
		// patrones de prueba
		FILE *salidaP = fopen(nomPrueba.c_str(),"w");
		for(int k=0;k<patronesPrueba.size();k++){
			for(j=0;j<patronesPrueba[k].size()-1;j++)
				fprintf(salidaP,"%f ",patronesPrueba[k][j]);
			fprintf(salidaP,"%f\n",patronesPrueba[k][j]);
		}
		fclose(salidaP);
		
	}
	return nombres;
}


int main (int argc, char *argv[]) {
	
	//variables del MLP
	double mu = 0.1; //tasa de aprendizaje
	double alfa=0.7;
	double curvatura=5;
	int cantCapas=4;
	vector<int> estructura(4,1);
	estructura[0]=71;
	estructura[1]=71;
	estructura[2]=10;
	estructura[3]=3;
	
	
	//variables del entrenamiento	
	double tolerancia = 0.1;
	double cantEpocas = 300; //cantidad de epocas de entrenamiento
	int cantParticiones=10;
	double porcentajeEntrenamiento = 0.9;
	double errorPromedio=0.0;
	
	//archivo con la totalidad de los datos
	string nomArchivo = "concent.dat";
	
	vector<string> rutas = generarParticiones(nomArchivo,cantParticiones,porcentajeEntrenamiento);
		
//	for(int i=0;i<cantParticiones*2;i+=2){
//		vector<vector<double> > datosEntrenamiento = parsearCSV(rutas[i]);
//		vector<vector<double> > datosPrueba = parsearCSV(rutas[i+1]);
//		
//		vector<double> entradas;
//		vector<double> salidas;
//		double aciertos=0.0;
//		double porcentajeError=1;
//		int ep=0;
//		
//		PerceptronMulticapa mlp(cantCapas,estructura,mu,alfa,curvatura);
//		
//		
//		//Entrenamos la red con datos de entrenamiento
//		while(ep<cantEpocas && porcentajeError>tolerancia){
//			int nEntrenamiento=datosEntrenamiento.size() * 0.9;
//			
//			
//			for(int j=0;j<nEntrenamiento;j++){
//				
//				for(int k=0;k<estructura[0];k++)
//					entradas.push_back(datosEntrenamiento[j][k]);
//				for(int k=0;k<estructura[cantCapas-1];k++)
//					salidas.push_back(datosEntrenamiento[j][estructura[0]+k]);
//				
//				mlp.propagacionAtras(entradas,salidas);
//				entradas.clear();salidas.clear();
//			}
//			for(int j=nEntrenamiento;j<datosEntrenamiento.size();j++){
//				for(int k=0;k<estructura[0];k++)
//					entradas.push_back(datosEntrenamiento[j][k]);
//				for(int k=0;k<estructura[cantCapas-1];k++)
//					salidas.push_back(datosEntrenamiento[j][estructura[0]+k]);
//				
//				mlp.propagacionAdelante(entradas);
//				if(!mlp.error(salidas))
//					aciertos++;
//				entradas.clear();
//				salidas.clear();
//			}
//			porcentajeError=1-(aciertos/datosEntrenamiento.size());
//			cout<<"Error durante el entrenamiento: "<<porcentajeError<<endl;
//			ep++;
//			aciertos=0;			
//			
//		}//fin while por epocas
//		
//		//Probamos la red con datos de prueba
//		for(int j=0;j<datosPrueba.size();j++){
//			for(int k=0;k<estructura[0];k++)
//				entradas.push_back(datosPrueba[j][k]);
//			for(int k=0;k<estructura[cantCapas-1];k++)
//				salidas.push_back(datosPrueba[j][estructura[0]+k]);
//				
//			mlp.propagacionAdelante(entradas);
//			if(!mlp.error(salidas))
//				aciertos++;
//			entradas.clear();
//			salidas.clear();
//		}
//		porcentajeError=1-(aciertos/datosPrueba.size());
//		errorPromedio+=porcentajeError;
//		aciertos=0;
//		
//		
//	}
//	
//	cout<<"El error promedio de la red en la prueba es: "<<errorPromedio/cantParticiones<<endl;
//
//	
	return 0;
}

