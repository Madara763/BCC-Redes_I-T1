#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int cria_raw_socket(char* nome_interface_rede) {

    // Cria um socket de nível de enlace, do tipo "raw" (SOCK_RAW) e protocolo Ethernet (ETH_P_ALL)
    int soquete = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (soquete == -1) {
        fprintf(stderr, "Erro ao criar socket: Verifique se você é root!\n");
        exit(-1);
    }

    // Converte o nome da interface de rede (e.g., "lo") no índice numérico da interface
    int ifindex = if_nametoindex(nome_interface_rede);
    
    // estrutura que define o endereço de um socket de nível de enlace. A estrutura é configurada 
    // para o protocolo Ethernet (sll_protocol) e associada ao índice da interface (sll_ifindex).
    struct sockaddr_ll endereco = {0};

    // Configura a estrutura de endereço de rede para o nível de enlace, 
    // define o protocolo Ethernet e associa ao índice da interface
    endereco.sll_family = AF_PACKET;
    endereco.sll_protocol = htons(ETH_P_ALL);
    endereco.sll_ifindex = ifindex;


    // bind(): vincula o socket à interface de rede especificada. 
    if (bind(soquete, (struct sockaddr*) &endereco, sizeof(endereco)) == -1) {
        fprintf(stderr, "Erro ao fazer bind no socket\n");
        exit(-1);
    }

    return soquete;
}

int main() {
    char *interface = "lo"; // substitua pela interface adequada
    int soquete = cria_raw_socket(interface);
    char pacote[ETH_FRAME_LEN]; //define um buffer pacote que armazenará os dados a serem enviados, com tamanho máximo de um quadro Ethernet.

    // Preparando dados fictícios para o pacote
    memset(pacote, 0xFF, sizeof(pacote)); // Conteúdo do pacote (dummy data)

    
    struct sockaddr_ll destino = {0};   //define a estrutura de endereço do destinatário.
    destino.sll_ifindex = if_nametoindex(interface);    // associa o índice da interface de rede.
    destino.sll_halen = ETH_ALEN;   //define o tamanho do endereço de hardware (6 bytes).
    memset(destino.sll_addr, 0xFF, ETH_ALEN); // Endereço de broadcast

    printf("Enviando pacote...\n");

    // sendto() envia o pacote para o destinatário especificado (modo de broadcast). Em caso de erro, imprime a mensagem e encerra o programa.
    if (sendto(soquete, pacote, sizeof(pacote), 0, (struct sockaddr*)&destino, sizeof(destino)) < 0) {
        perror("Erro ao enviar pacote");
        close(soquete);
        exit(-1);
    }

    printf("Pacote enviado com sucesso!\n");

    close(soquete);
    return 0;
}
