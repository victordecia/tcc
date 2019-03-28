/*
 * netdata.cpp
 *
 *  Created on: 09/03/2013
 *      Author: quiles
 */



#include "netdata.h"
#include <math.h>

TNetData::TNetData(int K, char *pwd){
	int i,j,k,ca;
	float value, sigma;
	double dvalue;
	double maxDist=0.0;
	string name;
	char lixo[256];
	FILE *stream;
	igraph_vector_t edges;
	float *line;
	int **knn; // store the knn indices
	char command[4096];

	stream = fopen(pwd, "r");

	if (stream){

		fscanf(stream,"%d",&numberOfSamples);

		line = new float[NOS];
		knn = new int*[NOS];
		for (i=0 ; i<NOS ; i++) knn[i] = new int[K];

		data = MatrixXf(NOS,NOS);
		classes = VectorXi(NOS);
		igraph_empty(&net,NOS,IGRAPH_UNDIRECTED);

		for (i=0 ; i<NOS ; i++){
			for (j=0 ; j<NOS ; j++){
				fscanf(stream,"%lf",&dvalue);
				if (i==j) {
					classes(i) = (int)dvalue;
					SETVAN(&net, "index", i, i); // original index from the dataset, it can be useful when dealing with dynamic datasets (indices might change)
					SETVAN(&net, "label1", i, (int)dvalue); // label1 means the original label
					SETVAN(&net, "label2", i, 0);  // label2 means the label assigned by the classification technique
					SETVAN(&net, "labeled", i, 0); //
					if (dvalue > numberOfClasses) numberOfClasses = dvalue;
				}
				else {
					data(i,j) = dvalue;
					data(j,i) = dvalue;
					if (dvalue > maxDist) maxDist = dvalue;
				}
			}

		}
		SETGAN(&net,"classes", NOC); // inform the number of classes


		// obtendo o valor de 2*(sigma^2)
		sigma = (-(maxDist)/(2.0*log(0.0001)));
		sigma *= 2;

//		cout << "Sigma: " << sigma << endl;

		float max;
		int pos;

		SETGAN(&net,"weighted", 0); // unweighted network
  		igraph_vector_init(&edges, 0);

		for (i=0 ; i<NOS ; i++){
//			cout << i;
			for (j=0 ; j<NOS; j++){
				if (i==j){
					line[j] = 0;
					continue;
				}
				value = data(i,j) / sigma;
				value = exp(-value);
				line[j] = value;
			}
			for (k=0 ; k<K ; k++){
				max = line[0];
				pos = 0;
				for (j=1 ; j<NOS ; j++){
					if (line[j]>max){
						max = line[j];
						pos = j;
					}
				}
				knn[i][k]=pos;
				line[pos] = 0.0;
			}
		}

		for (i=0 ; i<NOS ; i++){
//			cout << i;
			for (k=0 ; k<K ; k++){
//				cout << " : " << knn[i][k];
				igraph_vector_push_back(&edges,i);
				igraph_vector_push_back(&edges,(int)knn[i][k]);
			}
//			cout << endl;
		}
		igraph_add_edges(&net, &edges, 0);
		igraph_vector_destroy(&edges);
		fclose(stream);
	}
}

void TNetData::ResetLabels(){
    int i;
    
    for (i=0 ; i<igraph_vcount(&net) ; i++){
        SETVAN(&net, "label2", i, 0 );
        SETVAN(&net, "labeled", i, 0);
    }
}


void TNetData::RandomSelectionN(int n){
    int count=0, index, vc;
    vc = igraph_vcount(&net);
    do {
        index = rand()%vc;
        if (VAN(&net,"label2",index) == 0){
            SETVAN(&net, "label2", index, VAN(&net,"label1",index) );
            SETVAN(&net, "labeled", index, 1);
            count++;
        }
    } while (count<n);
}



