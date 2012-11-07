#include <GL/glut.h>
#include <stdlib.h>
#include <algorithm> 
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include <cmath>
#include "PerceptronMulticapa.h"
#include <ctime>
#include <time.h>

using namespace std;

//Variables Globales
double mu = 0.01; //tasa de aprendizaje
double tolerancia = 0.01;
double cantEpocas = 500; //cantidad de epocas de entrenamiento
string
	rutaEntrenamiento = ("irisprueba.csv"),
	rutaPrueba = ("irisprueba.csv");


int cantidadCapas=4;
int kout;
vector<int> estructura(4,1);

double alfa=0.0;
double curvatura=2;
vector<vector<double> > salidaPrueba;

//variables para graficar
GLfloat li=-0.2,ld=1.2,bottom=-0.2,top=1.2,znear=-1,zfar=1;
float    delta=1;
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



void dibujaPunto2d(string nomArchivo){    
	vector<vector<double> >patrones=parsearCSV(nomArchivo);
	int cantPatrones=patrones.size();
	glPointSize(5);
	glBegin(GL_POINTS);
	if (nomArchivo=="concentprueba.csv"){
		for(int i=0;i<cantPatrones;i++){
			if(salidaPrueba[i][0]<0)
				glColor3f(0,1,0);
			else
				glColor3f(1,0,0);
			glVertex2f(patrones[i][0],patrones[i][1]);
		}
	}
	else {
		double max1=0,max2=0,max3,max4;
		for(int i=0;i<cantPatrones;i++){
			if(patrones[i][0]>max1) max1=patrones[i][0];
			if(patrones[i][1]>max2) max2=patrones[i][1];
			if(patrones[i][3]>max3) max3=patrones[i][2];
			if(patrones[i][4]>max4) max4=patrones[i][3];
		}
		for(int i=0;i<cantPatrones;i++){
			if(salidaPrueba[i][0]<0 && salidaPrueba[i][1]<0)
				glColor3f(0,0,1);
			if(salidaPrueba[i][0]>0 && salidaPrueba[i][1]<0)
				glColor3f(0,1,0);
			if(salidaPrueba[i][0]>0 && salidaPrueba[i][1]>0)
				glColor3f(1,0,0);
			glVertex2f(patrones[i][0]/max1,patrones[i][1]/max2);
		}
	}
	glEnd();
}




void calcular(){
	vector<vector<double> > patrones=parsearCSV(rutaEntrenamiento);
	vector<double> entradas;
	vector<double> salidas;
	double aciertos=0.0;
	double porcentajeError=1;
	int i=0;
	PerceptronMulticapa mlp(cantidadCapas,estructura,mu,alfa,curvatura);    
	// Entrenamos la red mediante propagacion hacia atras
	clock_t t_ini, t_fin; //para calcular tiempo de entrenamiento
	float secs;
	t_ini = clock();
	
	while (i<cantEpocas && tolerancia<porcentajeError ){
		for(int j=0;j<patrones.size();j++){
			for(int k=0;k<estructura[0];k++)
				entradas.push_back(patrones[j][k]);
			for(int k=0;k<estructura[cantidadCapas-1];k++) //cargamos las entradas
				salidas.push_back(patrones[j][estructura[0]+k]); //cargamos las salidas
			mlp.propagacionAtras(entradas,salidas);
			entradas.clear();
			salidas.clear();
		} // fin de entrenamiento de una epoca
		// Prueba de la red luego del fin de una epoca
		// para determinar porcentaje de aciertos
		for(int j=0;j<patrones.size();j++){
			for(int k=0;k<estructura[0];k++){
				entradas.push_back(patrones[j][k]);
			}
			for(int k=0;k<estructura[cantidadCapas-1];k++)
				salidas.push_back(patrones[j][estructura[0]+k]);
			mlp.propagacionAdelante(entradas);
			if (!mlp.error(salidas))
				aciertos++;
			entradas.clear();
			salidas.clear();
		}
		porcentajeError=1-(aciertos/patrones.size()); //porcentaje de error por epoca
		//      if(i%10==0)
		//    cout<<"Epoca: "<<i<<" Porcentaje de acierto: "<<(1-porcentajeError)*100<<"%"<<endl;
		i++;
		aciertos=0;
	}//FIN DE BUCLE POR EPOCAS 
	
	t_fin = clock();
	secs = (float)(t_fin - t_ini) / CLOCKS_PER_SEC; // lo que demoro el entrenamiento
		
	// Guardamos datos de entrenamiento ----------------------------------
	string nomArchivo="datosmlp";
	int p=mlp.cantidadParametros();
	float porcentajeAcierto=(1-porcentajeError);
	FILE *salidaE = fopen(nomArchivo.c_str(),"a");
	fprintf(salidaE,"%s,",rutaEntrenamiento.c_str()); // problema
	fprintf(salidaE,"%i,",p); // cantidad de parametros
	fprintf(salidaE,"%f,",secs); // tiempo de entrenamiento
	fprintf(salidaE,"%f\n",porcentajeAcierto); // porcentaje de acierto
	fclose(salidaE);
	//--------------------------------------------------------------------
	cout<<"Epoca: "<<i<<" Porcentaje de acierto: "<<porcentajeAcierto*100<<"%"<<endl;
	// Probamos la red entrenada y graficamos el resultado
	for(int j=0;j<patrones.size();j++){
		for(int k=0;k<estructura[0];k++){
			entradas.push_back(patrones[j][k]);
		}
		mlp.propagacionAdelante(entradas);
		salidaPrueba.push_back(mlp.getSalidas());    
		//cout<<"salida "<<j<<": "<<salidaPrueba[j]<<endl;
		entradas.clear();
	}
}

