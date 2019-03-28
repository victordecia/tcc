#include <stdlib.h>
#include <stdio.h>

int main (){
	int i, j, k;
	int rows, cols, outs;
	float data[4], trash;

	scanf("%d", &rows);
	scanf("%d", &cols);
	scanf("%d", &outs);

	printf("%d %d %d\n", rows, cols, cols);

	for (i=0; i < rows; i++){
		for(j=0; j < cols; j++)
			scanf("%f", &data[j]);
		for(k=0; k < outs; k++)
			scanf("%f", &trash);
		for(j=0; j < cols; j++)
			printf("%f ", data[j]);
		printf("\n");
		for(j=0; j < cols; j++)
			printf("%f ", data[j]);
		printf("\n");
	}

	return 0;
}
