//
//  header.h
//  K-Means
//
//  Created by Miguel Garcia on 9/12/17.
//  Copyright © 2017 Miguel Garcia. All rights reserved.
//

#ifndef header_h
#define header_h

//
//  main.c
//  K-Means
//
//  Created by Miguel Garcia on 8/30/17.
//  Copyright © 2017 Miguel Garcia. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

int nData,nFeatures, nCentroides;   //Definicion de variables, tamaño de muestra, numero de caracteristicas y numero de centroides
double **datos,**centroidesPrincipales;    //Arreglos dinamicos de dos dimensiones para, matriz de la muestra, matriz de los centroides y matriz de centroides recalculados
static FILE *fp_data, *fp_centroides,*fp_resultados;    //Instancias de la clase FILE para manipular tres ficheros de texto, los cuales contiene la muestra, los centroides iniciales y el archivo donde se alojaran los resultados respectivamente


struct timeval t0;  //Instancias de la estructura timeval para acceder a sus segundos y mcirosegundos
struct timeval t1;
double elapsed; //Variable donde se alojará el tiempo transcurrido desde el inicio al fin del calculo del algoritmo

int change; //Flag que indica si hubo algun cambio al recalcular los centroides
int iteracion;  //Aloja el numero de iteraciones necesarias para agrupar los datos

int iteracionMejora=10; //Indica la iteracion desde la cual comenzará a realizarse la mejora
int mejoraFlag=0; //Indica si se aplicara la mejora o no, 0 es falso, 1 es verdadero

int cambioUnCentroide; //Indica si ocurrió un cambio alguno de los centroides evaluado

//Declaracion de las funciones
void readData(FILE *data, int Ndata,int NFeatures,double **datos);//
void inicializacion(int argc, char *argv[]);
void asignacion(double **datos,double **centroides,int nData, int nFeatures, int nCentroides);
void recalculo(double **datos, double **centroides,int nData, int nFeatures, int nCentroides);
double timedifference_msec(struct timeval t0, struct timeval t1);
double sumatoria_error(double **datos,int nData,double **centroides,int nCentroides, int nFeatures);
void centroideEstable(int k);

//Funcion que lee los datos de un fichero y los aloja en una matriz con el mismo numero de columnas y filas
void readData(FILE *data, int Ndatos,int Nfeatures,double **datos){
    int i;
    int j;
    
    char palabra[30];
    
    
    for (i=0;i<Ndatos; i++) {
        for (j=0; j<Nfeatures; j++) {
            fscanf(data, "%s",palabra);
            datos[i][j]=atof(palabra);
        }
    }
}

//Funcion que inicializa las varibales necesarias para el algoritmo
void inicializacion(int argc, char *argv[]){
    
    //A partir de los parametros se inicializan las variables
    nData=atoi(argv[2]);    //Numero de datos en la muestra
    nFeatures=atoi(argv[3]);    //Numero de caracteristicas de los datos
    nCentroides=atoi(argv[5]);  //Numero de centroides
    
    datos=(double **) malloc(nData*sizeof(double *));   //Se inicializan las filas de la matriz bidimensional dinamica de la muestra
    
    int i;
    for(i=0;i<nData;i++){
        datos[i]=(double *)malloc((nFeatures+2)*sizeof(double));    //Se inicializan las columnas de la matriz con una adicional para el grupo asignado
    }
    
    
    centroidesPrincipales=(double **) malloc(nCentroides*sizeof(double *)); //Se inicializan las filas de la matriz bidimensional dinamica de los centroides
    for(i=0;i<nCentroides;i++){
        centroidesPrincipales[i]=(double *)malloc((nFeatures+1)*sizeof(double));   //Se inicializan las columnas de la matriz de los centroides
    }
    
    
}

