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

vector<vector<vector<vector<double> > > > pesosPrueba;
vector<vector<vector<double> > > pesosPrueba1d;
bool som1db=false;
int contador=0;
int cantidadNeuronas=10;
int etapa1=200,
	etapa2=1000,
	etapa3=500;
int cantidadEntradas=2;
vector<int> salida;


//variables para graficar
GLfloat li=-1.5,ld=1.5,bottom=-1.5,top=1.5,znear=-1,zfar=1;
float	delta=1;
static const int ms_lista[]={1,2,5,10,20,50,100,200,500,1000,2000,5000},ms_n=12;
static int ms_i=4,msecs=ms_lista[ms_i]; // milisegundos por frame
//int contador=0;
bool dibuja = false;
bool clasifica = false;

int w=480,h=360,boton=-1,xclick,yclick;
float escala=120,escala0;
short modifiers=0;  // ctrl, alt, shift (de GLUT)
inline short get_modifiers() {return modifiers=(short)glutGetModifiers();}


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

void graficaClasificacion(){
	vector<vector<double> > patrones=parsearCSV(rutaEntrenamiento);    
	glPointSize(4);
	for(int i=0;i<patrones.size();i++){
		if(salida[i]==0)
			glColor3f(1,0,0);
		else
			glColor3f(0,.2,1);
		glBegin(GL_POINTS);
		glVertex2f(patrones[i][0],patrones[i][1]);
		glEnd();
	}
}

int guardaArchivo(string nomArchivoSalida,vector<vector<double> > v){
	//Guardamos el vector con los patrones aleatorios en formato csv.
	FILE *salida = fopen(nomArchivoSalida.c_str(),"w");
	if(salida==NULL) return -1;
	int j;
	//int f=v[0].size()-1
	for(int i=0;i<v.size();i++){
		for(j=0;j<v[0].size()-1;j++){
			fprintf(salida,"%f,",v[i][j]);
		}
		fprintf(salida,"%f\n",v[i][j]);	
	}
	fclose(salida);
}

void reducirCiruclo(){
	vector<vector<double> > patrones=parsearCSV("circulo-large.csv");
    int cantPatrones=patrones.size();
	int cantPuntos=500;
	vector<int> indice;
	for (int i=0;i<cantPatrones;i++)
		indice.push_back(i);
	random_shuffle(indice.begin(),indice.end());
	
	vector<vector<double> > nuevoPatrones;
	for(int i=0;i<cantPuntos;i++){
		nuevoPatrones.push_back(patrones[indice[i]]);
	}
	
	guardaArchivo("circulo.csv",nuevoPatrones);
	
}
void separaEntradas(vector<vector<double> > patrones,int cantEntradas,vector<vector<double> > & entradas){
	for(int i=0;i<patrones.size();i++){
		vector<double> aux;
		for(int j=0;j<cantEntradas;j++){
			aux.push_back(patrones[i][j]);
		}
		entradas.push_back(aux);
		aux.clear();
	}
}