void desordenaVector(vector<vector<double> > v,  vector<vector<double> > &vd){
	vector<int> indicesAleatorios(v.size());
	for(int i=0;i<v.size();i++) 
		indicesAleatorios[i]=i;
	//generamos los indices aleatorios
	random_shuffle(indicesAleatorios.begin(),indicesAleatorios.end());
	for (int j=0;j<v.size();j++)
		vd.push_back(v[indicesAleatorios[j]]);
}

void leavekout(){
	vector<vector<double> > patrones=parsearCSV(rutaEntrenamiento);
	int cantPatrones = patrones.size();
	int kpart=cantPatrones/kout;    //cantidad de particiones = cantidad de patrones/cantidad de elementos para probar
	vector<vector<double> > patronesDesordenados;
	//desordenamos el vector de patrones
	desordenaVector(patrones,patronesDesordenados);
	PerceptronMulticapa mlp(cantidadCapas,estructura,mu,alfa,curvatura);
	vector<double> entradas;
	vector<double> salidas;
	double aciertos=0.0;
	double porcentajeError=1;
	double promedioError=0;
	
	//ciclo por particiones
	for(int i=0;i<kpart;i++){
		vector<vector<double> > patronesPrueba;
		for(int p=kout*i;p<kout*i+kout;p++){
			patronesPrueba.push_back(patronesDesordenados[p]);    //guardamos patrones de prueba
			patronesDesordenados.erase(patronesDesordenados.begin()+p);
		}
		//ciclo por epocas
		int e=0;
		while(e<cantEpocas && tolerancia<porcentajeError){
			//recorremos cantpatrones-kout elementos para entrenamiento
			for(int j=0;j<patronesDesordenados.size();j++){
				for(int k=0;k<estructura[0];k++)
					entradas.push_back(patronesDesordenados[j][k]);//cargamos las entradas
				for(int k=0;k<estructura[cantidadCapas-1];k++) 
					salidas.push_back(patronesDesordenados[j][estructura[0]+k]); //cargamos las salidas
				mlp.propagacionAtras(entradas,salidas);
				entradas.clear();
				salidas.clear();
			}    //fin entrenamiento de una epoca
			
			//probamos el desempeÃ±o de la red ya entrenada
			for(int j=0;j<patronesPrueba.size();j++){
				for(int k=0;k<estructura[0];k++){
					entradas.push_back(patronesPrueba[j][k]);
				}
				for(int k=0;k<estructura[cantidadCapas-1];k++)
					salidas.push_back(patronesPrueba[j][estructura[0]+k]);
				mlp.propagacionAdelante(entradas);
				if (!mlp.error(salidas))
					aciertos++;
				entradas.clear();
				salidas.clear();
			}
			porcentajeError=1-(aciertos/patronesPrueba.size()); //porcentaje de error por epoca
			aciertos=0;
			e++;            
			
		}//Fin while por epocas
		promedioError+=porcentajeError;    
	}//Fin del for kpart
	promedioError=promedioError/(double)kpart;
	cout<<"Cantidad de Particiones: "<<kpart<<endl;
	cout<<"Datos de Entrenamiento: "<<cantPatrones-kout<<" Datos de Prueba: "<<kout<<endl;
	cout<<"Promedio de Error: "<<promedioError*100.0<<"%"<<endl;        
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
	for(int i=0;i<=10;i++)
		glVertex2f((float)i/10,0);        
	glEnd();
	glBegin(GL_POINTS);
	for(int j=0;j<=10;j++)
		glVertex2f(0,(float)j/10);
	glEnd();
	char numeros[10]={'0','1','2','3','4','5','6','7','8','9'};
	for(int i=0;i<=10;i++){
		if(i==10){
			glRasterPos2f(((float)i/10)-0.005,-0.04);
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, '1');
			glRasterPos2f(((float)i/10)+0.01,-0.04);
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, '.');
			glRasterPos2f(((float)i/10)+0.025,-0.04);
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, '0');
		}
		else{        
			glRasterPos2f(((float)i/10)-0.005,-0.04);
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, '0');
			glRasterPos2f(((float)i/10)+0.01,-0.04);
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, '.');
			glRasterPos2f(((float)i/10)+0.025,-0.04);
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, numeros[i]);
		}
	}
	for(int i=1;i<=10;i++){
		if(i==10){
			glRasterPos2f(-0.05-0.005,((float)i/10));
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, '1');
			glRasterPos2f(-0.05+0.01,((float)i/10));
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, '.');
			glRasterPos2f(-0.05+0.025,((float)i/10));
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, '0');
		}
		else{        
			glRasterPos2f(-0.05-0.005,((float)i/10));
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, '0');
			glRasterPos2f(-0.05+0.01,((float)i/10));
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, '.');
			glRasterPos2f(-0.05+0.025,((float)i/10));
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, numeros[i]);
		}
	}
	
	
	
	
	if(dibuja){
		dibujaPunto2d(rutaEntrenamiento);
	}
	glutSwapBuffers();
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
		dibuja=false;
		rutaEntrenamiento = ("concentprueba.csv");
		rutaPrueba = ("concentprueba.csv");
		tolerancia=0.01;
		estructura[0]=2;
		estructura[cantidadCapas-1]=1;
		cout<<"concentprueba.csv"<<endl;
		cout<<"La estructura es: "<<endl;
		for (int i=0;i<estructura.size();i++)
			cout<<estructura[i]<<" ";
		cout<<endl;
	}
	if (key == 'C')
	{
		dibuja=false;
		rutaEntrenamiento = ("clouds.csv");
		rutaPrueba = ("clouds.csv");
		double cantEpocas = 1000;
		tolerancia=0.1;
		estructura[0]=2;
		estructura[cantidadCapas-1]=1;
		cout<<"clouds.csv"<<endl;
		cout<<"La estructura es: "<<endl;
		for (int i=0;i<estructura.size();i++)
			cout<<estructura[i]<<" ";
		cout<<endl;
	}
	if (key == 'i')
	{
		dibuja=false;
		rutaEntrenamiento = ("irisprueba.csv");
		rutaPrueba = ("irisprueba.csv");
		tolerancia=0.15;
		cout<<"irisprueba.csv"<<endl;
		estructura[0]=4;
		estructura[cantidadCapas-1]=2;
		cout<<"La estructura es: "<<endl;
		for (int i=0;i<estructura.size();i++)
			cout<<estructura[i]<<" ";
		cout<<endl;
	}
	if (key == 'p')
	{
		dibuja=false;
		rutaEntrenamiento = ("phoneme.csv");
		rutaPrueba = ("phoneme.csv");
		tolerancia=0.15;
		cout<<"phoneme.csv"<<endl;
		estructura[0]=4;
		estructura[cantidadCapas-1]=1;
		cout<<"La estructura es: "<<endl;
		for (int i=0;i<estructura.size();i++)
			cout<<estructura[i]<<" ";
		cout<<endl;
	}
	if(key == 'l'){ //cantidad de capas +1
		cantidadCapas+=1;
		cout<<"Cantidad de capas: "<<cantidadCapas<<endl;    
	}
	if(key == 'L'){ //cantidad de capas -1
		cantidadCapas-=1;
		if (cantidadCapas <4)
			cantidadCapas=4;
		cout<<"Cantidad de capas: "<<cantidadCapas<<endl;
	}
	if(key == 'g'){ //estructura de la red
		int e;
		cout<<"Ingrese el numero de capas (>=4)"<<endl;
		cin>>cantidadCapas;
		while (cantidadCapas<4) {
			cout<<"Las capas no pueden ser menos de 4. Ingrese nuevamente: ";
			cin>>cantidadCapas;
		}
		for(int i=1;i<cantidadCapas-1;i++){
			cout<<"Ingrese la cantidad de neuronas de la capa "<<i<<": "<<endl;
			cin>>e;
			estructura[i]=e;
		}
		cout<<"La estructura es: "<<endl;
		for (int i=0;i<estructura.size();i++)
			cout<<estructura[i]<<" ";
		cout<<endl;
	}
	if(key == 'v'){    //metodo leave k-out
		cout<<"Ingrese k elementos de prueba para metodo leave-k-out: "<<endl;
		cin>>kout;
		while (kout<0) {
			cout<<"ERROR: k<0. Ingrese nuevamente "<<endl;
			cin>>kout;
		}
		leavekout();
		dibuja=false;
		
		
		
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
	if (key == 'k') // curvatura
	{    
		curvatura++;
		cout<< "Curvatura: "<<curvatura<<endl;
	}
	if (key == 'K') // curvatura
	{    
		curvatura--;
		if(curvatura<1) curvatura=1;
		cout<< "Curvatura: "<<curvatura<<endl;
	}
	if (key== 13) //entrenar
	{
		salidaPrueba.clear();
		calcular();
		dibuja=true;
	}
	display_cb();
}    

