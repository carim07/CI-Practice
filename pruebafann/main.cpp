#include<iostream>
#include "fann.h"
#include "floatfann.h"
#include <stdlib.h>
#include <algorithm> 
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include <cmath>
#include <ctime>
#include <time.h>
#include <vector>

using namespace std;
//-------------- Las dos funciones modificadas, puse comentarios asi podes copiar y pegar nomas
//--------------------------------------------------------------------------------------------------------------------------------------
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

//--------------------------------------------------------------------------------------------------------------------------------------
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
		//Primera linea del archivo de entrenamiento
		vector<int> linea1(3,1);
		linea1[0]=cantEntrenamiento; // cantidad de patrones de entrenamiento
		linea1[1]=127; // cantidad de entradas
		linea1[2]=7; // cantidad de salidas
		for(j=0;j<linea1.size()-1;j++)
			fprintf(salidaE,"%i ",linea1[j]);
		fprintf(salidaE,"%i\n",linea1[j]);
		for(int k=0;k<patronesEntrenamiento.size();k++){
			for(j=0;j<patronesEntrenamiento[k].size()-1;j++)
				fprintf(salidaE,"%f ",patronesEntrenamiento[k][j]);
			fprintf(salidaE,"%f\n",patronesEntrenamiento[k][j]);
		}
		fclose(salidaE);
		
		// patrones de prueba
		FILE *salidaP = fopen(nomPrueba.c_str(),"w");
		//Primera linea del archivo de prueba
		linea1[0]=cantPatrones-cantEntrenamiento; // cantidad de patrones de prueba
		linea1[1]=127; // cantidad de entradas
		linea1[2]=7; // cantidad de salidas
		for(j=0;j<linea1.size()-1;j++)
			fprintf(salidaE,"%i ",linea1[j]);
		fprintf(salidaE,"%i\n",linea1[j]);
		for(int k=0;k<patronesPrueba.size();k++){
			for(j=0;j<patronesPrueba[k].size()-1;j++)
				fprintf(salidaP,"%f ",patronesPrueba[k][j]);
			fprintf(salidaP,"%f\n",patronesPrueba[k][j]);
		}
		fclose(salidaP);
		
	}
	return nombres;
}
//--------------------------------------------------------------------------------------------------------------------------------------
unsigned int pos_max(fann_type *calc_out){
	unsigned int size_calc_out = 7;
	double max=-2.0;
	unsigned int pos_m;
	for(int i=0;i<size_calc_out;i++){
		if(calc_out[i]>max){
			max = calc_out[i];
			pos_m = i;
		}
	}
	return pos_m;
}
//--------------------------------------------------------------------------------------------------------------------------------------
unsigned int pos_max(double *desired_output){
	unsigned int size_desired_output = 7;
	double max=-2.0;
	unsigned int pos_m;
	for(int i=0;i<size_desired_output;i++){
		if(desired_output[i]>max){
			max = desired_output[i];
			pos_m = i;
		}
	}
	return pos_m;
}
//--------------------------------------------------------------------------------------------------------------------------------------

