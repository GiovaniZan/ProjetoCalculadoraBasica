/**************************************************
 *
 * This file contains an interrupt vector for Cortex-M written in C.
 * The actual interrupt functions must be provided by the application developer.
 *
 * Copyright 2007-2017 IAR Systems AB.
 *
 * $Revision: 112610 $
 * Adaptado por Guilherme Peron / Prof. Rafael de Góes
 *
 * Acrescentado interrupção de UART0 por Giovani Zanelatto 
 **************************************************/

#pragma language=extended
#pragma segment="CSTACK"

extern void __iar_program_start( void );

extern void NMI_Handler( void );
extern void HardFault_Handler( void );
extern void MemManage_Handler( void );
extern void BusFault_Handler( void );
extern void UsageFault_Handler( void );
extern void SVC_Handler( void );
extern void DebugMon_Handler( void );
extern void PendSV_Handler( void );
extern void SysTick_Handler( void );
extern void SysTick_Handler( void );
extern void IntDefaultHandler( void );

//extern void UART0_Handler( void);         

typedef void( *intfunc )( void );
typedef union { intfunc __fun; void * __ptr; } intvec_elem;

// The vector table is normally located at address 0.
// When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
// If you need to define interrupt service routines,
// make a copy of this file and include it in your project.
// The name "__vector_table" has special meaning for C-SPY, which
// is where to find the SP start value.
// If vector table is not located at address 0, the user has to initialize
// the  NVIC vector table register (VTOR) before using interrupts.


