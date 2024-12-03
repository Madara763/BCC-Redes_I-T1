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

    char *interface = "eth0";                 // Nome da Interface de rede usada
    int soquete = cria_raw_socket(interface);
    unsigned char buffer[ETH_FRAME_LEN];      // Receberá toda a mensagem enviada                       // Flag para identificar se o marcador de início está correto
    unsigned char dados[DATA_SIZE];           // Parte de dados da mensagem recebida
    unsigned char tamanho = 0;                // Campo tamanho da mensagem recebida
    unsigned char sequencia = 0;              // Sequência da mensagem
    unsigned char tipo = 0;                   // Tipo da mensagem (ex: backup, restaura, verifica)

    //Variaveis dos arquivos
    char nome[NAME_SIZE];

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
            
            
            while(tipo != TP_FIM_ENVIO){
                
                if (recebe_pacote(soquete, buffer))
                    desmontar_pacote(buffer, dados, &tamanho, &sequencia, &tipo);
                else
                    continue;       

                if(tipo = TP_ENVIA_NOME){
                    
                }

            }//Backup Finalizado

            break;
        
        default:
            break;
        } 

        // Exibe conteúdo do pacote decodificado
        printf("Pacote recebido:\n");
        printf("  Tamanho: %u\n", tamanho);
        printf("  Sequência: %u\n", sequencia);
        printf("  Tipo: %u\n", tipo);
        printf("  Dados: ");
        for (int i = 0; i < DATA_SIZE; i++) {
            printf("%02x ", dados[i]);
        }
        printf("\n");
    }

    close(soquete);
    return 0;
}