TNetData::TNetData(int code){
	int i,j,ca;
	float value, sigma;
	double dvalue;
	double maxDist=0.0;
	string name;
	char lixo[256];
	FILE *stream;
	igraph_vector_t edges;
	char command[4096];

	sprintf(command, "basquilva_%d",code);
	stream = fopen(command, "r");

	if (stream){
		fscanf(stream,"%s",lixo);
		fscanf(stream,"%d",&numberOfSamples);

		data = MatrixXf(NOS,NOS);
		classes = VectorXi(NOS);
		igraph_empty(&net,NOS,IGRAPH_UNDIRECTED);

		for (i=0 ; i<NOS ; i++){
			for (j=0 ; j<NOS ; j++){
				fscanf(stream,"%lf",&dvalue);
				if (i==j) {
					dvalue++;
					classes(i) = (int)dvalue;
					SETVAN(&net, "index", i, i); // original index from the dataset, it can be useful when dealing with dynamic datasets (indices might change)
					SETVAN(&net, "label1", i, (int)dvalue); // label1 means the original label
					SETVAN(&net, "label2", i, 0);  // label2 means the label assigned by the classification technique
					SETVAN(&net, "labeled", i, 0); //
					if (dvalue > numberOfClasses) numberOfClasses = dvalue;
				}
				else {
					data(i,j) = dvalue;
					data(j,i) = dvalue;
					if (dvalue > maxDist) maxDist = dvalue;
				}
			}

		}
		SETGAN(&net,"classes", NOC); // inform the number of classes


		// obtendo o valor de 2*(sigma^2)
		sigma = (-(maxDist)/(2.0*log(0.01)));
		sigma *= 2;

//		sigma = 1.2;

//		cout << "Sigma " << sigma << endl;


		SETGAN(&net,"weighted", 0); // unweighted network
  		igraph_vector_init(&edges, 0);
		for (i=0 ; i<NOS-1 ; i++){
			for (j=i+1 ; j<NOS; j++){
				value = data(i,j) / sigma;
				value = exp(-value);
				if (value > 0.6) {
					igraph_vector_push_back(&edges,i);
					igraph_vector_push_back(&edges,j);
				}
			}
		}
		igraph_add_edges(&net, &edges, 0);
		igraph_vector_destroy(&edges);
//		igraph_integer_t eid;
//		SETGAN(&net,"weighted", 1); // unweighted network
//		for (i=0 ; i<NOS-1 ; i++){
//			for (j=i+1 ; j<NOS; j++){
//				value = value / sigma;
//				value = exp(-value);
//				if (value > 0.9) {
//					igraph_add_edge(&net, i, j);
//					igraph_get_eid(&net, &eid, i, j, IGRAPH_UNDIRECTED, false);
//					SETEAN(&net,"weight",eid,value);
//				}
//			}
//		}
		fclose(stream);
	}
}
/*
 *
 *
 */



TNetData::TNetData(string filename){
	int i,j,ca;
	float value;
	string name;
	FILE *stream;
	name = filename+".info";
	stream = fopen(name.c_str(),"r");
	fscanf(stream,"%d",&numberOfClasses);
	fscanf(stream,"%d",&numberOfFeatures);
	fscanf(stream,"%d",&numberOfSamples);
	fclose(stream);

	nSigma = 0.0;
	nEpsilon = 0.0;
	nK = 0;

	data = MatrixXf(NOS,NOF);
	classes = VectorXi(NOS);

	igraph_empty(&net,NOS,IGRAPH_UNDIRECTED);
	SETGAN(&net,"classes", NOC); // inform the number of classes
	SETGAS(&net,"dataset", filename.c_str()); // inform the number of classes

	name = filename+".csv";
	stream = fopen(name.c_str(),"r");
	for (i=0 ; i<NOS ; i++){
		for (j=0 ; j<NOF ; j++){
			fscanf(stream,"%f",&value);
			data(i,j) = value;
		}
		fscanf(stream,"%d",&ca);
		classes(i) = ca;
		SETVAN(&net, "index", i, i); // original index from the dataset, it can be useful when dealing with dynamic datasets (indices might change)
		SETVAN(&net, "label1", i, ca); // label1 means the original label
		SETVAN(&net, "label2", i, 0);  // label2 means the label assigned by the classification technique
		SETVAN(&net, "labeled", i, 0); //
	}
//	data.data();
	fclose(stream);
}


