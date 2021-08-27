# PROJETO FINAL DE SISTEMAS MICROCONTROLADOS

## Contexto:

Utilizar programação em linguagem Assembly para configurar periféricos integrados
de um microcontrolador e implementar interface com o usuário para uma aplicação
específica.

## Objetivo:

Escrever um programa em linguagem Assembly para o kit EK-TM4C1294XL que
implemente uma calculadora capaz de realizar as quatro operações aritméticas básicas
(adição, subtração, multiplicação e divisão) com números inteiros de até três dígitos. A
entrada e saída de dados para o usuário se dará por meio de comunicação pela porta
serial do kit (COM virtual sobre USB) usando caracteres ASCII, de forma que um
aplicativo emulador de terminal possa ser utilizado como interface, conforme diagrama
em blocos ilustrado na Figura 1.

## Requisitos 

### Requisitos funcionais do sistema:

1. O sistema deverá receber o primeiro operando pela UART numa sequência de até
   três caracteres ASCII correspondentes aos dígitos decimais de 0 a 9. 
   1. RF1.1: O sistema deverá ecoar os caracteres recebidos, desde que sejam válidos,
      retransmitindo os de volta pela UART para realimentação do usuário.
   2. RF1.2: O sistema deverá converter os caracteres ASCII recebidos em um valor numérico
      equivalente para a realização da operação aritmética a ser selecionada em seguida.
2. RF2: O sistema deverá receber um caractere ASCII para selecionar a operação aritmética.
   1. RF2.1: O caractere ‘+’ deverá corresponder à operação aritmética de adição inteira.
   2. RF2.2: O caractere ‘-’ deverá corresponder à operação aritmética de subtração inteira.
   3. RF2.3: O caractere ‘*’ deverá corresponder à operação aritmética de multiplicação inteira.
   4. RF2.4: O caractere ‘/’ deverá corresponder à operação aritmética de divisão inteira. 
   5. RF2.5: A recepção de qualquer dos caracteres descritos nos requisitos RF2.1 a RF2.4 deve
      encerrar a recepção do primeiro operando caso o limite máximo de 4 dígitos não tenha
      sido atingido. 
   6. RF2.6: O sistema deverá ecoar o caractere recebido, desde que seja válido,
      retransmitindo-o de volta pela UART para realimentação do usuário.
3. RF3: O sistema deverá receber o segundo operando pela UART numa sequência de até
   três caracteres ASCII correspondentes aos dígitos decimais de 0 a 9.
   1. RF3.1: O sistema deverá ecoar os caracteres recebidos, desde que sejam válidos,
      retransmitindo-os de volta pela UART para realimentação do usuário.
   2. RF3.2: O sistema deverá converter os caracteres ASCII recebidos em um valor numérico
      equivalente para a realização da operação aritmética selecionada.
4. RF4: O sistema deverá receber o caractere ‘=’ para efetuar a operação aritmética.
   1. RF4.1: A recepção do caractere ‘=’ deve encerrar a recepção do segundo operando caso
      o limite máximo de 3 dígitos não tenha sido atingido.
   2. RF4.2: O sistema deverá ecoar o caractere recebido, desde que seja válido,
      retransmitindo-o de volta pela UART para realimentação do usuário.
5. RF5: O sistema deverá efetuar a operação aritmética selecionada e transmitir o resultado
   pela UART numa sequência de caracteres ASCII.
   1. RF5.1: O sistema deverá converter o valor numérico do resultado da operação aritmética
      em uma sequência de caracteres que o represente.
   2. RF5.2: Caso o resultado da operação seja negativo, o sistema deve transmitir o
      caractere ‘-’ antes da sequência de caracteres que o representa.
   3. RF5.3: A sequência de caracteres transmitidos deverá ser encerrada com os caracteres
      ‘\r’ e ‘\n’ (CR+LF).

### Requisitos não-funcionais do sistema:

1. RNF1: A comunicação serial entre o kit de desenvolvimento e o PC hospedeiro deverá
   ocorrer em 9600 bps, com 8 bits de dados, paridade ímpar e 1 bit de parada.
