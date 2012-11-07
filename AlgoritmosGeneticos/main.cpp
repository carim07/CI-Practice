#include <stdlib.h>
#include <algorithm> 
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include <cmath>
#include <ctime>
#include <vector>
#include <time.h>
using namespace std;

float binario2real(vector<int> b,int e){
	float r=0,
		decimal=0;
	int entero=0,
		d=b.size()-e-1; //cantidad de cifras parte decimal
	// e = cifras parte entera
	for (int i=1;i<=e;i++){
		entero+=b[i]*pow((float)2,(float)(i-1));
	}
	decimal=0;
	for (int i=e+1;i<b.size();i++){
		decimal+=b[i]*pow((float)2,(float)(i-(e+1)));
	}
	r=entero+decimal/pow((float)2,d);
	if (b[0]==1) r*=-1;
	return r;
}

double f(int i, float x, float y=0){
	float z;
	if (i==1) z=-x*sin(sqrt(abs(x))); 
	if (i==2) z=x+5*sin(3*x)+8*cos(5*x);
	if (i==3) z=pow((x*x+y*y),0.25)*(sin(50*pow((x*x+y*y),0.1)+1)*sin(50*pow((x*x+y*y),0.1)+1));
	
	return z; 
}

void ordenar(vector<double> &fitness, vector<vector<int> > &poblacion){
	double aux;
	vector<int> aux2;
	
	for (int i=0;i<fitness.size();i++){
		double max=fitness[i];		
		for(int j=i;j<fitness.size();j++){
			if(fitness[j]>max){
				max=fitness[j];
				//intercambio fitness
				aux=fitness[i];
				fitness[i]=fitness[j];
				fitness[j]=aux;
				//intercambio poblacion
				aux2=poblacion[i];
				poblacion[i]=poblacion[j];
				poblacion[j]=aux2;
			}
		}
	}
}




