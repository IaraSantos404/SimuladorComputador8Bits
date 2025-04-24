#include "uc.h"
#include "cpu.h"
#include <stdio.h>

void inicializar_uc(UnidadeControle *uc) {
    uc->halt = 0;
}

//alterado
void ciclo_busca(CPU *cpu, RAM *ram) {

	cpu->reg.MAR = cpu->reg.PC;
	cpu->reg.PC++;

	enviar_dado(cpu->bus, ram, cpu->reg.MAR, 0, "READ");
	cpu->reg.MBR = cpu->bus->dado;
	cpu->reg.IR = cpu->reg.MBR;

	if ((cpu->reg.IR & 0xF0) == LDA)
			cpu->reg.ICC = 1;
	else
			cpu->reg.ICC = 2;
}

void ciclo_indireto(CPU *cpu, RAM *ram) {
	
	cpu->reg.MAR = cpu->reg.IR & 0x0F;
	
	enviar_dado(cpu->bus, ram, cpu->reg.MAR, 0, "READ");
	
	cpu->reg.MBR = cpu->bus->dado & 0xFF;

	cpu->reg.IR = (cpu->reg.IR & 0xF0) | (cpu->reg.MBR & 0x0F);
	
	cpu->reg.ICC = 2;
}

//alterado

int ciclo_execucao(CPU *cpu, RAM *ram) {
	uint8_t instrucao = cpu->reg.IR & 0xF0;
	switch(instrucao) {
			case LDI:
					printf(">>> LDI executando\n");
					ldi_exec(cpu);
					break;
			case LDD:
					printf(">>> LDD executando\n");
					ldd_exec(cpu, ram);
					break;
			case STA:
					printf(">>> STA executando\n");
					sta_exec(cpu, ram);
					break;
			case ADD:
					printf(">>> ADD executando\n");
					add_exec(cpu, ram);
					break;
			case SUB:
					printf(">>> SUB executando\n");
					sub_exec(cpu, ram);
					break;
			case MUL:
					printf(">>> MUL executando\n");
					mul_exec(cpu, ram);
					break;
			case DIV:
					printf(">>> DIV executando\n");
					div_exec(cpu, ram);
					break;
			case OPC_OUT:
					printf(">>> OUT executando\n");
					out_exec(cpu, ram);
					break;
			case HLT:
					printf(">>> HALT processor\n");
					cpu->uc.halt = 1;
					return 0;
			default:
					printf("Instrução desconhecida: %X\n", instrucao);
					break;
	}
	return 1;
}



void ciclo_interrupcao(CPU *cpu, RAM *ram) {
	// FLAG == 2 indica divisão por zero
	// FLAG == 1 indica overflow
	if (cpu->reg.FLAG == 2) {
			printf(">>> ULA -> DIVISAO POR ZERO\n");
	} else if (cpu->reg.FLAG == 1) {
			printf(">>> ULA -> OVERFLOW\n");
	}
	
	cpu->reg.MBR = cpu->reg.PC;
	
	cpu->reg.MAR = 0x0F;
	
	// Escreve o valor do PC (armazenado no MBR) na memória no endereço 0x0F
	enviar_dado(cpu->bus, ram, cpu->reg.MAR, cpu->reg.MBR, "WRITE");

	// Executa o ciclo de busca de instruções até que seja encontrada a instrução HALT
	while ((cpu->reg.IR & 0xF0) != HLT) {
			printf(">>> Ciclo de Busca (Interrupcao)\n");
			ciclo_busca(cpu, ram);
			imprimir_registradores(&cpu->reg);
	}
}


void ldi_exec(CPU *cpu) {
	// Extrai os 4 bits menos significativos da instrução (operando imediato)
	cpu->reg.AC = cpu->reg.IR & 0x0F;
	
	// Reseta o indicador de condição (ICC) para 0
	cpu->reg.ICC = 0;
}


void ldd_exec(CPU *cpu, RAM *ram) {
	cpu->reg.MAR = cpu->reg.IR & 0x0F;
	
	enviar_dado(cpu->bus, ram, cpu->reg.MAR, 0, "READ");
	
	cpu->reg.AC = cpu->bus->dado & 0xFF;
	
	cpu->reg.ICC = 0;
}


