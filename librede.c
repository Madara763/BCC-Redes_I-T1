
#include "librede.h"

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


void* prepara_pacote(int tam, unsigned char sequencia, unsigned  char tipo, void* dados){

  //tamanho pacote = 4 + tam
  void* pacote=malloc(tam + 4);
  if(!pacote)
    return NULL;
  
  ((char*) pacote)[0] = 126;
  //em pacote [1] e pacote [2] colocar tamanho tamnho, seq e tipo
  //nos teste a seq vai na 1 e o tamanho na 2
  ((char*) pacote)[1] = sequencia;
  ((char*) pacote)[2] = (char)tam;

  //De pacote[3] ate pacote[2 + tam] vao os dados
  memcpy(pacote + 3, dados, tam );
  
  //pacote tam+3 recebe o crc 
  ((char*) pacote)[tam + 3] = '\0';
  return pacote;
}