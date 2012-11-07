#include<iostream>
#include <stdlib.h>
#include <algorithm> 
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include <cmath>
#include <vector>
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
	while (getline(linestream, dato, ',')){
		double a= atof(dato.c_str());
		vAux.push_back(a);
	}
	salida.push_back(vAux);
	}
	archi.close();
	return salida;
};

void guardaArchivo(string nomArchivoSalida,vector<vector<double> > v){
	FILE *salidaP = fopen(nomArchivoSalida.c_str(),"w");
	for(int k=0;k<v.size();k+=2){
		int j;
		for(j=0;j<v[k].size()-1;j++)
			fprintf(salidaP,"%f ",v[k][j]);
		fprintf(salidaP,"%f\n",v[k][j]);
		for(j=0;j<v[k+1].size()-1;j++)
			fprintf(salidaP,"%f ",v[k+1][j]);
		fprintf(salidaP,"%f\n",v[k+1][j]);
	}
	fclose(salidaP);
}

int main (int argc, char *argv[]) {
	string archivo="datosAKDEF.csv";
	int sel_archivo=3;
	
	vector<vector<double> > patrones=parsearCSV(archivo);	
	
	if (sel_archivo==2){
		int cantEntradas=4;
		
		vector<double> s1(3,-1);
		s1[0]=1;
		vector<double> s2(3,-1);
		s2[1]=1;
		vector<double> s3(3,-1);
		s3[2]=1;
		
		for(int i=0;i<patrones.size();i++){
			if(patrones[i][cantEntradas]==0){
				patrones[i].pop_back();
				patrones.insert(patrones.begin()+i+1,1,s1);
				i+=1;
			}
			else{
				if(patrones[i][cantEntradas]==1){
					patrones[i].pop_back();
					patrones.insert(patrones.begin()+i+1,1,s2);
					i+=1;
				}
				else {
					if(patrones[i][cantEntradas]==2){
						patrones[i].pop_back();
						patrones.insert(patrones.begin()+i+1,1,s3);
						i+=1;
					}
				}
			}
			
		}
		string salida="iris.dat";
		cout<<"size: "<<patrones.size()<<endl;
		guardaArchivo(salida,patrones);
	}
	if(sel_archivo==1){
		
		vector<double> s4(2,-1);
		s4[0]=1;
		vector<double> s5(2,-1);
		s5[1]=1;
		
		
		int cantEntradas=2;
		
		
		for(int i=0;i<patrones.size();i++){
			if(patrones[i][cantEntradas]==0){
				patrones[i].pop_back();
				patrones.insert(patrones.begin()+i+1,1,s4);
				i+=1;
			}
			else{
				if(patrones[i][cantEntradas]==1){
					patrones[i].pop_back();
					patrones.insert(patrones.begin()+i+1,1,s5);
					i+=1;
				}
				}
			
			
		}
		string salida="concent.dat";
		cout<<"size: "<<patrones.size()<<endl;
		guardaArchivo(salida,patrones);
	}
	if(sel_archivo==3){
		
		vector<double> s1(7,-1);
		s1[0]=1;
		vector<double> s2(7,-1);
		s2[1]=1;
		vector<double> s3(7,-1);
		s3[2]=1;
		vector<double> s4(7,-1);
		s4[3]=1;
		vector<double> s5(7,-1);
		s5[4]=1;
		vector<double> s6(7,-1);
		s6[5]=1;
		vector<double> s7(7,-1);
		s7[6]=1;		
		int cantEntradas=127;
		
		
		for(int i=0;i<patrones.size();i++){
			if(patrones[i][cantEntradas]==0){
				patrones[i].pop_back();
				patrones.insert(patrones.begin()+i+1,1,s1);
				i+=1;
			}
			else{
				if(patrones[i][cantEntradas]==1){
					patrones[i].pop_back();
					patrones.insert(patrones.begin()+i+1,1,s2);
					i+=1;
				}
				else{
					if(patrones[i][cantEntradas]==2){
						patrones[i].pop_back();
						patrones.insert(patrones.begin()+i+1,1,s3);
						i+=1;
					}
					else{
						if(patrones[i][cantEntradas]==3){
							patrones[i].pop_back();
							patrones.insert(patrones.begin()+i+1,1,s4);
							i+=1;
						}
						else{
							if(patrones[i][cantEntradas]==4){
								patrones[i].pop_back();
								patrones.insert(patrones.begin()+i+1,1,s5);
								i+=1;
							}
							else{
								if(patrones[i][cantEntradas]==5){
									patrones[i].pop_back();
									patrones.insert(patrones.begin()+i+1,1,s6);
									i+=1;
								}
								else{
									if(patrones[i][cantEntradas]==6){
										patrones[i].pop_back();
										patrones.insert(patrones.begin()+i+1,1,s7);
										i+=1;
									}
									
								}
							}
						}
						
					}
				}
			}
			
			
		}
		string salida="datosAKDEF.dat";
		cout<<"size: "<<patrones.size()<<endl;
		guardaArchivo(salida,patrones);
	}
	
		
	
	
	
	
	
	return 0;
}

