#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "libbckapp.h" 

#define TAM_MAX_DADOS 63
#define MAX_ERROS 5
#define MAX_SEQ 31



int trata_entrada(const int argc, char **argv, char **envp, int *opt, char **nome_arq );
int inc_sequencia(int seq);

void* prepara_pacote(int tam, unsigned char sequencia, unsigned  char tipo, void* dados){

  //tamanho pacote = 4 + tam
  void* pacote=malloc(tam + 4);
  if(!pacote)
    return NULL;
  
  ((char*) pacote)[0] = 126;
  //em pacote [1] e pacote [2] colocar tamanho tamnho, seq e tipo
  //nos teste a seq vai na 1 e o tamanho na 2
  ((char*) pacote)[1] = sequencia;
  ((char*) pacote)[2] = (char)tam;

  //De pacote[3] ate pacote[2 + tam] vao os dados
  memcpy(pacote + 3, dados, tam );
  
  //pacote tam+3 recebe o crc 
  ((char*) pacote)[tam + 3] = '\0';
  return pacote;
} 

//imprime os dados do pacote
int imprime_pacote(void* pacote){

  //printf("Recebe a msg com sequencia: %i \nE tamanho: %i\n", ((char*) pacote)[1], ((char*) pacote)[2]);
  //printf("%s", ((char*) pacote+3));
  for(int i=3; i < ((char*) pacote)[2] +3; i++){
    printf("%c", ((char*) pacote)[i]);
  }
  
  //printf("\n\n");
  return 1;
}



int main(int argc, char **argv, char **envp){

  //alocacao de mem. e definicao de variaveis
  int opt, lidos, tam_dados_msg, cont_erro=0, tam_nome;
  
  unsigned char seq = 0, tipo = 0;
  char* nome_arq;
  char nome[NAME_SIZE];
  char caminho[PATH_SIZE];
  char caminho_atual[PATH_SIZE];
  char caminho_completo[NAME_SIZE + PATH_SIZE];
  
  FILE* arquivo;

  void* pacote;
  void* ptr_buffer;
  void* buffer=malloc(TAM_BUFFER);
  if(!buffer) return 0;

  
  //verifica os parametros da main
  if(!trata_entrada(argc, argv, envp, &opt, &nome_arq)) return 0;
  
  //Menu, executa a funcionalidade chamada pelo usuario
  switch (opt)
  {
    //backup
    case 'b': 
      trata_nome_dir(nome_arq, nome, caminho, caminho_completo );

      //FAZ O ENVIO DO CAMINHO COMPLETO DO ARQUIVO
      seq=0;
      tam_nome=strlen(caminho_completo);
      ptr_buffer = caminho_completo; //aponta pro inicio do caminho
              
      //sequencializa, prepara e envia se o total de dados do nome eh maior de 63
      while(tam_nome > TAM_MAX_DADOS){

        tam_dados_msg = TAM_MAX_DADOS;
        pacote = prepara_pacote (tam_dados_msg, seq, tipo, ptr_buffer);
        
        if(imprime_pacote(pacote)){         //se o envio deu certo
          tam_nome -= tam_dados_msg;        //diminui a quantidade restante
          seq = inc_sequencia(seq);         //prepara a proxima sequencia
          ptr_buffer += tam_dados_msg;      //avanca o ptr no buffer
          free(pacote);
        }
        else{                               //se o envio deu errado
          if(cont_erro > MAX_ERROS){
            fprintf(stderr, MSG_ERRO_ENVIO);
            return 1;
          }
          cont_erro++;                    
        }
      }

      //Envia o final do buffer com tamanho menor que 63 bytes
      tam_dados_msg = tam_nome;
      pacote = prepara_pacote (tam_dados_msg, seq, tipo, ptr_buffer);

      if(imprime_pacote(pacote) ){        //se o envio deu certo
        tam_nome -= tam_dados_msg;        //diminui a quantidade restante
        seq = inc_sequencia(seq);         //prepara a proxima sequencia
        ptr_buffer += tam_dados_msg;      //avanca o ptr no buffer
        free(pacote);     
      }
      else{                               //se o envio deu errado
        if(cont_erro > MAX_ERROS){
          fprintf(stderr, MSG_ERRO_ENVIO);
          return 1;
        }
        cont_erro++;
      }

      //FAZ O ENVIO DO ARQUIVO
      seq=0;
      arquivo = abre_arquivo(caminho, nome, caminho_completo, caminho_atual, "r"); 
      lidos = le_arquivo(arquivo, buffer, TAM_BUFFER);
      while(lidos){
        //printf("%s", (char*) buffer); 

        ptr_buffer = buffer; //aponta pro inicio do buffer
                
        //sequencializa, prepara e envia se o total de dados eh maior de 63
        while(lidos > TAM_MAX_DADOS){

          tam_dados_msg = TAM_MAX_DADOS;
          pacote = prepara_pacote (tam_dados_msg, seq, tipo, ptr_buffer);
          
          if(imprime_pacote(pacote)){         //se o envio deu certo
            lidos -= tam_dados_msg;           //diminui a quantidade restante
            seq = inc_sequencia(seq);         //prepara a proxima sequencia
            ptr_buffer += tam_dados_msg;      //avanca o ptr no buffer
            free(pacote);
          }
          else{                               //se o envio deu errado
            if(cont_erro > MAX_ERROS){
              fprintf(stderr, MSG_ERRO_ENVIO);
              return 1;
            }
            cont_erro++;                    
          }
        }

        //Envia o final do buffer com tamnho menor que 63 bytes
        tam_dados_msg = lidos;
        pacote = prepara_pacote (tam_dados_msg, seq, tipo, ptr_buffer);

        if(imprime_pacote(pacote) ){        //se o envio deu certo
          lidos -= tam_dados_msg;           //diminui a quantidade restante
          seq = inc_sequencia(seq);         //prepara a proxima sequencia
          ptr_buffer += tam_dados_msg;      //avanca o ptr no buffer
          free(pacote);     
        }
        else{                               //se o envio deu errado
          if(cont_erro > MAX_ERROS){
            fprintf(stderr, MSG_ERRO_ENVIO);
            return 1;
          }
          cont_erro++;
        }

        //Aqui ja enviou o buffer todo, parte para o proximo
        lidos=le_arquivo(arquivo, buffer, TAM_BUFFER);
      }
      
      printf("Backup finalizado!!!\n%s salvo.\n", caminho_completo);
      fclose(arquivo);
      break;

    //verifica
    case 'v': 
      trata_nome_dir(nome_arq, nome, caminho, caminho_completo);

      long long tam_arq=0, valor_cksum;
      valor_cksum=cksum(caminho_completo, &tam_arq);
      printf("CheckSum -> %lli %lli\n", valor_cksum, tam_arq);

      printf("Checando se há backup de: %s/%s ...\n", caminho, nome);

      break;

    //retaura
    case 'r':
      trata_nome_dir(nome_arq, nome, caminho, caminho_completo );
      printf("Restaurando: %s/%s ...\n", caminho, nome);
      break;

    default:
      printf("Opção inválida, consulte as opções -h ou --help.\n");
      break;
  }

  free(buffer);
  return 0;
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