/*
 * distances.cpp
 *
 *  Created on: 09/03/2013
 *      Author: quiles
 */

#include "distances.h"
#include <math.h>


void Distances::SetSize(int samples, int dimension){
//	this.dimension = dimension;
//	this.samples = samples;
}

void Distances::SetDistance(MatrixXf *data, string distance){
	dimension =  (*data).row(0).size();
	samples = (*data).col(0).size();
	if (distance == "euclidean2"){
		funcao = &Distances::GetEuclidean2;
	}
	else if (distance == "euclidean"){
		funcao = &Distances::GetEuclidean;
	}
	else if (distance == "neuclidean"){
		int i;
		MatrixXf mean;
		mean.resize(samples,dimension);
		var.resize(dimension);
		for (i=0 ; i<dimension ; i++){ // for variando o nœmero de colunas da linha zero (e demais linhas)
			 mean.col(i).setConstant(data->col(i).mean());
			 mean.col(i) = (*data).col(i) - mean.col(i);
			 var(i) = sqrt(mean.col(i).squaredNorm() / (float)(samples-1));
		}
		funcao = &Distances::GetNEuclidean;
	}
	else funcao = &Distances::GetEuclidean;
}

float Distances::GetDistance(VectorXf v1, VectorXf v2){
	return (this->*funcao)(v1,v2);
}

// Implementar
float Distances::GetNEuclidean(VectorXf v1, VectorXf v2){
	VectorXf sum;
	int i;
	sum = v1-v2;
	for (i=0 ; i<dimension ; i++){
		sum(i) /= var(i);
	}
	return sum.norm();
}

float Distances::GetEuclidean(VectorXf v1, VectorXf v2){
	VectorXf sum;
	sum = v1-v2;
	return sum.norm();
}

float Distances::GetEuclidean2(VectorXf v1, VectorXf v2){
	VectorXf sum;
	sum = v1-v2;
	return sum.squaredNorm();
}
