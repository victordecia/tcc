#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main (){
	int i, j;
	float soma, media, mediana, desvioPadrao, atual;
	float data[100];

	soma = media = mediana = desvioPadrao = 0.0;

	for(i = 0; i < 100; i++){
		scanf("%f", &data[i]);
		soma += data[i];
	}
	//printf("\nMédia: %f\n", soma/100);
	printf("%f ", soma/100);

	for(i = 0; i < 100; i++){
		desvioPadrao += pow(data[i] - media, 2);
	}
	desvioPadrao = sqrt(desvioPadrao/99);
	//printf("Desvio padrão: %f\n", desvioPadrao);
	printf("%f ", desvioPadrao);


	for (i = 1; i < 100; i++)
	{
		atual = data[i];
		j = i - 1;

		while ((j >= 0) && (atual < data[j]))
		{
			data[j + 1] = data[j];
            		j = j - 1;
		}
    
		data[j + 1] = atual;
	}
	//printf("Mediana: %f\n\n", (data[49] + data[50])/2);
	printf("%f ", (data[49] + data[50])/2);
	
	return 0;
}