2. RNF2: A estratégia de sincronização CPU-UART deverá ser por sondagem (polling)
   Funcionalidades extras que podem ser implementadas:
   - Indicar erro quando ocorrer uma divisão por zero.
   - Permitir operações com números inteiros sinalizados.
   - Utilizar os LEDs do kit para indicar estados da calculadora



# Apresentação do Projeto:

Este projeto implementa uma calculadora de 4 operações com números inteiros, conforme os requisitos solicitados. Embora o projeto original solicitasse uma implementação apenas em Assembler, este foi implementado em C, conforme acordado com os professores.

A programação foi orientada a ocupar o menor tempo possível de processamento, evitando entradas desnecessárias em rotinas de tratamento de caracteres ou Leds (I/O) quando nada houvesse sido modificado. Também se explorou a criação de rotinas que fossem facilmente expansíveis para maior capacidade. 



##### Requisitos RF1, RF1.1 e RF1.2

O tamanho de apenas 3 caracteres aceitos na entrada é determinado no preâmbulo do código através da diretiva:

```c
#define DIGITOSENTRADA ( 3 -  1)
```

Esta limitação não se aplica ao caractere de sinal (que qualifica o número como negativo). O valor máximo deve ser de 9 caracteres, sem estourar a capacidade dos registradores.

Os caracteres são verificados assim que recebidos e ecoados e armazenados em strings caso sejam válidos ou em sequência válida. Os argumentos numéricos são armazenados nas strings globais definidas por

```c
uint8_t argAstring[BUFFERSIZE] = {0}; // primeiro argumento
uint8_t argBstring[BUFFERSIZE] = {0};  // segundo argumento
```



```c
#define BUFFERSIZE 20
```

O BUFFERSIZE foi predefinido para suportar 19 caracteres de saída de um valor de 64 bits, valores máximos absolutos para este projeto.

O valor numérico correspondente à representação em caracteres é obtida na função 

```c
int32_t ParseArgString(uint8_t *argstr)
```

##### Requisitos RF2, RF2.1 a RF2.4

   Uma vez que o caractere é recebido, a estrutura

```c
         switch(caractere)
```

avalia se é um caractere legal ou não, conta caracteres numéricos e operadores para determinar a continuidade da execução.

##### Requisitos RF3

A contagem de caracteres é feita no trecho

```c

      switch(caractere)
      . . .      
           case '0' ... '9':
           if (n > DIGITOSENTRADA + s) { // o + s corrige entrada de sinal
              flow_flag = ArgMax;
           } else {
              flow_flag = NovoAlgarismo;
           }
           break;

```

A entrada dos argumentos é realizada por 

```c
      switch(flow_flag)
      {
      case NovoAlgarismo:
        Uart0_Send(caractere);
        switch(argumento)
          {
          case ArgA:
            argAstring[n] = caractere;
            argAstring[n+1] = 0;
            LedState = Argumento1;
            break;
          case ArgB:
            argBstring[n] = caractere;
            argBstring[n+1] = 0;
            LedState = Argumento2;
            break;
          default:
            break;
          }
        n++;
        flow_flag = OK;
        break;

```

A alteração de um argumento para o próximo é feito em 

```c
        case NovoArgumento:
          if (n==0) {
            flow_flag = Reinicia;  
          }else {
            Uart0_Send(caractere);
            flow_flag = OK;
          }
          n=0; // reseta contador para proximo argumento
          s=0; // reseta contador de sinal
          argumento++;
          break;
       

```

A conversão do string em valor numérico equivalente é realizada em 

```c
// transforma a string em número correspondente
int32_t ParseArgString(uint8_t *argstr)
{
  int32_t result = 0;
  int16_t sinal = 1;
  
  // trata o caso negativo
  if ( (*argstr == '-') ) // || (*argstr == NEG)) 
  {
    sinal = -1;
    argstr++; // avança um caractere para não entrar na conta abaixo
  }
  // trata o caso genérico 
  while (*argstr != 0){
    result = result*10;
    result += *argstr++ - '0';
  }
  // retorna corrigindo o sinal
  return result * sinal;
}

```



