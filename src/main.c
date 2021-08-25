// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// Algumas rotinas foram descaradamente copiadas do projeto
// uart_c
// Prof. Guilherme Peron / Prof. Rafael de Góes
// 
//  
//  Calculadora Básica
// Os valores são inseridos via porta serial com baud de 9600, 8bits,
// sem paridade e 1 stop bit
//
//
//

#include <stdint.h>
#include "tm4c1294ncpdt.h"
// para suportar valores de 64 bits, com finalizador de string arrays de caracteres possuem 20 posições
#define BUFFERSIZE 20
// Seleciona valores de entrada de 4 algarismos
#define DIGITOSENTRADA ( 4 -  1)


void GPIO_Init(void);
uint32_t PortJ_Input(void);
void PortN_Output(uint32_t leds);
void Uart0_Init(void);
uint8_t Uart0_Rcv(void);
void Uart0_Send(uint8_t dado);

const uint8_t errostr[] = "Erro " ;  

uint8_t argAstring[BUFFERSIZE] = {0}; // primeiro argumento
uint8_t argBstring[BUFFERSIZE] = {0};  // segundo argumento
uint8_t outputbuffer[BUFFERSIZE] = {0}; // resposta/mensagem de erro
uint16_t caractere; 
uint16_t n; // posição do ultimo caractere lido
uint16_t n_op;
uint16_t n_fim; // posicao do final da string
int32_t operandoA, operandoB;
int64_t res;
int32_t arg = 0; // debug

typedef enum alertas_tipos {OK, NovoAlgarismo, DivPZero, ArgMax, NovoArgumento
            ,Executa, Reinicia} alertas;
typedef enum operacao_tipo { nulo, soma, subtracao, multiplicacao, divisao}
              operacao_enum;
//typedef enum acao_tipo { acumuladado, executa, reinicia } acao_enum;
typedef enum argumento_numero {ArgA, ArgB, ArgC} argumento_num;


operacao_enum operacao;
//acao_enum acao;
alertas flow_flag;
argumento_num argumento;

int64_t ExecucaoOperacao (operacao_enum op);
void OutputBuffRes64bitsHL(int64_t res);
void TransmiteOutputBuff();
void TransmiteString(uint8_t *string );
int32_t ParseArgString(uint8_t *argstr);
void TransmiteConstString(const uint8_t *string );



int main(void)
{
  uint8_t chaves;
  GPIO_Init();
  Uart0_Init();
  n=0; // endereço inicio do buffer
  operacao = nulo; // nenhuma operacao definida - aquisicao do primeiro operando
  argumento = ArgA;  
  flow_flag = OK; // nenhum alerta
    while (1)
    {
       // verifica presença de algo para ler na UART
      if ((UART0_FR_R&0x0010) ==0)
      {
         caractere = ((uint8_t)(UART0_DR_R & 0xFF));
//         flow_flag = NovoAlgarismo;
         switch(caractere)
         {
         case '+':
           operacao = soma;
           flow_flag = NovoArgumento;
           break;
         case '-':
           operacao = subtracao;
           flow_flag = NovoArgumento;
           break;
         case '*':
           operacao = multiplicacao;
           flow_flag = NovoArgumento;
           break;
         case '/':
           operacao = divisao;
           flow_flag = NovoArgumento;
           break;
         case '=':
           flow_flag = Executa;
           break;
         case '0' ... '9':
           if (n > DIGITOSENTRADA) {
              flow_flag = ArgMax;
           } else {
              flow_flag = NovoAlgarismo;
           }
           break;
         default :
           flow_flag=Reinicia;
           break;
         }
      } // if uart
         
      switch(flow_flag)
      {
      case NovoAlgarismo:
        Uart0_Send(caractere);
        switch(argumento)
          {
          case ArgA:
            argAstring[n] = caractere;
            argAstring[n+1] = 0;
            break;
          case ArgB:
            argBstring[n] = caractere;
            argBstring[n+1] = 0;
            break;
          default:
            break;
          }
        n++;
        flow_flag = OK;
        break;
        
        case NovoArgumento:
          n=0; // reseta contador para proximo argumento
          Uart0_Send(caractere);
          argumento++;
          flow_flag = OK;
          break;
       
      case ArgMax:
        break;
        
        
      case Reinicia:
          n=0; // endereço inicio do buffer
          operacao = nulo; // nenhuma operacao definida - aquisicao do primeiro operando
          Uart0_Send('\n');
          Uart0_Send('\r');
          flow_flag = OK;
          argumento = ArgA;
          break;
          
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
          } else {
            TransmiteConstString(errostr);
            
          }
          flow_flag = Reinicia; 
          break;

      }
        
      
       chaves = PortJ_Input();
       switch (chaves)
       {
           case 0x3: /*11*/
               PortN_Output(0x0);
               break;
           case 0x2: /*10*/
               PortN_Output(0x1);
               Uart0_Send('1');
               break;
           case 0x1: /*01*/
               PortN_Output(0x2);
               Uart0_Send('2');
               break;
           case 0x0: /*00*/
               PortN_Output(0x3);
               Uart0_Send('3');
               break;            
       }
    }
}




int32_t ParseArgString(uint8_t *argstr)
{
  uint32_t result = 0;
  uint16_t sinal = 1;
  
  if (*argstr == '-') sinal = -1;
  while (*argstr != 0){
    result = result*10;
    result += *argstr++ - '0';
  }
  
  return result * sinal;
}


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
      flow_flag = DivPZero;
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





void OutputBuffRes64bitsHL(int64_t res)
{       
  uint8_t cont = 0, n=0, m=0;
  uint8_t invstring[20]={0};

  if (res <0) 
  {
    outputbuffer[m++] = '-';
    res = -res;
  }
  if (res == 0)  
  {
     outputbuffer[m++] = '0';
     outputbuffer[m++] = 0;
     return ;
  }
  
  while (res > 0)
  {
      cont = res % 10;
      invstring[n++] = '0'+ cont;
      res = res / 10;
  }
  
 
  while (n > 0)
    outputbuffer[m++] = invstring[--n];
  
  outputbuffer[m++] = 0;
  
  return;
}


void TransmiteString(uint8_t *string )
{
//  uint8_t m = 0;
  while (*string != 0 )
    Uart0_Send( *string++);
  
}

void TransmiteConstString(const uint8_t *string )
{
//  uint8_t m = 0;
  while (*string != 0 )
    Uart0_Send( *string++);
  
}

void TransmiteOutputBuff()
{
  uint8_t m = 0;
  while (outputbuffer[m] != 0 )
    Uart0_Send(outputbuffer[m++]);
  
}


int32_t div10(int32_t dividend)
{
    int64_t invDivisor = 0x1999999A;
    return (int32_t) ((invDivisor * dividend) >> 32);
}

int32_t mult10(int32_t multip )
{
    return (int32_t) ((multip << 3 + multip << 1));
}