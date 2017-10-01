#include <stdio.h>
#include <cstdlib>
#include "mpi.h"

void bubblesort(int n, int∗ vetor)
{
	int c = 0, d, troca, trocou = 1;
	while (c < (n−1) & trocou) {
		trocou = 0;
		for (d = 0 ; d < n − c − 1 ; d++){
			if (vetor[d] > vetor [d + 1]){
			troca = vetor [ d ] ;
			vetor [ d ] = vetor [ d+1];
			v e t o r [ d+1] = t r o c a ;
			trocou = 1;
			}
		}
		c++;
	}
}

int ∗ interleaving(int vetor[], int tam)
{
	int ∗ vetor a u x i l i a r ;
	v e t o r a u x i l i a r = ( int ∗ ) m a l l o c ( s i z e o f ( int ) ∗ tam ) ;
	int i 1 , i 2 , i a u x ;
	i1 = 0;
	i 2 = tam / 2 ;
	for ( i a u x = 0 ; i a u x < tam ; i a u x++)
	i f ( ( ( v e t o r [ i 1 ] <= v e t o r [ i 2 ] )
	| | ( i 2 == tam ) )
	v e t o r a u x i l i a r [ i aux ]
	else
	v e t o r a u x i l i a r [ i aux ]
	}
	return v e t o r a u x i l i a r ;
	{
	&& ( i 1 < ( tam / 2 ) ) )
	= v e t o r [ i 1 ++];
	= v e t o r [ i 2 ++];
}

