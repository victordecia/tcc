/*--------------- TCC ---------------*/
/*        Victor De Cia Costa        */
/*               77551               */
/*-----------------------------------*/

/* Codigo TCC
   compile:
	 gcc ALGORITMO.c -o algoritmo -lfann -lm
   use:
	./algoritmo datasetname.data "datasetname"
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "fann.h"

void print_dataset (float **data, int rows, int cols, int n_class){
	int i, j, k;

	for (i=0; i < rows; i++){
		for(j=0; j < cols; j++) printf("%.4f ", data[i][j]);
		for(k=j; k < cols + n_class - 1 ; k++) printf("%.0f ", data[i][k]);
		printf("%.0f\n", data[i][cols + n_class - 1]);
	}
}

int main (int argc, char *argv[]){
	float **dataset;
	char **classe;
	float min, max;
	int rows, cols, n_class, i, j, k;

	scanf("%d %d %d", &rows, &cols, &n_class);

	/* alocar matriz do dataset */
	dataset = (float **)malloc(rows * sizeof(float *)); 
	for (i = 0; i < rows; ++i) dataset[i] = (float *)malloc((cols + n_class) * sizeof(float)); 

	/* ler o dataset sem normalização
	for (i=0; i < rows; i++){
		for(j=0; j < cols + n_class; j++){
			scanf("%f", &dataset[i][j]);
		}
	}*/

	/* ler o dataset e já fazer normalização dos dados */
	for (i = 0; i < rows; i++){
		for(j = 0; j < cols; j++){
			scanf("%f", &dataset[i][j]);
			if (dataset[i][j] < min) min = dataset[i][j];
			if (dataset[i][j] > max) max = dataset[i][j];
		}
		for(k = j; k < cols + n_class; k++)	scanf("%f", &dataset[i][k]);
	}

	/* fazer a normalizacao */
	for (i=0; i < rows; i++){
		for (j=0; j < cols; j++){
			dataset[i][j] = (dataset[i][j] - min) / (max - min);
		}
	}

	//print_dataset(dataset, rows, cols, n_class);

	

	/* desalocar dataset */
	for (i = 0; i < rows; ++i) free(dataset[i]);
	free(dataset);
	return 0;
}



