//Funcion que asigna un grupo a cada objeto en la muestra de acuerdo a los centroides actuales
void asignacion(double **datos,double **centroides,int nData, int nFeatures, int nCentroides){
    
    int i,j,k,group;
    double distancia;
    
    
    for (i=0; i<nData&&datos[i][nFeatures+1]!=1; i++) {   //Recorre las filas de la muestra de los datos
        
        datos[i][nFeatures]=pow(10, 8); //Asigna un valor muy alto en la columna de grupo en el indice actual
        group=0;    //Se re-inicia la varible grupo para cada fila de datos
        
        for (k=0; k<nCentroides; k++) { //Recorre segun la cantidad de centroides
            
            distancia=0; //Limpia la variable distancia tras el calculo a cada centroide
            
            for (j=0; j<nFeatures; j++) {   //Recorre tantas veces como caracteristicas tenga un objeto
                distancia+=pow(datos[i][j]-centroides[k][j], 2);    //Obtiene la distancia de cada caracteristica y la suma temporalmente
            }
            distancia=sqrt(distancia);  //Calcula la distancia final hacia el centroide
            if (distancia<datos[i][nFeatures]){ //Si la distancia es menor a la almacenada en la columna de grupo, esta se actualiza
                
                datos[i][nFeatures]=distancia;  //Actualiza la distancia
                group=(k+1);    //Almacena el grupo prospecto previo a asignarlo al objeto
            }
            if (k==nCentroides-1) { //Si ya se evaluó el ultimo centroide
                datos[i][nFeatures]=group;  //Se asgina el grupo al objeto
            }
        }
    }
    
}

//Funcion que recalcula la posicion de los centroides
void recalculo(double **datos, double **centroides,int nData, int nFeatures, int nCentroides){
    
    double sum; //Variable que almacena la suma de las caracteristicas en cada columna para un grupo
    int size;   //Numero de ocasiones que se repitio ese grupo
    change=0;   //Bandera de cambio en los centroides
    int i,j,k;
    for (k=1; k<=nCentroides&&centroidesPrincipales[k-1][nFeatures]!=1; k++) {    //Recorre segun la cantidad de centroides
        for (j=0; j<nFeatures; j++) {   //Recorre segun la cantidad de caracteristicas
            sum=0;  //Inicializa las variable
            size=0;
            
            cambioUnCentroide=0;
            
            for (i=0; i<nData; i++) {   //Recorre segun la cantidad de datos en la muestra
                if ((int)datos[i][nFeatures]==k) {  //Evalua el grupo del objeto indicado por el ciclo mas externo
                    sum+=datos[i][j];   //En dado caso suma el valor de dicha caracteristica
                    size++; //Incrementa la cantidad de objetos en ese grupo
                }
            }
            double caracteristicaCentroide=((double)sum/(double)size);    //Recalcula el centroide
            
            if ((double)centroidesPrincipales[k-1][j]!=caracteristicaCentroide&&isnan(caracteristicaCentroide)==0) { //Evalua el centroide actual en caso de que hubiese un cambio
                change=1;   //Activa la bandera de cambio
                cambioUnCentroide=1;
                centroidesPrincipales[k-1][j]=((double)sum/(double)size);    //Actualiza el centroide
            }
            //Al terminar de evaluar un centroide se determina si no ha cambiado de lugar
            else if(j==nFeatures-1&&cambioUnCentroide==0&&iteracion>=iteracionMejora&&mejoraFlag==1){
                //De ser asi se establece como estable
                centroidesPrincipales[k-1][nFeatures]=1; //La ultima columna del centroide se activa como verdadera
                centroideEstable(k); //Se llama a la funcion correspondiente que establece como estable a todo el grupo del ahora centroide estable
            }
        }
    }
}

//Fucion que establece como estables los objetos correspodientes a un centroide estable
void centroideEstable(int k){
    for (int i=0; i<nData; i++) {
        if (datos[i][nFeatures]==(double)k) {
            datos[i][nFeatures+1]=1;
        }
    }
}

//Funcion que calcula la sumatoria del error al cuadrado
double sumatoria_error(double **datos,int nData,double **centroides,int nCentroides, int nFeatures ){
    int i,j,k;
    double error=0; //Variable que almacenará el error
    double distancia;
    
    for (k=1; k<=nCentroides; k++) {    //Recorre segun la cantidad de centroides
        for (i=0; i<nData; i++) {   //Recorre segun la cantidad de datos en la muestra
            distancia=0;    //Inicializa la distancia tras cada objeto
            for (j=0; j<nFeatures; j++) {   //Recorre segun la cantidad de caracteristicas
                if (datos[i][nFeatures]==k) {   //Evalua el grupo del objeto
                    distancia+=pow(datos[i][j]-centroides[k-1][j], 2);  //Almacena la distancia de esta caracteristia hacia la del centroide
                }
            }
            error+=sqrt(distancia); //Calcula y suma el error completo hacia el centroide
        }
    }
    return error;   //Devuelve el error total
}

//Funcion que calcula el tiempo en milisegundos
double timedifference_msec(struct timeval t0, struct timeval t1) //Recibe el tiempo inicial y final del computo
{
    return (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
}

#endif /* header_h */
