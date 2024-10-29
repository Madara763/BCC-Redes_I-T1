#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "libbckapp.h"

int trata_entrada(const int argc, char **argv, char **envp, int *opt, char **nome_arq );

int main(int argc, char **argv, char **envp){
  //alocacao de mem. e definicao de variaveis
  int opt;
  char* nome_arq;
  char nome[NAME_SIZE];
  char caminho[PATH_SIZE];
  char caminho_atual[PATH_SIZE];
  char caminho_completo[NAME_SIZE + PATH_SIZE];
  
  FILE* arquivo;

  void* buffer=malloc(TAM_BUFFER);
  if(!buffer)return 0;

  //verifica os parametros da main
  if(!trata_entrada(argc, argv, envp, &opt, &nome_arq)) return 0;
  
  //Menu, executa a funcionalidade chamada pelo usuario
  switch (opt)
  {
    //backup
    case 'b': 
      trata_nome_dir(nome_arq, nome, caminho);
      printf("Fazendo Backup: %s/%s ...\n", caminho, nome);
      
      arquivo=abre_arquivo(caminho, nome, caminho_completo, caminho_atual);
      if(arquivo)
        printf("Arquivo existe e pode ser aberto.\n");
      else
        printf("Arquivo nao existe ou nao pode ser aberto.\n");
      
      while(le_arquivo(arquivo, buffer, TAM_BUFFER)){



      }
      printf("Caminho atual do prog: %s\n", caminho_atual);
      printf("Caminho do arquivo: %s\n", caminho_completo);

      break;

    //consulta
    case 'c': 
      trata_nome_dir(nome_arq, nome, caminho);
      printf("Checando se há backup de: %s/%s ...\n", caminho, nome);
      break;

    //retaura
    case 'r':
      trata_nome_dir(nome_arq, nome, caminho);
      printf("Restaurando: %s/%s ...\n", caminho, nome);
      break;

    default:
      printf("Opção inválida, consulte as opções -h ou --help.\n");
      break;
  }

  
  return 0;
}

//retorna 1 se a entrada e valida, e 0 cc
//o identificador da opc e retornado em opt, e o nome do arquivo em nome_arq
//Imprime o help e avisa em caso de arguemntos invalidos
int trata_entrada(const int argc, char **argv, char **envp, int *opt, char **nome_arq ){
  /*
    -h ou --help exibe ajuda
    -b -> backup
    -c -> consulta arquivo
    -r -> restaura arquivo
  */
  char options[]="hb:r:c:";
  char msg_erro[]="Opção inválida, consulte as opções -h ou --help.\n";
  char msg_ajuda[]=" ./prog -opt nome_arquivo \n\
  opções: \n\
    -b: BACKUP - Envia o arquivo ao servidor.\n\
    -c: CONSULTAR - Consulta se o arquivo existe no servidor.\n\
    -r: RESTAURAR - Restaura o arquivo do servidor no diretório corrente.\n\
    -h ou --help: AJUDA -  Exibe essa mensagem de ajuda.\n \
  \nBCKAPP - v1.0 \n";

  *opt = getopt (argc, argv, options);
  
  if((*opt == 'b' || *opt == 'c' || *opt == 'r' ) && argc == 3 )
    *nome_arq = argv[2];

  else
    if( (argc == 2 && !strcmp(argv[1], "--help")) || *opt == 'h' ) 
      printf("%s", msg_ajuda); 
    
    else{
      printf("%s", msg_erro); 
      return 0;
    }

  return 1;
}