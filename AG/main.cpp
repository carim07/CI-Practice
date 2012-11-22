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

struct individuo{
	int *cromosoma;
	float fitness;
};

bool cmp_individuos(const individuo& x,const individuo& y){
	return x.fitness>y.fitness;
}
// bool operator< (individuo x, individuo y){ return (x.fitness)<(y.fitness); }

double f(int i, float x, float y=0){
	float z;
	if (i==0) z=-x*sin(sqrt(abs(x))); 
	if (i==1) z=x+5*sin(3*x)+8*cos(5*x);
	if (i==2) z=pow((x*x+y*y),0.25)*(sin(50*pow((x*x+y*y),0.1)+1)*sin(50*pow((x*x+y*y),0.1)+1));
	
	return z; 
}

float binario2real(int *b,int n, int e){
	float real=0, decimal=0;
	int	d=n-e-1; //cantidad de cifras parte decimal
	
	// parte entera
	for (int i=0;i<e;i++){
		real+=b[i+1]*pow((float)2,(float)(e-1-i));
	}
	
	// parte decimal
	for (int i=0;i<d;i++){
		decimal+=b[e+1+i]*pow((float)2,(float)(d-1-i));
	}
	decimal/=pow(2,d);
	real+=(decimal);
	
	// signo
	if (b[0]==1) real*=-1;
	return real;
}

void real2binario(float f, int *b, int n, int e){
	// signo
	if(f<0){
		b[0]=1;
		//f*=-1;
	}
	// parte entera
	int cociente = (int)f;
	for (int i=0;i<e;i++){
		//cout<<cociente%2<<endl;
		b[e-i]=cociente%2;
		cociente/=2;
	}
	// parte real
	int	d=n-e-1;
	float decimal = f-(int)f;
	int resto;
	for (int i=0;i<d;i++){
		//cout<<decimal<<endl;
		decimal*=2;
		resto=(int)decimal;
		//cout<<resto<<" "<<i<<endl;
		b[e+1+i]=resto;
		decimal=decimal-resto;
		
	}
}

int guardaArchivo(string nomArchivoSalida,vector<float> v,unsigned int it)
{	//Guardamos el vector con los patrones aleatorios en formato csv.
	FILE *salida = fopen(nomArchivoSalida.c_str(),"w");
	if(salida==NULL) return -1;
	for(int i=0;i<it;i++)
		fprintf(salida,"%f\n",v[i]);	
	
	fclose(salida);
	return 1;
}