int main (int argc, char *argv[]) {
	
	
	
	const unsigned int num_input = 127; // entradas
	const unsigned int num_output = 7; // salidas
	const unsigned int num_layers = 3; // cantidad de capas
	const unsigned int num_neurons_hidden = 10; // neuronas capa oculta
	const float desired_error = (const float) 0.0001; 
	const unsigned int max_epochs = 100000; // cien mil epocas maximo
	const unsigned int epochs_between_reports = 1000;
	fann_type bit_fail_limit = 0.1; // si da 0.9 y tiene que dar 1 no da error
	unsigned int bit_fail; // neuronas con error en cada particion de prueba
	unsigned int bit_fail_accum=0; // para acumular los errores en todas las particiones y promediar
	float avg_bit_fail; // promedio de bit_fails en todas las particiones
	float MSE; // MSE en cara particion de prueba
	float avg_MSE; // promedio de MSE en todas las particiones
	float MSE_accum = 0.0; // para acumular los errores en todas las particiones y promediar
	
	const unsigned int num_part = 5; // numero de particiones
	
	bool stop_bit_fail = false; // detener o no el entrenamiento por numero de bits de error
	
	vector<string> rutas= generarParticiones("datos.dat",num_part,0.8);
	
	struct fann *ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);
	
	// funciones de activacion sigmoideas simetricas (-1, 1)
	fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC); 
	fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);
	
	// seteamos el bit fail limit = limite de error por neurona
	fann_set_bit_fail_limit(ann, bit_fail_limit);
	
	if(stop_bit_fail){
		fann_set_train_stop_function(ann,FANN_STOPFUNC_BIT); 
		const float desired_error = (const float) 33; // aproximadamente 1 % de error
		// lo anterior esta hardcodeado, viene de (cantPatrones de datos*num neur salida)x(80% entrenamiento)x(>5% error permitido) 
	}
	else{
		fann_set_train_stop_function(ann,FANN_STOPFUNC_MSE);
		const float desired_error = (const float) 0.001; //error cuadratico medio deseado
	}
	
	//Hacemos la validacion cruzada
	
	
	int index = 0; // indice para recorrer archivos de entrenamiento y prueba
	
	for (int i=0;i<num_part;i++){
		
		// entrenamiento
		fann_train_on_file(ann, rutas[index].c_str(), max_epochs, epochs_between_reports, desired_error);
		
		index+=1;
		
		//prueba
		struct fann_train_data *data = fann_read_train_from_file(rutas[index].c_str());
		
		fann_reset_MSE(ann);
		fann_test_data(ann, data);
		
		
		
		MSE = fann_get_MSE(ann);
		printf("Mean Square Error: %f\n", MSE); // error cuadratico medio en esta prueba
		MSE_accum += MSE; // acumulamos el error cuadratico medio
		// ver varianza
		
		bit_fail = fann_get_bit_fail(ann); 
		bit_fail_accum += bit_fail; // acumulamos la cant de neuronas con errores en la salida 
		
		fann_destroy_train(data);
	}
	
	avg_MSE = MSE_accum/num_part;
	avg_bit_fail = (float) bit_fail_accum/num_part;
	
	printf("AVG Mean Square Error: %f\n", avg_MSE);
	printf("AVG Bit Fails: %f\n", avg_bit_fail);

	fann_save(ann, "Japanese_Database.net");
	
	enum expression {ANGRY, DISGUST, FEAR, HAPPY, NEUTRAL, SAD, SURPRISE};
	
	//struct fann *ann = fann_create_from_file("Japanese_Database.net");
	
	// Contruccion de la matriz de confusion
	
	// confusion matrix => rows:desired output, columns: calculated output
	unsigned int confusion_matrix[num_output][num_output] = {0};
	unsigned int d_o;
	unsigned int c_o;
	
	for (int k=1;k<num_part*2;k+=2){
		
		vector<vector<double> > test_data = parsearCSV(rutas[k]);
		cout<<rutas[k].c_str()<<endl;
		
		const unsigned int size_test_data = test_data.size();
		const unsigned int num_test_data = (size_test_data-1)/2;
			
		fann_type *calc_out;
		fann_type input[num_test_data][num_input];
		
		
		double desired_output[num_test_data][num_output];
		
		
		for (int i=0;i<num_test_data;i++){
			for (int j=0;j<num_input;j++)
				input[i][j]=test_data[i*2+1][j];
			for (int j=0;j<num_output;j++)
				desired_output[i][j]=test_data[i*2+2][j];
		}
		
		for(int i=0;i<num_test_data;i++){
			
			calc_out = fann_run(ann, input[i]);
			
			d_o = pos_max(desired_output[i]);
			c_o = pos_max(calc_out);
			
			confusion_matrix[d_o][c_o]++;
		}
	}
	
	
	
	for(int i=0;i<num_output;i++){
		for(int j=0;j<num_output-1;j++)
			printf("%i,",confusion_matrix[i][j]);
		printf("%i\n",confusion_matrix[i][num_output-1]);
	}
	
	
	fann_destroy(ann);
	
	return 0;
}