TNetData::~TNetData(){
//	delete data;
//	delete classes;
}

void TNetData::SaveSimulation(int sim){
	char file[256];
	string fname;
	FILE *stream;

	sprintf(file,"%s_netpar.%d",GAS(&net,"dataset"),sim);
	fname.assign(file);
	stream = fopen(fname.c_str(),"w");


	if (stream){
		fprintf(stream, "%s\n", GAS(&net,"dataset"));
		fprintf(stream, "%s\n", distanceName.c_str());
		fprintf(stream, "%.3f\n", nSigma);
		fprintf(stream, "%.3f\n", nEpsilon);
		fprintf(stream, "%d\n", nK);
		fclose(stream);
	}
}

void TNetData::SaveNetMeasures(int sim){
	char file[256];
	string fname;
	FILE *stream;
	igraph_vector_t degree;
	int i;

	igraph_vector_init(&degree,NOS);
	igraph_degree(&net, &degree, igraph_vss_all(), IGRAPH_ALL, IGRAPH_NO_LOOPS);

	sprintf(file,"%s_netmeasures.%d",GAS(&net,"dataset"),sim);
	fname.assign(file);
	stream = fopen(fname.c_str(),"w");
	if (stream){
		fprintf(stream, "To be implemented!\n");
		fprintf(stream, "Average Degree: %.2f\n", igraph_vector_sum(&degree)/(float)NOS);
		cout << "Grau m�dio: " << igraph_vector_sum(&degree)/(float)NOS << endl;
		fclose(stream);
	}
	igraph_vector_destroy(&degree);
}

void TNetData::SaveNetwork(int sim){
	char file[256];
	string fname;
	FILE *stream;

	sprintf(file,"%s_network.%d",GAS(&net,"dataset"),sim);
	fname.assign(file);
	stream = fopen(fname.c_str(),"w");
	if (stream){
//		igraph_write_graph_graphml(&net, stream, false);
		igraph_write_graph_dot(&net, stream);
		fclose(stream);
	}
}

void TNetData::Percolation(float amostra, int mlog){
	VectorXf v1, v2;
	int *vA;
	int i,j,k;
	float value;
    float sigma;
    float maxDist=0;
    float prob;
	igraph_vector_t edges;

	igraph_vector_init(&edges, 0);
	SETGAN(&net,"weighted", 0); // unweighted network
	SETGAS(&net,"netgen", "Percolation");

	amostra *= NOS;
	vA = new int[(int)amostra];
	for (i=0 ; i<amostra ; i++){
		vA[i] = rand()%NOS;
	}

	// conectar as amostras (j� foram calculadas) ?????? verificar
	dist.SetSize(NOS,NOF);
	for (i=0 ; i<amostra-1 ; i++){
		v1 = data.row(vA[i]);
		for (j=i+1 ; j<amostra ; j++){
			v2 = data.row(vA[j]);
			value = dist.GetDistance(v1,v2);
			if (value > maxDist) maxDist = value;
		}
	}

	// obtendo o valor de 2*(sigma^2)
	sigma = (-(maxDist)/(2.0*log(0.001)));
	sigma *= 2;

//	sigma = 0.01;

	cout << "Sigma " << sigma << endl;

	for (i=0 ; i<NOS ; i++){
		v1 = data.row(i);
		for (k=0 ; k<(int)log10(NOS)*mlog; k++){ // verificar o n�mero de itera��es....
			j = rand()%NOS;
			if (v1==v2) continue;
			v2 = data.row(j);
			value = dist.GetDistance(v1,v2);
			value = value / sigma;
			value = exp(-value);
			prob = (float)(rand()%1001) / 1000.0;
			if (value > prob){
//cout << "+";

				igraph_vector_push_back(&edges,i);
				igraph_vector_push_back(&edges,j);
			}
//else cout << "-";
		}
//		cout << endl;
	}


	igraph_add_edges(&net, &edges, 0);
//cout << igraph_ecount(&net) << endl;

	igraph_vector_destroy(&edges);

	igraph_simplify(&net,1,1,0);

	delete[] vA;

}



