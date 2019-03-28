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

#define KNN_N 3
#define EPSILON 0.01
#define MAX_EPOCAS 10000
#define HIDDEN 4
#define CROSSVALIDATION 10

void print_dataset (float **data, int *classe, int rows, int cols){
	int i, j;

	for (i=0; i < rows; i++){
		for(j=0; j < cols; j++) printf("%f ", data[i][j]);
		printf("%d\n", classe[i]);
	}
}

void datasetToFile (float **data, int rows, int cols, char name[27]){
	FILE *fp;
	int i, j;

	fp = fopen(name, "w");
	if (fp == NULL) exit(EXIT_FAILURE);
	fprintf(fp, "%d %d\n", rows, cols);
	for (i=0; i < rows; i++){
		for(j=0; j < cols-1; j++) fprintf(fp, "%f ", data[i][j]);
		fprintf(fp, "%f\n", data[i][cols-1]);
	}
	fclose(fp);
}

void datasetToFileShuffle (float **data, int rows, int cols, char name[27]){
	FILE *fp, *fdataset;
	int i, j;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	//srand(time(NULL));

	fp = fopen("dataset.temp", "w");
	if (fp == NULL) exit(EXIT_FAILURE);
	for (i=0; i < rows; i++){
		for(j=0; j < cols-1; j++) fprintf(fp, "%f ", data[i][j]);
		fprintf(fp, "%f\n", data[i][cols-1]);
	}
	fclose(fp);
	
	/* fazer o embaralhamento das linhas */
	system("shuf dataset.temp > dataset_shuffled.temp");
	
	/* montar o arquivo para o autoencoder */
	fp = fopen("dataset_shuffled.temp", "r");
	fdataset = fopen(name, "w");

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
	unlink("dataset_shuffled.temp");
	unlink("dataset.temp");
}

