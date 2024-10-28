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

#define NAME_SIZE 256
#define PATH_SIZE 4096

//Funcao recebe em nome_arq um nome de arquivo ou diretorio???
//Retorna o nome e o caminho 
//Retorna 1 em caso de sucesso e 0 cc
int trata_nome_dir(char* nome_arq, char* nome, char* caminho);

#endif