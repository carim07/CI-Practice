#include <GL/glut.h>
#include <stdlib.h>
#include <algorithm> 
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include "PerceptronSimple.h"
#include <ctime>
#include <time.h>
using namespace std;

//variables globales
double mu = 0.2; //tasa de aprendizaje
double tolerancia = 0.0; //tolerancia de error
double cantEpocas = 10; //cantidad de epocas de entrenamiento
double desvio = 1;
int cantidadPuntos=200;
string
	rutaEntrenamiento = ("OR.txt"),
	rutaPrueba = ("OR.txt");

//variables para graficar
GLfloat li=-5,ld=5,bottom=-5,top=5,znear=-1,zfar=1;
float	delta=1;
static const int ms_lista[]={1,2,5,10,20,50,100,200,500,1000,2000,5000},ms_n=12;
static int ms_i=8,msecs=ms_lista[ms_i]; // milisegundos por frame
int contador=0;
bool dibuja = false;
bool dibujarectas = false;
vector<vector<double> > wEpoca;
int numEpocas;



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

vector<double> puntoAleatorio(vector<double> punto,double desviacion){
	int n=punto.size(); //dimensiones
	int norma=0;
	double distancia;
	for (int k=0;k<n;k++)
	{
		norma+=punto[k]*punto[k];
	}
	norma=sqrt(norma); //norma 2 del punto (distancia a 0)
	//calculamos la distancia del punto aleatorio al punto original, que
	//esta entre 0 y la desviacion (porcentaje)
	distancia=((double)rand()/(double)RAND_MAX)*(norma*desviacion);
	//hacemos un punto aleatorio entre -1 y 1 (para que pueda moverse para cualquier lado)
	//y calculamos su norma
	vector<double> v;
	double normaV=0;
	for (int k=0;k<n;k++)
	{
		v.push_back(-1+((double)rand()/(double)RAND_MAX)*2);
		normaV+=v[k]*v[k];
	}
	normaV=sqrt(normaV);
	//hacemos unitario el vector aleatorio y luego lo escalamos a la norma buscada
	for(int k=0;k<n;k++)
	{
		v[k]*=(distancia/normaV);
	}
	//trasladamos el punto
	for (int k=0;k<n;k++)
	{
		v[k]=punto[k]+v[k];
	}
	return v;
}


int generaPatronesAleatorios(string nomArchivo,string nomArchivoSalida,int cantidad,double porcentaje){
	vector<vector<double> > patrones=parsearCSV(nomArchivo);
	int cantPatrones = patrones.size();
	int cantEntradas = patrones[0].size()-1;
	
	vector<vector<double> > nuevoPatrones;
	vector<double> punto;
	vector<double> aux;
	
	for(int i=0;i<cantPatrones;i++){
		nuevoPatrones.push_back(patrones[i]);
		for(int k=0;k<cantEntradas;k++){
			punto.push_back(patrones[i][k]);
		}		
		for(int j=0;j<cantidad;j++){
			aux = puntoAleatorio(punto,porcentaje);
			aux.push_back(patrones[i][cantEntradas]);
			nuevoPatrones.push_back(aux);
			aux.clear();
		}
		punto.clear();
	}
	
		
	//Guardamos el vector con los patrones aleatorios en formato csv.
	FILE *salida = fopen(nomArchivoSalida.c_str(),"w");
	if(salida==NULL) return -1;
	int j;
	for(int i=0;i<nuevoPatrones.size();i++){
		for(j=0;j<cantEntradas;j++){
			fprintf(salida,"%f,",nuevoPatrones[i][j]);
		}
		fprintf(salida,"%f\n",nuevoPatrones[i][j]);	
	}
	fclose(salida);
	return 1;
		
};
void recalcular(){
	vector<vector<double> > patrones=parsearCSV(rutaEntrenamiento);
	PerceptronSimple p1;
	p1.setTasa(mu);
	p1.entrenamiento(patrones,cantEpocas,tolerancia);
	vector<vector<double> > patronesPrueba=parsearCSV(rutaPrueba);
	vector<double> salida;
	double efectividad=p1.prueba(patronesPrueba,salida);
	cout<< "Efectividad: " <<" "<<efectividad<< endl;
	
	contador=0;
	//guardo los parametros de recta paratodas las epocas para luego graficar
	wEpoca=p1.obtenerwEpoca();
	//necesito el tamaño tambien para graficar
	numEpocas=wEpoca.size();	
	
}

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


void dibujarRecta(double w0,double w1,double w2){
	//grafica la recta con los pesos calculados en entrenamiento
	double p=-w1/w2;//pendiente
	double y1,y2;
	double oo=w0/w2;//ordenada al origen
	y1=li*p+oo;
	y2=ld*p+oo;
	glBegin(GL_LINES);
	glVertex2f(li,y1);glVertex2f(ld,y2);
	glEnd();
}

void dibujaPunto2d(string nomArchivo){
	
	vector<vector<double> >patrones=parsearCSV(nomArchivo);
	
	int cantPatrones=patrones.size();
	
	glPointSize(5);
	glBegin(GL_POINTS);
	for(int i=0;i<cantPatrones;i++){
		if(patrones[i][2]==1)
			glColor3f(0,1,0);
		else
			glColor3f(1,0,0);
		glVertex2f(patrones[i][0],patrones[i][1]);
		
	}
	glEnd();
	
}

