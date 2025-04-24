#include "cpu.h"
#include <stdio.h>

void inicializar_cpu(CPU *cpu, Barramento *bus) {
    inicializar_registradores(&cpu->reg);
    inicializar_uc(&cpu->uc);
    cpu->bus = bus;
}

void executar_ciclo(CPU *cpu, RAM *ram) {
	imprimir_registradores(&cpu->reg);
    while (!cpu->uc.halt) { 
        switch(cpu->reg.ICC){
			case 0:
                printf(">>> Ciclo de Busca\n");
                ciclo_busca(cpu, ram);
                break;
            case 1:
                ciclo_indireto(cpu, ram);
                break;
            case 2:
                ciclo_execucao(cpu, ram);
                break;
            case 3:
                ciclo_interrupcao(cpu, ram);
                break;
            default:
                printf("Erro: ICC não reconhecido\n");
                cpu->uc.halt = 1;
                break;
				
		};
		imprimir_registradores(&cpu->reg);
		//getchar();
    }
}
