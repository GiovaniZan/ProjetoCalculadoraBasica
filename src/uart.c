// serial.c
// Desenvolvido para a placa EK-TM4C1294XL
// Inicializa a porta serial UART0
// Prof. Guilherme Peron

#include <stdint.h>

#include "tm4c1294ncpdt.h"

#define UART_UART0 (0x0001) //bit 0
#define GPIO_PORTA (0x0001) //bits 0

// -------------------------------------------------------------------------------
// Função Uart0_Init
// Inicializa a serial Uart0
// Parâmetro de entrada: Não tem
// Parâmetro de saída: Não tem
void Uart0_Init(void)
{
    //1a. Ativar o clock da UART0
    SYSCTL_RCGCUART_R |= UART_UART0;        //ativar o clock da UART0
    //1b. Verificar no PRUART se a UART está pronta para uso.
    while((SYSCTL_PRUART_R & (UART_UART0) ) != (UART_UART0) ){};
    // 2. Desabilitar a UART
    UART0_CTL_R &= ~0x0001;
    // 3. Baud-rate nos registradores UARTIBRD e UARTFBRD
    // Para um clock de 16 MHz e um baud de 9600 bps (clockdiv=16)
    UART0_IBRD_R = 104;  //Parte inteira
    UART0_FBRD_R = 11;  //Parte fracionária    
    // 4. Configurar 8 bits, sem FIFO, sem paridade e 1 stop-bit
    UART0_LCRH_R = 0x0060;
    // 5. Garantir que a fonte de clock seja o clock do sistema
    UART0_CC_R = 0x0000;
    // 6. Habilitar o RXE, TXE e UART
    UART0_CTL_R = 0x0301;        
    
    //GPIO
    //7a. Habilitar o clock no GPIO do port A
    SYSCTL_RCGCGPIO_R |= GPIO_PORTA;        //ativar o clock do PortA
    //7b. Verificar no PRGPIO se a porta está pronta para uso.
    while((SYSCTL_PRGPIO_R & (GPIO_PORTA) ) != (GPIO_PORTA) ){};
    // 8. Desabilitar a funcionalidade analógica 
    GPIO_PORTA_AHB_AMSEL_R &= ~0x03;
    // 9. Escolher a função alternativa para TX e RX 
    // Função 1 nos 2 nibles inferiores    
    GPIO_PORTA_AHB_PCTL_R = (GPIO_PORTA_AHB_PCTL_R & 0xFFFFFF00) | 0x00000011;
    //10. Habilitar os bits de função alternativa nos bits 0 e 1
    GPIO_PORTA_AHB_AFSEL_R |= 0x03;
    //11. Habilitar a função digital
    GPIO_PORTA_AHB_DEN_R |= 0x03;
}

// -------------------------------------------------------------------------------
// Função Uart0_Rcv
// Espera a recepção de um byte da serial
// Parâmetro de entrada: Não tem
// Parâmetro de saída: Byte recebido da serial
uint8_t Uart0_Rcv(void)
{
    // Esperar enquanto a FIFO estiver vazia
    while ((UART0_FR_R&0x0010) !=0) {};
    // Verificar se a FIFO não está vazia, então há caracter para receber
      return ((uint8_t)(UART0_DR_R & 0xFF));
}

// -------------------------------------------------------------------------------
// Função Uart0_Send
// Espera a recepção de um byte da serial
// Parâmetro de entrada: Byte a ser enviado pela serial
// Parâmetro de saída: Não tem
void Uart0_Send(uint8_t dado)
{
    // Esperar enquanto a FIFO estiver cheia
    while ((UART0_FR_R&0x0020) !=0) {};
    // Escreve no registrador de dados o dado a ser enviado
    UART0_DR_R = dado;
}
