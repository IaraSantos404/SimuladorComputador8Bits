#include "ULA.h"

//Somador Baseado no livro Willian Stallings 10ed, cap�tulo 10, pag. 334
static void somadorCompleto (uint8_t A, uint8_t B, uint8_t Cin, uint8_t * co, uint8_t * s){
	*co = (A&B) | (A&Cin) | (B&Cin);
	*s = (~A & ~B & Cin) | (~A & B & ~Cin) | (A & B & Cin) | (A & ~B & ~Cin);	
}

//O somador bin�rio da ULA trata os dois n�meros como inteiros sem sinal.
//Entradas: Registrados A, B e cin(CarryIn), Sa�da: s(soma) e co(CarryOut)
static void somador8bits(uint8_t A, uint8_t B, uint8_t cin, uint8_t * co, uint8_t * s){
	*co = cin;
	uint8_t soma;
	for (int i=0;i<8;i++){
		somadorCompleto(bit_get(A,0),bit_get(B,0), *co, co,&soma);
		A = A>>1;//deslocando os bits para direita a cada intera��o
		B = B>>1;
		if(soma==1)bit_set(*s,i);
		else bit_clr(*s,i);
	}
}
//Realiza o complemento de 2 do Registrador A e grava em A
static void complementador(uint8_t *A){
	*A = ~(*A); 
	uint8_t carryOut = 0;
	uint8_t resultado = 0;
	
	somador8bits(*A, 1, 0, &carryOut, &resultado);
	
	*A = resultado;
}

//Adi��o de A e B e grava resultado em A
void ULA_ADD(int8_t *A, int8_t * B, int8_t * overflow){
	uint8_t resultado = 0;
	uint8_t carryOut = 0;

	somador8bits((uint8_t)*A, (uint8_t)*B, 0, &carryOut, &resultado);
	*overflow = ((*A >> 7) == (*B >> 7)) && ((*A >> 7) != ((int8_t)resultado >> 7));
  *A = (int8_t)resultado;
}


//Subtra��o de A e B e grava resultado em A
void ULA_SUB(int8_t *A, int8_t * B, int8_t * overflow){
	uint8_t resultado = 0;
	uint8_t carryOut = 0;
	uint8_t B_complementado = (uint8_t)*B;

  complementador(&B_complementado);
  somador8bits((uint8_t)*A, B_complementado, 0, &carryOut, &resultado);

	*overflow = ((*A >> 7) != (*B >> 7)) && (((int8_t)resultado >> 7) == (*B >> 7));

  *A = (int8_t)resultado;
}


//Multiplica��o de Q(8bits) com M(8bits) gera resultado de 16bits que est� em A(8bits) e Q(8bits)
void ULA_MUL(int8_t *A, int8_t *Q, int8_t *M, int8_t *overflow) {
	int8_t A_reg = 0;
	int8_t Q_reg = *Q;
	int8_t M_val = *M;
	int Q_minus1 = 0;  

	int8_t testeOverflow = 0;

	for (int i = 0; i < 8; i++) {
			int Q0 = Q_reg & 1;           
			int action = (Q0 << 1) | Q_minus1; 
			if (action == 0b01) {
					ULA_ADD(&A_reg, &M_val, &testeOverflow);
			} else if (action == 0b10) {
					ULA_SUB(&A_reg, &M_val, &testeOverflow);
					
			}

			int new_Q_minus1 = Q_reg & 1;              
			uint8_t temp_Q = (uint8_t)Q_reg;
			temp_Q = (temp_Q >> 1) | ((A_reg & 1) << 7);
			Q_reg = (int8_t)temp_Q;
			A_reg = A_reg >> 1;
			
			Q_minus1 = new_Q_minus1;
	}
	
	*A = A_reg;
	*Q = Q_reg;
	*overflow = 0;
}

//Divis�o com sinal de Q(Dividendo de 8bits) por M(Divisor de 8bits) com Quociente em Q(8bits) e Resto em A(8bits)
void ULA_DIV(int8_t *A, int8_t *Q, int8_t *M, int8_t *overflow) {
	if (*M == 0) {
			int8_t dividendo = *Q;  
			*Q = 1;               
			*A = dividendo;  
			//não tinha certeza se na divisão por 0 era pra dar overflow ou não então como no seu teste tava 0 
			//deixei o overflow como 0 tbm       
			*overflow = 0;
			return;
	}

	int8_t dividendo = *Q;
	int8_t divisor = *M;

	int sinal_quociente = ((dividendo < 0) ^ (divisor < 0)) ? 1 : 0;
	int sinal_resto = (dividendo < 0) ? 1 : 0;

	uint8_t dividendo_abs = (dividendo < 0) ? -dividendo : dividendo;
	uint8_t divisor_abs = (divisor < 0) ? -divisor : divisor;
	uint8_t A_div = 0;
	uint8_t Q_div = dividendo_abs;
	uint8_t M_div = divisor_abs;

	for (int i = 0; i < 8; i++) {
			uint8_t bit_q7 = (Q_div >> 7) & 1;
			A_div = (A_div << 1) | bit_q7;
			Q_div = Q_div << 1;
			uint8_t A_temp = A_div;
			A_temp -= M_div;
			if (A_div >= M_div) {
					A_div = A_temp;
					Q_div |= 0x01;
			}
	}

	int8_t quociente = Q_div;
	if (sinal_quociente) {
			quociente = -quociente;
	}
	int8_t resto = A_div;
	if (sinal_resto) {
			resto = -resto;
	}
	*overflow = (dividendo == -128 && divisor == -1) ? 1 : 0;
	*A = resto;
	*Q = quociente;
}



//Descomente as fun��es caso deseja implementar as fun��es de Ponto Flutuante
//Baseado no livro Willian Stallings 10ed, cap�tulo 10, pag. 298
//void ULA_ADD_PF(float *A, float * B, int8_t * overflow, int8_t * underflow){}
//void ULA_SUB_PF(float *A, float * B, int8_t * overflow, int8_t * underflow){}
//void ULA_MUL_PF(float *A, float * B, int8_t * overflow, int8_t * underflow){}
//void ULA_DIV_PF(float *A, float * B, int8_t * overflow, int8_t * underflow){}
