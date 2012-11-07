#include <GL/glut.h>
#include <stdlib.h>
#include <algorithm> 
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include <cmath>
#include "rbf.h"
#include <ctime>
#include <time.h>

using namespace std;

//Variables globales
double mu = 0.01; //tasa de aprendizaje
double tolerancia = 0.01;
int cantEpocas = 5000,	//cantidad de epocas de entrenamiento
	cantParticiones=1; 
string
	archivo,	
	rutaEntrenamiento,
	rutaPrueba;
int cantGauss=4;
int cantPerc;

vector<vector<double> > salidasPrueba,
						vcentroides;



//variables para graficar
GLfloat li=-2,ld=2,bottom=-2,top=2,znear=-1,zfar=1;
float	delta=1;
static const int ms_lista[]={1,2,5,10,20,50,100,200,500,1000,2000,5000},ms_n=12;
static int ms_i=8,msecs=ms_lista[ms_i]; // milisegundos por frame
//int contador=0;
bool dibuja = false;

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

vector<string> generarParticiones(string nomArchivo,int cantidadParticiones,double porcentajeEntrenamiento){
	vector<vector<double> > patrones = parsearCSV(nomArchivo);
	int cantEntradas = patrones[0].size()-1;
	int cantEntrenamiento = (int) (porcentajeEntrenamiento*patrones.size());
	vector<vector<double> > patronesEntrenamiento;
	vector<vector<double> > patronesPrueba;
	vector<string> nombres;
	
	vector<int> indicesAleatorios(patrones.size());
	for(int i=0;i<patrones.size();i++) 
		indicesAleatorios[i]=i;
	
	for(int i=0;i<cantidadParticiones;i++){
		//generamos los indices aleatorios
		random_shuffle(indicesAleatorios.begin(),indicesAleatorios.end());
		
		for (int j=0;j<cantEntrenamiento;j++)
			patronesEntrenamiento.push_back(patrones[indicesAleatorios[j]]);
		for (int j=cantEntrenamiento;j<patrones.size();j++)
			patronesPrueba.push_back(patrones[indicesAleatorios[j]]);
		
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
		for(int k=0;k<cantEntrenamiento;k++){
			for(j=0;j<cantEntradas;j++){
				fprintf(salidaE,"%f,",patronesEntrenamiento[k][j]);
			}
			fprintf(salidaE,"%f\n",patronesEntrenamiento[k][j]);
		}
		fclose(salidaE);
		
		// patrones de prueba
		FILE *salidaP = fopen(nomPrueba.c_str(),"w");
		for(int k=0;k<patronesPrueba.size();k++){
			for(j=0;j<cantEntradas;j++){
				fprintf(salidaP,"%f,",patronesPrueba[k][j]);
			}
			fprintf(salidaP,"%f\n",patronesPrueba[k][j]);
		}
		fclose(salidaP);
		
	}
	return nombres;
}


void separaEntradas(vector<vector<double> > patrones,int cantEntradas,vector<vector<double> > & entradas){
	entradas.clear();
	for(int i=0;i<patrones.size();i++){
		vector<double> aux;
		for(int j=0;j<cantEntradas;j++){
			aux.push_back(patrones[i][j]);
		}
		entradas.push_back(aux);
		aux.clear();
	}
}

void dibujaPunto2d(string nomArchivo){	
	vector<vector<double> >patrones=parsearCSV(nomArchivo);
	int cantPatrones=patrones.size();
	glPointSize(5);
	glBegin(GL_POINTS);
	if (nomArchivo!="iris.csv" && nomArchivo!="phoneme.csv"){
		for(int i=0;i<cantPatrones;i++){
			if(salidasPrueba[0][i]<0)
				glColor3f(0,1,0);
			else
				glColor3f(1,0,0);
			glVertex2f(patrones[i][0],patrones[i][1]);
		}
	}
	else { //iris o phoneme, 4 o 5 entradas
		//calculo media para cada componente		
		int dim=patrones[0].size()-cantPerc;
		vector<double> mu;
		vector<double> var;
		for(int j=0;j<dim;j++){
			mu.push_back(0.0);
			var.push_back(0.0);
		}
		for(int i=0;i<cantPatrones;i++){
			for(int j=0;j<dim;j++){
				mu[j]+=patrones[i][j];
			}
		}
		for(int j=0;j<dim;j++){
			mu[j]/=cantPatrones;
		}
		//calculo la varianza de cada componente de la entrada
		for(int i=0;i<cantPatrones;i++){
			for(int j=0;j<dim;j++){
				var[j]+=(mu[j]-patrones[i][j])*(mu[j]-patrones[i][j]);
			}
		}
		for(int j=0;j<dim;j++){
			var[j]/=cantPatrones;
		}
		//ahora grafico para las componentes de mayor varianza
		double maxVar1=0.0,maxVar2=0.0;
		int indice1,indice2;
		for(int j=0;j<dim;j++){
			if(var[j]>maxVar1){
				maxVar2=maxVar1;
				indice2=indice1;
				maxVar1=var[j];
				indice1=j;
			}
			else if (var[j]>maxVar2){
				maxVar2=var[j];
				indice2=j;
			}
		}
		
		for(int i=0;i<cantPatrones;i++){
			if(salidasPrueba[0][i]<0 && salidasPrueba[1][i]<0)
				glColor3f(0,0,1);
			if(salidasPrueba[0][i]>0 && salidasPrueba[1][i]<0)
				glColor3f(0,1,0);
			if(salidasPrueba[0][i]>0 && salidasPrueba[1][i]>0)
				glColor3f(1,0,0);
			glVertex2f(patrones[i][indice1],patrones[i][indice2]);
		}
	}
	glEnd();
}

