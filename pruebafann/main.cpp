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

//--------------------------------------------------------------------------------------------------------------------------------------
vector<vector<double> > parseCSV(string archive){
	vector<vector<double> > patterns;
	ifstream file(archive.c_str());
	string line;
	while (getline(file,line))
	{   vector<double> v_aux;
	istringstream linestream(line);
	string data;
	while (getline(linestream, data, ' ')){
		double a= atof(data.c_str());
		v_aux.push_back(a);
	}
	patterns.push_back(v_aux);
	}
	file.close();
	return patterns;
};

//--------------------------------------------------------------------------------------------------------------------------------------
vector<string> gen_partition(string archive,int num_partition,double train_percent){
	// Function that generates NUM_PARTITION partitions for cross-validation
	// A partition is given by a train subset and a test subset
	// The train subset contains the TRAIN_PERCENT of the total of data
	// The train subset is also divided in a estimation subset and a validation subset
	// in order to test during training to avoid overfitting and achieve good generalization
	
	vector<vector<double> > patterns = parseCSV(archive);
	
	int num_patterns = patterns.size()/2; // pattern = input + desired_output (2 lines)
	
	int num_train = (int) (num_patterns*train_percent);
	int num_estimation = (int) (num_train*0.9);
	int num_validation = num_train-num_estimation;
	int num_test = num_patterns-num_train;
	
	vector<vector<double> > estimation_subset;
	vector<vector<double> > validation_subset;
	vector<vector<double> > test_subset;
	vector<string> paths;
	
	vector<int> random_index(num_patterns);
	for(int i=0;i<num_patterns;i++)
		random_index[i]=i;
	int contador=0;
	for(int i=0;i<num_partition;i++){
		// shuffle the index vector
		random_shuffle(random_index.begin(),random_index.end());
		
		cout<<contador<<endl;
		
		for (int j=0;j<num_estimation;j++){
			estimation_subset.push_back(patterns[random_index[j]*2]);
			estimation_subset.push_back(patterns[random_index[j]*2+1]);
		}
		
		
		for (int j=num_estimation;j<num_train;j++){
			validation_subset.push_back(patterns[random_index[j]*2]);
			validation_subset.push_back(patterns[random_index[j]*2+1]);
		}
		
		for (int j=num_train;j<num_patterns;j++){
			test_subset.push_back(patterns[random_index[j]*2]);
			test_subset.push_back(patterns[random_index[j]*2+1]);
		}
		
		//save files in the appropiate format for fann library
		stringstream num;
		num<<i;
		
		string path_estimation_subset = "estimation"+ num.str();
		paths.push_back(path_estimation_subset);
		string path_validation_subset = "validation"+ num.str();
		paths.push_back(path_validation_subset);
		string path_test_subset = "test"+ num.str();
		paths.push_back(path_test_subset);
		
		//estimation file
		FILE *e_file = fopen(path_estimation_subset.c_str(),"w");
		unsigned int j;
		//first line of estimation subset file
		vector<int> line1(3,1);
		line1[0]=num_estimation; // number of patterns
		line1[1]=127; // inputs
		line1[2]=7; // outputs
		for(j=0;j<line1.size()-1;j++)
			fprintf(e_file,"%i ",line1[j]);
		fprintf(e_file,"%i\n",line1[j]);
		for(unsigned int k=0;k<estimation_subset.size();k++){
			for(j=0;j<estimation_subset[k].size()-1;j++)
				fprintf(e_file,"%f ",estimation_subset[k][j]);
			fprintf(e_file,"%f\n",estimation_subset[k][j]);
		}
		fclose(e_file);
		
		// validation file
		FILE *v_file = fopen(path_validation_subset.c_str(),"w");
		//first line of validation subset file
		line1[0]=num_validation; // number of patterns
		line1[1]=127; // inputs
		line1[2]=7; // outputs
		for(j=0;j<line1.size()-1;j++)
			fprintf(v_file,"%i ",line1[j]);
		fprintf(v_file,"%i\n",line1[j]);
		for(unsigned int k=0;k<validation_subset.size();k++){
			for(j=0;j<validation_subset[k].size()-1;j++)
				fprintf(v_file,"%f ",validation_subset[k][j]);
			fprintf(v_file,"%f\n",validation_subset[k][j]);
		}
		fclose(v_file);
		
		// test file
		FILE *t_file = fopen(path_test_subset.c_str(),"w");
		//first line of validation subset file
		line1[0]=num_test; // number of patterns
		line1[1]=127; // inputs
		line1[2]=7; // outputs
		for(j=0;j<line1.size()-1;j++)
			fprintf(t_file,"%i ",line1[j]);
		fprintf(t_file,"%i\n",line1[j]);
		for(unsigned int k=0;k<test_subset.size();k++){
			for(j=0;j<test_subset[k].size()-1;j++)
				fprintf(t_file,"%f ",test_subset[k][j]);
			fprintf(t_file,"%f\n",test_subset[k][j]);
		}
		fclose(t_file);
		contador++;
	}
	return paths;
}
//--------------------------------------------------------------------------------------------------------------------------------------
unsigned int pos_max(fann_type *calc_out){
	unsigned int size_calc_out = 7;
	double max=-2.0;
	unsigned int pos_m;
	for(unsigned int i=0;i<size_calc_out;i++){
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
	for(unsigned int i=0;i<size_desired_output;i++){
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
	float desired_error = 0.0001; 
	const unsigned int max_epochs = 100000; // cien mil epocas maximo
	const unsigned int epochs_between_reports = 1000;
	fann_type bit_fail_limit = 0.1; // si da 0.9 y tiene que dar 1 no da error
	unsigned int bit_fail; // neuronas con error en cada particion de prueba
	unsigned int bit_fail_accum=0; // para acumular los errores en todas las particiones y promediar
	float avg_bit_fail; // promedio de bit_fails en todas las particiones
	float MSE; // MSE en cara particion de prueba
	float avg_MSE; // promedio de MSE en todas las particiones
	float MSE_accum = 0.0; // para acumular los errores en todas las particiones y promediar
	
	const unsigned int num_part = 7; // numero de particiones
	
	bool stop_bit_fail = false; // detener o no el entrenamiento por numero de bits de error
	
	vector<string> paths= gen_partition("datos.dat",num_part,0.8);
	cout<<"algo"<<endl;
	struct fann *ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);
	
	// funciones de activacion sigmoideas simetricas (-1, 1)
	fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC); 
	fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);
	
	// seteamos el bit fail limit = limite de error por neurona
	fann_set_bit_fail_limit(ann, bit_fail_limit);
	
	if(stop_bit_fail){
		fann_set_train_stop_function(ann,FANN_STOPFUNC_BIT); 
		desired_error = 33; // aproximadamente 1 % de error
		// lo anterior esta hardcodeado, viene de (cantPatrones de datos*num neur salida)x(80% entrenamiento)x(>5% error permitido) 
	}
	else{
		fann_set_train_stop_function(ann,FANN_STOPFUNC_MSE);
		desired_error = 0.001; //error cuadratico medio deseado
	}
	
	//Hacemos la validacion cruzada
		
	for (unsigned int i=0;i<num_part*3;i+=3){
		
		// entrenamiento -> estimacion
		fann_train_on_file(ann, paths[i].c_str(), max_epochs, epochs_between_reports, desired_error);
		
	
		
		// entrenamiento -> validacion
		vector<vector<double> > validation_subset=parseCSV(paths[i+1].c_str());
		cout<<paths[i+1].c_str()<<endl;
		
		
		
		//prueba
		struct fann_train_data *data = fann_read_train_from_file(paths[i+2].c_str());
		
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
	
	// Construccion de la matriz de confusion
	
	// confusion matrix => rows:desired output, columns: calculated output
	unsigned int confusion_matrix[num_output][num_output] = {0};
	unsigned int d_o;
	unsigned int c_o;
	
	for (unsigned int k=1;k<num_part*3;k+=3){
		
		vector<vector<double> > test_data = parseCSV(paths[k]);
		cout<<paths[k].c_str()<<endl;
		
		const unsigned int size_test_data = test_data.size();
		const unsigned int num_test_data = (size_test_data-1)/2;
			
		fann_type *calc_out;
		fann_type input[num_test_data][num_input];
		
		
		double desired_output[num_test_data][num_output];
		
		
		for (unsigned int i=0;i<num_test_data;i++){
			for (unsigned int j=0;j<num_input;j++)
				input[i][j]=test_data[i*2+1][j];
			for (unsigned int j=0;j<num_output;j++)
				desired_output[i][j]=test_data[i*2+2][j];
		}
		
		for(unsigned int i=0;i<num_test_data;i++){
			
			calc_out = fann_run(ann, input[i]);
			
			d_o = pos_max(desired_output[i]);
			c_o = pos_max(calc_out);
			
			confusion_matrix[d_o][c_o]++;
		}
	}
	
	
	
	for(unsigned int i=0;i<num_output;i++){
		for(unsigned int j=0;j<num_output-1;j++)
			printf("%i,",confusion_matrix[i][j]);
		printf("%i\n",confusion_matrix[i][num_output-1]);
	}
	
	
	fann_destroy(ann);
	
	return 0;
	
	
	
	
}