void initialize() {
	glutInitDisplayMode (GLUT_RGBA|GLUT_DOUBLE);
	glutInitWindowSize (640,480);
	glutInitWindowPosition (500,100);
	glutCreateWindow ("Perceptron Multicapa");
	glutDisplayFunc (display_cb);
	glutReshapeFunc (reshape_cb);
	glutKeyboardFunc(Keyboard_cb);
	glClearColor(1.f,1.f,1.f,1.f);
	//InicializaciÃ³n por defecto de la estructura de la red
	estructura[0]=4;
	estructura[1]=3;
	estructura[2]=4;
	estructura[3]=2;
	//estructura[4]=1;
	cout<<"---------------------------------------------"<<endl;
	cout<<"Perecptron Multicapa"<<endl;
	cout<<"-------------Seleccion de archivo------------"<<endl;
	cout<<"concentprueba.csv --> c"<<endl;
	cout<<"iris.csv --> i"<<endl;
	cout<<"phoneme.csv --> p"<<endl;
	cout<<"clouds.csv --> C"<<endl;
	cout<<"-------------Comandos------------------------"<<endl;
	cout<<"+1 Cantidad de capas --> l"<<endl;
	cout<<"-1 Cantidad de capas --> L"<<endl;
	cout<<"+1 curvatura de la sigmoidea --> k"<<endl;
	cout<<"-1 curvatura de la sigmoidea --> K"<<endl;
	cout<<"Seleccionar entradas / neuronas x capa / salidas --> g"<<endl;
	cout<<"+0.1 Tasa de aprendizaje --> m"<<endl;    
	cout<<"+1% Tolerancia a errores --> e "<<endl;
	cout<<"-1% Tolerancia a errores --> E "<<endl;
	cout<<"+10 Cantidad de epocas --> +"<<endl;
	cout<<"-10 Cantidad de epocas --> -"<<endl;
	cout<<"Entrenar/Probar --> ENTER "<<endl;
	cout<<"-------------Metodo de validacion------------"<<endl;
	cout<<"Ingresar k para metodo leave-k out --> v"<<endl;    
	cout<<"------------Valores Iniciales----------------"<<endl;
	cout<<"Tasa de aprendizaje = "<<mu<<endl;
	cout<<"Curvatura = "<<curvatura<<endl;
	cout<<"Termino de Momento = "<<alfa<<endl;
	cout<<"Cantidad de epocas = "<<cantEpocas<<endl;
	cout<<"Tolerancia a errores = "<<tolerancia*100<<"%"<<endl;
	cout<<"Cantidad de capas de la red = "<<cantidadCapas<<endl;
	cout<<"La estructura inicial de la red es: "<<endl;
	for (int i=0;i<estructura.size();i++)
		cout<<estructura[i]<<" ";
	cout<<endl;
	cout<<"---------------------------------------------"<<endl;
}

int main (int argc, char **argv) {
	glutInit (&argc, argv);
	initialize();
	glutMainLoop();
	return 0;
}