void graficasom2d(){
	
	int ancho=pesosPrueba[0][0].size();
	for(int i=0;i<ancho;i++){
		for(int j=0;j<ancho;j++){
			glPointSize(5);
			glBegin(GL_POINTS);
			glColor3f(0.5,0.5,0.5);
			glVertex2f(pesosPrueba[contador][i][j][0],pesosPrueba[contador][i][j][1]);
			glEnd();
			if(i<ancho-1){
				glBegin(GL_LINES);
				glColor3f(0,0,0.7);
				glVertex2f(pesosPrueba[contador][i][j][0],pesosPrueba[contador][i][j][1]);
				glVertex2f(pesosPrueba[contador][i+1][j][0],pesosPrueba[contador][i+1][j][1]);
				glEnd();
			}
			if(j<ancho-1){
				glBegin(GL_LINES);
				glColor3f(0.8,0.4,0);
				glVertex2f(pesosPrueba[contador][i][j][0],pesosPrueba[contador][i][j][1]);
				glVertex2f(pesosPrueba[contador][i][j+1][0],pesosPrueba[contador][i][j+1][1]);
				glEnd();
			}
			
		}
	}
}
void graficasom1d(){
	
	for(int i=0;i<cantidadNeuronas;i++){
		glPointSize(5);
		glBegin(GL_POINTS);
		glColor3f(0.5,0.5,0.5);
		glVertex2f(pesosPrueba1d[contador][i][0],pesosPrueba1d[contador][i][1]);
		glEnd();
		if(i<cantidadNeuronas-1){
			glBegin(GL_LINES);
			glColor3f(0.8,0.4,0);
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
		redsom.entrenar(patrones,pesosPrueba1d,etapa1,etapa2,etapa3);
	}
	else {
		som2d redsom(cantidadNeuronas,2);
	
		redsom.entrenar(patrones,pesosPrueba,etapa1,etapa2,etapa3);
	}
	
}

void clasificar(){
	vector<vector<double> > patrones=parsearCSV(rutaEntrenamiento);
	vector<vector<double> > entradas;
	separaEntradas(patrones,cantidadEntradas,entradas);
	som2d clasificador(cantidadNeuronas,cantidadEntradas,2);
	
	clock_t t_ini, t_fin; //para calcular tiempo de entrenamiento
	double secs;
	t_ini = clock();
	
	clasificador.entrenar(entradas,pesosPrueba,etapa1,etapa2,etapa3);
	clasificador.etiquetar(patrones);
	
	t_fin = clock();
	secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
	cout<<" Entrenamiento y etiquetado duro: "<<secs <<" seg"<<endl;
	
	
	clasificador.probar(patrones,salida);
	
	
	
	int error=0;
	//verificamos cantidad de aciertos
	for(int i=0;i<patrones.size();i++){
		if(patrones[i][cantidadEntradas]!=salida[i])
			error++;
	}
	
	double efectividad=1-((float)error/patrones.size());
	cout<<"Error"<<error<<" patrones.size()"<<patrones.size()<<" Efectividad: "<<efectividad<<endl;
	
	// Guardamos datos de entrenamiento ----------------------------------
	string nomArchivo="datossom";
	int p=clasificador.cantidadParametros();
	FILE *salidaE = fopen(nomArchivo.c_str(),"a");
	fprintf(salidaE,"%s,",rutaEntrenamiento.c_str()); // problema
	fprintf(salidaE,"%i,",p); // cantidad de parametros
	fprintf(salidaE,"%f,",secs); // tiempo de entrenamiento
	fprintf(salidaE,"%f\n",efectividad); // porcentaje de acierto
	fclose(salidaE);
	//--------------------------------------------------------------------
	
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
			
			if(contador<nroEpocas-1){
				graficasom2d();
				contador++;
			}
			graficasom2d();
			
		}
		
	}
	if(clasifica){
		graficaClasificacion();
	}
	
	glutSwapBuffers();
	
}

void regen() {
	//  if (cl_info) cout << "regen" << endl;
	if (!dibuja) return;
	
	// matriz de proyeccion
	glMatrixMode(GL_PROJECTION);  glLoadIdentity();
	
	double w0=(double)w/2/escala,h0=(double)h/2/escala; // semiancho y semialto en el target
	
	glOrtho(-w0,w0,-h0,h0,znear,zfar);
	
	display_cb(); // avisa que hay que redibujar
}


void reshape_cb(int width, int height){
	h=height; w=width;
		
	glViewport(0,0,w,h); // region donde se dibuja
	
	regen(); //regenera la matriz de proyeccion
}

void Motion_cb(int xm, int ym){ // drag
	if (boton==GLUT_LEFT_BUTTON){
		if (modifiers==GLUT_ACTIVE_SHIFT){ // cambio de escala
			escala=escala0*exp((yclick-ym)/100.0);
			regen();
		}

	}
}
void Mouse_cb(int button, int state, int x, int y){
	
	if (button==GLUT_LEFT_BUTTON){
		if (state==GLUT_DOWN) {
			xclick=x; yclick=y;
			boton=button;
			
			get_modifiers();
			glutMotionFunc(Motion_cb); // callback para los drags
			if (modifiers==GLUT_ACTIVE_SHIFT){ // cambio de escala
				escala0=escala;
			}
			
		}
		else if (state==GLUT_UP){
			
			boton=-1;
			glutMotionFunc(0); // anula el callback para los drags
		}
	}
}
void Idle_cb() {
	static int suma,counter=0;// esto es para analisis del framerate real
	// Cuenta el lapso de tiempo
	// (int!!?? en seguida se hace >MAX_INT pero ... funciona)
	static int anterior=glutGet(GLUT_ELAPSED_TIME); // milisegundos desde que arranco
	if (msecs!=1){ // si msecs es 1 no pierdo tiempo
		int tiempo=glutGet(GLUT_ELAPSED_TIME), lapso=tiempo-anterior;
		if (lapso<msecs) return;
		suma+=lapso;
		if (++counter==100) {
			
			counter=suma=0;
		}
		anterior=tiempo;
	}
	
	display_cb(); // redibuja
}

