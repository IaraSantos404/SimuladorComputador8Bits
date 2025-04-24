#include "uc.h"
#include "cpu.h"
#include <stdio.h>

void inicializar_uc(UnidadeControle *uc) {
    uc->halt = 0;
}

void ciclo_busca(CPU *cpu, RAM * ram){
	
}

void ciclo_indireto(CPU *cpu, RAM * ram){
	
}


void ciclo_execucao(CPU *cpu, RAM *ram) {
    	
    return 1;
}

void ciclo_interrupcao(CPU *cpu, RAM *ram){
		
}


void ldi_exec(CPU *cpu){
	
}

void ldd_exec(CPU *cpu, RAM *ram){
	
}

void sta_exec(CPU *cpu, RAM *ram){
	
}

void add_exec(CPU *cpu, RAM *ram){
		
}

void sub_exec(CPU *cpu, RAM *ram){
	
}

void mul_exec(CPU *cpu, RAM *ram){
	
}

void div_exec(CPU *cpu, RAM *ram){
		
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

