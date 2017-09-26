#include <stdio.h>
#include <stdlib.h>

/*  Gera um vetor com "n" numeros randomicos de 1 a "n" */
void GeraVetor(int *vetor, int n)
{
    srand((unsigned)time(NULL));

    int i = 0;
    for(i = 0; i < n; ++i)
        vetor[i] = (rand() % n) + 1;
}

/*  Escreve o vetor na tela */
void PrintVetor(int vetor[], int n) {
    int i = 0;
    for(i = 0; i < n; i++){
        printf("%d ", vetor[i]);
    }
}

/*
int main(){

    int n = 5;
    int vetor[n];
    GeraVetor(vetor, n);
    PrintVetor(vetor, n);

}
*/

