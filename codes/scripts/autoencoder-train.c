/*--------------- TCC ---------------*/
/*        Victor De Cia Costa        */
/*               77551               */
/*---------- Version 1.0.1 ----------*/

/* Codigo de treinamento de uma rede autoencoder 
   utilizando a biblioteca FANN 
   compile:
	 gcc autoencoder-train.c -o treino -lfann -lm
   use:
	./treino
*/

#include <stdlib.h>
#include <stdio.h>
#include "fann.h"

int main (int argc, char *argv[]){

	struct fann_connection *con;   /* weight matrix */
	unsigned int connum;           /* connections number */
	size_t i;

	// Simple training
	const char *dataset = "iris.data";
	const char *netName = "autoencoder.net";
	const unsigned int num_layers = 3;
	const unsigned int num_input = 4;
	const unsigned int num_output = 4;
	const unsigned int num_neurons_hidden = 2;
	const float desired_error = (const float) 0.001;
	const unsigned int max_epochs = 500000;
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
		printf("weight from %u to %u: %f\n", con[i].from_neuron,
		con[i].to_neuron, con[i].weight);
	}
	free(con);

	/* Print the connections */
	fann_print_connections(ann);

	/*----- //IMPRIME OS PESOS DOS NEURONIOS -----*/

	fann_save(ann, netName);
	fann_destroy(ann);

	return 0;
}
