//============================================================================
// Name        : NetGen.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description :
// Obs.
//     a) Labels start in 1, zero means an unlabeled sample
//============================================================================

#include <iostream>
#include <string>
#include <igraph.h>
#include <time.h>
#include "netdata.h"
#include "algs_ssl.h"
//#include "tolabel.h"


using namespace std;


void LGC(int argc, char *argv[]);

int main(int argc, char *argv[]) {
//int main() {

	igraph_i_set_attribute_table(&igraph_cattribute_table);
	srand(time(NULL));

	LGC(argc, argv);
//	Percolation();
	return 0;
}


void LGC(int argc, char *argv[]){
	float acuracia[100];
	int code=0;
	float media;
	TNetData *network;
	TSSLAlgorithm Alg;
	int i;
	float acc;
    int knn;
	char command[4096];
	char pwd[4096];

    if (argc < 3) {
        cout << "Informar:\n$ LGC matriz.dat #k\n\n\t$ LGC iris_AE1.dat 5\n\tpara executar o lgc com a matriz informada e o KNN com K=5" << endl;
        return;
    }

	knn = atoi(argv[2]);
	network = new TNetData(knn,argv[1]);
    
    
    
    
//	network = new TNetData(code);
	for (i=0 ; i<100 ; i++){
        // 10% rotulados
        network->RandomSelectionN(0.1*network->GetNOS());
		Alg.LGC(network->GetNetwork(),acc);
        acuracia[i] = acc;
		network->ResetLabels();
    }
	delete network;

	for (i=0 ; i<100 ; i++){
		cout << acuracia[i] << " ";
	}
    cout << endl;

}

