/*--------------- TCC ---------------*/
/*        Victor De Cia Costa        */
/*               77551               */
/*---------- Version 1.0.0 ----------*/

/* Código para fazer o teste da rede autoencoder "autoencoder.net"
   compile:
	gcc autoencoder-test.c -o test -lfann -lm 
   use:
	./test < iris.test
*/

#include <stdlib.h>
#include <stdio.h>
#include "fann.h"

int main(){
	// Simple execution
	int rows, cols, i, j;
	float **testData;

	scanf("%d %d", &rows, &cols);

	fann_type *calc_out;
	fann_type input[cols];
	struct fann *ann = fann_create_from_file("autoencoder.net");

	// ler os dados de teste
	testData = (float **)malloc(rows * sizeof(float *));
	for (i=0; i < rows; i++){
		testData[i] = (float *)malloc(cols * sizeof(float));
		for (j=0; j < cols; j++){
			scanf("%f", &testData[i][j]);
		}
	}
	
	// fazer o teste para cada linha da matriz de testes
	printf("\n---------------- TESTES ---------------\n");
	for (i=0; i < rows; i++){
		for (j=0; j < cols; j++) input[j] = testData[i][j];
		calc_out = fann_run(ann, input);
		printf("\n\n%d\tInput:  ", i);
		for (j=0; j < cols; j++) printf("%f ", input[j]);
		printf("\n\tOutput: ");
		for (j=0; j < cols; j++) printf("%f ", calc_out[j]);
	}
	printf("\n-----------------------------------\n\n");

	fann_destroy(ann);
	return 0;
}
