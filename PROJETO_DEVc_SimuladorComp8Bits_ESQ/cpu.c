#include "cpu.h"
#include <stdio.h>

void inicializar_cpu(CPU *cpu, Barramento *bus) {
    inicializar_registradores(&cpu->reg);
    inicializar_uc(&cpu->uc);
    cpu->bus = bus;
}

void executar_ciclo(CPU *cpu, RAM *ram) {
	imprimir_registradores(&cpu->reg);
    while (!cpu->uc.halt) {  // Loop contínuo para simular um processador real
        switch(cpu->reg.ICC){
			
			
			//Fluxo do Ciclo de Instrução
			
				
		};
		imprimir_registradores(&cpu->reg);
		//getchar();
    }
}

