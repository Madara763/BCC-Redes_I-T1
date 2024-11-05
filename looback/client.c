#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MARKER 126      // Marcador de início (1 byte fixo)
#define DATA_SIZE 63     // Tamanho fixo para o campo Dados (63 bytes)
#define CRC_SEED 0xFF    // CRC inicial (1 byte fixo)

int cria_raw_socket(char* nome_interface) {
    int soquete = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (soquete == -1) {
        fprintf(stderr, "Erro ao criar socket: Verifique se você é root!\n");
        exit(-1);
    }

    int ifindex = if_nametoindex(nome_interface);
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

// Função para calcular o CRC simples de 8 bits
unsigned char calcula_crc(const unsigned char *dados, size_t tamanho) {
    unsigned char crc = CRC_SEED;
    for (size_t i = 0; i < tamanho; i++) {
        crc ^= dados[i];
    }
    return crc;
}


void envia_pacote(void* pacote, char* interface, int soquete){

    //declarando estrutura para o endereço de destino
    struct sockaddr_ll destino = {0};  // Endereço do destinatário
    destino.sll_ifindex = if_nametoindex(interface);
    destino.sll_halen = ETH_ALEN;
    memset(destino.sll_addr, 0xFF, ETH_ALEN);  

    //envia o pacote usando sendto()
    printf("Enviando pacote...\n");
    // Envia o pacote
    if (sendto(soquete, pacote, 4 + DATA_SIZE, 0, (struct sockaddr*)&destino, sizeof(destino)) < 0) {
        perror("Erro ao enviar pacote");
        close(soquete);
        exit(-1);
    }


}

int main() {
    char *interface = "lo";  // Interface de rede
    int soquete = cria_raw_socket(interface);
    unsigned char pacote[ETH_FRAME_LEN];  // Buffer de pacote

    // Configurando o protocolo
    unsigned char marcador = MARKER;
    unsigned char tamanho = DATA_SIZE;
    unsigned char sequencia = 0x1F;  // Exemplo de valor para o campo Sequência (5 bits)
    unsigned char tipo = 4;       // Exemplo de valor para o campo Tipo (5 bits)
    const char *dados = "Este é um exemplo de dados com tamanho fixo de 63 bytes!";
    
    // Configurando os primeiros campos do pacote
    pacote[0] = marcador;
    pacote[1] = ((tamanho & 0x3F) << 2) | ((sequencia >> 3) & 0x03);  // 6 bits de tamanho, 2 bits de sequência
    pacote[2] = ((sequencia & 0x07) << 5) | (tipo & 0x1F);             // 3 bits de sequência, 5 bits de tipo

    // Copia os dados de 63 bytes no pacote a partir do byte 3
    memcpy(&pacote[3], dados, DATA_SIZE);

    // Calcula o CRC para o campo de dados
    unsigned char crc = calcula_crc((unsigned char *)dados, DATA_SIZE);
    pacote[3 + DATA_SIZE] = crc;

    

    envia_pacote(pacote, interface, soquete);

    

    printf("Pacote enviado com sucesso!\n");

    close(soquete);
    return 0;
}
