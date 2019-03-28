#include <stdlib.h>
#include <stdio.h>

int main (){
	int i, j, k;
	int rows, cols, outs;
	float **data, trash, min, max;

	scanf("%d %d %d", &rows, &cols,&outs);

	data = (float **)malloc(rows * sizeof(float *));
	for (i=0; i < rows; i++) data[i] = (float *)malloc(cols * sizeof(float));

	// ler os dados de entrada
	for (i=0; i < rows * 2; i++){
		if (i % 2 == 0){
			for(j=0; j < cols; j++){
				scanf("%f", &data[i/2][j]);
				if (data[i/2][j] < min) min = data[i/2][j];
				if (data[i/2][j] > max) max = data[i/2][j];
			}
		} else {
			for(k=0; k < outs; k++){
				scanf("%f", &trash);
			}
		}
	}

	// fazer a normalizacao
	for (i=0; i < rows; i++){
		for (j=0; j < cols; j++){
			data[i][j] = (data[i][j] - min) / (max - min);
		}
	}

	// montar o arquivo de dados para o autoencoder	
	printf("%d %d %d\n", rows, cols, cols);	
	for (i=0; i < rows; i++){
		for(j=0; j < cols; j++) printf("%f ", data[i][j]);
		printf("\n");
		for(j=0; j < cols; j++) printf("%f ", data[i][j]);
		printf("\n");
	}

	return 0;
}
