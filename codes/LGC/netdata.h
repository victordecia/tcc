/*
 * netdata.h
 *
 *  Created on: 09/03/2013
 *      Author: quiles
 */

#ifndef NETDATA_H_
#define NETDATA_H_

#include <igraph.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <Eigen/Dense>

#include "distances.h"

using namespace Eigen;
using namespace std;

#define NOS numberOfSamples
#define NOU numberOfUnlabeled
#define NOF numberOfFeatures
#define NOC numberOfClasses

class TNetData{
private:
	igraph_t net;
	MatrixXf data;
	VectorXi classes;
	float nSigma;
	float nEpsilon;
	int nK;
	int numberOfSamples;
	int numberOfClasses;
	int numberOfFeatures;
	string distanceName;
	Distances dist;
public:
	TNetData(int K, char *pwd);
	TNetData(int code);
	TNetData(string filename);
	~TNetData();
    
    void ResetLabels();
    void RandomSelectionN(int n);

	void KNN(float sigma, int K);
	void wKNN(float sigma, int K);

	void eCut(float sigma, float epsilon);
	void weCut(float sigma, float epsilon);

	void Percolation(float amostra, int mlog);

	void Post1NN(float sigma);
	void PostSupervised();

	void SaveSimulation(int sim);
	void SaveNetwork(int sim);
	void SaveNetMeasures(int sim);

	void SetDistance(string distance);
	igraph_t *GetNetwork();

	int GetNOS(){
		return NOS;
	};

//	MatrixXf *GetData();

	// Incorporar Dados Especialista
	// Metodos combinados
	// adicionar/remover vertices e arestas
	// t�cnicas alternativas, crescimento, mistas, etc...
	// fun��es de dist�ncia, variancia, covariancia
	// para cada rede gerada, salvar suas medidas (ver a possibilidade de criar um indice para armazenar cada experimento
};


#endif /* NETDATA_H_ */
