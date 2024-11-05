#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MARKER 126       // Valor esperado para o marcador de início
#define DATA_SIZE 63      // Tamanho do campo Dados
#define CRC_SEED 0xFF     // CRC inicial

// Função para calcular o CRC simples de 8 bits
// unsigned char calcula_crc(unsigned char* dados, size_t tamanho) {
//     unsigned char crc = CRC_SEED;
//     for (size_t i = 0; i < tamanho; i++) {
//         crc ^= dados[i];
//     }
//     return crc;
// }

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

short int verifica_marcador(unsigned char* buffer) {
    // Verifica se o marcador de início é válido
    if (buffer[0] != MARKER) {
        printf("Marcador de início inválido!\n");
        return 1;
    }
    return 0;
}

void recebe_pacote(int soquete, unsigned char* buffer) {
    // Receber pacotes
    ssize_t tamanho_recebido = recvfrom(soquete, buffer, ETH_FRAME_LEN, 0, NULL, NULL);
    if (tamanho_recebido < 0) {
        perror("Erro ao receber pacote");
        close(soquete);
        exit(-1);
    }
}

void desmontar_pacote(unsigned char* buffer, unsigned char* dados, unsigned char* tamanho, unsigned char* sequencia, unsigned char* tipo) {
    // Extrair tamanho (6 bits) e sequência (5 bits)
    *tamanho = (buffer[1] >> 2) & 0x3F;
    *sequencia = ((buffer[1] & 0x03) << 3) | (buffer[2] >> 5);

    // Extrair tipo (5 bits)
    *tipo = buffer[2] & 0x1F;

    // Extrair dados (63 bytes)
    memcpy(dados, &buffer[3], DATA_SIZE);
}

int main() {
    char *interface = "lo";  // Usando loopback
    int soquete = cria_raw_socket(interface);
    unsigned char buffer[ETH_FRAME_LEN];  // Receberá toda a mensagem enviada
    short int flag;  // Flag para identificar se o marcador de início está correto
    unsigned char dados[DATA_SIZE];  // Parte de dados da mensagem recebida
    unsigned char tamanho = 0;       // Campo tamanho da mensagem recebida
    unsigned char sequencia = 0;     // Sequência da mensagem
    unsigned char tipo = 0;          // Tipo da mensagem (ex: backup, restaura, verifica)

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
