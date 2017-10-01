#include <stdio.h>
#include <stdlib.h>

#define DEBUG 1            // comentar esta linha quando for medir tempo
#define ARRAY_SIZE 40      // trabalho final com o valores 10.000, 100.000, 1.000.000

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

int main ( int argc , char **argv )
{
    int my_rank;
    // Identificador deste processo
    int proc_n
    // Numero de processos disparados pelo usuario na linha de comando (np )
    MPI_Status status;
    // Estrutura que guarda o estado de retorno
    MPI_Init (&argc, &argv ) ;
    // Funcao que inicializa o MPI, todo o codigo paralelo esta abaixo
    MPI_Comm_rank(MPI_COMMWORLD, &my_rank ) ;
    // Pega o numero do processo atual ( rank )
    MPI_Comm_size (MPI_COMMWORLD, &proc_n ) ;
    // Pega informacao do numero de processos ( quantidade total )
    /*VARIAVEIS DE TAMANHO */
    int ARRAY SIZE = 100000;
    int *LINHA; // Linha individual para ordenacao
    LINHA = ( int *) mal loc (ARRAY_SIZE * sizeof( int ) ) ;
    /* VARIAVEIS DE CONTROLE */
    int i = 0 ; // Va r iav e i s a u x i l i a r e s para l o o p s
    int j = 0 ;
    double time1 , time2 ; // Cont role do tempo de execucao
    int delta = 25000;
    int limit = 1 + ( proc_n / 2 ) ;
    int FILHO1_NUMBER = 1 + 2*my_rank ;
    int FILHO2_NUMBER = 2 + 2*my_rank ;
    // Verifica se o delta sera alcancado
    j = ARRAY_SIZE;
    for( i = 0; i<limit; i++)
    {
        j = j / 2 ;
    }
    if ( delta < j ){
        printf ( "\nDelta jamais sera alcancado \n" ) ;
        MPI_Finalize( ) ;
    }else{
        //Inicializacoes
        if(my_rank == 0) // RAIZ
        {
            time1 = MPI_Wtime( );
            printf ( "\n" ) ;
            // Preenche o v e t o r
            for( i =0; i<ARRAY SIZE; i++)
            {
                LINHA[ i ] = ARRAY_SIZE-i-1;
            }
            // Manda metades do array para os filhos
            MPI_Send(&LINHA[ 0 ], ARRAY_SIZE/2 , MPI_INT, FILHO1_NUMBER, ARRAY_SIZE/2 , MPI_COMMWORLD);
            MPI_Send(&LINHA[ARRAY_SIZE/ 2 ] , ARRAY_SIZE/2 , MPI_INT, FILHO2_NUMBER, ARRAY_SIZE/2 , MPI_COMMWORLD) ;
            // Recebe as metades ordenadas
            MPI_Recv(&LINHA[ 0 ] , ARRAY_SIZE/2 , MPI_INT, FILHO1_NUMBER, ARRAY_SIZE/2 , MPI_COMMWORLD, &status ) ;
            MPI_Recv(&LINHA[ARRAY_SIZE/ 2 ] , ARRAY_SIZE/2 , MPI_INT, FILHO2_NUMBER, ARRAY_SIZE/2 , MPI_COMMWORLD, &status ) ;
            // Interlaca vetores resultantes
            LINHA = interleaving (LINHA, ARRAY_SIZE) ;
            time2 = MPI_Wtime( ) ; // Acaba a contagem do tempo
            printf( "Tempo de execucao : %f \n\n" , time2-time1 ) ;
        }
        else // NODOS INTERMEDIARIOS E FOLHAS
        {
            // Antecipa recebimento da mensagem
            MPI_Probe (MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMMWORLD, &status);
            // Tamanho do v e t o r r e c e b i d o
            // ( f o i a p r o v e i t a d o o campo TAG do MPI, que nao s e r i a u t i l i z a d o )
            int TAMANHO = status.MPI_TAG;
            int METADETAMANHO = TAMANHO/ 2 ;
            // Numero do proc e s so pai
            int SOURCE_NUMBER = status.MPI_SOURCE;
            MPI_Recv(LINHA, TAMANHO, MPI_INT, SOURCE_NUMBER, TAMANHO, MPI_COMMWORLD, &status);

            if ( delta > TAMANHO){
                // Vetor maior do que o v a l o r de d e l t a
                // Divide a l i s t a p e l a metade e manda para os f i l h o s
                MPI_Send(&LINHA[ 0 ] , METADETAMANHO, MPI_INT, FILHO1_NUMBER, METADETAMANHO, MPI_COMMWORLD);
                MPI_Send(&LINHA[METADETAMANHO] , METADETAMANHO, MPI_INT, FILHO2_NUMBER, METADETAMANHO, MPI_COMMWORLD);
                // Recebe de volta os vetores ordenados
                MPI_Recv(&LINHA[ 0 ] , METADETAMANHO, MPI_INT, FILHO1_NUMBER, METADETAMANHO, MPI_COMMWORLD, &status);
                MPI_Recv(&LINHA[METADETAMANHO] , METADETAMANHO, MPI_INT, FILHO2_NUMBER, METADETAMANHO, MPI_COMMWORLD, &status);
                // I n t e r c a l a v e t o r e s
                int *LINHA_INTERCALADA = interleaving (LINHA, TAMANHO);
                // Manda o v e t o r ordenado de v o l t a para o proc e s so pai
                MPI_Send (LINHA_INTERCALADA, TAMANHO, MPI_INT, SOURCE_NUMBER, TAMANHO, MPI_COMMWORLD);
            }
            else
            {
                // Delta menor ou igual ao tamanho do vetor
                // Ordena e devolve ao processo pai
                bubblesort(TAMANHO, LINHA) ;
                MPI_Send(LINHA, TAMANHO, MPI_INT, SOURCE_NUMBER, TAMANHO, MPI_COMMWORLD) ;
            }
        }
        MPI_Finalize ( ) ;
    }
}

/*Rotina de Intercalação*/

/* recebe um ponteiro para um vetor que contem as mensagens recebidas dos filhos e            */
/* intercala estes valores em um terceiro vetor auxiliar. Devolve um ponteiro para este vetor */

int *interleaving(int vetor[], int tam)
{
	int *vetor_auxiliar;
	int i1, i2, i_aux;

	vetor_auxiliar = (int *)malloc(sizeof(int) * tam);

	i1 = 0;
	i2 = tam / 2;

	for (i_aux = 0; i_aux < tam; i_aux++) {
		if (((vetor[i1] <= vetor[i2]) && (i1 < (tam / 2)))
		    || (i2 == tam))
			vetor_auxiliar[i_aux] = vetor[i1++];
		else
			vetor_auxiliar[i_aux] = vetor[i2++];
	}

	return vetor_auxiliar;
}

/* Chamada para a rotina de Intercalação */

//int *vetor_auxiliar;         /* ponteiro para o vetor resultantes que sera alocado dentro da rotina */

//vetor_aux = interleaving(vetor, tam);
