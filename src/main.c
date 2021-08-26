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
// Operações soma, subtração, multiplicação e divisão inteira.
// forma geral:
//      argumento1 operador argumento2 = resultado 
// Argumentos numéricos devem ser inteiros e podem ser sinalizados
// Os argumentos devem ser inteiros.
// A entrada de um caractere não numérico (excetuando o operador) reinicia a entrada
// A entrada de um operador sem que haja um argumento numérico precedente reinicia a entrada
// A entrada de dois operadores reinicia a entrada
// Divisão por zero causa a mensagem Erro na saída.

#include <stdint.h>
#include "tm4c1294ncpdt.h"

// para suportar valores de 64 bits, com finalizador de string arrays de caracteres possuem 20 posições
#define BUFFERSIZE 20

// Seleciona valores de entrada de 4 algarismos - valor máximo é 9 para acomodar valor de 32 bits
#define DIGITOSENTRADA ( 4 -  1)

// caractere que marca string de valor negativo 
// #define NEG 'n'
// resolvi a parada com o caractere '-', não há mais a necessidade desta malandragem

void GPIO_Init(void);
uint32_t PortJ_Input(void);
void PortN_Output(uint32_t leds);
void Uart0_Init(void);
uint8_t Uart0_Rcv(void);
void Uart0_Send(uint8_t dado);


// variáveis globais
const uint8_t errostr[] = "Erro " ;  
uint8_t argAstring[BUFFERSIZE] = {0}; // primeiro argumento
uint8_t argBstring[BUFFERSIZE] = {0};  // segundo argumento
uint8_t outputbuffer[BUFFERSIZE] = {0}; // resposta/mensagem de erro
uint16_t caractere = 0 ; 
uint16_t n = 0 ; // posição do ultimo caractere lido
uint16_t s = 0 ; // contador para entradas de sinal negativo (Só pode haver um!)
            // corrige algarismos máximos
uint16_t op= 0 ; // conta numero de operadores... (Só Pode Haver UM!...
                // exceto quando este um é o '-', aí depende de onde está)
int32_t operandoA = 0, operandoB = 0 ;
int64_t res = 0 ;
int32_t arg = 0; // debug

typedef enum alertas_tipos {OK, NovoAlgarismo, DivPZero, ArgMax, NovoArgumento
            ,Executa, Reinicia, Negativo} alertas;
typedef enum operacao_tipo { nulo, soma, subtracao, multiplicacao, divisao}
              operacao_enum;
typedef enum argumento_numero {ArgA, ArgB, ArgC} argumento_num;


operacao_enum operacao;
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
  s=0; // nenhum caracrtere de sinal
  op=0 ; 
    while (1)
    {
       // verifica presença de algo para ler na UART
      if ((UART0_FR_R&0x0010) ==0)
      {
         caractere = ((uint8_t)(UART0_DR_R & 0xFF));
//         Avalia caractere - define ação a ser tomada em estrutura switch seguinte
         switch(caractere)
         {
         case '+':
           operacao = soma;
           flow_flag = NovoArgumento;
           op++ ;
           break;
         case '-':
           op++;
           if ((n==0) && (s==0))  {
             flow_flag = NovoAlgarismo;
             op--;
             s++;
           } else {
             operacao = subtracao;
             flow_flag = NovoArgumento;
           }
           if (n == 1) op++; // pega - consecutivos
         
           break;
         case '*':
           operacao = multiplicacao;
           flow_flag = NovoArgumento;
           op++ ;
           break;
         case '/':
           operacao = divisao;
           flow_flag = NovoArgumento;
           op++ ;
           break;
         case '=':
           flow_flag = Executa;
           if (n==0)  flow_flag = Reinicia; // pega o caso sem segundo argumento
           break;
         case '0' ... '9':
           if (n > DIGITOSENTRADA + s) { // o + s corrige entrada de sinal
              flow_flag = ArgMax;
           } else {
              flow_flag = NovoAlgarismo;
           }
           break;
//         case NEG:
//           flow_flag = NovoAlgarismo;
//           s++; // acusa a presença de um sinal
//           if ((s>1) || (n>0)) 
//             flow_flag = Reinicia; // aborta se sinal no meio do numero ou se
//                                   // mais de um sinal
//           break;

         default :
           flow_flag = Reinicia;
           break;
         }
         
      if ( (op > 1) || (s>1) ) flow_flag = Reinicia; // aborta se segundo operador

      } // if uart
         
      
      // Toma a ação definida em função do caractere recebido
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
       
      case ArgMax:
        break;
        
        
      case Reinicia:
          n=0; // endereço inicio do buffer
          operacao = nulo; // nenhuma operacao definida - aquisicao do primeiro operando
          Uart0_Send('\n');
          Uart0_Send('\r');
          flow_flag = OK;
          argumento = ArgA;
          s=0;
          op=0 ; 
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


// transmite a string através da porta UART0
// versão RAM
void TransmiteString(uint8_t *string )
{
  while (*string != 0 )
    Uart0_Send( *string++);
  
}

// transmite a string através da porta UART0
// versão ROM para strings constantes, como mensagens de erro.
void TransmiteConstString(const uint8_t *string )
{
  while (*string != 0 )
    Uart0_Send( *string++);
  
}



//// subrotinas não utilizadas na versão final
// primeira versão transmite apenas a tring outputbuffer
// abandonada em função da versão mais genérica.
void TransmiteOutputBuff()
{
  uint8_t m = 0;
  while (outputbuffer[m] != 0 )
    Uart0_Send(outputbuffer[m++]);
  
}


// estas funções não se justificam para este processador...
// são adequadas para processadores sem multiplicador em hardware.
int32_t div10(int32_t dividend)
{
    int64_t invDivisor = 0x1999999A;
    return (int32_t) ((invDivisor * dividend) >> 32);
}

int32_t mult10(int32_t multip )
{
    return (int32_t) ((multip << 3 + multip << 1));
}