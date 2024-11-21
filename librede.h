/*
  Essa biblioteca implementa funcoes de manipulacao de arquivos e diretorios 
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

#define MARKER 126         // Valor esperado para o marcador de início
#define DATA_SIZE 63       // Tamanho do campo Dados
#define CRC_SEED 0xFF      // CRC inicial


//cria o sockets
int cria_raw_socket(char* nome_interface_rede);

//verifica o marcador de inicio da mensagem - se ele é valido ou nao
short int verifica_marcador(unsigned char* buffer);

//essa funcção apenas recebe o pacote atraves do soquete
//guarda os dados em buffer para ser desmontado
void recebe_pacote(int soquete, unsigned char* buffer);

//Essa função desmonta a mensagem separando cada seção e guardando nas variaveis do parametro
//dados esse recebeos os dados enviados!
void desmontar_pacote(unsigned char* buffer, unsigned char* dados, unsigned char* tamanho, unsigned char* sequencia, unsigned char* tipo);


//Envia o pacote atraves do soquete
int envia_pacote(void* pacote, char* interface, int soquete);

// prepara o pacote para envia-lo
//separa a dados em seus respectivos indices detrno do pacote e de toda a mensagem; 
void* monta_pacote(int tam, unsigned char sequencia, unsigned  char tipo, void* dados);

#endif