void Keyboard_cb(unsigned char key,int x=0,int y=0){
	if (key == 'c')
	{
		dibuja=false;
		clasifica=false;
		rutaEntrenamiento = ("cuadrado.csv");
		rutaPrueba = ("cuadrado.csv");
		pesosPrueba.clear();
		if(pesosPrueba.empty()) cout<<"vacion"<<endl;
		pesosPrueba1d.clear();
		cout<<"cuadrado.csv"<<endl;	
		if(som1db)	cout<<"SOM 1D"<<endl;
		else cout<<"SOM 2D"<<endl;
			
	}
	if (key == 'C')
	{
		dibuja=false;
		clasifica=false;
		rutaEntrenamiento = ("circulo.csv");
		rutaPrueba = ("circulo.csv");
		etapa2=2000;
		pesosPrueba.clear();
		pesosPrueba1d.clear();
		int cantidadNeuronas=200;
		cout<<"circulo.csv"<<endl;		
		cout<<"Cantidad de Neuronas: "<<cantidadNeuronas<<endl;
	}
	if (key == 't')
	{
		dibuja=false;
		clasifica=false;
		rutaEntrenamiento = ("t.csv");
		rutaPrueba = ("t.csv");
		pesosPrueba.clear();
		pesosPrueba1d.clear();
		cout<<"t.csv"<<endl;		
	}
	if (key == 'l')
	{
		dibuja=false;
		rutaEntrenamiento = ("clouds.csv");
		rutaPrueba = ("clouds.csv");
		pesosPrueba.clear();
		pesosPrueba1d.clear();
		cantidadEntradas=2;
		cout<<"clouds.csv"<<endl;		
		clasificar();
		clasifica=true;
	}
	if (key == 'p')
	{
		dibuja=false;
		rutaEntrenamiento = ("phoneme.csv");
		rutaPrueba = ("phoneme.csv");
		pesosPrueba.clear();
		pesosPrueba1d.clear();
		cantidadEntradas=4;
		cout<<"phoneme.csv"<<endl;		
		clasificar();
		clasifica=true;
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
	if (key=='s'){
		reducirCiruclo();
	}
	if (key== 13) //entrenar
	{
		contador=0;
		if(som1db){
			if(pesosPrueba1d.empty()){
				calcular();
			}
		}
		else {
			if(pesosPrueba.empty()){
				cout<<" "<<endl;
				calcular();
				
			}
		}
			
		dibuja=true;
	}
	display_cb();
}

void initialize() {
	glutInitDisplayMode (GLUT_RGBA|GLUT_DOUBLE);
	glutInitWindowSize (640,480);
	glutInitWindowPosition (100,100);
	glutCreateWindow ("SOM");
	glutDisplayFunc (display_cb);
	glutMouseFunc(Mouse_cb); // botones picados
	glutKeyboardFunc(Keyboard_cb);
	glutIdleFunc(Idle_cb);
	glutReshapeFunc (reshape_cb);
	glClearColor(1.f,1.f,1.f,1.f);
	
	cout<<"---------------------------------------------"<<endl;
	cout<<"SOM"<<endl;
	cout<<"-------------Seleccion de archivo------------"<<endl;
	cout<<"cuadrado.csv --> c"<<endl;
	cout<<"circulo.csv --> C"<<endl;
	cout<<"t.csv --> t"<<endl;
	cout<<"clouds.csv --> l"<<endl;
	cout<<"phoneme.csv --> p"<<endl;
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
