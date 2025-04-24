#include "ULA.h"


//Somador Baseado no livro Willian Stallings 10ed, capítulo 10, pag. 334
static void somadorCompleto (uint8_t A, uint8_t B, uint8_t Cin, uint8_t * co, uint8_t * s){
	*co = (A&B) | (A&Cin) | (B&Cin);
	//*s = A ^ B ^ Cin;
	*s = (~A & ~B & Cin) | (~A & B & ~Cin) | (A & B & Cin) | (A & ~B & ~Cin);	
}

//O somador binário da ULA trata os dois números como inteiros sem sinal.
static void somador8bits(uint8_t A, uint8_t B, uint8_t cin, uint8_t * co, uint8_t * s){
	*co = cin;
	uint8_t soma;
	for (int i=0;i<8;i++){
		somadorCompleto(bit_get(A,0),bit_get(B,0), *co, co,&soma);
		A = A>>1;//deslocando os bits para direita a cada interação
		B = B>>1;
		if(soma==1)bit_set(*s,i);
		else bit_clr(*s,i);
	}
}

static void complementador(uint8_t *A){
	
}


//Adição de A e B e grava resultado em A
void ULA_ADD(int8_t *A, int8_t * B, int8_t * overflow){ 
	
}

//Subtração de A e B e grava resultado em A
void ULA_SUB(int8_t *A, int8_t * B, int8_t * overflow){ 
		
}

//Multiplicação de Q(8bits) com M(8bits) gera resultado de 16bits que está em A(8bits) e Q(8bits)
void ULA_MUL(int8_t * A, int8_t *Q, int8_t * M,  int8_t * overflow){
		
}


//Divisão com sinal de Q(Dividendo de 8bits) por M(Divisor de 8bits) com Quociente em Q(8bits) e Resto em A(8bits)
void ULA_DIV(int8_t *A, int8_t *Q, int8_t * M, int8_t * overflow){
			
}


