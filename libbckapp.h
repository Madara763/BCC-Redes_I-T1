/*
  Essa biblioteca implementa funcoes de manipulacao de arquivos e diretorios 
  Desenvolvida para uso no Trabalho 1 de Redes de BCC - UFPR

  Criado por Davi Garcia Lazzarin e Mardoqueu Freire Nunes
  Data: 27/10/2024
*/

#ifndef _LIBBCKAPP_
#define _LIBBCKAPP_

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <libgen.h>

#define NAME_SIZE 256    //Tamanho maximo nome do arquivo
#define PATH_SIZE 4096   //Tamanho maximo caminho do arquivo
#define TAM_BUFFER 4096  //Tamanho maximo buffer de leitura do arquivo
#define MAX_SEQ 31       //Sequencia maxima nas mensagens (maximo em 5 bits)

#define PASTA_BKP "backupapp"

#define NOME_ARQ_CKSUM "Arquivo_temporario_do_cksum.tmp"
#define MSG_ERRO_ENVIO "Não foi possivel enviar arquivo.\n"

//Funcao recebe em nome_arq um nome de arquivo ou diretorio???
//Retorna o nome e o caminho 
//Retorna 1 em caso de sucesso e 0 cc
int trata_nome_dir(char* nome_arq, char* nome, char* caminho, char* caminho_completo);

//Verifica se o arquivo existe 
//Retorna um stream para o arquivo se der tudo certo, e NULL cc
//caminho_completo e caminho_atual devem ter sido alocados
//Abre no modo de leitura
FILE* abre_arquivo(const char* caminho, const char* nome, char* caminho_completo, char* caminho_atual, char* c);

//Le o arquivo e retorna um buffer do tamanho recebido
//Recebe um ponteiro para um arquivo aberto no modo leitura
//Retorna a quantidade de bytes lidos
int le_arquivo(FILE* arquivo, void* buffer, int tamanho_buffer);

//Calcula o checksum do arquivo
//Retorna o tamnho do arquivo em bytes, recebe o caminho completo do arq
//Salva o valor od cksum no segundo parametro
long long cksum(char* caminho_completo, long long* valor_cksum);

//retorna 1 se a entrada e valida, e 0 cc
//o identificador da opc e retornado em opt, e o nome do arquivo em nome_arq
//Imprime o help e avisa em caso de arguemntos invalidos
int trata_entrada(const int argc, char **argv, char **envp, int *opt, char **nome_arq );

//Incrementa a sequencia da msg
//Se for to manhao maximo volta ao 0
//Retorna o valor incrementado
int inc_sequencia(int seq);

//Cria um arquivo com o nome recebido 
//Cria no subdiretorio corrente chamado arquivo_backup, cria o diretorio se nao existir
FILE* cria_arq(char* nome_arq, char* caminho_atual);

//Fecha o arquivo e volta para o diretorio inicial em cria_arq
void fecha_arq(FILE* arq, char* caminho);

//Recebe um stream para o arquivo
//Recebe um buffer dados, e o tamanho
//Grava esses dados no arq
int grava_dados(FILE* arq, unsigned char* dados, unsigned char tamanho);

//Recebe a ultima sequencia e a atual
//Retorna 1 se a atual eh a proxima e 0 cc
int verifia_seq(unsigned char ultima_seq, unsigned char seq_atual);

#endif