void dibujacentroides(){
	glPointSize(5);
	
	glBegin(GL_POINTS);
	
	for(int i=0;i<vcentroides.size();i++){
		glColor3f(0.2,0.2,0.2);
		glVertex2f(vcentroides[i][0],vcentroides[i][1]);
		}
	glEnd();
}

void reshape_cb (int w, int h) {
	if (w==0||h==0) return;
	glViewport(0,0,w,h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glOrtho(li,ld,bottom,top,znear,zfar);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
}

void display_cb() {
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0,0,0); glLineWidth(1);
	glBegin(GL_LINES);
	glVertex2i(li,0); glVertex2i(ld,0);
	glVertex2i(0,bottom); glVertex2i(0,top);
	glEnd();
	glColor3f(0,0,0);
	glPointSize(3);
	glBegin(GL_POINTS);
	for(int i=-10;i<=10;i++)
		glVertex2f((float)i*li/10,0);		
	glEnd();
	glBegin(GL_POINTS);
	for(int j=-10;j<=10;j++)
		glVertex2f(0,(float)j*ld/10);
	glEnd();
		
	if(dibuja){
		dibujaPunto2d(rutaPrueba);
		dibujacentroides();
		}
	glutSwapBuffers();
}

void calcular(){
	vector<string> rutas=generarParticiones(archivo,cantParticiones,0.8);
	rutaEntrenamiento=rutas[0];
	rutaPrueba=rutas[1];
	//Entrenamiento
	vector<vector<double> > patronesEntrenamiento=parsearCSV(rutaEntrenamiento);
	vector<vector<double> > entradas;
	int cantEntradas=patronesEntrenamiento[0].size()-cantPerc;
	rbf rbf1(cantGauss,cantPerc);
	separaEntradas(patronesEntrenamiento,cantEntradas,entradas);
	
	clock_t t_ini, t_fin; //para calcular tiempo de entrenamiento
	double secs;
	t_ini = clock();
	
	rbf1.entrenarRbf(entradas,vcentroides);
	rbf1.entrenarPesos(patronesEntrenamiento,cantEpocas,tolerancia);
	
	t_fin = clock();
	secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
	cout<<" El entrenamiento de las RBF y de los Pesos duro: "<<secs <<" seg"<<endl;
	
	//Prueba
	vector<vector<double> > patronesPrueba=parsearCSV(rutaPrueba);
	salidasPrueba.clear();
	double efectividad=rbf1.probar(patronesPrueba,salidasPrueba);
	cout<<"Efectividad: "<<efectividad*100<<"%"<<endl;
	
	// Guardamos datos de entrenamiento ----------------------------------
	string nomArchivo="datosrbf";
	int p=rbf1.cantidadParametros();
	FILE *salidaE = fopen(nomArchivo.c_str(),"a");
	fprintf(salidaE,"%s,",archivo.c_str()); // problema
	fprintf(salidaE,"%i,",p); // cantidad de parametros
	fprintf(salidaE,"%f,",secs); // tiempo de entrenamiento
	fprintf(salidaE,"%f\n",efectividad); // porcentaje de acierto
	fclose(salidaE);
	//--------------------------------------------------------------------
	
}