#pragma location = ".intvec"
const intvec_elem __vector_table[] =
{
  { .__ptr = __sfe( "CSTACK" ) },
  __iar_program_start,

  NMI_Handler,
  HardFault_Handler,
  MemManage_Handler,
  BusFault_Handler,
  UsageFault_Handler,
  0,
  0,
  0,
  0,
  SVC_Handler,
  DebugMon_Handler,
  0,
  PendSV_Handler,
  SysTick_Handler,
  /*Colocar o nome da ISR aqui e definir como extern void acima*/
  IntDefaultHandler,                      // GPIO Port A
  IntDefaultHandler,                      // GPIO Port B
  IntDefaultHandler,                      // GPIO Port C
  IntDefaultHandler,                      // GPIO Port D
  IntDefaultHandler,                      // GPIO Port E
  IntDefaultHandler, //UART0_Handler,                      // UART0 Rx and Tx
  IntDefaultHandler,                      // UART1 Rx and Tx
  IntDefaultHandler,                      // SSI0 Rx and Tx
  IntDefaultHandler,                      // I2C0 Master and Slave
  IntDefaultHandler,                      // PWM Fault
  IntDefaultHandler,                      // PWM Generator 0
  IntDefaultHandler,                      // PWM Generator 1
  IntDefaultHandler,                      // PWM Generator 2
  IntDefaultHandler,                      // Quadrature Encoder 0
  IntDefaultHandler,                      // ADC Sequence 0
  IntDefaultHandler,                      // ADC Sequence 1
  IntDefaultHandler,                      // ADC Sequence 2
  IntDefaultHandler,                      // ADC Sequence 3
  IntDefaultHandler,                      // Watchdog timer
  IntDefaultHandler,                      // Timer 0 subtimer A
  IntDefaultHandler,                      // Timer 0 subtimer B
  IntDefaultHandler,                      // Timer 1 subtimer A
  IntDefaultHandler,                      // Timer 1 subtimer B
  IntDefaultHandler,                      // Timer 2 subtimer A
  IntDefaultHandler,                      // Timer 2 subtimer B
  IntDefaultHandler,                      // Analog Comparator 0
  IntDefaultHandler,                      // Analog Comparator 1
  IntDefaultHandler,                      // Analog Comparator 2
  IntDefaultHandler,                      // System Control (PLL, OSC, BO)
  IntDefaultHandler,                      // FLASH Control
  IntDefaultHandler,                      // GPIO Port F
  IntDefaultHandler,                      // GPIO Port G
  IntDefaultHandler,                      // GPIO Port H
  IntDefaultHandler,                      // UART2 Rx and Tx
  IntDefaultHandler,                      // SSI1 Rx and Tx
  IntDefaultHandler,                      // Timer 3 subtimer A
  IntDefaultHandler,                      // Timer 3 subtimer B
  IntDefaultHandler,                      // I2C1 Master and Slave
  IntDefaultHandler,                      // CAN0
  IntDefaultHandler,                      // CAN1
  IntDefaultHandler,                      // Ethernet
  IntDefaultHandler,                      // Hibernate
  IntDefaultHandler,                      // USB0
  IntDefaultHandler,                      // PWM Generator 3
  IntDefaultHandler,                      // uDMA Software Transfer
  IntDefaultHandler,                      // uDMA Error
  IntDefaultHandler,                      // ADC1 Sequence 0
  IntDefaultHandler,                      // ADC1 Sequence 1
  IntDefaultHandler,                      // ADC1 Sequence 2
  IntDefaultHandler,                      // ADC1 Sequence 3
  IntDefaultHandler,                      // External Bus Interface 0
  IntDefaultHandler,                      // GPIO Port J
  IntDefaultHandler,                      // GPIO Port K
  IntDefaultHandler,                      // GPIO Port L
  IntDefaultHandler,                      // SSI2 Rx and Tx
  IntDefaultHandler,                      // SSI3 Rx and Tx
  IntDefaultHandler,                      // UART3 Rx and Tx
  IntDefaultHandler,                      // UART4 Rx and Tx
  IntDefaultHandler,                      // UART5 Rx and Tx
  IntDefaultHandler,                      // UART6 Rx and Tx
  IntDefaultHandler,                      // UART7 Rx and Tx
  IntDefaultHandler,                      // I2C2 Master and Slave
  IntDefaultHandler,                      // I2C3 Master and Slave
  IntDefaultHandler,                      // Timer 4 subtimer A
  IntDefaultHandler,                      // Timer 4 subtimer B
  IntDefaultHandler,                      // Timer 5 subtimer A
  IntDefaultHandler,                      // Timer 5 subtimer B
  IntDefaultHandler,                      // FPU
  0,                                      // Reserved
  0,                                      // Reserved
  IntDefaultHandler,                      // I2C4 Master and Slave
  IntDefaultHandler,                      // I2C5 Master and Slave
  IntDefaultHandler,                      // GPIO Port M
  IntDefaultHandler,                      // GPIO Port N
  0,                                      // Reserved
  IntDefaultHandler,                      // Tamper
  IntDefaultHandler,                      // GPIO Port P (Summary or P0)
  IntDefaultHandler,                      // GPIO Port P1
  IntDefaultHandler,                      // GPIO Port P2
  IntDefaultHandler,                      // GPIO Port P3
  IntDefaultHandler,                      // GPIO Port P4
  IntDefaultHandler,                      // GPIO Port P5
  IntDefaultHandler,                      // GPIO Port P6
  IntDefaultHandler,                      // GPIO Port P7
  IntDefaultHandler,                      // GPIO Port Q (Summary or Q0)
  IntDefaultHandler,                      // GPIO Port Q1
  IntDefaultHandler,                      // GPIO Port Q2
  IntDefaultHandler,                      // GPIO Port Q3
  IntDefaultHandler,                      // GPIO Port Q4
  IntDefaultHandler,                      // GPIO Port Q5
  IntDefaultHandler,                      // GPIO Port Q6
  IntDefaultHandler,                      // GPIO Port Q7
  IntDefaultHandler,                      // GPIO Port R
  IntDefaultHandler,                      // GPIO Port S
  IntDefaultHandler,                      // SHA/MD5 0
  IntDefaultHandler,                      // AES 0
  IntDefaultHandler,                      // DES3DES 0
  IntDefaultHandler,                      // LCD Controller 0
  IntDefaultHandler,                      // Timer 6 subtimer A
  IntDefaultHandler,                      // Timer 6 subtimer B
  IntDefaultHandler,                      // Timer 7 subtimer A
  IntDefaultHandler,                      // Timer 7 subtimer B
  IntDefaultHandler,                      // I2C6 Master and Slave
  IntDefaultHandler,                      // I2C7 Master and Slave
  IntDefaultHandler,                      // HIM Scan Matrix Keyboard 0
  IntDefaultHandler,                      // One Wire 0
  IntDefaultHandler,                      // HIM PS/2 0
  IntDefaultHandler,                      // HIM LED Sequencer 0
  IntDefaultHandler,                      // HIM Consumer IR 0
  IntDefaultHandler,                      // I2C8 Master and Slave
  IntDefaultHandler,                      // I2C9 Master and Slave
  IntDefaultHandler                       // GPIO Port T  
};

#pragma call_graph_root = "interrupt"
__weak void NMI_Handler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void HardFault_Handler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void MemManage_Handler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void BusFault_Handler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void UsageFault_Handler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void SVC_Handler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void DebugMon_Handler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void PendSV_Handler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void SysTick_Handler( void ) { while (1) {} }
#pragma call_graph_root = "interrupt"
__weak void IntDefaultHandler( void ) { while (1) {} }



void __cmain( void );
__weak void __iar_init_core( void );
__weak void __iar_init_vfp( void );

#pragma required=__vector_table
void __iar_program_start( void )
{
  __iar_init_core(); 
  __iar_init_vfp();
  __cmain();
}