##### Requisito RF4

O recebimento de um caractere '=' causa a execução da operação. Inicialmente é reconhecido na estrutura abaixo

```
         switch(caractere)....


		case '=':
           flow_flag = Executa;
           if (n==0)  flow_flag = Reinicia; // pega o caso sem segundo argumento
           break;

```

e caso não haja nenhum valor numérico antes dele, reinicia a aquisição de caracteres. Caso tenha avido a recepção de valores numéricos, então executa a operação em 

```c
      case Executa:
          Uart0_Send(' ');
          operandoA = ParseArgString(argAstring);
          Uart0_Send('=');
          operandoB = ParseArgString(argBstring);
          Uart0_Send(' ');

          res = ExecucaoOperacao(operacao);
          OutputBuffRes64bitsHL(res);
          if(flow_flag != DivPZero){
            TransmiteString(outputbuffer);
             LedState = ResultadoValido; 
          } else {
            TransmiteConstString(errostr);
            LedState = Erro;            
          }
          flow_flag = Reinicia; 
          break;


```

Onde ecoa o caractere '=' entre ' ', para facilitar a leitura 



##### RF5

O programa executa a operação em `res = ExecucaoOperacao(operacao);`

```c
//nulo, soma, subtracao, multiplicacao, divisao
int64_t ExecucaoOperacao (operacao_enum op)
{
  int64_t resultado=0;
  switch(op)
  {
  case soma:
    resultado = operandoA + operandoB;
    break;
  case subtracao:
    resultado = operandoA - operandoB;
    break;
  case multiplicacao:
    resultado = operandoA * operandoB;
    break;
  case divisao:
    if(operandoB == 0) {
      flow_flag = DivPZero;   // sinaliza erro
    } else {
      flow_flag = OK;
      resultado = operandoA / operandoB;
    }
    break;
  default:
    resultado = 0;
    break;
  }
  return resultado;
}

```

 onde verifica a possível divisão por zero, capturando este caso e identificando o erro.

A conversão do resultado em uma string de caracteres é realizada na rotina:

```c
// converte o resultado res na string outputbuffer, 
// toma o resto da divisão por dez, converte em ascii e armazena em string local
// divide por dez e reinicia o processo até que não haja mais argumento.
// isto cria a sequência de algarismos invertido na string. Esta então é
// reordenada na string de saída.
void OutputBuffRes64bitsHL(int64_t res)
{       
  uint8_t cont = 0, n=0, m=0;
  uint8_t invstring[20]={0};

  if (res <0) // trata caso de valor negativo
  {
    outputbuffer[m++] = '-';
    res = -res;
  }
  if (res == 0)  // trata caso trivial de valor zero
  {
     outputbuffer[m++] = '0';
     outputbuffer[m++] = 0;
     return ;
  }
  while (res > 0) // trata o caso geral
  {
      cont = res % 10;
      invstring[n++] = '0'+ cont;
      res = res / 10;
  }
  while (n > 0) //reordena na saída
    outputbuffer[m++] = invstring[--n];
  
  outputbuffer[m++] = 0; // marca final da string
  
  return;
}

```

Esta rotina apenas cria a string contendo a sequência de caracteres (incluindo o eventual '-'), terminada por 0 . A inclusão dos caracteres '\r' e '\n' é feita no trecho:

```c
      case Reinicia:
          n=0; // endereço inicio do buffer
          operacao = nulo; // nenhuma operacao definida - aquisicao do primeiro operando
          Uart0_Send('\n');
          Uart0_Send('\r');
          flow_flag = OK;
          argumento = ArgA;
          s=0;
          op=0 ; 
//           LedState =P0d0; // sinaliza reinicio - algo errado
          break;

```

 que é chamado após a execução da operação. Serve para incluir caracteres tanto após transmissão da string da resposta como da string de erro.



##### RNF1

O programa define a comunicação por UART a um baud rate de 9600 bps, com 8 bits de dados, 1 bit de parada e paridade ímpar. Ajustado no trecho