void display_cb() {
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0,0,0); glLineWidth(1);
	
	glBegin(GL_LINES);
	glVertex2i(li,0); glVertex2i(ld,0);
	glVertex2i(0,bottom); glVertex2i(0,top);
	glEnd();
	
	glColor3f(0,0,0);
	glPointSize(3.0);
	glBegin(GL_POINTS);
	for(int i=li;i<=ld;i+=delta)
		glVertex2i(i,0);
	glEnd();
	
	glBegin(GL_POINTS);
	for(int j=bottom;j<=top;j+=delta)
		glVertex2i(0,j);
	glEnd();
	
	if(dibuja)
		dibujaPunto2d(rutaEntrenamiento);
	
	if(dibujarectas)
	{
		if (contador<numEpocas)
		{			
			dibujarRecta(wEpoca[contador][0],wEpoca[contador][1],wEpoca[contador][2]);
			contador++;
			
		}
		else
		{
			dibujarRecta(wEpoca[numEpocas-1][0],wEpoca[numEpocas-1][1],wEpoca[numEpocas-1][2]);
						
		}
					
	}	
	glutSwapBuffers();
	
};



void reshape_cb (int w, int h) {
	if (w==0||h==0) return;
	glViewport(0,0,w,h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glOrtho(li,ld,bottom,top,znear,zfar);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
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
	if (key == 43) //+epocas
	{	
		cantEpocas+= 1;
		cout<< "Cantidad de epocas: "<<cantEpocas<<endl;
	}
	if (key == 45) //-epocas
	{	
		cantEpocas= (cantEpocas<=1) ? cantEpocas=1 : cantEpocas-= 1;
		cout<< "Cantidad de epocas: "<<cantEpocas<<endl;
	}	
	if (key == 't') //tasa de aprendizaje
	{	
		mu = fmod((mu+0.1),1);
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
	if (key == 'o')
	{
		rutaEntrenamiento = ("OR.txt");
		rutaPrueba = ("OR.txt");
		dibuja=true;
		cout<<"OR"<<endl;
	}	
	if (key == 'x')
	{
		rutaEntrenamiento = ("XOR.txt");
		rutaPrueba = ("XOR.txt");
		dibuja=true;
		cout<<"XOR"<<endl;
	}
	if (key== 13) //entrenar
	{
		recalcular();
		dibujarectas=true;
	}
	if (key== 'w') //particiones
	{
		generarParticiones("ORaleatorio.txt",2,0.7);
	}
	
	if (key== 'g') //guardamos patrones aleatorios
	{
		int err;
		err=generaPatronesAleatorios("circulo","circulo.csv",cantidadPuntos,desvio);
		if (err==1) cout<<"Archivo guardado"<<endl; else cout<<"Archivo NO guardado"<<endl;
		rutaEntrenamiento = ("XORaleatorio.txt");
		rutaPrueba = ("XORaleatorio.txt");
		dibuja=true;
		cout<<"circulo.csv"<<endl;
	}
	if (key== 'd') //desvio
	{
		desvio = fmod((desvio+0.1),1);
		cout<< "Desvio de puntos aleatorios: "<<desvio<<endl;
	}
	if (key == 'p') //Cantidad de puntos aleatorios
	{	
		cantidadPuntos += 5;
		cout<< "Cantidad de puntos aleatorios: "<<cantidadPuntos<<endl;
	}
	if (key == 'P') //Cantidad de puntos aleatorios
	{	
		cantidadPuntos = (cantidadPuntos<=5) ?  cantidadPuntos=5 : cantidadPuntos -= 5;
		cout<< "Cantidad de puntos aleatorios: "<<cantidadPuntos<<endl;
	}
	
	
	display_cb();
};	


void initialize() {
	glutInitDisplayMode (GLUT_RGBA|GLUT_DOUBLE);
	glutInitWindowSize (640,480);
	glutInitWindowPosition (100,100);
	glutCreateWindow ("Perceptron Simple");
	cout<<"---------------------------------------------"<<endl;
	cout<<"Perecptron Simple"<<endl;
	cout<<"-------------Seleccion de archivo------------"<<endl;
	cout<<"OR.txt--> o"<<endl;
	cout<<"XOR.txt--> x"<<endl;
	cout<<"-------------Comandos------------------------"<<endl;
	cout<<"+0.1 Tasa de aprendizaje --> t"<<endl;	
	cout<<"+1% Tolerancia a errores --> e "<<endl;
	cout<<"-1% Tolerancia a errores --> E "<<endl;
	cout<<"+1 Cantidad de epocas --> +"<<endl;
	cout<<"-1 Cantidad de epocas --> -"<<endl;
	cout<<"Generar patrones aleatorios --> g"<<endl;
	cout<<"+5% Desvio de puntos aleatorios --> d"<<endl;
	cout<<"+5 Puntos aleatorios --> p"<<endl;
	cout<<"-5 Puntos aleatorios --> P"<<endl;
	cout<<"Entrenar/Probar --> ENTER "<<endl;		
	cout<<"------------Valores Iniciales----------------"<<endl;
	cout<<"Tasa de aprendizaje = "<<mu<<endl;
	cout<<"Cantidad de epocas = "<<cantEpocas<<endl;
	cout<<"Tolerancia a errores = "<<tolerancia*100<<"%"<<endl;
	cout<<"Desvio de puntos aleatorios = "<<desvio*100<<"%"<<endl;
	cout<<"Cantidad de puntos aleatorios = "<<cantidadPuntos<<endl;
	cout<<"---------------------------------------------"<<endl;
	
	glutDisplayFunc (display_cb);
	glutKeyboardFunc(Keyboard_cb);
	glutIdleFunc(Idle_cb);
	glutReshapeFunc (reshape_cb);
	glClearColor(1.f,1.f,1.f,1.f);
	srand((unsigned)time(NULL));
}

int main (int argc, char **argv) {
	glutInit (&argc, argv);
	initialize();
	glutMainLoop();
	return 0;
}