main ( int argc , char ∗∗ argv )
{
int my rank ;
// I d e n t i f i c a d o r d e s t e p r o c e s s o
int p r o c n ;
// Numero de p r o c e s s o s d i s p a r a d o s p e l o u s u a r i o na l i n h a de comando ( np )
MPI Status s t a t u s ;
// E s t r u t u r a que guarda o e s t a d o de r e t o r n o
MPI Init (& argc , &argv ) ;
// Funcao que i n i c i a l i z a o MPI, t o d o o c o d i g o p a r a l e l o e s t a h a b a i x o
MPI Comm rank (MPI COMM WORLD, &my rank ) ;
// Pega pega o numero do p r o c e s s o a t u a l ( rank )
MPI Comm size (MPI COMM WORLD, &p r o c n ) ;
// Pega informacao do numero de p r o c e s s o s ( q u a n t i d a d e t o t a l )/∗
VARIAVEIS DE TAMANHO
∗/
int ARRAY SIZE = 1 0 0 0 0 0 ;
int ∗LINHA ;
// Linha i n d i v i d u a l para ordenacao
LINHA = ( int ∗ ) m a l l o c (ARRAY SIZE ∗ s i z e o f ( int ) ) ;
/∗
VARIAVEIS DE CONTROLE
∗/
int i = 0 ; // V a r i a v e i s a u x i l i a r e s para l o o p s
int j = 0 ;
double time1 , time2 ; // C o n t r o l e do tempo de e x e c u c a o
int
int
int
int
delta = 25000;
l i m i t = 1 + ( proc n / 2 ) ;
FILHO1 NUMBER = 1 + 2∗ my rank ;
FILHO2 NUMBER = 2 + 2∗ my rank ;
// V e r i f i c a s e o d e l t a s e r a a l c a n c a d o
j = ARRAY SIZE ;
for ( i =0; i <l i m i t ; i ++)
{
j = j / 2;
}
i f ( d e l t a < j ) {
p r i n t f ( ”\ nDelta j a m a i s s e r a a l c a n c a d o \n ” ) ;
MPI Finalize ( ) ;
} else {
// I n i c i a l i z a c o e s
i f ( my rank == 0 ) // RAIZ
{
time1 = MPI Wtime ( ) ;
p r i n t f ( ”\n ” ) ;
// Preenche o v e t o r
fo r ( i =0; i <ARRAY SIZE ; i ++)
{
LINHA [ i ] = ARRAY SIZE−i −1;
}
// Manda metades do a r r a y para os f i l h o s
MPI Send(&LINHA [ 0 ] , ARRAY SIZE/ 2 , MPI INT , FILHO1 NUMBER,
ARRAY SIZE/ 2 , MPI COMM WORLD) ;
MPI Send(&LINHA [ ARRAY SIZE / 2 ] , ARRAY SIZE/ 2 , MPI INT ,
FILHO2 NUMBER, ARRAY SIZE/ 2 , MPI COMM WORLD) ;
// Recebe as metades ordenad as
MPI Recv(&LINHA [ 0 ] , ARRAY SIZE/ 2 , MPI INT ,
FILHO1 NUMBER, ARRAY SIZE/ 2 , MPI COMM WORLD, &s t a t u s ) ;
MPI Recv(&LINHA [ ARRAY SIZE / 2 ] , ARRAY SIZE/ 2 , MPI INT ,
FILHO2 NUMBER, ARRAY SIZE/ 2 , MPI COMM WORLD, &s t a t u s ) ;
// I n t e r l a c a v e t o r e s r e s u l t a n t e s
LINHA = i n t e r l e a v i n g (LINHA, ARRAY SIZE ) ;
time2 = MPI Wtime ( ) ; // Acaba a contagem do tempo
p r i n t f ( ”Tempo de e x e c u c a o : %f \n\n ” , time2−time1 ) ;}
e l s e // NODOS INTERMEDIARIOS E FOLHAS
{
// A n t e c i p a r e c e b i m e n t o da mensagem
MPI Probe (MPI ANY SOURCE, MPI ANY TAG, MPI COMM WORLD, &s t a t u s ) ;
// Tamanho do v e t o r r e c e b i d o
// ( f o i a p r o v e i t a d o o campo TAG do MPI, que nao s e r i a u t i l i z a d o )
int TAMANHO = s t a t u s . MPI TAG ;
int METADE TAMANHO = TAMANHO/ 2 ;
// Numero do p r o c e s s o p a i
int SOURCE NUMBER = s t a t u s . MPI SOURCE ;
MPI Recv (LINHA, TAMANHO, MPI INT , SOURCE NUMBER, TAMANHO,
MPI COMM WORLD, &s t a t u s ) ;
i f ( d e l t a > TAMANHO) {
// Vetor maior do que o v a l o r de d e l t a
// D i v i d e a l i s t a p e l a metade e manda para os f i l h o s
MPI Send(&LINHA [ 0 ] , METADE TAMANHO, MPI INT ,
FILHO1 NUMBER, METADE TAMANHO, MPI COMM WORLD) ;
MPI Send(&LINHA [METADE TAMANHO] , METADE TAMANHO,
MPI INT , FILHO2 NUMBER, METADE TAMANHO, MPI COMM WORLD) ;
// Recebe de v o l t a os v e t o r e s ordena dos
MPI Recv(&LINHA [ 0 ] , METADE TAMANHO, MPI INT ,
FILHO1 NUMBER, METADE TAMANHO, MPI COMM WORLD,
&s t a t u s ) ;
MPI Recv(&LINHA [METADE TAMANHO] , METADE TAMANHO,
MPI INT , FILHO2 NUMBER, METADE TAMANHO, MPI COMM WORLD,
&s t a t u s ) ;
// I n t e r c a l a v e t o r e s
int ∗LINHA INTERCALADA
= i n t e r l e a v i n g (LINHA, TAMANHO) ;
// Manda o v e t o r ordenado de v o l t a para o p r o c e s s o p a i
MPI Send (LINHA INTERCALADA, TAMANHO, MPI INT ,
SOURCE NUMBER, TAMANHO, MPI COMM WORLD) ;}
else
{
// D e l t a menor ou i g u a l ao tamanho do v e t o r
// Ordena e d e v o l v e ao p r o c e s s o p a i
b u b b l e s o r t (TAMANHO, LINHA ) ;
MPI Send (LINHA, TAMANHO, MPI INT , SOURCE NUMBER,
TAMANHO, MPI COMM WORLD) ;
}
}
MPI Finalize ( ) ;
}
}
