#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "librede.h"
#include "libbckapp.h"

#define MARKER 126        // Valor esperado para o marcador de início
#define DATA_SIZE 63      // Tamanho do campo Dados
#define CRC_SEED 0xFF     // CRC inicial


int main() {

    //Nomes Interface
    //Notebook Davi - enp7s0
    //PC Davi - enp9s0

    char *interface = "lo";                 // Nome da Interface de rede usada
    int soquete = cria_raw_socket(interface);
    unsigned char buffer[ETH_FRAME_LEN];      // Receberá toda a mensagem enviada                       
    unsigned char dados[DATA_SIZE];           // Parte de dados da mensagem recebida
    unsigned char tamanho = 0;                // Campo tamanho da mensagem recebida
    unsigned char sequencia = 0;              // Sequência da mensagem
    unsigned char tipo = 0;                   // Tipo da mensagem (ex: backup, restaura, verifica)

    unsigned char ultima_seq = 0;             // Sequência da mensagem
    
    int i_aux;

    //Variaveis dos arquivos
    FILE* arq;
    char caminho_atual[PATH_SIZE];
    char nome[NAME_SIZE] = {0};
    
    //Essa matriz armazena os pacotes recebidos um por linha
    //A primeira coluna de cada linha diz se a linha deve ser escrita no arquivo ou nao
    //0 se nao, 1 se sim 
    unsigned char buffer_seq[MAX_SEQ][DATA_SIZE + 1];
    
    printf("Aguardando pacotes...\n");

    while (1) {

        // Recebe o pacote e guarda em buffer, 
        if (!recebe_pacote(soquete, buffer)) {
            printf("Marcador inválido!\n");
            continue;
        }

        // Desmonta o pacote, colocando cada parte da mensagem nas variáveis passadas
        desmontar_pacote(buffer, dados, &tamanho, &sequencia, &tipo);

        switch (tipo)
        {
        case TP_BACKUP_INI: //Inicio de um backup
            printf("Backup iniciado.\n");

            while (tamanho == DATA_SIZE){
                if(tipo == TP_BACKUP_INI || tipo == TP_ENVIA_NOME){
                    i_aux = (((int) sequencia) * 63) ;
                    strncpy(nome + i_aux, (char *) dados, tamanho );
                }

                if (recebe_pacote(soquete, buffer))
                    desmontar_pacote(buffer, dados, &tamanho, &sequencia, &tipo);
                else 
                    continue;  
            }

            //ultimos 63 bytes do nome
            if(tipo == TP_BACKUP_INI || tipo == TP_ENVIA_NOME){
                i_aux = (((int) sequencia) * 63) ;
                strncpy(nome + i_aux, (char *) dados, tamanho );
            }
            
            //Recebi o nome todo, cria o arquivo
            arq = cria_arq(nome, caminho_atual);
            if(!arq){fprintf(stderr, "Erro ao criar aquivo.\n"); return 0;}   

            if(recebe_pacote(soquete, buffer))
                desmontar_pacote(buffer, dados, &tamanho, &sequencia, &tipo);
            

            printf("Pacote recebido:\n");
            printf("  Tamanho: %u\n", tamanho);
            printf("  Sequência: %u\n", sequencia);
            printf("  Tipo: %u\n", tipo);
            printf("  Dados: ");
            printf("\n");
    
            if(recebe_pacote(soquete, buffer))
                desmontar_pacote(buffer, dados, &tamanho, &sequencia, &tipo);

            //inicia recepcao do arquivo
            if(tipo == TP_ENVIA_ARQ){
                printf("entrou no tp_envia_arq\n");
                while(tipo != TP_FIM_ENVIO){
                    
                    if( verifia_seq(sequencia, ultima_seq) ){
                        if(grava_dados(arq, dados, tamanho))
                            ultima_seq = sequencia;//retorna erro
                        //else
                            //retorna erro

                    }
                    
                    if (recebe_pacote(soquete, buffer))
                        desmontar_pacote(buffer, dados, &tamanho, &sequencia, &tipo);
                    else
                        continue;       

                }//Backup Finalizado
            }
            fecha_arq(arq, caminho_atual);
            break;
        
        default:
            break;
        } 

        // Exibe conteúdo do pacote decodificado
        // printf("Pacote recebido:\n");
        // printf("  Tamanho: %u\n", tamanho);
        // printf("  Sequência: %u\n", sequencia);
        // printf("  Tipo: %u\n", tipo);
        // printf("  Dados: ");
        // for (int i = 0; i < DATA_SIZE; i++) {
        //     printf("%02x ", dados[i]);
        // }
        // printf("\n");
    }

    close(soquete);
    return 0;
}