int main (int argc, char *argv[]) {
	
	const unsigned int func = 1;
	const float aptitud_requerida[3] = {415,9,-0.1};
	
	const unsigned int num_individuos = 100; // size poblacion
	const unsigned int num_genes = 30; // size cromosoma
	const float porc_brecha = 0.1;
	const unsigned int num_progenitores = num_individuos*porc_brecha; // numero de padres
	const unsigned int e = 9; // parte entera del cromosoma
	
	const float prob_cruza = 0.9;
	const float prob_muta = 0.1;
	
	const int rango[3][2] = {{-512,512},{0,20},{-100,100}};
	
	bool elitismo = true;
	bool brecha = true;
	
	float fenotipo;
	
	individuo x;
	vector<individuo> poblacion(num_individuos,x);
	//poblacion.reserve(num_individuos);
	vector<individuo> nueva_generacion(num_individuos,x);
	//nueva_generacion.reserve(num_individuos);
	int progenitores[num_progenitores];
	
	// Indices
	unsigned int i,j,k;
	
	// Inicializamos poblacion aleatoriamente
	
//	x.cromosoma = new int[num_genes];
//	for(i=0;i<num_individuos;i++){
//		for(j=0;j<num_genes;j++)
//			x.cromosoma[j]=rand()%2; // binario aleatorio
//		poblacion[i]=x;
//	}
	
	// Creamos un individuo en el rango [a,b] y lo codificamos con un cromosoma en el rango [0,2^e]
	int cromosoma[num_genes]={0};
	for (i=0;i<num_individuos;i++){
		fenotipo = rango[func][0] + (float)rand()/RAND_MAX*(rango[func][1]-rango[func][0]+1);
		//cout<<fenotipo<<endl;
		fenotipo/=rango[func][1];
		//cout<<fenotipo<<endl;
		fenotipo*=pow((float)2,(float)(e));
		//cout<<fenotipo<<endl;
		real2binario(fenotipo,cromosoma,num_genes,e);
		x.cromosoma=cromosoma;
		poblacion[i]=x;
	}
	
	// Evaluamos el fitness de la poblacion
	// previa conversion del rango [0,2^e] a [a,b]
	for(i=0;i<num_individuos;i++)
		poblacion[i].fitness = -f(func,(binario2real(poblacion[i].cromosoma,num_genes,e)/pow((float)2,(float)(e))*rango[func][1]));
	
	// Ordenamos por fitness descendente
	
	sort(poblacion.begin(),poblacion.begin()+num_individuos,cmp_individuos);
	
	// Loop de Evolucion
	
	float max_fitness = poblacion[0].fitness;
	const unsigned int maxit = 100;
	
	// vectores para guardar valores de fitness y graficar
	
	vector<float> max_fit(maxit,-300);
	//max_fit.reserve(maxit);
	vector<float> prom_fit(maxit,-300);
	//prom_fit.reserve(maxit);
	
	i=0;
	
	while(max_fitness<aptitud_requerida[func] && i<maxit){
		
		// Elegimos los padres por metodo de ventana
		// guardamos vector de indices de padres
		
		// Contador de numero de nuevos individuos
		unsigned int cont_nueva_gen = 0;
		
		unsigned int size_ventana = num_individuos;
		
		for(j=0;j<num_progenitores;j++){
			progenitores[j]=rand()%size_ventana;
			size_ventana *= 0.9;
		}
		
			
		// si hay brecha generacional, los padres viven en la nueva generacion
		if(brecha){
			for(j=cont_nueva_gen;j<num_progenitores;j++){
				nueva_generacion[j]=poblacion[progenitores[j]];
			}
			cont_nueva_gen += num_progenitores; 
		}
		
		// si hay elitismo, el mejor vive
		if(elitismo){
			nueva_generacion[cont_nueva_gen]=poblacion[0];
			cont_nueva_gen += 1;
		}
		
		// Cruza
		unsigned int num_hijos = num_individuos - cont_nueva_gen;
		unsigned int punto_corte = num_genes - (e+1);
		
		for(j=0;j<num_hijos/2;j++){
			
			float cruza = (float)rand()/RAND_MAX;
			int padre1 = rand()%num_progenitores;
			int padre2 = rand()%num_progenitores;
			
			if(cruza<prob_cruza){
				individuo hijo1;
				hijo1.cromosoma = new int[num_genes];
				individuo hijo2;
				hijo2.cromosoma = new int[num_genes];
				
				for(k=0;k<punto_corte;k++){
					hijo1.cromosoma[k]=poblacion[progenitores[padre1]].cromosoma[k];
					hijo2.cromosoma[k]=poblacion[progenitores[padre2]].cromosoma[k];
				}
				for(k=punto_corte;k<num_genes;k++){
					hijo1.cromosoma[k]=poblacion[progenitores[padre2]].cromosoma[k];
					hijo2.cromosoma[k]=poblacion[progenitores[padre1]].cromosoma[k];
				}
				
				nueva_generacion[cont_nueva_gen]=hijo1;
				cont_nueva_gen +=1;
				nueva_generacion[cont_nueva_gen]=hijo2;
				cont_nueva_gen +=1;
			}
			else{
				nueva_generacion[cont_nueva_gen]=poblacion[progenitores[padre1]];
				cont_nueva_gen +=1;
				nueva_generacion[cont_nueva_gen]=poblacion[progenitores[padre2]];
				cont_nueva_gen +=1;
			}
		}
		
		// si falto agregar un hijo (num_hijos impar) agrego un hijo
		if(num_hijos%2==1){
			float cruza = (float)rand()/RAND_MAX;
			int padre1 = rand()%num_progenitores;
			int padre2 = rand()%num_progenitores;
			
			if(cruza<prob_cruza){
				individuo hijo1;
				hijo1.cromosoma = new int[num_genes];
							
				for(k=0;k<punto_corte;k++){
					hijo1.cromosoma[k]=poblacion[progenitores[padre1]].cromosoma[k];
				}
				for(k=punto_corte;k<num_genes;k++){
					hijo1.cromosoma[k]=poblacion[progenitores[padre2]].cromosoma[k];
				}
				
				nueva_generacion[cont_nueva_gen]=hijo1;
				cont_nueva_gen +=1;
			}
			else{
				nueva_generacion[cont_nueva_gen]=poblacion[progenitores[padre1]];
				cont_nueva_gen +=1;
			}
		}
		
		// Mutacion
		if(elitismo)
			j=1;
		else
			j=0;
		
		
		for(j;j<num_individuos;j++){
			
			float muta = (float)rand()/RAND_MAX;
			//cout<<muta<<endl;
			if(muta<prob_muta){
				cout<<"muto! "<<"en "<<i<<endl;
				
				int gen = rand()%num_genes; 
				
				// modificamos operador para no generar individuo invalido
				if(func==1 && gen==0) 
					gen = gen%(num_genes-1)+1; 
														
				if(nueva_generacion[j].cromosoma[gen]==0)
					nueva_generacion[j].cromosoma[gen]=1;
				else
					nueva_generacion[j].cromosoma[gen]=0;
				
			}
		}
		
		poblacion = nueva_generacion;
		
		// Nace la poblacion (se decodifica y evaluamos su fitness)
		
		for(j=0;j<num_individuos;j++)
			poblacion[j].fitness = -f(func,binario2real(poblacion[j].cromosoma,num_genes,e)/pow((float)2,(float)(e))*rango[func][1]);
		
		// Ordenamos por fitness descendente
		
		sort(poblacion.begin(),poblacion.begin()+num_individuos,cmp_individuos);
		
		max_fitness = poblacion[0].fitness;
		
		// Guardamos fitness max, min y promedio para graficar
		max_fit[i] = max_fitness;
		prom_fit[i] = 0;
		
		for(j=0;j<num_individuos;j++){
			prom_fit[i] += poblacion[j].fitness;
		}
		
		prom_fit[i] /= num_individuos;
		
		i++;
		
		
		//cout<<"La mejor solucion obtenida es: "<<binario2real(poblacion[0].cromosoma,e)<<" Fitness: "<<poblacion[0].fitness<<"Iteraciones: "<<i<<endl;
		
		
	}
	cout<<"La mejor solucion obtenida es: "<<binario2real(poblacion[0].cromosoma,num_genes,e)/pow((float)2,(float)(e))*rango[func][1]<<" Fitness: "<<poblacion[0].fitness<<"Iteraciones: "<<i<<endl;
	guardaArchivo("fitness_max",max_fit,i);
	guardaArchivo("fitness_prom",prom_fit,i);
	
	
	/*
	int a[10]={0, 1, 1, 0, 0, 1, 0, 1, 0, 1};
	int e=4;
	float f=binario2real(a,10,e);
	cout<<f<<endl;
	int b[10]={0};
	real2binario(f,b,10,e);
	cout<<"binario"<<endl;
	for(int i=0;i<10;i++){
		cout<<b[i]<<" ";
	}
	cout<<endl;
	
	*/
	
	
	return 0;
}

