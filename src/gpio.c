// gpio.c
// Desenvolvido para a placa EK-TM4C1294XL
// Inicializa as portas J e N
// Prof. Guilherme Peron / Prof. Rafael de Góes


#include <stdint.h>
#include "tm4c1294ncpdt.h"

#define GPIO_PORTJ (0x0100) //bit 8
#define GPIO_PORTN (0x1000) //bit 12
#define GPIO_PORTN_LEDS   (*((volatile uint32_t *)0x4006403C))
#define GPIO_PORTJ_BOTOES (*((volatile uint32_t *)0x4006003C))


// -------------------------------------------------------------------------------
// Função GPIO_Init
// Inicializa os ports J e N
// Parâmetro de entrada: Não tem
// Parâmetro de saída: Não tem
void GPIO_Init(void)
{
	//1a. Ativar o clock para a porta setando o bit correspondente no registrador RCGCGPIO
	SYSCTL_RCGCGPIO_R |= GPIO_PORTJ | GPIO_PORTN;
	//1b.   após isso verificar no PRGPIO se a porta está pronta para uso.
        while((SYSCTL_PRGPIO_R & (GPIO_PORTJ | GPIO_PORTN) ) != (GPIO_PORTJ | GPIO_PORTN) ){};
	
	// 2. Destravar a porta somente se for o pino PD7 e PE7
		
	// 3. Limpar o AMSEL para desabilitar a analógica
	GPIO_PORTJ_AHB_AMSEL_R = 0x00;
	GPIO_PORTN_AMSEL_R = 0x00;
		
	// 4. Limpar PCTL para selecionar o GPIO
	GPIO_PORTJ_AHB_PCTL_R = 0x00;
	GPIO_PORTN_PCTL_R = 0x00;

	// 5. DIR para 0 se for entrada, 1 se for saída
	GPIO_PORTJ_AHB_DIR_R = 0x00;
	GPIO_PORTN_DIR_R = 0x03; //BIT0 | BIT1
		
	// 6. Limpar os bits AFSEL para 0 para selecionar GPIO sem função alternativa	
	GPIO_PORTJ_AHB_AFSEL_R = 0x00;
	GPIO_PORTN_AFSEL_R = 0x00; 
		
	// 7. Setar os bits de DEN para habilitar I/O digital	
	GPIO_PORTJ_AHB_DEN_R = 0x03;   //Bit0 e bit1
	GPIO_PORTN_DEN_R = 0x03;       //Bit0 e bit1
	
	// 8. Habilitar resistor de pull-up interno, setar PUR para 1
	GPIO_PORTJ_AHB_PUR_R = 0x03;   //Bit0 e bit1	
        
}	

// -------------------------------------------------------------------------------
// Função PortJ_Input
// Lê os valores de entrada do port J
// Parâmetro de entrada: Não tem
// Parâmetro de saída: o valor da leitura do port
uint32_t PortJ_Input(void)
{
	return GPIO_PORTJ_BOTOES;
}

// -------------------------------------------------------------------------------
// Função PortN_Output
// Escreve os valores no port N
// Parâmetro de entrada: Valor a ser escrito
// Parâmetro de saída: não tem
void PortN_Output(uint32_t valor)
{
	//Ponteiro para o valor dos bits com leitura amigável
	GPIO_PORTN_LEDS = valor;
}