void TNetData::eCut(float sigma, float epsilon){
	VectorXf v1, v2;
	int i,j;
	float value;
	igraph_vector_t edges;

	nSigma = sigma;
	nEpsilon = epsilon;

	igraph_vector_init(&edges, 0);
	SETGAN(&net,"weighted", 0); // unweighted network

//	sigma = sigma*sigma*2.0;

	float max=0;

	dist.SetSize(NOS,NOF);
	for (i=0 ; i<NOS-1 ; i++){
		v1 = data.row(i);
		for (j=i+1 ; j<NOS ; j++){
			v2 = data.row(j);
			value = dist.GetDistance(v1,v2);
			if (value > max) max = value;
			value = value / sigma;
			value = exp(-value);
			if (value > epsilon){
				igraph_vector_push_back(&edges,i);
				igraph_vector_push_back(&edges,j);
			}
		}
	}
	cout << "MAX: " << max << endl;

	igraph_add_edges(&net, &edges, 0);
	igraph_vector_destroy(&edges);
}

void TNetData::weCut(float sigma, float epsilon){
	VectorXf v1, v2;
	int i,j;
	float value;
	igraph_integer_t eid;
	igraph_vector_t edges, weights;

	igraph_vector_init(&edges, 0);
	igraph_vector_init(&weights, 0);

	SETGAN(&net,"weighted", 1); // weighted network
	nSigma = sigma;
	nEpsilon = epsilon;

	sigma = sigma*sigma*2.0;

	dist.SetSize(NOS,NOF);
	eid = 0;
	for (i=0 ; i<NOS-1 ; i++){
		v1 = data.row(i);
		for (j=i+1 ; j<NOS ; j++){
			v2 = data.row(j);
			value = dist.GetDistance(v1,v2);
			value = value / sigma;
			value = exp(-value);
			if (value > epsilon){
				igraph_vector_push_back(&edges,i);
				igraph_vector_push_back(&edges,j);
				igraph_vector_push_back(&weights,value);
//				igraph_add_edge(&net,i,j);
//				SETEAN(&net, "weight", eid, value);
				eid++;
			}
		}
	}
	igraph_add_edges(&net, &edges, 0);
	for (i=0 ; i<eid ; i++){
		SETEAN(&net, "weight", i, VECTOR(weights)[i] );
	}
	igraph_vector_destroy(&edges);
	igraph_vector_destroy(&weights);
}

void TNetData::wKNN(float sigma, int K){
	VectorXf v1, v2, line(NOS);
	MatrixXi knn(NOS,K); // store the knn indices
	int i,j,k; //,eids;
	float value;
	igraph_vector_t edges;
//	igraph_vector_t weights;

	igraph_vector_init(&edges, 0);
//	igraph_vector_init(&weights, 0);
	SETGAN(&net,"weighted", 0); // unweighted network

	nSigma = sigma;
	nK = K;

	sigma = sigma*sigma*2.0;

	dist.SetSize(NOS,NOF);
	for (i=0 ; i<NOS ; i++){
		line = ArrayXf::Zero(NOS);
		v1 = data.row(i);
		for (j=0 ; j<NOS ; j++){
			if (i==j) continue;
			v2 = data.row(j);
			value = dist.GetDistance(v1,v2);
			value = value / sigma;
			value = exp(-value);
			line(j) = value;
		}
		float max;
		int pos;
//		cout << "I: " << i << " - ";
		for (k=0 ; k<K ; k++){
			max = line(0);
			pos = 0;
			for (j=1 ; j<NOS ; j++){
				if (line(j)>max){
					max = line(j);
					pos = j;
				}
			}
			knn(i,k)=pos;
			line(pos) = 0.0;
//			cout << pos << " ";
		}
//		cout << endl;
	}

	for (i=0 ; i<NOS ; i++){
		for (k=0 ; k<K ; k++){
			igraph_vector_push_back(&edges,i);
			igraph_vector_push_back(&edges,(int)knn(i,k));
//			igraph_vector_push_back(&weights,);
		}
	}

	igraph_add_edges(&net, &edges, 0);
	igraph_vector_destroy(&edges);
//	igraph_vector_destroy(&weights);

}


