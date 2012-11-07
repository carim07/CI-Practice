#include "som2d.h"
#include "som1d.h"
#include <GL/glut.h>
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

using namespace std;
//varibales globales
string
	rutaEntrenamiento = ("cuadrado.csv"),
	rutaPrueba = ("cuadrado.csv");

vector<vector<vector<double> > > pesosPrueba;
vector<vector<vector<double> > > pesosPrueba1d;
bool som1db=false;
int contador=0;
int cantidadNeuronas=10;



//variables para graficar
GLfloat li=-1.5,ld=1.5,bottom=-1.5,top=1.5,znear=-1,zfar=1;
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

void graficasom2d(){

	int ancho=sqrt(cantidadNeuronas);
	for(int i=0;i<cantidadNeuronas;i++){
		glPointSize(5);
		glBegin(GL_POINTS);
		glColor3f(1,0,0);
		glVertex2f(pesosPrueba[contador][i][0],pesosPrueba[contador][i][1]);
		glEnd();
		if((i-ancho)>0){ // linea i-norte
			glBegin(GL_LINES);
			glColor3f(0,1,0);
			glVertex2f(pesosPrueba[contador][i][0],pesosPrueba[contador][i][1]);
			glVertex2f(pesosPrueba[contador][i-ancho][0],pesosPrueba[contador][i-ancho][1]);
			glEnd();
		}
		if(((i<=ancho)&&((i-1)>0))||((i>ancho)&&(i-1)%ancho)!=0){ // linea i-oeste
			glBegin(GL_LINES);
			glColor3f(0,0,1);
			glVertex2f(pesosPrueba[contador][i][0],pesosPrueba[contador][i][1]);
			glVertex2f(pesosPrueba[contador][i-1][0],pesosPrueba[contador][i-1][1]);
			glEnd();
		}		
	}
	
	
}
void graficasom1d(){
	
	for(int i=0;i<cantidadNeuronas;i++){
		glPointSize(5);
		glBegin(GL_POINTS);
		glColor3f(1,0,0);
		glVertex2f(pesosPrueba1d[contador][i][0],pesosPrueba1d[contador][i][1]);
		glEnd();
		if(i<cantidadNeuronas-1){
			glBegin(GL_LINES);
			glVertex2f(pesosPrueba1d[contador][i][0],pesosPrueba1d[contador][i][1]);
			glVertex2f(pesosPrueba1d[contador][i+1][0],pesosPrueba1d[contador][i+1][1]);
			glEnd();
		}
		
	}
	
}


void calcular(){
	vector<vector<double> > patrones=parsearCSV(rutaEntrenamiento);	
	
	if(som1db){
		som1d redsom(cantidadNeuronas,2);
		redsom.entrenar(patrones,pesosPrueba1d);
	}
	else {
		som2d redsom(cantidadNeuronas,2);
		redsom.entrenar(patrones,pesosPrueba);
		cout<<"salio"<<endl;
	}
	
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
		
	if(dibuja){
		
		if(som1db){
			int nroEpocas=pesosPrueba1d.size();	
			//contador=nroEpocas-1;
			if(contador<nroEpocas-1){
				graficasom1d();
				contador++;
			}
			graficasom1d();
			
		}
		else{
			int nroEpocas=pesosPrueba.size();	
			//contador=nroEpocas-1;
			cout<<"Nro Graficas: "<<nroEpocas<<" - Grafica actual: "<<contador<<endl;
			if(contador<nroEpocas-1){
				graficasom2d();
				contador++;
			}
			graficasom2d();
			
		}
		
	}
	
	
	glutSwapBuffers();
	
}

void Idle_cb() {
	static int suma,counter=0;// esto es para analisis del framerate real
	// Cuenta el lapso de tiempo
	
	static int anterior=glutGet(GLUT_ELAPSED_TIME); // milisegundos desde que arranco
	if (msecs!=1){ // si msecs es 1 no pierdo tiempo
		int tiempo=glutGet(GLUT_ELAPSED_TIME), lapso=tiempo-anterior;
		if (lapso<msecs) return;
		suma+=lapso;
		if (++counter==100) {
			//      cout << "<ms/frame>= " << suma/100.0 << endl;
			counter=suma=0;
		}
		anterior=tiempo;
	}
	
	display_cb(); // redibuja
	
};

void Keyboard_cb(unsigned char key,int x=0,int y=0){
	if (key == 'c')
	{
		dibuja=false;
		rutaEntrenamiento = ("cuadrado.csv");
		rutaPrueba = ("cuadrado.csv");
		cout<<"cuadrado.csv"<<endl;		
	}
	if (key == 'C')
	{
		dibuja=false;
		rutaEntrenamiento = ("circulo.csv");
		rutaPrueba = ("circulo.csv");
		cout<<"circulo.csv"<<endl;		
	}
	if (key == 't')
	{
		dibuja=false;
		rutaEntrenamiento = ("t.csv");
		rutaPrueba = ("t.csv");
		cout<<"t.csv"<<endl;		
	}
	if (key == '1')
	{
		dibuja=false;
		som1db=true;
		cout<<"SOM 1D"<<endl;
	}
	if (key == '2')
	{
		dibuja=false;
		som1db=false;		
		cout<<"SOM 2D"<<endl;
	}
	if (key == 'n')
	{
		cout<<"Ingrese cantidad de neuronas: "<<endl;
		cin>>cantidadNeuronas;
		
	}
	if (key== 13) //entrenar
	{
		contador=0;
		if(som1db){
			if(pesosPrueba1d.empty())
				calcular();
		}
		else if(pesosPrueba.empty())
			calcular();
		dibuja=true;
	}
	display_cb();
}

void initialize() {
	glutInitDisplayMode (GLUT_RGBA|GLUT_DOUBLE);
	glutInitWindowSize (640,480);
	glutInitWindowPosition (100,100);
	glutCreateWindow ("Ventana OpenGL");
	glutDisplayFunc (display_cb);
	glutKeyboardFunc(Keyboard_cb);
	glutIdleFunc(Idle_cb);
	glutReshapeFunc (reshape_cb);
	glClearColor(1.f,1.f,1.f,1.f);
	
	cout<<"---------------------------------------------"<<endl;
	cout<<"SOM"<<endl;
	cout<<"-------------Seleccion de archivo------------"<<endl;
	cout<<"cuadrado.csv --> c"<<endl;
	cout<<"-------------Comandos------------------------"<<endl;
	cout<<"Entrenar/Probar --> ENTER "<<endl;
	cout<<"Ingresar cantidad de neuronas --> n "<<endl;
	cout<<"---------------------------------------------"<<endl;
}

int main (int argc, char **argv) {
	glutInit (&argc, argv);
	initialize();
	glutMainLoop();
	return 0;
}
