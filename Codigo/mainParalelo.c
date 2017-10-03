#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define tamArrayGlobal 1000000 //Utilizar 10k, 100k e 1kk

void bs(int n, int * vetor)
{
    int c=0, d, troca, trocou =1;

    while (c < (n-1) & trocou )
        {
        trocou = 0;
        for (d = 0 ; d < n - c - 1; d++)
            if (vetor[d] > vetor[d+1])
                {
                troca      = vetor[d];
                vetor[d]   = vetor[d+1];
                vetor[d+1] = troca;
                trocou = 1;
                }
        c++;
        }
}

int *interleaving(int vetor[], int tam)
{
	int *vetor_auxiliar;
	int i1, i2, i_aux;

	vetor_auxiliar = (int *)malloc(sizeof(int) * tam);

	i1 = 0;
	i2 = tam / 2;

	for (i_aux = 0; i_aux < tam; i_aux++) {
		if ( ( (vetor[i1] <= vetor[i2]) && (i1 < (tam / 2)) ) || (i2 == tam) )
			vetor_auxiliar[i_aux] = vetor[i1++];
		else
			vetor_auxiliar[i_aux] = vetor[i2++];
	}

	return vetor_auxiliar;
}


int main ( int argc , char **argv )
{
	//ID do processo
    int my_rank;
	//Numero de processos alocados passados por paramatro
    int proc_n;
	//Objeto para o estado de retorno utilizado no MPI_Recv
    MPI_Status status;
	//Funcao de inicialização do MPI
    MPI_Init(&argc, &argv ) ;
    //Pega o rank do processo atualmente executando o codigo
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank ) ;
    //Busca a quantidade total de processos
    MPI_Comm_size(MPI_COMM_WORLD, &proc_n );
	
	//Inicializaçao de variaveis
	// array utilizado para ordenacao
    int *array; 
    array = (int *) malloc(tamArrayGlobal * sizeof(int) );
	//Variaveil utilizada nos lacos
    int i = 0 ;
	//Variaveis para o medicao de tempo
    double time1 , time2 ;
	//Threshold utilizado para definir quando o array deve ser
	//dividido em dois ou ser ordenado
    int threshold = (int) tamArrayGlobal*0.25;
	//Processos filhos que receberao as duas metades do array
    int filhoProc1 = 1 + (2 * my_rank);
    int filhoProc2 = 2 + (2 * my_rank);
    
    //Inicializacoes
	//Nodo raiz com rank 0
    if(my_rank == 0) 
    {
		//Captura o tempo no comeco do processo raiz
        time1 = MPI_Wtime();
        //Popula o vetor
        for( i =0; i<tamArrayGlobal; i++)
        {
            array[ i ] = tamArrayGlobal-i-1;
        }
		
		//Verifica se foram utilizados apenas 1 ou 2 processos
		//entao a raiz faz a ordenacao e retorna o array
        if(proc_n == 1 || proc_n == 2){
            bs(tamArrayGlobal, array);
        }else{
            //Separa o array enviando uma parte para dois filhos subsequentes
            MPI_Send(&array[ 0 ], tamArrayGlobal/2 , MPI_INT, filhoProc1, tamArrayGlobal/2 , MPI_COMM_WORLD);
            MPI_Send(&array[tamArrayGlobal/ 2 ] , tamArrayGlobal/2 , MPI_INT, filhoProc2, tamArrayGlobal/2 , MPI_COMM_WORLD) ;
			
            //Recebe a as metades enviadas ja ordenadas pelos filhos
            MPI_Recv(&array[ 0 ] , tamArrayGlobal/2 , MPI_INT, filhoProc1, tamArrayGlobal/2 , MPI_COMM_WORLD, &status ) ;
            MPI_Recv(&array[tamArrayGlobal/ 2 ] , tamArrayGlobal/2 , MPI_INT, filhoProc2, tamArrayGlobal/2 , MPI_COMM_WORLD, &status ) ;
            
			//Realizar a intercalacao do array para juntar as partes retornadas
            array = interleaving (array, tamArrayGlobal) ;
        }
        time2 = MPI_Wtime( ) ; //Captura o tempo final para realizar a medicao
        printf( "Tempo total da execucao : %f \n" , time2-time1 ) ;

    }
    else //todos os outros nodos que nao sao raiz
    {
        //Realiza uma antecipacao das mensagens
		//preparando para realizar o recebimento depois
        MPI_Probe (MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        //Pega o tamanho do vetor utilizando a estrutura de retorno
        int tamArray = status.MPI_TAG;
		//Metade do tamanho do array para poder enviar as mensagens para os filhos
        int metTamArray = tamArray/ 2;
        //Pega o numero do processo que enviou a parte do array
        int sourceProc = status.MPI_SOURCE;
		//Recebe a parte do array
        MPI_Recv(array, tamArray, MPI_INT, sourceProc, tamArray, MPI_COMM_WORLD, &status);
		
		//Verifica se o vetor é maior ou igual ao threshold
		//e se existem os processos filhos que irao receber
		//as partes do vetor
        if ( threshold <= tamArray && filhoProc1 <= proc_n-1 && filhoProc2 <= proc_n-1){
            			
            //Realiza o mesmo processo de dividir e enviar o vetor
            MPI_Send(&array[ 0 ] , metTamArray, MPI_INT, filhoProc1, metTamArray, MPI_COMM_WORLD);
            MPI_Send(&array[metTamArray] , metTamArray, MPI_INT, filhoProc2, metTamArray, MPI_COMM_WORLD);
            
			
            MPI_Recv(&array[ 0 ] , metTamArray, MPI_INT, filhoProc1, metTamArray, MPI_COMM_WORLD, &status);
            MPI_Recv(&array[metTamArray] , metTamArray, MPI_INT, filhoProc2, metTamArray, MPI_COMM_WORLD, &status);
            // Intercala vetores
            int *arrayIntercalado = interleaving(array, tamArray);
            // Manda o vetor ordenado de volta para o processo pai
            MPI_Send (arrayIntercalado, tamArray, MPI_INT, sourceProc, tamArray, MPI_COMM_WORLD);
        }
        else //Threshold maior que o tamanho do vetor recebido pelo filho
        {
			//Ordena o vetor recebido
            bs(tamArray, array);
			//Envia de volta para o processo que o enviou
            MPI_Send(array, tamArray, MPI_INT, sourceProc, tamArray, MPI_COMM_WORLD) ;
        }
    }
	//Finaliza os processos
    MPI_Finalize( );
}

/*Rotina de Intercalação*/

/* recebe um ponteiro para um vetor que contem as mensagens recebidas dos filhos e            */
/* intercala estes valores em um terceiro vetor auxiliar. Devolve um ponteiro para este vetor */

/* Chamada para a rotina de Intercalação */

//int *vetor_auxiliar;         /* ponteiro para o vetor resultantes que sera alocado dentro da rotina */

//vetor_auxiliar = interleaving(vetor, tam);
