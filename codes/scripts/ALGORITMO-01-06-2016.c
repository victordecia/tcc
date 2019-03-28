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
#define KNN 3
#define MAX_EPOCAS 10000

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

typedef struct {
	char *classe;
	float valor;
} distancia;

int cmpfunc (const void * a, const void * b) {
	distancia *A = (distancia *)a;
	distancia *B = (distancia *)b;

	if (A->valor < B->valor) return -1;
	else if (A->valor > B->valor) return 1;
}

void cria_knn(float **instancia, char **classe, float **peso, int row, int col, int n_class){
	int i, j, k, m, n, cont, max;
	char *classeAtual;

	float graphKnnPrevia[row][col];
	int class_visited[row];
	char *nomeClasse[n_class];
	int contaClasse[n_class];

	float instanciaFiltrada[row][col/2];
	float graphKnn[row + n_class][col/2];
	char *graphClass[row + n_class];

	distancia dist[row + n_class];

	i = j = k = m = n = cont = max = 0;

	/* inicializar as estruturas */
	for (i = 0; i < row; i++){
		graphClass[i] = "";
		class_visited[i] = 0;
		for (j = 0; j < col; j++){
			graphKnnPrevia[i][j] = 0.0;
			if (j < col/2){
				graphKnn[i][j] = 0.0;
				instanciaFiltrada[i][j] = 0.0;
			}
		}
	}
	for (i = row; i < row + n_class; i++){
		contaClasse[i-row] = 0; 
		graphClass[i] = "";
		for (j = 0; j < col/2; j++) graphKnn[i][j] = 0.0;
	}

	/* inicizalizar as variaveis antes do laço */
	classeAtual = classe[0];
	cont = 0;

	/*--- primeiro encontrar os centroides das classes ---*/
	for (k = 0; k < n_class; k++) {

		//printf("Classe atual: %s\tCont: %d\n", classeAtual, cont);
		graphClass[k] = classeAtual;
		nomeClasse[k] = classeAtual;

		/* faz o somatorio das instancias com a mesma classe */
		for (i = 0; i < row; i++){
			if (strcmp(classeAtual, classe[i]) == 0){
				cont++;
				/* marcar a classe como visitada */
				class_visited[i] = 1;
				//printf("%d: ", cont);
				for (j = 0; j < col; j++){
					graphKnnPrevia[k][j] += instancia[i][j];
					//printf("%f ", graphKnnPrevia[k][j]);
				}
				//printf("%s\n", graphClass[k]);
			}
		}

		/* retira a média desse valor */
		for (j = 0; j < col; j++){
			graphKnnPrevia[k][j] = graphKnnPrevia[k][j]/cont;
			//printf("%f ", graphKnnPrevia[k][j]);
		}
		//printf("\nClasse atual: %s\tCont: %d\n", classeAtual, cont);

		/* prepara as variaveis para a proxima classe */
		cont = 0;
		for (i = 0; i < row; i++) {
			if ((strcmp(classeAtual, classe[i])!=0) && (class_visited[i] != 1)) {
				classeAtual = classe[i];
				break;
			}
		}
	}

	/* imprime centroides do knn na representacao original *
	printf("\n");
	for (k = 0; k < n_class; k++) {
		for (j = 0; j < col; j++){
			printf("%f ", graphKnnPrevia[k][j]);
		}
		printf("%s\n", graphClass[k]);
	}


	/*--- Montar o grafo KNN da nova representação ---*/

	//printf("\n");
	/* Aplicar o filtro nos centroides */
	for (k = 0; k < n_class; k++) {
		for (i = 0; i < col/2; i++) {
			for (j = 0; j < col; j++) {
				graphKnn[k][i] += graphKnnPrevia[k][j] * peso[i][j+1];
				//printf("%f * %f\t", graphKnnPrevia[k][j], peso[i][j+1]);
			}
			//printf("= %f = G[%d][%d]\n", graphKnn[k][i], k, i);
		}
		//printf("\n");
	}

	/* imprime centroides do knn na representacao filtrada *
	printf("\n");
	for (k = 0; k < n_class; k++) {
		for (j = 0; j < col/2; j++){
			printf("%f ", graphKnn[k][j]);
		}
		printf("%s\n", graphClass[k]);
	}*/


	/*--- aplica o filtro autoencoder nas instancias e cria o grafo knn ---*/
	for (m = 0; m < row; m++) { /* para cada instancia */
		//printf("\n---------- Instancia %d ----------\n\n", m);
		/* aplica o filtro autoencoder */
		//printf("Instancia normal: ");
		//for(i = 0; i < col; i++) printf("%f ", instancia[m][i]);
		//printf("\n");
		//printf("Instancia filtrada: ");
		for (i = 0; i < col/2; i++) {
			for (j = 0; j < col; j++) {
				instanciaFiltrada[m][i] += instancia[m][j] * peso[i][j+1];
			}
			printf("%f,", instanciaFiltrada[m][i]);
		}
		printf("%s\n", classe[m]);

		/* medir as distâncias da nova instância até que já estão no grafo knn */
		//printf("Distâncias da nova instância até as que já estão no grafo:\n");
		for (i = 0; i < n_class + m; i++) {
			dist[i].classe = graphClass[i];
			dist[i].valor = dist_euclidiana(graphKnn[i], instanciaFiltrada[m], col/2);
			//printf("M = %d até i = %d (%s): %f\n", m, i, dist[i].classe, dist[i].valor);
		}
		//printf("\n");

		/* ordena as distâncias */
		//printf("\nNão ordenado:\n"); for (i = 0; i < n_class + m; i++) printf("%s %f\n", dist[i].classe, dist[i].valor);
		qsort(dist, n_class + m, sizeof(distancia), cmpfunc);
		//printf("\nOrdenado:\n"); for (i = 0; i < n_class + m; i++) printf("%s %f\n", dist[i].classe, dist[i].valor);

		/* define a classe de acordo com a maioria entre os KNN */
		for (i = 0; i < n_class; i++) {
			for (j = 0; j < KNN; j++) {
				if(strcmp(dist[j].classe, nomeClasse[i]) == 0)	contaClasse[i]++;
			}
		}
		//printf("%s:%d %s:%d %s:%d\n", nomeClasse[0],contaClasse[0],nomeClasse[1],contaClasse[1], nomeClasse[2],contaClasse[2]);

		/* encontra a classe que mais apareceu */
		max=0;
		for (i = 0; i < n_class; i++) {
			if (contaClasse[max] < contaClasse[i]) max = i;
		} 
		//printf("Max:%s\n", nomeClasse[max]);

		/* adiciona a nova instancia do grafo KNN */
		graphClass[n_class + m] = nomeClasse[max];
		for (j = 0; j < col/2; j++) {
			graphKnn[n_class + m][j] = instanciaFiltrada[m][j];
		}

		/*for(j=0; j < col/2; j++) printf("%.4f ", graphKnn[i][j]);
		printf("%s\n", graphClass[i]);
		printf("--------------------------------\n");*/

		/*printf("Ao final: \n");
		for (i = 0; i < n_class; i++) {
			printf("contaClasse[%d] = %d\t", i, contaClasse[i]);
			printf("dist[%d].classe = %s\t",i, dist[i].classe);
			printf("dist[%d].valor = %f\n", i, dist[i].valor);
		}
		for (i = n_class; i < n_class + m; i++) {
			printf("\t\t\tdist[%d].classe = %s\t",i, dist[i].classe);
			printf("dist[%d].valor = %f\n", i, dist[i].valor);
		}*/

		/* zerar os contadores para a proxima iteracao */
		for (i = 0; i < n_class; i++) {
			contaClasse[i] = 0;
			dist[i].classe = "";
			dist[i].valor = 0;
		}
		for (i = n_class; i < n_class + m; i++) {
			dist[i].classe = "";
			dist[i].valor = 0;
		}

	}

	/* imprime grafo knn */
	for (i=0; i < row + n_class; i++){
		for(j=0; j < col/2; j++) printf("%.4f ", graphKnn[i][j]);
		printf("%s\n", graphClass[i]);
	}

	free(classeAtual);
}

