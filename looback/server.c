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
    char *interface = "lo"; // Usando loopback
    int soquete = cria_raw_socket(interface);
    char buffer[ETH_FRAME_LEN];

    printf("Aguardando pacotes...\n");

    while (1) {
        // Receber pacotes
        ssize_t tamanho_recebido = recvfrom(soquete, buffer, ETH_FRAME_LEN, 0, NULL, NULL);
        if (tamanho_recebido < 0) {
            perror("Erro ao receber pacote");
            close(soquete);
            exit(-1);
        }

        // Exibir conteúdo do pacote recebido
        printf("Pacote recebido com %ld bytes: ", tamanho_recebido);
        for (ssize_t i = 0; i < tamanho_recebido; i++) {
            printf("%02x ", (unsigned char)buffer[i]);
        }
        printf("\n");
    }

    close(soquete);
    return 0;
}