int treinaAutoencoder (float **instancia, int row, int col, float **hidden_weights, int kFolds){
	struct fann_connection *con;   /* weight matrix */
	unsigned int connum;           /* connections number */
	//size_t i;
	int i;

	char trainingFile[27] = "treino.data";
	char testFile[27] = "teste.data";
	const char *netName = "autoencoder.net";
	const char *dataset = trainingFile;
	const unsigned int num_layers = 3;
	const unsigned int num_input = col;
	const unsigned int num_output = col;
	const unsigned int num_neurons_hidden = HIDDEN;
	const float desired_error = (const float) 0.001;
	const unsigned int max_epochs = MAX_EPOCAS;
	const unsigned int epochs_between_reports = 10;
	fann_type *calc_out;
	fann_type inputTest[col];

	float erro, validacao, acuracia;
	int qtdFold = row/kFolds;
	int k, j, n, m, p;
	float **instanciaTreino, **instanciaTeste;

	/* cria a rede com os parâmetros definidos */
	struct fann *ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);

	/* confirugações da rede */
	fann_set_activation_function_hidden(ann, FANN_SIGMOID_STEPWISE);
	fann_set_activation_function_output(ann, FANN_SIGMOID_STEPWISE);
	fann_set_training_algorithm(ann, FANN_TRAIN_RPROP);

	/* alocando os folds */
	instanciaTreino = (float **) malloc((row - qtdFold) * sizeof(float *));
	for(k = 0; k < (row - qtdFold); k++) instanciaTreino[k] = (float *) malloc(col * sizeof(float));
	instanciaTeste = (float **) malloc(qtdFold * sizeof(float *));
	for(k = 0; k < qtdFold; k++) instanciaTeste[k] = (float *) malloc(col * sizeof(float));

	/* laço para fazer o k-fold validation */
	erro = validacao = acuracia = 0;
	for (k = 0; k < kFolds; k++ ) {

		printf("\n---\tTreino para o %d-fold\t---\n", k);

		/* conjuntos de treino e teste */
		m = n = 0;
		for (i = 0; i < row; i++){
			if ((i >= k*kFolds) && (i < qtdFold*(k+1))) {
				instanciaTeste[m] = instancia[i];
				m++;
			} else {
				instanciaTreino[n] = instancia[i];
				n++;
			}
		}

		/*printf("\n-------- Teste -------\n");
		for(i = 0; i < qtdFold; i++) {
			for(m = 0; m < col; m++) {
				printf("%.1f ",instanciaTeste[i][m]);
			}
			printf("\n");
		}

		printf("\n-------- Treino -------\n");
		for(i = 0; i < (row - qtdFold); i++) {
			for(m = 0; m < col; m++) {
				printf("%.1f ",instanciaTreino[i][m]);
			}
			printf("\n");
		}*/

		/* criar arquivo do conjunto de treinamento*/
		datasetToFileShuffle (instanciaTreino, row - qtdFold, col, trainingFile);

		/* criar arquivo do conjunto de teste*/
		//datasetToFile (instanciaTeste, qtdFold, col, testFile);
		
		/* faz o treinamento */
		fann_train_on_file(ann, dataset, max_epochs, epochs_between_reports, desired_error);

		/* faz o teste e calcula a validação */
		//printf("\n---\tTeste\t---\n");
		erro = 0;
		for (i=0; i < qtdFold; i++){
			for (j=0; j < col; j++) inputTest[j] = instanciaTeste[i][j];
			calc_out = fann_run(ann, inputTest);
			//printf("\n\n%d\tInput:  ", i);
			//for (j=0; j < col; j++) printf("%f ", inputTest[j]);
			//printf("\n\tOutput: ");
			//for (j=0; j < col; j++) printf("%f ", calc_out[j]);

			for (j=0; j < col; j++) {
				//erro += fabs(calc_out[j] - inputTest[j]);
				erro += pow(calc_out[j] - inputTest[j], 2); // erro quadrático da instancia i
			}
			//printf("\n\t\tErro[%i]: %f\n", i, erro);
		}
		validacao += erro/qtdFold; // erro quadrático médio
		//printf("\n>>>\tValidação: %f\n\n", validacao);

		unlink(trainingFile);
		//unlink(testFile);
	}

	acuracia = validacao/kFolds;
	printf("\n\n\n>>>\tACURACIA: %f\n>>>\tHidden neurons: %d\n\n", acuracia, HIDDEN);

	/*----- RECUPERA OS PESOS DOS NEURONIOS -----*/
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
		if (con[i].to_neuron <= col + HIDDEN) {
			//printf("weight from %u to %u: %f\n", con[i].from_neuron, con[i].to_neuron, con[i].weight);
			hidden_weights[con[i].to_neuron%(col+1)][con[i].from_neuron] = con[i].weight;
		}
	}
	free(con);

	/* Print the connections *
	fann_print_connections(ann); */
	/*----- //IMPRIME OS PESOS DOS NEURONIOS -----*/

	fann_save(ann, netName);
	fann_destroy(ann);

	/* desalocando */
	for(k = 0; k < (row - qtdFold); k++) free(instanciaTreino[k]);
	for(k = 0; k < qtdFold; k++) free(instanciaTeste[k]);
	//free(instanciaTreino);
	//free(instanciaTeste);
	return 0;
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
	int **KNN;

	KNN = (int **)calloc(row, sizeof(int *));

	/*printf("\nDistancias:\n");
	for(i = 0; i < row; i++){
		for(j = 0; j < row; j++){
			printf("%.3f ", dist[i][j]);
		}
		printf("\n");
	}
	printf("\n");*/

	printf("\n--------- Geração da Rede KNN ----------\n");
	for(i = 0; i < row; i++){
		KNN[i] = (int *)calloc(row, sizeof(int));
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
		free(KNN[i]);
	}
	printf("\n--------- //Geração da Rede KNN ----------\n");
	free(KNN);
}

void epsilonCut(size_t rows, float dist[rows][rows], int row){
	int i, j;
	int **corteEpsilon;	

	corteEpsilon = (int **)calloc(row, sizeof(int *));

	printf("\n--------- Geração da Rede Corte Epsilon ----------\n");
	for(i = 0; i < row; i++){
		corteEpsilon[i] = (int *)calloc(row, sizeof(int));
		for(j = 0; j < row; j++){
			if ((dist[i][j] < EPSILON) && (i != j)) {
				corteEpsilon[i][j] = 1;
			}
			printf("%d ", corteEpsilon[i][j]);
		}
		printf("\n");
		free(corteEpsilon[i]);
	}
	printf("\n--------- //Geração da Rede Corte Epsilon ----------\n");
	free(corteEpsilon);
}

void criaMatrixAdj(float **inst, float **peso, int *classe, int row, int col){
	int i, j, k;
	float distancia[row][row];
	float instanciaFiltrada[row][HIDDEN];

	/*--- aplica o filtro autoencoder nas instancias ---*/
	for (k = 0; k < row; k++) {
		for (i = 0; i < HIDDEN; i++) {
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
			if(i != j) {
				distancia[i][j] = dist_euclidiana(instanciaFiltrada[i],instanciaFiltrada[j], HIDDEN);
				printf("%f ", distancia[i][j]);
			} else {
				distancia[i][j] = classe[i];
				printf("%.0f ", distancia[i][j]);
			}
		}
		printf("\n");
	}
	
	//kNearestNeighbors(row, dist, row);
	//epsilonCut(row, dist, row);
}

