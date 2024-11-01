#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MARKER 0xAA       // Valor esperado para o marcador de início
#define DATA_SIZE 63      // Tamanho do campo Dados
#define CRC_SEED 0xFF     // CRC inicial

// Função para calcular o CRC simples de 8 bits
unsigned char calcula_crc(const unsigned char *dados, size_t tamanho) {
    unsigned char crc = CRC_SEED;
    for (size_t i = 0; i < tamanho; i++) {
        crc ^= dados[i];
    }
    return crc;
}

int cria_raw_socket(char* nome_interface_rede) {
    int soquete = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (soquete == -1) {
        fprintf(stderr, "Erro ao criar socket: Verifique se você é root!\n");
        exit(-1);
    }

    int ifindex = if_nametoindex(nome_interface_rede);
    if (ifindex == 0) {
        fprintf(stderr, "Erro ao obter o índice da interface\n");
        exit(-1);
    }

    struct sockaddr_ll endereco = {0};
    endereco.sll_family = AF_PACKET;
    endereco.sll_protocol = htons(ETH_P_ALL);
    endereco.sll_ifindex = ifindex;

    if (bind(soquete, (struct sockaddr*) &endereco, sizeof(endereco)) == -1) {
        fprintf(stderr, "Erro ao fazer bind no socket\n");
        exit(-1);
    }

    return soquete;
}

int main() {
    char *interface = "lo";  // Usando loopback
    int soquete = cria_raw_socket(interface);
    unsigned char buffer[ETH_FRAME_LEN];

    printf("Aguardando pacotes...\n");

    while (1) {
        // Receber pacotes
        ssize_t tamanho_recebido = recvfrom(soquete, buffer, ETH_FRAME_LEN, 0, NULL, NULL);
        if (tamanho_recebido < 0) {
            perror("Erro ao receber pacote");
            close(soquete);
            exit(-1);
        }

        // Verifica o marcador de início
        if (buffer[0] != MARKER) {
            fprintf(stderr, "Marcador de início inválido!\n");
            continue;
        }

        // Extrair tamanho (6 bits) e sequência (5 bits)
        unsigned char tamanho = (buffer[1] >> 2) & 0x3F;
        unsigned char sequencia = ((buffer[1] & 0x03) << 3) | (buffer[2] >> 5);

        // Extrair tipo (5 bits)
        unsigned char tipo = buffer[2] & 0x1F;

        // Extrair dados (63 bytes)
        unsigned char dados[DATA_SIZE];
        memcpy(dados, &buffer[3], DATA_SIZE);

        // Extrair CRC do pacote
        unsigned char crc_recebido = buffer[3 + DATA_SIZE];

        // Calcula o CRC dos dados recebidos
        unsigned char crc_calculado = calcula_crc(dados, DATA_SIZE);

        // Verifica a integridade do pacote pelo CRC
        if (crc_recebido != crc_calculado) {
            fprintf(stderr, "Erro: CRC inválido! Esperado %02x, Recebido %02x\n", crc_calculado, crc_recebido);
            continue;
        }

        // Exibir conteúdo do pacote decodificado
        printf("Pacote recebido com %ld bytes:\n", tamanho_recebido);
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
