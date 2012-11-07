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
		
//		//Primera linea del archivo de entrenamiento
//		vector<double> linea1(3,1);
//		linea1[0]=cantEntrenamiento; // cantidad de patrones de entrenamiento
//		linea1[1]=127; // cantidad de entradas
//		linea1[2]=7; // cantidad de salidas
//		patronesEntrenamiento.push_back(linea1);
		for (int j=0;j<cantEntrenamiento;j++){
			patronesEntrenamiento.push_back(patrones[indicesAleatorios[j]*2]);
			patronesEntrenamiento.push_back(patrones[indicesAleatorios[j]*2+1]);
		}
		
//		//Primera linea del archivo de prueba
//		linea1[0]=cantPatrones-cantEntrenamiento; // cantidad de patrones de prueba
//		linea1[1]=127; // cantidad de entradas
//		linea1[2]=7; // cantidad de salidas
//		patronesPrueba.push_back(linea1);
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
		//Primera linea del archivo de entrenamiento
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



int main (int argc, char *argv[]) {
	
	vector<string> rutas= generarParticiones("datosMerged.dat",1,0.8);
	
	const unsigned int num_input = 127; // entradas
	const unsigned int num_output = 7; // salidas
	const unsigned int num_layers = 3; // cantidad de capas
	const unsigned int num_neurons_hidden = 10; // neuronas capa oculta
	const float desired_error = (const float) 0.00001; 
	const unsigned int max_epochs = 500000;
	const unsigned int epochs_between_reports = 1000;
	fann_type bit_fail_limit = 0.05;
	
	struct fann *ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);
		
	fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
	fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);
	fann_set_bit_fail_limit(ann, bit_fail_limit);
	fann_train_on_file(ann, rutas[0].c_str(), max_epochs, epochs_between_reports, desired_error);
	
	fann_save(ann, "datosMerged_float.net");
	
	//Test
	
	struct fann_train_data *data = fann_read_train_from_file(rutas[1].c_str());
	fann_reset_MSE(ann);
	fann_test_data(ann, data);
	printf("Mean Square Error: %f\n", fann_get_MSE(ann));
	fann_destroy_train(data);
	
	//Prueba de un patron
	vector<vector<double> > patronesPrueba=parsearCSV("prueba0");
	//struct fann *ann = fann_create_from_file("datos_float.net");

	fann_type *calc_out;
	fann_type input[127];
	
	for (int i=0;i<num_input;i++)
			input[i]=patronesPrueba[37][i];
			
	calc_out = fann_run(ann, input);
	
	for (int i=0;i<num_output-1;i++)
		printf("%f,",calc_out[i]);
	printf("%f\n",calc_out[num_output-1]);
	
	fann_destroy(ann);
	
//-----------------------PRUEBA---------------------------------------------------//	
//	fann_type *calc_out;
//	fann_type input[2];
//	
//	struct fann *ann = fann_create_from_file("xor_float.net");
//	
//	input[0] = -1.2000112;
//	input[1] = -0.98876;
//	calc_out = fann_run(ann, input);
//	
//	printf("xor test (%f,%f) -> %f\n", input[0], input[1], calc_out[0]);
//	
//	fann_destroy(ann);
	
	return 0;
}

