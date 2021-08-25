// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// Verifica se recebeu alguma coisa pela serial e acende um dos leds de acordo com o caractere recebido
// Verifica o estado da chave USR_SW2, e, caso seja pressionada envia o caracter A pela serial
// Prof. Guilherme Peron / Prof. Rafael de Góes

#include <stdint.h>
#include "tm4c1294ncpdt.h"
#define BUFFERSIZE 32
#define DIGITOSENTRADA 8


void GPIO_Init(void);
uint32_t PortJ_Input(void);
void PortN_Output(uint32_t leds);
void Uart0_Init(void);
uint8_t Uart0_Rcv(void);
void Uart0_Send(uint8_t dado);




uint8_t errostring[] = "Erro" ; 
uint8_t buffer[BUFFERSIZE] = {0};
uint8_t outputbuffer[BUFFERSIZE] = {0};
uint8_t n; // posição do ultimo caractere lido
uint8_t n_op;
uint8_t n_fim; // posicao do final da string
int32_t operandoA, operandoB;
int64_t res;
int32_t arg = 0; // debug

typedef enum alertas_tipos {OK, DivPZero, Arg1Max, Arg2Max} alertas;
typedef enum operacao_tipo 
            { nulo, soma, subtracao, multiplicacao, divisao}
              operacao_enum;
typedef enum acao_tipo { acumuladado, executa, reinicia } acao_enum;

operacao_enum operacao;
acao_enum acao;
alertas flow_flags;

void Parsestring(uint8_t n_op, uint8_t n_fim);
int64_t ExecucaoOperacao (operacao_enum op);
void OutputBuffRes64bitsHL(int64_t res);
void TransmiteOutpurBuff();
void TransmiteString(uint8_t *string );



int main(void)
{
  uint8_t chaves;
  GPIO_Init();
  Uart0_Init();
  n=0; // endereço inicio do buffer
  operacao = nulo; // nenhuma operacao definida - aquisicao do primeiro operando
  acao = acumuladado; // acumula algarismos e operadores
  
    while (1)
    {
       // verifica presença de algo para ler na UART
      if ((UART0_FR_R&0x0010) ==0)
      {
         buffer[n] = ((uint8_t)(UART0_DR_R & 0xFF));
         Uart0_Send(buffer[n]);
         switch(buffer[n])
         {
         case '+':
           operacao = soma;
           n_op = n;
           break;
         case '-':
           operacao = subtracao;
           n_op = n;
           break;
         case '*':
           operacao = multiplicacao;
            n_op = n;
           break;
         case '/':
           operacao = divisao;
           n_op = n;
           break;
         case '=':
           acao = executa;
           n_fim = n;
           break;
         case '0' ... '9':
           break;
         default :
           acao=reinicia;
           break;
         }
         n++;
      } // if uart
      // 
      
      switch(acao)
      {
      case reinicia:
          n=0; // endereço inicio do buffer
          operacao = nulo; // nenhuma operacao definida - aquisicao do primeiro operando
          acao = acumuladado; // acumula algarismos e operadores
          Uart0_Send('\n');
          Uart0_Send('\r');
          flow_flags = OK;
          break;
          
      case executa:
          Parsestring(n_op, n_fim);
          res = ExecucaoOperacao(operacao);
          OutputBuffRes64bitsHL(res);
          if(flow_flags == DivPZero){
            TransmiteOutpurBuff();
          } else {
            TransmiteString(errostring);
          }
          acao = reinicia; 
          break;
          
      default :
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


// executa o parsing da string
void Parsestring(uint8_t n_op, uint8_t n_fim)
{
  uint8_t cont = 0;
  //int32_t arg = 0;
  
  arg =0; //debug
  while (cont < n_op)
  {
      arg = arg * 10; // multiplica por 10
      arg = arg + (buffer[cont] - '0');
      cont++;
  }
  operandoA = arg;
  cont++; // pula o caractere do operador
  arg = 0;
  while (cont < n_fim)
  {
      arg = arg*10; // multiplica por 10
      arg = arg + (buffer[cont] - '0');
      cont++;
  }
  operandoB = arg;
  
  return;
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
      erroflag = 1;
    } else {
      erroflag = 0;
      resultado = operandoA / operandoB;
    }
    break;
  default:
    resultado = 0;
    break;
  }
  return resultado;
}



void OutputBuffRes64bits(int64_t res)
{       
  uint8_t cont = 0;
  uint64_t sub = 1000000000000000000;

  
  if(res <0) 
  {
    outputbuffer[n++] = '-';
    res = -res;
  }
  if (res == 0)  
  {
     outputbuffer[n++] = '0';
     outputbuffer[n++] = '\n';
     outputbuffer[n++] = '\r';
     return ;
  }


  while(res != 0){
      
    cont = 0;
    while ( res - sub > 0)
    {
      res -= sub;
      cont++;
    }
    
    outputbuffer[n++] = '0'+ cont;
    sub = sub / 10;
  }
  
  return;
}



void OutputBuffRes64bitsHL(int64_t res)
{       
  uint8_t cont = 0, n=0, m=0;
  uint8_t invstring[20]={0};

  // outputbuffer[m++] = '='; //já tem um igual ecoado...

  if (res <0) 
  {
    outputbuffer[m++] = '-';
    res = -res;
  }
  if (res == 0)  
  {
     outputbuffer[m++] = '0';
//     outputbuffer[m++] = '\n'; // no reinicia
//     outputbuffer[m++] = '\r';
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
    
//   outputbuffer[m++] = '\n';// no reinicia
//   outputbuffer[m++] = '\r';
   outputbuffer[m++] = 0;
  
  return;
}


void TransmiteString(uint8_t *string )
{
//  uint8_t m = 0;
  while (*string != 0 )
    Uart0_Send( *string++);
  
}

void TransmiteOutpurBuff()
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