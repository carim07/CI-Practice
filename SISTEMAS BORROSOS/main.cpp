#include <GL/glut.h>
#include <stdlib.h>
#include <algorithm> 
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include "controlador.h"
#include "trapecio.h"
#include <ctime>
#include <time.h>
using namespace std;


//Variables globales
int it=720;
string ruta="controlador.txt";

int guardaArchivo(string nomArchivoSalida,vector<double> v)
{	//Guardamos el vector con los patrones aleatorios en formato csv.
	FILE *salida = fopen(nomArchivoSalida.c_str(),"w");
	if(salida==NULL) return -1;
	int j;
	for(int i=0;i<v.size();i++)
		fprintf(salida,"%f\n",v[i]);	
	
	fclose(salida);
	return 1;
}
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

void graficatrapecio(trapecio t){
	glBegin(GL_POINTS);
	glPointSize(3.0);
	glColor3f(0.3,0.3,0.3);
	glVertex2f(t.x0,0);
	glVertex2f(t.x1,1);
	glVertex2f(t.x2,1);
	glVertex2f(t.x3,0);
	glEnd();
	glBegin(GL_LINES);
	glVertex2f(t.x0,0);
	glVertex2f(t.x1,1);
	glEnd();
	glBegin(GL_LINES);
	glVertex2f(t.x1,1);
	glVertex2f(t.x2,1);
	glEnd();
	glBegin(GL_LINES);
	glVertex2f(t.x2,1);
	glVertex2f(t.x3,0);
	glEnd();
}

void curvaSistema(){
	//cargamos reglas -> conjuntoEntrada   conjuntoSalida
	
	vector<vector<double> > creglas=parsearCSV(ruta);
	int cantReglas=creglas.size();
//	double creglas[5][8]={
//		{-8,-7,-3,-2,		-150,-122,-90,-80},
//			{-3,-2,-0.5,0,		-90,-80,-0.3,0},
//			{-0.5,0,0,0.5,		-0.3 ,0,0,0.3 },
//			{0,0.5,3,6,0,    	0.3,1.3,1.5 },
//			{3,6,12,13,			1.3,1.5,6,8}
//	};
	//creamos controlador
	controlador fuzzy(cantReglas);
	
	for (int r=0;r<cantReglas;r++){
		trapecio ent(creglas[r][0],creglas[r][1],creglas[r][2],creglas[r][3]);
		trapecio sal(creglas[r][4],creglas[r][5],creglas[r][6],creglas[r][7]);
		fuzzy.mapeo(ent,sal);
	}
	//Vamos desde delta negativo a delta positivo probando el sistema
		
	double deltaTempInicio=-10.0, // Temp Interior 10 grados mas alta que la Deseada
		deltaTempFin=10.0, // Temp Interior 10 grados mas baja que la Deseada
		rango=deltaTempFin-deltaTempInicio, // rango a barrer para generar la curva
		deltaTemp=deltaTempInicio; 
	
	int cantPuntos=1000; 
	double salto=rango/cantPuntos;
	vector<double> entrada;
	vector<double> salida;
	
	for (int i=0;i<cantPuntos;i++){
		entrada.push_back(deltaTemp);
		salida.push_back(fuzzy.salidaTotal(deltaTemp));
		deltaTemp+=salto;
	}
	
	//Guardamos para graficar
	guardaArchivo("entrada",entrada);
	guardaArchivo("Curva_sistema",salida);
}

void calcular(){
	//Vectores para almacenamiento
	vector<double> tempInterior,
		tempExterior,
		tempDeseada;
		
	vector<bool> puerta;
	vector<vector<double> > creglas=parsearCSV(ruta);
	int cantReglas=creglas.size();
	//creamos controlador
	controlador fuzzy(cantReglas);
	
	for (int r=0;r<cantReglas;r++){
		trapecio ent(creglas[r][0],creglas[r][1],creglas[r][2],creglas[r][3]);
		trapecio sal(creglas[r][4],creglas[r][5],creglas[r][6],creglas[r][7]);
		fuzzy.mapeo(ent,sal);
	}
	
	srand((unsigned)(time(NULL)));//semilla random
	//iniciamos variables
	double tempi = 25;
	double tempe = 15;
	double tempd = 18;
	double voltaje = 0;
	double intensidad = 0;
	
	for(int t=0; t<it;t++){
		//variamos la temperatura exterior cada 60 iteraciones
		if(t%120<60) tempe=15;
		else tempe=20;
		//variamos la temperatura deseada cada 60 iteraciones
		if(t%120<60) tempd=18;
		else tempd=22;
		
		//la entrada del controlador fuzzy es el delta de temperatura = tdeseada-tinterior
		
		double deltaTemp=tempd-tempi;
		double salida=fuzzy.salidaTotal(deltaTemp);
		
		//si salidaControlador es >=0 se activan los conjuntos de intensidad (falta calor)
		if(deltaTemp >= 0){			
			intensidad = salida;
			voltaje = 0;
		}
		//si salidaControlador es <0 se activan los conjuntos de voltaje (falta frio)
		else{			
			voltaje = salida;	//los conjuntos de voltaje son negativos para evitar superposicion
			intensidad = 0;
		}
		
		double valor=((double)rand())/((double)RAND_MAX);//entre 0 y 1
		//simulamos probabilidad de abertura de la puerta
		bool seAbre = valor<1.0/360.0;
		if(seAbre)//puerta abierta
			tempi = 0.169*tempi + 0.831*tempe + 0.112*intensidad*intensidad - 0.002*voltaje;
		else //puerta cerrada
			tempi = 0.912*tempi + 0.088*tempe + 0.604*intensidad*intensidad - 0.0121*voltaje;
		
		//guardamos resultados para graficar		
		tempExterior.push_back(tempe);
		tempInterior.push_back(tempi);
		tempDeseada.push_back(tempd);
		
				
	}//end for
	
	guardaArchivo("Temperatura_exterior",tempExterior);
	guardaArchivo("Temperatura_interior",tempInterior);
	guardaArchivo("Temperatura_deseada",tempDeseada);	
	
}
void keyboard_cb(unsigned char key,int x=0,int y=0){
	if (key== 13) //entrenar
	{
		calcular();
		curvaSistema();
		cout<<"Archivos guardados"<<endl;
	}
	
	
}

void reshape_cb (int w, int h) {
	if (w==0||h==0) return;
	glViewport(0,0,w,h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluOrtho2D(0,w,0,h);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
}

void display_cb() {
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1,1,0); glLineWidth(3);
	glBegin(GL_LINE_STRIP);
		glVertex2i(130,060); glVertex2i( 50,060);
		glVertex2i(130,150); glVertex2i( 50,150);
	glEnd();
	glBegin(GL_LINES);
		glVertex2i( 70,100); glVertex2i(110,100);
		glVertex2i(150,100); glVertex2i(230,100);
		glVertex2i(190,140); glVertex2i(190,070);
		glVertex2i(250,100); glVertex2i(330,100);
		glVertex2i(290,140); glVertex2i(290,070);
	glEnd();
	glutSwapBuffers();
}

void initialize() {
	glutInitDisplayMode (GLUT_RGBA|GLUT_DOUBLE);
	glutInitWindowSize (640,480);
	glutInitWindowPosition (100,100);
	glutCreateWindow ("Ventana OpenGL");
	glutDisplayFunc (display_cb);
	glutReshapeFunc (reshape_cb);
	glutKeyboardFunc(keyboard_cb);
	glClearColor(1.f,1.f,1.f,1.f);
}

int main (int argc, char **argv) {
	glutInit (&argc, argv);
	initialize();
	glutMainLoop();
	return 0;
}