```c
    // 3. Baud-rate nos registradores UARTIBRD e UARTFBRD
    // Para um clock de 16 MHz e um baud de 9600 bps (clockdiv=16)
    UART0_IBRD_R = 104;  //Parte inteira
    UART0_FBRD_R = 11;  //Parte fracionária    
    // 4. Configurar 8 bits, sem FIFO,  paridade impar e 1 stop-bit
    UART0_LCRH_R = 0x0062;

```



A estratégia de polling da porta serial é utilizada, pela inclusão da checagem do estado da porta no loop principal do programa

```c
    while (1)
    {
       // verifica presença de algo para ler na UART
      if ((UART0_FR_R&0x0010) ==0)
      {
         caractere = ((uint8_t)(UART0_DR_R & 0xFF));
//         Avalia caractere - define ação a ser tomada em estrutura switch seguinte
         switch(caractere)
....
```



##### Funcionalidades extras

###### Indicação de erro na divisão por zero

A indicação de erro quando ocorre divisão por zero é feita antes da divisão por zero, identificando esta situação.

```c
  case divisao:
    if(operandoB == 0) {
      flow_flag = DivPZero;   // sinaliza erro
    } else {
      flow_flag = OK;
      resultado = operandoA / operandoB;
    }
    break;

```

A indicação que o erro ocorreu ocorre pela transmissão da mensagem armazenada na string de erro

```
const uint8_t errostr[] = "Erro " ;  // número par de caracteres, incluindo o finalizador
```

através da função

```c
// transmite a string através da porta UART0
// versão ROM para strings constantes, como mensagens de erro.
void TransmiteConstString(const uint8_t *string )
{
  while (*string != 0 )
    Uart0_Send( *string++);
  
}
```



###### Permitir operações com números sinalizados.

O tratamento do caractere '-' é feito de forma diferente dos demais caracteres para permitir que o primeiro argumento seja iniciado por '-', sem que isto acuse um erro como a operação sem um primeiro argumento numérico. Para isso, conta-se o número de operadores e avalia-se o números de dígitos recebidos. 

```c
         case '-':
           op++;
           if ((n==0) && (s==0))  {
             flow_flag = NovoAlgarismo;
             op--;
             s++;
           } else {
             operacao = subtracao;
             flow_flag = NovoArgumento;
             LedState = Operacao;
           }
           if (n == 1) op++; // pega - consecutivos
         
           break;

```

As rotinas de parsing da string em valor numérico levam em conta a presença do sinal negativo.

```c
// transforma a string em número correspondente
int32_t ParseArgString(uint8_t *argstr)
{
  int32_t result = 0;
  int16_t sinal = 1;
  
  // trata o caso negativo
  if ( (*argstr == '-') ) // || (*argstr == NEG)) 
  {
    sinal = -1;
    argstr++; // avança um caractere para não entrar na conta abaixo
  }
  // trata o caso genérico 
  while (*argstr != 0){
    result = result*10;
    result += *argstr++ - '0';
  }
  // retorna corrigindo o sinal
  return result * sinal;
}

```

###### Utiliza os Leds do kit para indicar os estados da calculadora

Para utilizar todos os Leds do kit, foi modificado a inicialização do gpio.c para incluir

