#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "librede.h"

#define MARKER 126        // Valor esperado para o marcador de início
#define DATA_SIZE 63      // Tamanho do campo Dados
#define CRC_SEED 0xFF     // CRC inicial


int main() {
    char *interface = "eth0";                 // Nome da Interface de rede usada
    int soquete = cria_raw_socket(interface);
    unsigned char buffer[ETH_FRAME_LEN];      // Receberá toda a mensagem enviada
    short int flag;                           // Flag para identificar se o marcador de início está correto
    unsigned char dados[DATA_SIZE];           // Parte de dados da mensagem recebida
    unsigned char tamanho = 0;                // Campo tamanho da mensagem recebida
    unsigned char sequencia = 0;              // Sequência da mensagem
    unsigned char tipo = 0;                   // Tipo da mensagem (ex: backup, restaura, verifica)

    printf("Aguardando pacotes...\n");

    while (1) {
        // Recebe o pacote e guarda em buffer
        recebe_pacote(soquete, buffer);

        // Verifica o marcador de início
        flag = verifica_marcador(buffer);
        if (flag == 1) {
            printf("Marcador inválido!\n");
            continue;
        }

        // Desmonta o pacote, colocando cada parte da mensagem nas variáveis passadas
        desmontar_pacote(buffer, dados, &tamanho, &sequencia, &tipo);

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