/*
  Essa biblioteca implementa funcoes de manipulacao pacotes e envio/recebimento em rede 
  Desenvolvida para uso no Trabalho 1 de Redes de BCC - UFPR

  Criado por Davi Garcia Lazzarin e Mardoqueu Freire Nunes
  Data: 18/11/2024
*/

#ifndef _LIBREDE_
#define _LIBREDE_

#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define MARKER 126        // Valor esperado para o marcador de início
#define DATA_SIZE 63      // Tamanho do campo Dados
#define VALOR_CRC 0x07    // CRC inicial

#define TP_ACK 0          //00000 ACK
#define TP_NACK 1         //00001 NACK
#define TP_OK 2           //00010 OK

#define TP_BACKUP_INI 4   //00100 backup – inidica realização de backup
#define TP_RESTAURA_INI 5 //00101 restaura – indica realização de restaura
#define TP_VERIFICA_INI 6 //00110 verefica – indica realização de verificação

#define TP_ENVIA_NOME 7   //00111 Envia o nome do arquivo
#define TP_ENVIA_ARQ 8    //01000 Marcador de inicio do envio do arquivo

#define TP_ENVIO 16       //10000 enviando dados
#define TP_FIM_ENVIO 17   //10001 Fim da transmissão

#define TP_ERRO 31        //11111 ERRO

/*
Tabela de Tipos
TIPOS DEFINIÇÂO
00000 ACK – tudo ok, manda o próximo pedaço/arquivo
00001 NACK – problema de crc, não entendi e mando dnv, se ocorrer manda dnv
00010 OK – Tudo certo.
00100 backup – inidica realização de backup
00101 restaura – indica realização de restaura
00110 verefica – indica realização de vereficação

11111 Erro – um código de erro
10000 Dados – enviando dados
01111 Tamanho
10001 Fim da transmissão
01110 ok + tamanho
01101 ok+checksum – cksum() função de verificação de crc
*/

//cria o sockets
int cria_raw_socket(char* nome_interface_rede);

//verifica o marcador de inicio da mensagem - se ele é valido ou nao
int verifica_marcador(unsigned char* buffer);

//essa funcção apenas recebe o pacote atraves do soquete
//guarda os dados em buffer para ser desmontado
//retorna 1 se o pacote eh valido e 0 cc
int recebe_pacote(int soquete, unsigned char* buffer);

//Essa função desmonta a mensagem separando cada seção e guardando nas variaveis do parametro
//dados esse recebeos os dados enviados!
void desmontar_pacote(unsigned char* buffer, unsigned char* dados, unsigned char* tamanho, unsigned char* sequencia, unsigned char* tipo);

//Envia o pacote atraves do soquete
int envia_pacote(void* pacote, char* interface, int soquete);

//prepara o pacote para envia-lo
//separa a dados em seus respectivos indices detrno do pacote e de toda a mensagem; 
void* monta_pacote(int tam, unsigned char sequencia, unsigned  char tipo, void* dados);

#endif