/*
  Essa biblioteca implementa funcoes de manipulacao de arquivos e diretorios 
  Desenvolvida para uso no Trabalho 1 de Redes de BCC - UFPR

  Criado por Davi Garcia Lazzarin e Mardoqueu Freire Nunes
  Data: 27/10/2024
*/

#include "libbckapp.h"

//--------------------FUNCOES INTERNAS----------------------

//recebe nome e o caminho e concatena eles
void geraCaminhoCompleto(const char* caminho, const char* nome, char* l_caminho){
    strcpy(l_caminho, caminho);
    strcat(l_caminho, "/");
    strcat(l_caminho, nome);
}// Fim geraCaminhoCompleto

//--------------------FUNCOES BIBLIOTECA--------------------

//Funcao recebe em nome_arq um nome de arquivo ou diretorio???
//Retorna o nome e o caminho e o caminho completo, deve ter sido previamantre alocado
//Retorna 1 em caso de sucesso e 0 cc
int trata_nome_dir(char* nome_arq, char* nome, char* caminho, char* caminho_completo){
  
  char* aux;//recebe se o nome eh um diretorio ou nao

  aux = strrchr(nome_arq, '/');

  if(aux != NULL){ //if eh diretorio
        //salva o nome
        strcpy(nome, aux+1);

        //salva o caminho
        int tamnome, tampath;
        tamnome = strlen(nome);
        tampath = strlen(nome_arq);
        strncpy(caminho, nome_arq, tampath-tamnome-1 );
    }
    else{                   
        //salva o nome e pega o diretório
        strcpy(nome, nome_arq);
        getcwd(caminho, PATH_SIZE); //Get path name of the working directory
    }

      
  //Salva caminho completo do arquivo
  geraCaminhoCompleto(caminho, nome, caminho_completo);

  return 1;
}//Fim trata_nome_dir


//Verifica se o arquivo existe 
//Retorna um stream para o arquivo se der tudo certo, e NULL cc
//Caminho_completo e caminho_atual devem ter sido alocados
//Abre no modo c
FILE* abre_arquivo(const char* caminho, const char* nome, char* caminho_completo, char* caminho_atual, char* c ){

  //salva diretorio atual
  getcwd(caminho_atual, PATH_SIZE);
  
  //Vai para o diretorio do arquivo e tenta abrir
  chdir(caminho);
  FILE* arq;
  arq=fopen(caminho_completo, c);
  if(arq == NULL){
    fprintf(stderr, "Impossível abrir: %s\n", caminho_completo);
    return NULL;
  }
  
  return arq;
}//Fim abre_arquivo

//Le o arquivo e retorna um buffer do tamanho recebido
//Recebe um ponteiro para um arquivo aberto no modo leitura
//Retorna a quantidade de bytes lidos
int le_arquivo(FILE* arquivo, void* buffer, int tamanho_buffer){
  
  if(!arquivo){fprintf(stderr, "Erro ao ler aquivo.\n"); return 0;}

  int b_lidos=0;

  b_lidos=fread(buffer, 1, tamanho_buffer, arquivo);
  
  return b_lidos;
}

//Calcula o checksum do arquivo
//Retorna o valor do cksum, recebe o caminho completo do arq
//Salva o  tamanho do arquivo em bytes no segundo parametro
long long cksum(char* caminho_completo, long long* total_bytes){

  //Verifica se o arquivo existe
  FILE* arq=fopen(caminho_completo, "r");
  if(!arq){fprintf(stderr, "Erro ao ler aquivo.\n"); return 0;}
  fclose(arq);

  //Cria arquivo se saida do cksum
  arq=fopen(NOME_ARQ_CKSUM, "w");
  if(!arq){fprintf(stderr, "Erro ao ler aquivo.\n"); return 0;}
  char comando[8192];
  sprintf(comando, "cksum %s > %s ", caminho_completo, NOME_ARQ_CKSUM);
  system(comando);
  fclose(arq);

  //Le arquivo criado pelo cksum
  long long vlr_cksum=0, bytes=0;
  arq=fopen(NOME_ARQ_CKSUM, "r");
  if(!arq){fprintf(stderr, "Erro ao ler aquivo.\n"); return 0;}
  
  if( fscanf(arq, "%lli", &vlr_cksum ) == 0 ) {fprintf(stderr, "Erro ao ler checksum.\n"); return 0;}
  if( fscanf(arq, "%lli", &bytes ) == 0 ) {fprintf(stderr, "Erro ao ler checksum.\n"); return 0;}
  fclose(arq);

  //apaga arquivo temporario
  sprintf(comando, "rm %s", NOME_ARQ_CKSUM);
  system(comando);
  
  //retorna e finaliza
  *total_bytes = bytes;
  return vlr_cksum;

}

//retorna 1 se a entrada e valida, e 0 cc
//o identificador da opc e retornado em opt, e o nome do arquivo em nome_arq
//Imprime o help e avisa em caso de arguemntos invalidos
int trata_entrada(const int argc, char **argv, char **envp, int *opt, char **nome_arq ){
  /*
    -h ou --help exibe ajuda
    -b -> backup
    -v -> verifica arquivo
    -r -> restaura arquivo
  */
  char options[]="hb:r:v:";
  char msg_erro[]="Opção inválida, consulte as opções -h ou --help.\n";
  char msg_ajuda[]=" ./prog -opt nome_arquivo \n\
  opções: \n\
    -b: BACKUP - Envia o arquivo ao servidor.\n\
    -v: VERIFICAR - Consulta se o arquivo existe no servidor.\n\
    -r: RESTAURAR - Restaura o arquivo do servidor no diretório corrente.\n\
    -h ou --help: AJUDA -  Exibe essa mensagem de ajuda.\n \
  \nBCKAPP - v1.0 \n";

  *opt = getopt (argc, argv, options);
  
  if((*opt == 'b' || *opt == 'v' || *opt == 'r' ) && argc == 3 )
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


//Incrementa a sequencia da msg
//Se for to manhao maximo volta ao 0
//Retorna o valor incrementado
int inc_sequencia(int seq){
  int ret = seq;
  if(seq < MAX_SEQ )
    ret++;
  else
    ret=0;

  return ret;
}

