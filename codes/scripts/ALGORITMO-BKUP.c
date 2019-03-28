/*--------------- TCC ---------------*/
/*        Victor De Cia Costa        */
/*               77551               */
/*-----------------------------------*/

/* Codigo TCC
   compile:
	 gcc ALGORITMO.c -o algoritmo -lfann -lm
   use:
	./algoritmo < datasetname.data
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "fann.h"

#define CHARSIZE 20
#define KNN_N 3
#define EPSILON 0.1
#define MAX_EPOCAS 10000
#define HIDDEN 5

int treina_autoencoder (int d, float **hidden_weights){

	struct fann_connection *con;   /* weight matrix */
	unsigned int connum;           /* connections number */
	size_t i;

	// Simple training
	const char *dataset = "autoencoder_trainning.data";
	const char *netName = "autoencoder.net";
	const unsigned int num_layers = 3;
	const unsigned int num_input = d;
	const unsigned int num_output = d;
	const unsigned int num_neurons_hidden = d/2;
	const float desired_error = (const float) 0.001;
	const unsigned int max_epochs = MAX_EPOCAS;
	const unsigned int epochs_between_reports = 10000;

	struct fann *ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);

	fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
	fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

	fann_train_on_file(ann, dataset, max_epochs, epochs_between_reports, desired_error);

	/*----- IMPRIME OS PESOS DOS NEURONIOS -----*/
	connum = fann_get_total_connections(ann);
	if (connum == 0) {
		fprintf(stderr, "Error: connections count is 0\n");
		return EXIT_FAILURE;
	}

	con = calloc(connum, sizeof(*con));
	if (con == NULL) {
		fprintf(stderr, "Error: unable to allocate memory\n");
		return EXIT_FAILURE;
	}

	/* Get weight matrix */
	fann_get_connection_array(ann, con);

	/* Print weight matrix */
	for (i = 0; i < connum; ++i) {
		if (con[i].to_neuron <= d + (d/2)) {
			//printf("weight from %u to %u: %f\n", con[i].from_neuron, con[i].to_neuron, con[i].weight);
			hidden_weights[con[i].to_neuron%(d+1)][con[i].from_neuron] = con[i].weight;
		}
	}
	free(con);

	/* Print the connections *
	fann_print_connections(ann); */

	/*----- //IMPRIME OS PESOS DOS NEURONIOS -----*/

	fann_save(ann, netName);
	fann_destroy(ann);

	return 0;
}

void print_dataset (float **data, char **classe, int rows, int cols){
	int i, j;

	for (i=0; i < rows; i++){
		for(j=0; j < cols; j++) printf("%.4f ", data[i][j]);
		printf("%s\n", classe[i]);
	}
}

void print_dataset_to_file (float **data, int rows, int cols){
	FILE *fp, *fdataset;
	int i, j;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	//srand(time(NULL));

	fp = fopen("dataset.temp", "w");
	if (fp == NULL) exit(EXIT_FAILURE);
	//fprintf(fp, "%d %d %d\n", rows, cols, cols);
	for (i=0; i < rows; i++){
		for(j=0; j < cols-1; j++) fprintf(fp, "%f ", data[i][j]);
		fprintf(fp, "%f\n", data[i][cols-1]);
		/*for(j=0; j < cols-1; j++) fprintf(fp, "%f ", data[i][j]);
		fprintf(fp, "%f\n", data[i][cols-1]);*/
	}
	fclose(fp);
	
	/* fazer o embaralhamento das linhas */
	system("shuf dataset.temp > autoencoder_trainning.temp");
	
	/* montar o arquivo para o autoencoder */
	fp = fopen("autoencoder_trainning.temp", "r");
	fdataset = fopen("autoencoder_trainning.data", "w");

	if (fp == NULL) exit(EXIT_FAILURE);
	if (fdataset == NULL) exit(EXIT_FAILURE);

	fprintf(fdataset, "%d %d %d\n", rows, cols, cols);
		while ((read = getline(&line, &len, fp)) != -1) {
		fprintf(fdataset,"%s", line);
		fprintf(fdataset,"%s", line);
	}

	fclose(fp);
	fclose(fdataset);
	if (line) free(line);

	/* removendo arquivo temporário */
	unlink("autoencoder_trainning.temp");
	unlink("dataset.temp");
}

float dist_euclidiana(float *u, float *v, int t) {
	int i;
	float sum = 0;
	float aux;

	for (i = 0; i < t; i++) {
		aux = v[i] - u[i];
		sum += aux * aux;
	}

	return sqrt(sum);
}

float Sigmoid(float x) {
    return 2 / (1 + exp(-2 * x)) - 1;
}

void kNearestNeighbors(size_t rows, float dist[rows][rows], int row){
	int i, j, k;
	float aux;
	int min;
	int KNN[row][row];	
	
	/*--- inicializar ---*/
	for(i = 0; i < row; i++){
		for(j = 0; j < row; j++){
			KNN[i][j] = 0;
		}
	}

	/*printf("\nDistancias:\n");
	for(i = 0; i < row; i++){
		for(j = 0; j < row; j++){
			printf("%.3f ", dist[i][j]);
		}
		printf("\n");
	}
	printf("\n");*/

	for(i = 0; i < row; i++){
		for (k = 0; k < KNN_N; k++) {
			min = 0;
			aux = 100000000000;
			for(j = 0; j < row; j++){
				if ((dist[i][j] < aux) && (i != j) && (KNN[i][j] != 1)) {
					min = j;
					aux = dist[i][j];
				}
			}
			KNN[i][min] = 1;
		}
		for(j = 0; j < row; j++){
			printf("%d ", KNN[i][j]);
		}
		printf("\n");
	}
}