void funcion1(){
	
	int n=40, // tamanio de la poblacion
		nP=4, // cantidad de progenitores
		nC=30, // tamanio cromosoma
		e=9, // cifras parte entera cromosoma
		f1=1; // funcion ejercicio 1
	float probCruza=0.9,
		probMutacion=0.1;
	bool elitismo=true;
	vector<vector<int> > poblacion;
	vector<double> fitness;
	
	//Inicializamos la poblacion aleatoriamente
	vector<int> aux;
	for(int i=0;i<n;i++){
		for(int j=0;j<nC;j++){
			//srand((unsigned)(time(NULL)));
			aux.push_back(rand()%2); // binario aleatorio
			//cout<<aux[j]<<" ";
		}
		//cout<<endl;
		poblacion.push_back(aux);
		aux.clear();
	}
	
	// Calculamos el fitness para la poblacion
	for(int i=0;i<n;i++){
		fitness.push_back(-f(f1,binario2real(poblacion[i],e)));
	}
	// Ordenamos la poblacion de mayor a menor segun su fitness
	ordenar(fitness,poblacion);
	
	int it=0,
		maxit=100;
	
	while(fitness[0]<418 && it<maxit){
		//Usamos metodo de ventanas para la seleccion
		//Los padres son parte de la nueva generacion
		vector<vector<int> > nuevaPoblacion;
		int tVentana=n; // tamanio de la ventana
		for(int i=0;i<nP;i++){
			int indice=rand()%tVentana;
			nuevaPoblacion.push_back(poblacion[indice]);
			tVentana*=0.9;
		}
		
		//Hacemos la cruza
		int corte=nC-(e+1); 
		int nHijos=n-nP; 
		vector<int> hijo1,
			hijo2;
		//Elitismo
		if(elitismo){
			nuevaPoblacion.push_back(poblacion[0]); // agregamos el mejor
			nuevaPoblacion.push_back(poblacion[1]);
			nHijos-=2;
		}
		
		for(int i=0;i<(nHijos/2);i++){
			
			float cruza=(float)rand()/RAND_MAX;
			int padre1=rand()%nP; 
			int	padre2=rand()%nP; 
			
			if(cruza<probCruza){
				for(int j=0;j<corte;j++){
					hijo1.push_back(nuevaPoblacion[padre1][j]);
					hijo2.push_back(nuevaPoblacion[padre2][j]);
				}
				for(int j=corte;j<nC;j++){
					hijo1.push_back(nuevaPoblacion[padre2][j]);
					hijo2.push_back(nuevaPoblacion[padre1][j]);
				}
				nuevaPoblacion.push_back(hijo1);
				nuevaPoblacion.push_back(hijo2);
				hijo1.clear();
				hijo2.clear();
			}
			else {
				nuevaPoblacion.push_back(nuevaPoblacion[padre1]);
				nuevaPoblacion.push_back(nuevaPoblacion[padre2]);
			}
		} // fin nueva Poblacion (cruza)
		
		//Hacemos la mutacion
		for(int i=0;i<n;i++){
			
			float muta=(float)rand()/RAND_MAX;
			if(muta<probMutacion){
				cout<<"Muto!"<<endl;
				
				int gen=rand()%nC;
				if(nuevaPoblacion[i][gen]==0) nuevaPoblacion[i][gen]=1;
				else nuevaPoblacion[i][gen]=0;
			}
		}
		
		poblacion=nuevaPoblacion;
		nuevaPoblacion.clear();
		it++;
		
		//fitness.clear();
		//Evaluamos la poblacion
		
		for(int i=0;i<n;i++){
			fitness[i]=-f(f1,binario2real(poblacion[i],e));
		}
		ordenar(fitness,poblacion);
		cout<<"La mejor solucion obtenida es: "<<binario2real(poblacion[0],e)<<" Fitness: "<<fitness[0]<<"Iteraciones: "<<it<<endl;
		
	} 
	cout<<"La mejor solucion obtenida es: "<<binario2real(poblacion[0],e)<<" Fitness: "<<fitness[0]<<"Iteraciones: "<<it<<endl;
}

