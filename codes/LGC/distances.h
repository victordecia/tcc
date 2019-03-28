/*
 * distances.h
 *
 *  Created on: 09/03/2013
 *      Author: quiles
 */

#ifndef DISTANCES_H_
#define DISTANCES_H_


#include <string>
#include <Eigen/Dense>
using namespace Eigen;
using namespace std;

// controlar nœmero de exemplos no dataset
// armazenar desvio padrao, variancia, matriz covariancia e outras informacaoes necessarias para o calculo da distancia

class Distances{
private:
//	bool isUpdate;
	int dimension;
	int samples;
	float (Distances::*funcao)(VectorXf v1, VectorXf v2);
//	float **cov;
	VectorXf var;


//	void calculate_covariance();

public:
//	Distance(float **data, int samples, int dimension);
	void SetSize(int samples, int dimension);
	void SetDistance(MatrixXf *data, string distance);
	float GetDistance(VectorXf v1, VectorXf v2);
	float GetEuclidean(VectorXf v1, VectorXf v2);
	float GetEuclidean2(VectorXf v1, VectorXf v2);
	float GetNEuclidean(VectorXf v1, VectorXf v2);
//	float GetCityblock(float *v1, float *v2);
//	float GetMinkowski(float *v1, float *v2, int exp);
//	float GetMahalanobis(float *v1, float *v2);
//	float GetCosine(float *v1, float *v2);

};



#endif /* DISTANCES_H_ */
