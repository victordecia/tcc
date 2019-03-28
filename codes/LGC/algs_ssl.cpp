/*
 * algs_ssl.cpp
 *
 *  Created on: 09/03/2013
 *      Author: quiles
 */

#include "algs_ssl.h"

#include <iostream>

#include <Eigen/Cholesky>
#include <Eigen/Dense>
#include <math.h>

using namespace std;
using namespace Eigen;

//TSSLAlgorithm::TSSLAlgorithm(igraph_t *network){
//	net = network;
////	numberOfClasses = GAN(net,"classes");
////	numberOfSamples = (int)igraph_vcount(net);
//}

//TSSLAlgorithm::~TSSLAlgorithm(){
//}





void TSSLAlgorithm::LGC(igraph_t *net, float &accuracy){
	int i,j;
	int NOC, NOS;

	NOC = GAN(net,"classes");
	NOS = igraph_vcount(net);



	MatrixXf Y(NOS,NOC), F(NOS,NOC);
	MatrixXf W(NOS,NOS), X(NOS,NOS);
	VectorXf D(NOS);
	igraph_integer_t eid;
//	igraph_real_t weight;
	float sum, value, alpha=0.5;



	// Victor: Copia as informações dos rótulos para o vetor Y, caso não utilize o igraph, pode gerar o vetor Y automaticamente com os seus rótulos
    Y = ArrayXXf::Zero(NOS,NOC);
	for (i=0 ; i<NOS ; i++){
		if (VAN(net,"label2",i) != 0){
			Y(i,VAN(net,"label2",i)-1) = 1;
		}
	}
    
    

	W = ArrayXXf::Zero(NOS,NOS);
	// verificar se a rede possui ou n�o peso.....

    // Victor: Le a matriz de peso do grafo. Essa matriz é a matriz gerada pelo seu sistema, pode ser a matriz com a similaridade (kernel RBF) entre os pares de exemplos i e j. para todo i e j.
    // quando o grafo não possui peso, ele atribui 1 se a aresta existe, e zero caso contrário.
	for (i=0 ; i<igraph_vcount(net) ; i++){
		for (j=0 ; j<igraph_vcount(net) ; j++){
			igraph_get_eid(net, &eid, i, j, IGRAPH_UNDIRECTED, false);
			if (eid != -1) {
				if (GAN(net,"weighted")) value = EAN(net,"weight",eid);
				else value = 1.0;
				W(i,j) = value;
				W(j,i) = value;
			}
		}
	}




//	cout << W << endl;
//	return;

    D = ArrayXf::Zero(NOS);
	for (i=0 ; i<NOS ; i++){
		sum = 0.0;
		for (j=0 ; j<NOS ; j++){
			sum += W(i,j);
		}
		D(i) = sum;
	}

	///// TRECHO COM PROBLEMA
	///// TRECHO COM PROBLEMA

	X = ArrayXXf::Zero(NOS,NOS);
	for (i=0 ; i<NOS-2 ; i++){
		X(i,i) = 1.0;
		for (j=i+1 ; j<NOS-1 ; j++){
			value = (-alpha*W(i,j)) / (sqrt((float)D(i)) * sqrt((float)D(j)));
			X(i,j) = value;
			X(j,i) = value;
		}
	}
	X(NOS-1,NOS-1) = 1.0;




	///// TRECHO COM PROBLEMA
	///// TRECHO COM PROBLEMA



//	F = X.colPivHouseholderQr().solve(Y);

	F = X.ldlt().solve(Y);


	// verificando as classes atribu�das a cada exemplo
	int pos;
	float max;
	for (i=0 ; i<NOS ; i++){
		pos = 0;
		max = F(i,0);
		for (j=1 ; j<NOC ; j++){
			if (F(i,j)>max){
				max = F(i,j);
				pos = j;
			}
		}
		if (max>0.0) SETVAN(net, "label2", i, pos+1); // adicionar um, pois as classes est�o rotuladas de 1 at� NOC
//		VECTOR(classes)[i] = pos+1;
	}


//	cout << "Resultado Classifica��o:\n " << F << endl;

	int count=0, NOU=0;
	for (i=0 ; i<NOS ; i++){
		if (VAN(net, "labeled",i)==0) {
			NOU++;
			if (VAN(net,"label1",i) == VAN(net,"label2",i)) count++;
		}
//		cout << i << " : " << VAN(net,"label1",i) << " -  " << VAN(net,"label2",i) << endl;
	}
	accuracy = (float)count / (float)NOU;

}