```c
/ acrescenta porta F
#define GPIO_PORTF (0x0020) // bit 5

// mascara bits para acesso apenas aos bits 4 e 0
#define GPIO_PORTF_LEDS  (*((volatile uint32_t *)0x4005D044)) 

void GPIO_Init(void)
{
	//1a. Ativar o clock para a porta setando o bit correspondente no registrador RCGCGPIO
	SYSCTL_RCGCGPIO_R |= GPIO_PORTJ | GPIO_PORTN | GPIO_PORTF;
	//1b.   após isso verificar no PRGPIO se a porta está pronta para uso.
        while((SYSCTL_PRGPIO_R & (GPIO_PORTJ | GPIO_PORTN | GPIO_PORTF) ) != (GPIO_PORTJ | GPIO_PORTN | GPIO_PORTF) ){};
	
	// 2. Destravar a porta somente se for o pino PD7 e PE7
		
	// 3. Limpar o AMSEL para desabilitar a analógica
	GPIO_PORTJ_AHB_AMSEL_R = 0x00;
	GPIO_PORTN_AMSEL_R = 0x00;
     	GPIO_PORTF_AHB_AMSEL_R = 0x00;  // acrescenta PortF

		
	// 4. Limpar PCTL para selecionar o GPIO
	GPIO_PORTJ_AHB_PCTL_R = 0x00;
	GPIO_PORTN_PCTL_R = 0x00;
	GPIO_PORTF_AHB_PCTL_R = 0x00;

	// 5. DIR para 0 se for entrada, 1 se for saída
	GPIO_PORTJ_AHB_DIR_R = 0x00;
	GPIO_PORTN_DIR_R = 0x03; //BIT0 | BIT1
	GPIO_PORTF_AHB_DIR_R = 0x11; // bit 0 e bit 4
		
	// 6. Limpar os bits AFSEL para 0 para selecionar GPIO sem função alternativa	
	GPIO_PORTJ_AHB_AFSEL_R = 0x00;
	GPIO_PORTN_AFSEL_R = 0x00; 
	GPIO_PORTF_AHB_AFSEL_R = 0x00;
        
		
	// 7. Setar os bits de DEN para habilitar I/O digital	
	GPIO_PORTJ_AHB_DEN_R = 0x03;   //Bit0 e bit1
	GPIO_PORTN_DEN_R = 0x03;       //Bit0 e bit1
        GPIO_PORTF_AHB_DEN_R = 0x11;   //Bit0 e bit4
	
	// 8. Habilitar resistor de pull-up interno, setar PUR para 1
	GPIO_PORTJ_AHB_PUR_R = 0x03;   //Bit0 e bit1	
        
}
```

E acrescentou-se a função

```c
// -------------------------------------------------------------------------------
// Função PortF_Output
// Escreve os valores no port F
// Parâmetro de entrada: Valor a ser escrito
// Parâmetro de saída: não tem
void PortF_Output(uint32_t valor)
{
	//Ponteiro para o valor dos bits com leitura amigável
	GPIO_PORTF_LEDS = valor;
}
```

No programa principal foi incluida a estrutura

```c
     
      if (LedState != LedStateAnt){
        LedStateAnt = LedState; // atualiza estado dos leds 
                                //- evita reentada do código se nada a fazer
        switch(LedState)
        {
           case P0d0: /*0000*/
               PortN_Output(0x0);
               PortF_Output(0x00);
               break;
           case Argumento1: /*1000*/
               PortN_Output(0x2);
              PortF_Output(0x00);
                break;
           case Operacao: /*0100*/
               PortN_Output(0x1);
               PortF_Output(0x00);
                break;
           case Argumento2: /*0010*/
               PortN_Output(0x0);
              PortF_Output(0x10);
                break;            
           case ResultadoValido: /*xxx1*/
             switch(operacao)
             {
             case soma: /*0011*/
               PortN_Output(0x0);
               PortF_Output(0x11);
               break;
             case subtracao: /*0101*/
               PortN_Output(0x1);
               PortF_Output(0x01);
               break;
             case multiplicacao: /*0111*/
               PortN_Output(0x1);
               PortF_Output(0x11);
               break;
             case divisao: /*1001*/
               PortN_Output(0x2);
               PortF_Output(0x01);
               break;
             }
             break;
           case Erro: /*1110*/
              PortN_Output(0x3);
              PortF_Output(0x10);
                break;            

        }

```

Esta estrutura faz com que o recebimento do primeiro argumento ilumine apenas o led D1,

o recebimento do operador ilumine apenas o Led D2 e o recebimento do segundo argumento ilumine apenas o led D3. 

A operação acende o led D4 apenas se for válida, e os leds D1 a D3 de acordo com o valor binário da operação ('+'  $\to$ 001, '-'  $\to$ 010, '*' $\to$​ 011, '/'$\to$ 100).

O caso de erro é apontado como 1110.

