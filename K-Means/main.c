//
//  main.c
//  K-Means
//
//  Created by Miguel Garcia on 9/12/17.
//  Copyright © 2017 Miguel Garcia. All rights reserved.
//

#include "header.h" //Importa header con las funciones necesarias




//Funcion principal
int main(argc, argv)
int argc;
char * argv[];
{
    //Arroja un mensaje de error en caso de no encontrar alguno de los ficheros de texto necesarios para la ejecucion
    if ((fp_data=fopen(argv[1], "r"))==NULL) {
        fprintf(stderr, "No file datos %s\n",argv[1]);
        return 0;
    }
    if ((fp_data=fopen(argv[4], "r"))==NULL) {
        fprintf(stderr, "No file centriodes %s\n",argv[4]);
        return 0;
    }
    if ((fp_data=fopen(argv[6], "a"))==NULL) {
        fprintf(stderr, "No file resultados %s\n",argv[6]);
        return 0;
    }
    
    
    iteracion=0;    //Se inicializa la variable que contará las iteraciones
    gettimeofday(&t0, 0);   //Se obtiene el tiempo actual a partir de esta funcion, y se almacenan los segundos y microsegundos en la variable de tiempo inicial
    
    mejoraFlag=atoi(argv[7]);
    iteracionMejora=atoi(argv[8]);
    
    inicializacion(argc,argv);  //Se inicializa lo necesario para comenzar la ejecucion
    
    fp_data=fopen(argv[1],"r"); //Se abre en modo lectura el primer argumento recibido, siendo este un fichero con los datos de la muestra y se asigna a su variable reservada
    readData(fp_data, nData,nFeatures, datos);  //Se leen los datos contenidos en este archivo y se almacenan en una matriz
    fclose(fp_data); //Se cierra el fichero abierto
    
    fp_centroides=fopen(argv[4],"r");   //Se abre en modo lectura el fichero con los centroides iniciales y se asigna a su variable reservada
    readData(fp_centroides, nCentroides, nFeatures, centroidesPrincipales);    //Se leen los centroides contenidos y se almacenan en una matriz
    fclose(fp_centroides);  //Se cierra el fichero abierto
    
    //centroidesNuevos=centroides; //Se crea una copia identica de los centroides iniciales en una matriz secundaria
    
    
    //Comienza el calculo del algoritmo
    do {
        asignacion(datos,centroidesPrincipales,nData,nFeatures,nCentroides); //Se re-asigna grupo a los datos segun el recalculo
        recalculo(datos, centroidesPrincipales, nData, nFeatures, nCentroides);  //Se re-calcula la posicion de los centroides con los grupos reasignados
        
        iteracion++;    //Se incrementa la variable de iteracion tras cada ciclo
        
    }
    while(change==1);   //Si se detecta algun cambio en el recalculo el algoritmo continua
    //Termina el algoritmo
    
    
    gettimeofday(&t1, 0);   //Se obtiene el tiempo actual, los segundos y microsegundos se almacenan en la variable reservada para ello
    
    
    
    //Error de agrupamiento
    double error=sumatoria_error(datos,nData,centroidesPrincipales,nCentroides,nFeatures);
    
    
    //Tiempo en milisegundos
    elapsed = timedifference_msec(t0, t1);
    
    //Se guarda el tiempo, el error y el numero de iteraciones en el fichero de resultados
    fp_resultados = fopen(argv[6],"a"); //Se abre el fichero
    fprintf(fp_resultados,"%.16g\t %lf\t %d\n", elapsed, error, iteracion); //Imprime en el archivo
    fclose(fp_resultados);  //Se cierra el fichero
    
//    for(int i=0;i<nCentroides;i++){
//        for(int j=0;j<nFeatures;j++){
//            printf("%lf",centroidesPrincipales[i][j]);
//            printf("\n");
//        }
//    }
    
    return 0;
}