void epsilonCut(size_t rows, float dist[rows][rows], int row){
	int i, j;
	int corteEpsilon[row][row];	
	
	/*--- inicializar ---*/
	for(i = 0; i < row; i++){
		for(j = 0; j < row; j++){
			corteEpsilon[i][j] = 0;
		}
	}

	for(i = 0; i < row; i++){
		for(j = 0; j < row; j++){
			if ((dist[i][j] < EPSILON) && (i != j)) {
				corteEpsilon[i][j] = 1;
			}
			printf("%d ", corteEpsilon[i][j]);
		}
		printf("\n");
	}
}

void cria_matrix_adj(float **inst, float **peso, int row, int col){
	int i, j, k;
	float distancia[row][row];
	float instanciaFiltrada[row][col/2];

	/*--- aplica o filtro autoencoder nas instancias ---*/
	for (k = 0; k < row; k++) {
		/*--- SEM FILTRO ---*
		for (j = 0; j < col; j++) {
			printf("%f ", inst[k][j]);
		}
		printf("\n");
		/*--- //SEM FILTRO ---*/

		for (i = 0; i < col/2; i++) {
			for (j = 0; j < col; j++) {
				instanciaFiltrada[k][i] += inst[k][j] * peso[i][j+1];
			}
			instanciaFiltrada[k][i] = Sigmoid(instanciaFiltrada[k][i]);
			//printf("%f ", instanciaFiltrada[k][i]);
		}
		//printf("\n");
	}

	/*--- MATRIZ DE DISTANCIAS ---*/
	for (i = 0; i < row ; i++){
		for (j = 0; j < row; j++){
			distancia[i][j] = dist_euclidiana(instanciaFiltrada[i],instanciaFiltrada[j], col/2);
			//printf("%f ", distancia[i][j]);
		}
		//printf("\n");
	}

	//printf("\n------------ KNN ---------------\n");
	kNearestNeighbors(row, distancia, row);
	//printf("\n------------ CORTE EPSILON ---------------\n");
	//epsilonCut(row, distancia, row);
}

int main (int argc, char *argv[]){
	float **instancia, **instanciaAutoencoder, **weights;
	char **classe;
	float min = 0, max = 0;
	int rows, cols, n_class, i, j, k;

	scanf("%d %d %d", &rows, &cols, &n_class);

	/* alocar matriz do dataset */
	instancia = (float **)malloc(rows * sizeof(float *));
	instanciaAutoencoder = (float **)malloc(rows * sizeof(float *));
	classe = (char **)malloc(rows * sizeof(char *));
	for (i = 0; i < rows; i++) {
		instancia[i] = (float *)malloc(cols * sizeof(float));
		instanciaAutoencoder[i] = (float *)malloc(cols * sizeof(float));
		classe[i] = (char *)malloc(CHARSIZE * sizeof(char));
	}

	/*                 LER DATASET
	   para o autoencoder o dataset é normalizado */
	for (i = 0; i < rows; i++){
		for(j = 0; j < cols; j++){
			scanf("%f", &instancia[i][j]);
			instanciaAutoencoder[i][j] = instancia[i][j];
			if (instanciaAutoencoder[i][j] < min) min = instanciaAutoencoder[i][j];
			if (instanciaAutoencoder[i][j] > max) max =  instanciaAutoencoder[i][j];
		}
		scanf("%s", classe[i]);
	}
	for (i=0; i < rows; i++){
		for (j=0; j < cols; j++){
			instanciaAutoencoder[i][j] = (instanciaAutoencoder[i][j] - min) / (max - min);
		}
	}

	//printf("\n---------- Dataset Original ----------\n");
	//print_dataset(instancia, classe, rows, cols);
	//printf("\n--------- Dataset Normalizado ----------\n");
	//print_dataset(instanciaAutoencoder, classe, rows, cols);

	/* aloca a matrix dos pesos das camadas ocultas
	   para a criacao do grafo Knn/corte E          */
	weights = (float **)malloc((cols/2) * sizeof(float *));
	for (i = 0; i < (cols + 1); ++i) weights[i] = (float *)malloc((cols + 1) * sizeof(float));	
	
	/* treina o autoencoder */
	print_dataset_to_file(instanciaAutoencoder, rows, cols);
	treina_autoencoder(cols, weights);

	/* imprime matriz de pesos *
	printf("\n");
	for (i = 0; i < (cols/2); ++i){
		for (j = 0; j < (cols+1); ++j){
			printf("%f ", weights[i][j]);
		}
		printf("\n");
	} */

	/* cria a matrix de distancias utilizando os pesos das camadas ocultas do autoencoder */
	cria_matrix_adj(instanciaAutoencoder, weights, rows, cols);
	//cria_matrix_adj(instancia, weights, rows, cols);

	/* desalocar memória */
	for (i = 0; i < rows; ++i) {
		free(instancia[i]);
		free(instanciaAutoencoder[i]);
		free(classe[i]);
	}
	free(instancia);
	free(instanciaAutoencoder);
	free(classe);
	return 0;
}
