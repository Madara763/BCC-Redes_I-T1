#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int cria_raw_socket(char* nome_interface_rede) {
    int soquete = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (soquete == -1) {
        fprintf(stderr, "Erro ao criar socket: Verifique se você é root!\n");
        exit(-1);
    }

    int ifindex = if_nametoindex(nome_interface_rede);
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
    char *interface = "lo"; // substitua pela interface adequada
    int soquete = cria_raw_socket(interface);
    char pacote[ETH_FRAME_LEN];

    // Preparando dados fictícios para o pacote
    memset(pacote, 0xFF, sizeof(pacote)); // Conteúdo do pacote (dummy data)

    struct sockaddr_ll destino = {0};
    destino.sll_ifindex = if_nametoindex(interface);
    destino.sll_halen = ETH_ALEN;
    memset(destino.sll_addr, 0xFF, ETH_ALEN); // Endereço de broadcast

    printf("Enviando pacote...\n");

    if (sendto(soquete, pacote, sizeof(pacote), 0, (struct sockaddr*)&destino, sizeof(destino)) < 0) {
        perror("Erro ao enviar pacote");
        close(soquete);
        exit(-1);
    }

    printf("Pacote enviado com sucesso!\n");

    close(soquete);
    return 0;
}