int main (int argc, char *argv[]){
	float **instancia, **weights;
	char **classe;
	float min = 0, max = 0;
	int rows, cols, n_class, i, j, k;

	scanf("%d %d %d", &rows, &cols, &n_class);

	/* alocar matriz do dataset */
	instancia = (float **)malloc(rows * sizeof(float *)); 
	for (i = 0; i < rows; ++i) instancia[i] = (float *)malloc(cols * sizeof(float));

	/* alocar matriz das classes */
	classe = (char **)malloc(rows * sizeof(char *)); 
	for (i = 0; i < rows; ++i) classe[i] = (char *)malloc(CHARSIZE * sizeof(char));

	/* ler o dataset sem normalização 
	for (i=0; i < rows; i++){
		for(j=0; j < cols; j++) scanf("%f", &instancia[i][j]);
		for(j=0; j < n_class; j++) scanf("%s", &classe[j]);
	}*/

	/* ler o dataset e já fazer normalização dos dados */
	for (i = 0; i < rows; i++){
		for(j = 0; j < cols; j++){
			scanf("%f", &instancia[i][j]);
			if ( instancia[i][j] < min) min = instancia[i][j];
			if ( instancia[i][j] > max) max =  instancia[i][j];
			//printf("%.2f ",  instancia[i][j]);
		}
		scanf("%s", classe[i]);
		//printf("%s\n", classe[i]);
	}

	/* fazer a normalizacao */
	for (i=0; i < rows; i++){
		for (j=0; j < cols; j++){
			instancia[i][j] = (instancia[i][j] - min) / (max - min);
		}
	}

	//print_dataset(instancia, classe, rows, cols);
	print_dataset_to_file(instancia, rows, cols);

	/* aloca a matrix dos pesos das camadas ocultas
	   para a criacao do grafo Knn/corte E          */
	weights = (float **)malloc((cols/2) * sizeof(float *));
	for (i = 0; i < (cols + 1); ++i) weights[i] = (float *)malloc((cols + 1) * sizeof(float));	
	
	/* treina o autoencoder */
	treina_autoencoder(cols, weights);

	/* imprime matriz de pesos *
	printf("\n");
	for (i = 0; i < (cols/2); ++i){
		for (j = 0; j < (cols+1); ++j){
			printf("%f ", weights[i][j]);
		}
		printf("\n");
	} */

	/* cria o grafo knn utilizando os pesos das camadas ocultas do autoencoder */
	cria_knn(instancia, classe, weights, rows, cols, n_class);

	/* desalocar */
	for (i = 0; i < rows; ++i) {
		free(instancia[i]);
		free(classe[i]);
	}
	free(instancia);
	free(classe);
	return 0;
}