void funcion2(){
	
	int n=100, // tamanio de la poblacion
		nP=20, // cantidad de progenitores
		nC=10, // tamanio cromosoma
		e=5, // cifras parte entera cromosoma
		f1=2; // funcion ejercicio 1
	float probCruza=0.9,
		probMutacion=0.1;
	bool elitismo=true;
	vector<vector<int> > poblacion;
	vector<double> fitness;
	
	//Inicializamos la poblacion aleatoriamente
	vector<int> aux;
	for(int i=0;i<n;i++){
		aux.push_back(0); // queremos un num positivo
		for(int j=1;j<nC;j++)
			aux.push_back(rand()%2); // binario aleatorio
		
		// Chequeamos si esta entre 0 y 20
		if(aux[e]==1){
			if(aux[e-1]==1){
				i-=1; // tenemos que generar otro individuo
				aux.clear();
				if(i<0) i=0; // por si las moscas
			}
		}
		else{
			poblacion.push_back(aux);
			aux.clear();
		}
		
	}
	
	
	
	
	
	// Calculamos el fitness para la poblacion
//	for(int i=0;i<n;i++){
//		fitness.push_back(-f(f1,x));
//	}
	// Ordenamos la poblacion de mayor a menor segun su fitness
	ordenar(fitness,poblacion);
	
	int it=0,
		maxit=50;
	
	while(fitness[0]<8.5 && it<maxit){
		//Usamos metodo de ventanas para la seleccion
		//Los padres son parte de la nueva generacion
		vector<vector<int> > nuevaPoblacion;
		int tVentana=n; // tamanio de la ventana
		for(int i=0;i<nP;i++){
			int indice=rand()%tVentana;
			nuevaPoblacion.push_back(poblacion[indice]);
			tVentana*=0.9;
		}
		
		//Hacemos la cruza
		int corte=nC-(e+1); 
		int nHijos=n-nP; 
		vector<int> hijo1,
			hijo2;
		//Elitismo
		if(elitismo){
			nuevaPoblacion.push_back(poblacion[0]); // agregamos el mejor
			nuevaPoblacion.push_back(poblacion[1]);
			nHijos-=2;
		}
		
		for(int i=0;i<(nHijos/2);i++){
			
			float cruza=(float)rand()/RAND_MAX;
			int padre1=rand()%nP; 
			int	padre2=rand()%nP; 
			
			if(cruza<probCruza){
				for(int j=0;j<corte;j++){
					hijo1.push_back(nuevaPoblacion[padre1][j]);
					hijo2.push_back(nuevaPoblacion[padre2][j]);
				}
				for(int j=corte;j<nC;j++){
					hijo1.push_back(nuevaPoblacion[padre2][j]);
					hijo2.push_back(nuevaPoblacion[padre1][j]);
				}
				nuevaPoblacion.push_back(hijo1);
				nuevaPoblacion.push_back(hijo2);
				hijo1.clear();
				hijo2.clear();
			}
			else {
				nuevaPoblacion.push_back(nuevaPoblacion[padre1]);
				nuevaPoblacion.push_back(nuevaPoblacion[padre2]);
			}
		} // fin nueva Poblacion (cruza)
		
		//Hacemos la mutacion
		for(int i=0;i<n;i++){
			
			float muta=(float)rand()/RAND_MAX;
			if(muta<probMutacion){
				cout<<"Muto!"<<endl;
				
				int gen=rand()%nC;
				if(nuevaPoblacion[i][gen]==0) nuevaPoblacion[i][gen]=1;
				else nuevaPoblacion[i][gen]=0;
			}
		}
		
		poblacion=nuevaPoblacion;
		nuevaPoblacion.clear();
		it++;
		
		//fitness.clear();
		//Evaluamos la poblacion
		
		for(int i=0;i<n;i++){
			float x=binario2real(poblacion[i],e);
			if(x<0||x>20)
				fitness[i]=0.0;
			else
				fitness[i]=-f(f1,x);
		}
		ordenar(fitness,poblacion);
		cout<<"La mejor solucion obtenida es: "<<binario2real(poblacion[0],e)<<" Fitness: "<<fitness[0]<<"Iteraciones: "<<it<<endl;
		
	} 
	cout<<"La mejor solucion obtenida es: "<<binario2real(poblacion[0],e)<<" Fitness: "<<fitness[0]<<"Iteraciones: "<<it<<endl;
}

int main (int argc, char *argv[]) {
	
	//funcion2();
//	vector<int> b(10,1);
//	b[9]=0;
//	b[8]=0;
//	b[7]=0;
//	b[6]=0;
//	b[5]=0;
//	b[4]=0;
//	b[3]=0;
//	b[2]=0;
//	b[1]=0;
//	b[0]=0;
//	float f=binario2real(b,5);
//	cout<<f<<endl;
	
	//Inicializamos la poblacion aleatoriamente
	int contador=0;
	vector<int> aux;
	vector<vector<int> > poblacion;
	int n=10,nC=10,e=5;
	int i=0;
	while(i<n){
		i++;
		aux.clear();
		aux.push_back(0); // queremos un num positivo
		for(int j=1;j<nC;j++){
			aux.push_back(rand()%2); // binario aleatorio
		}
		cout<<aux.size()<<endl;
		// Chequeamos si esta entre 0 y 20
		if(valido(aux))
			poblacion.push_back(aux);
		else{
			i-=1; // tenemos que generar otro individuo
			if(i<0) i=0; // por si las moscas
			contador++;
		}
	}
	
	for(int i=0;i<n;i++)
	cout<<"poblacion ["<<i<<"]= "<<binario2real(poblacion[i],5)<<endl;
	//cout<<poblacion.size()<<" "<<contador<<endl;
	return 0;
}

