/*
 * algs_ssl.h
 *
 *  Created on: 09/03/2013
 *      Author: quiles
 */

#ifndef ALGS_SSL_H_
#define ALGS_SSL_H_

#include <iostream>
#include <igraph.h>

//#define NOS numberOfSamples
//#define NOC numberOfClasses

class TSSLAlgorithm{
private:
//	igraph_t *net; // network generated from dataset
//	int numberOfSamples; // number of samples (or vertices in the graph)
//	int numberOfClasses; // inform the number of classes

public:
//	TSSLAlgorithm(igraph_t *network);
//	~TSSLAlgorithm();
	void LGC(igraph_t *net, float &accuracy);
	void InfoMAP01(igraph_t *net, float &accuracy);
	void InfoMAP_NS(igraph_t *net, float &accuracy);
	void InfoMAPMult(igraph_t **net1, int nnets, float &accuracy);
	void Homogeneity(igraph_t *net, float &accuracy);
	float TesteNulo();
};


#endif /* ALGS_SSL_H_ */
