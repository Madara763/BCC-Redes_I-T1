/*
  Essa biblioteca implementa funcoes de manipulacao de arquivos e diretorios 
  Desenvolvida para uso no Trabalho 1 de Redes de BCC - UFPR

  Criado por Davi Garcia Lazzarin e Mardoqueu Freire Nunes
  Data: 27/10/2024
*/
#include "libbckapp.h"

//Funcao recebe em nome_arq um nome de arquivo ou diretorio???
//Retorna o nome e o caminho, deve ter sido previamantre alocado
//Retorna 1 em caso de sucesso e 0 cc
int trata_nome_dir(char* nome_arq, char* nome, char* caminho){
  
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
        getcwd(caminho, PATH_SIZE);//Get path name of the working directory
    }
    return 1;
}