void sta_exec(CPU *cpu, RAM *ram){
	cpu->reg.MAR = cpu->reg.IR & 0x0F;
    enviar_dado(cpu->bus, ram, cpu->reg.MAR, cpu->reg.AC, "WRITE");
    cpu->reg.ICC = 0;
}


void add_exec(CPU *cpu, RAM *ram) {
	cpu->reg.MAR = cpu->reg.IR & 0x0F;
	
	enviar_dado(cpu->bus, ram, cpu->reg.MAR, 0, "READ");
	cpu->reg.MBR = cpu->bus->dado & 0xFF;
	
	ULA_ADD(&cpu->reg.AC, (int8_t *)&cpu->reg.MBR, &cpu->reg.FLAG);
	
	if(cpu->reg.FLAG == 1){
		cpu->reg.ICC = 3;
	}else{
		cpu->reg.ICC = 0;
	}
}



void sub_exec(CPU *cpu, RAM *ram) {
	cpu->reg.MAR = cpu->reg.IR & 0x0F;
	
	enviar_dado(cpu->bus, ram, cpu->reg.MAR, 0, "READ");
	cpu->reg.MBR = cpu->bus->dado & 0xFF;

	
	ULA_SUB(&cpu->reg.AC, (int8_t *)&cpu->reg.MBR, &cpu->reg.FLAG);
	
	if(cpu->reg.FLAG == 1){
		cpu->reg.ICC = 3;
	}else{
		cpu->reg.ICC = 0;
	}
}



void mul_exec(CPU *cpu, RAM *ram){
	cpu->reg.MAR = cpu->reg.IR & 0x0F;

	enviar_dado(cpu->bus, ram, cpu->reg.MAR, 0, "READ");

	cpu->reg.MBR = cpu->bus->dado & 0x00FF;
	cpu->reg.Y = cpu->reg.AC;

	ULA_MUL(&cpu->reg.Z, &cpu->reg.Y, &cpu->reg.MBR, &cpu->reg.FLAG);
	cpu->reg.AC = cpu->reg.Y;

	if(cpu->reg.FLAG == 1){
		cpu->reg.ICC = 3;
	}else{
		cpu->reg.ICC = 0;
	}
}

void div_exec(CPU *cpu, RAM *ram) {

	cpu->reg.MAR = cpu->reg.IR & 0x0F;
	
	enviar_dado(cpu->bus, ram, cpu->reg.MAR, 0, "READ");
	cpu->reg.MBR = cpu->bus->dado & 0xFF;
	
	if (cpu->reg.MBR == 0) {
			cpu->reg.FLAG = 2;  
			cpu->reg.ICC = 3;
			
	} else {
			
			cpu->reg.Y = cpu->reg.AC;
			
			ULA_DIV(&cpu->reg.Z, &cpu->reg.Y, &cpu->reg.MBR, &cpu->reg.FLAG);
			
			cpu->reg.AC = cpu->reg.Y;
			
			if (cpu->reg.FLAG == 1)
					cpu->reg.ICC = 3;
			else
					cpu->reg.ICC = 0;
	}
}


void out_exec(CPU *cpu, RAM * ram){
	cpu->reg.OTR = cpu->reg.AC;
	cpu->reg.PC-=2;
	ciclo_busca(cpu, ram);	
	if((cpu->reg.IR&0xF0)==MUL)
		printf("DISPLAY\tMUL: REG Z: %d\tREG OTR:%d\tZOTR: %d\n",(int8_t)cpu->reg.Z, (int8_t)cpu->reg.OTR,(int16_t) (((cpu->reg.Z & 0x00FF) << 8) | (cpu->reg.OTR & 0x00FF)));
	else if((cpu->reg.IR&0xF0)==DIV)
		printf("DISPLAY\tDIV: Resto em REG Z: %d\tQuociente em REG OTR:%d\n",(int8_t)cpu->reg.Z, (int8_t)cpu->reg.OTR);
	else
		printf(" DISPLAY: %d\n", (int8_t) cpu->reg.OTR);
	
	cpu->reg.PC++;
	cpu->reg.ICC = 0;
}