void Keyboard_cb(unsigned char key,int x=0,int y=0){
	if (key == 43) //+epocas
	{	
		cantEpocas+= 10;
		cout<< "Cantidad de epocas: "<<cantEpocas<<endl;
	}
	if (key == 45) //-epocas
	{	
		cantEpocas= (cantEpocas<=1) ? cantEpocas=1 : cantEpocas-= 10;
		cout<< "Cantidad de epocas: "<<cantEpocas<<endl;
	}
	if (key == 'c')
	{
		li=-2;ld=2;bottom=-2;top=2;
		reshape_cb(640,480);
		dibuja=false;
		archivo = ("concentprueba.csv");
		cout<<"Concent"<<endl;
		cantPerc=1;		
	}
	if (key == 'l')
	{
		li=-5;ld=5;bottom=-5;top=5;
		reshape_cb(640,480);
		dibuja=false;
		archivo = ("clouds.csv");
		cout<<"Clouds"<<endl;
		cantPerc=1;		
	}
	if (key == 'p')
	{
		li=-5;ld=5;bottom=-5;top=5;
		reshape_cb(640,480);
		dibuja=false;
		archivo = ("phoneme.csv");
		cout<<"Phoneme"<<endl;
		cantPerc=1;		
	}
	if (key == 'x')
	{
		li=-2;ld=2;bottom=-2;top=2;
		reshape_cb(640,480);
		dibuja=false;
		archivo = ("xoraleatorio.csv");
		tolerancia=0.06;
		cout<<"XOR"<<endl;
		cantPerc=1;		
	}
	if (key == 'i')
	{
		li=-10;ld=10;bottom=-10;top=10;
		reshape_cb(640,480);
		dibuja=false;
		archivo = ("irisprueba.csv");
		tolerancia=0.15;
		cout<<"Iris"<<endl;
		cantPerc=3;		
	}
	if(key == 'g'){ //cantidad de gaussianas +1
		cantGauss+=1;
		cout<<"Cantidad de Gaussianas: "<<cantGauss<<endl;	
	}
	if(key == 'G'){ //cantidad de gaussianas -1
		cantGauss-=1;
		if (cantGauss <1)
			cantGauss=1;
		cout<<"Cantidad de Gaussianas: "<<cantGauss<<endl;
	}
		
	if (key == 'm') //tasa de aprendizaje
	{	
		mu = fmod((mu+0.01),1);
		cout<< "Tasa de aprendizaje: "<<mu<<endl;
	}
	if (key == 'M') //tasa de aprendizaje
	{	
		mu = fmod((mu-0.01),1);
		cout<< "Tasa de aprendizaje: "<<mu<<endl;
	}
	if (key == 'e') //tolerancia a error
	{	
		tolerancia = fmod((tolerancia+0.01),1);
		cout<< "Tolerancia a errores: "<<tolerancia*100<<"%"<<endl;
	}
	if (key == 'E') //tolerancia a error
	{	
		tolerancia = (tolerancia<=0) ?  tolerancia=0 : fmod((tolerancia-0.01),1);
		cout<< "Tolerancia a errores: "<<tolerancia*100<<"%"<<endl;
	}
	
	if (key== 13) //calcular
	{
		if(archivo.empty()){
		cout<<"ERROR: Elija un archivo"<<endl;
		}
		else{
			salidasPrueba.clear();
			vcentroides.clear();
			calcular();
		}
		
		dibuja=true;
	}
	display_cb();
}

void initialize() {
	glutInitDisplayMode (GLUT_RGBA|GLUT_DOUBLE);
	glutInitWindowSize (640,480);
	glutInitWindowPosition (500,100);
	glutCreateWindow ("RBF-NN");
	glutDisplayFunc (display_cb);
	glutReshapeFunc (reshape_cb);
	glutKeyboardFunc(Keyboard_cb);
	glClearColor(1.f,1.f,1.f,1.f);
	
	cout<<"---------------------------------------------"<<endl;
	cout<<"RBF-NN"<<endl;
	cout<<"-------------Seleccion de archivo------------"<<endl;
	cout<<"concent.csv --> c"<<endl;
	cout<<"iris.csv --> i"<<endl;
	cout<<"xor.txt --> x"<<endl;
	cout<<"clouds.csv --> l"<<endl;
	cout<<"phoneme.csv --> p"<<endl;
	cout<<"-------------Comandos------------------------"<<endl;
	cout<<"+1 Cantidad de gaussianas --> g"<<endl;
	cout<<"-1 Cantidad de gaussianas --> G"<<endl;
	cout<<"+0.1 Tasa de aprendizaje --> m"<<endl;	
	cout<<"+1% Tolerancia a errores --> e "<<endl;
	cout<<"-1% Tolerancia a errores --> E "<<endl;
	cout<<"+10 Cantidad de epocas --> +"<<endl;
	cout<<"-10 Cantidad de epocas --> -"<<endl;
	cout<<"Entrenar/Probar --> ENTER "<<endl;
	cout<<"------------Valores Iniciales----------------"<<endl;
	cout<<"Tasa de aprendizaje = "<<mu<<endl;	
	cout<<"Cantidad de epocas = "<<cantEpocas<<endl;
	cout<<"Tolerancia a errores = "<<tolerancia*100<<"%"<<endl;
	
	
	
	cout<<"---------------------------------------------"<<endl;
}

int main (int argc, char **argv) {
	glutInit (&argc, argv);
	initialize();
	glutMainLoop();
	return 0;
}