void TNetData::KNN(float sigma, int K){
	VectorXf v1, v2, line(NOS);
	MatrixXi knn(NOS,K); // store the knn indices
	int i,j,k;
	float value;
	igraph_vector_t edges;

	igraph_vector_init(&edges, 0);
	SETGAN(&net,"weighted", 0); // unweighted network

	nSigma = sigma;
	nK = K;

	sigma = sigma*sigma*2.0;

	dist.SetSize(NOS,NOF);
	for (i=0 ; i<NOS ; i++){
		line = ArrayXf::Zero(NOS);
		v1 = data.row(i);
		for (j=0 ; j<NOS ; j++){
			if (i==j) continue;
			v2 = data.row(j);
			value = dist.GetDistance(v1,v2);
			value = value / sigma;
			value = exp(-value);
			line(j) = value;
		}
		float max;
		int pos;
//		cout << "I: " << i << " - ";
		for (k=0 ; k<K ; k++){
			max = line(0);
			pos = 0;
			for (j=1 ; j<NOS ; j++){
				if (line(j)>max){
					max = line(j);
					pos = j;
				}
			}
			knn(i,k)=pos;
			line(pos) = 0.0;
//			cout << pos << " ";
		}
//		cout << endl;
	}

	for (i=0 ; i<NOS ; i++){
		for (k=0 ; k<K ; k++){
			igraph_vector_push_back(&edges,i);
			igraph_vector_push_back(&edges,(int)knn(i,k));
		}
	}

	igraph_add_edges(&net, &edges, 0);
	igraph_vector_destroy(&edges);

}

void TNetData::Post1NN(float sigma){
	VectorXf v1, v2, line(NOS);
	VectorXi knn(NOS); // store the knn indices
	int i,j;
	float value;
	igraph_vector_t edges;

	igraph_vector_init(&edges, 0);
	SETGAN(&net,"weighted", 0); // unweighted network

	nSigma = sigma;
//	nK = K;


	sigma = sigma*sigma*2.0;

	dist.SetSize(NOS,NOF);
	for (i=0 ; i<NOS ; i++){
		line = ArrayXf::Zero(NOS);
		v1 = data.row(i);
		for (j=0 ; j<NOS ; j++){
			if (i==j) continue;
			v2 = data.row(j);
			value = dist.GetDistance(v1,v2);
			value = value / sigma;
			value = exp(-value);
			line(j) = value;
		}
		float max;
		int pos;
		max = line(0);
		pos = 0;
		for (j=1 ; j<NOS ; j++){
			if (line(j)>max){
				max = line(j);
				pos = j;
			}
		}
		knn(i)=pos;
	}

	for (i=0 ; i<NOS ; i++){
		igraph_vector_push_back(&edges,i);
		igraph_vector_push_back(&edges,(int)knn(i));
	}

	igraph_add_edges(&net, &edges, 0);
	igraph_vector_destroy(&edges);

}




void TNetData::SetDistance(string distance){
	distanceName = distance;
	dist.SetDistance(&data, distanceName);
	SETGAS(&net,"metric", distance.c_str()); // m�trica utilizada

}

igraph_t *TNetData::GetNetwork(){
	return &net;
}

//MatrixXf *TNetData::GetData(){
//	return &data;
//}