void criaMatrizDistancias(float **inst, int *classe, int row, int col){
	int i, j, k;
	float **distancia;

	distancia = (float **)calloc(row, sizeof(float *));
	for (i = 0; i < row ; i++){
		distancia[i] = (float *)calloc(row, sizeof(float));
		for (j = 0; j < row; j++){
			if(i != j) {
				distancia[i][j] = dist_euclidiana(inst[i],inst[j], col);
				printf("%f ", distancia[i][j]);
			} else {
				distancia[i][j] = classe[i];
				printf("%.0f ", distancia[i][j]);
			}
		}
		printf("\n");
		free(distancia[i]);
	}
	free(distancia);
}


int main (int argc, char *argv[]){
	float **instancia, **instanciaNormalizada, **weights;
	int *classe;
	float min = 0, max = 0;
	int rows, cols, n_class, i, j, k;

	scanf("%d %d %d", &rows, &cols, &n_class);

	/* alocar matriz do dataset */
	classe = (int *)calloc(rows, sizeof(int));
	instancia = (float **)calloc(rows, sizeof(float *));
	instanciaNormalizada = (float **)calloc(rows, sizeof(float *));
	for (i = 0; i < rows; i++) {
		instancia[i] = (float *)calloc(cols, sizeof(float));
		instanciaNormalizada[i] = (float *)calloc(cols, sizeof(float));
	}

	/*-----------  LER DATASET  -----------
	   para o autoencoder o dataset é normalizado */
	for (i = 0; i < rows; i++){
		for(j = 0; j < cols; j++){
			scanf("%f", &instancia[i][j]);
			instanciaNormalizada[i][j] = instancia[i][j];
			if (instanciaNormalizada[i][j] < min) min = instanciaNormalizada[i][j];
			if (instanciaNormalizada[i][j] > max) max =  instanciaNormalizada[i][j];
		}
		scanf("%d", &classe[i]);
	}
	for (i=0; i < rows; i++){
		for (j=0; j < cols; j++){
			instanciaNormalizada[i][j] = (instanciaNormalizada[i][j] - min) / (max - min);
		}
	}
	/*----------- //LER DATASET -----------*/

	/* aloca a matrix dos pesos das camadas ocultas
	   para a criacao do grafo Knn/corte E          */
	weights = (float **)calloc(HIDDEN, sizeof(float *));
	for (i = 0; i < (cols + 1); ++i) weights[i] = (float *)calloc((cols + 1), sizeof(float));	
	
	/*printf("\n--------- Parâmetros ----------\n");
	printf("KNN_N: %d\nEPSILON: %f\nMAX_EPOCAS: %d\nHIDDEN: %d\n", KNN_N, EPSILON, MAX_EPOCAS, HIDDEN);
	printf("\n--------- //Parâmetros ----------\n");*/

	/* treina o autoencoder */
	//printf("\n--------- Treinamento da Rede ----------\n");
	treinaAutoencoder(instanciaNormalizada, rows, cols, weights, CROSSVALIDATION);
	//printf("\n--------- //Treinamento da Rede ----------\n");

	//printf("\n---------- Dataset Original ----------\n");
	//print_dataset(instancia, classe, rows, cols);
	//printf("\n--------- Dataset Normalizado ----------\n");
	//print_dataset(instanciaNormalizada, classe, rows, cols);
	//printf("\n--------- //Dataset Normalizado ----------\n");

	/* imprime matriz de pesos *
	printf("\n");
	for (i = 0; i < (HIDDEN); ++i){
		for (j = 0; j < (cols+1); ++j){
			printf("%f ", weights[i][j]);
		}
		printf("\n");
	} */

	/* cria a matrix de distancias utilizando os pesos das camadas ocultas do autoencoder */
	criaMatrixAdj(instanciaNormalizada, weights, classe, rows, cols);

	/* Cria matriz de distancias */
	//criaMatrizDistancias(instanciaNormalizada, classe, rows, cols);

	/* desalocar memória */
	unlink("autoencoder.net");
	for (i = 0; i < rows; ++i) {
		free(instancia[i]);
		free(instanciaNormalizada[i]);
	}
	free(classe);
	free(instancia);
	free(instanciaNormalizada);
	return 0;
}
