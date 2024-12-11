/*
  Essa biblioteca implementa funcoes de manipulacao pacotes e envio/recebimento em rede
  Desenvolvida para uso no Trabalho 1 de Redes de BCC - UFPR

  Criado por Davi Garcia Lazzarin e Mardoqueu Freire Nunes
  Data: 18/11/2024
*/

#include "librede.h"


//--------------------FUNCOES INTERNAS----------------------

void compactar_valores(void* pacote,  int tamanho, unsigned int sequencia, unsigned int tipo) {
    // Garantir que os valores estão dentro dos limites de bits
    tamanho &= 0x3F;     // 6 bits (0b111111)
    sequencia &= 0x1F;   // 5 bits (0b11111)
    tipo &= 0x1F;        // 5 bits (0b11111)

    // Compactar os valores em 2 bytes
    unsigned short combinado = (tamanho << 10) | (sequencia << 5) | tipo;

    // Dividir em dois bytes
    ((char*) pacote)[1] = (combinado >> 8) & 0xFF; // Byte mais significativo
    ((char*) pacote)[2] = combinado & 0xFF;        // Byte menos significativo
}

// Recebe um vetor de tam bytes
// Calcula um CRC de 8 bits com um polinômio de 9 bits em cima dos dados recebidos
unsigned char monta_crc(int tam, void* buffer) {
  uint16_t polynomial = 0x107;  // polinomio de 9 bits (x⁹ + x⁷ + 1)
  unsigned char crc = 0x00;     
  unsigned char *data = (unsigned char *)buffer; 
  
  // Percorre os bytes do buffer
  for (int i = 0; i < tam; i++) {
    crc ^= data[i]; // XOR do byte atual com o CRC
    
    // Processa cada bit do byte
    for (int j = 0; j < 8; j++) {
      if (crc & 0x80) // Verifica se o 8 bit do CRC está definido
        crc = (crc << 1) ^ (polynomial & 0xFF); 
      else
        crc <<= 1; // apenas desloca
      
    }
  }
  //printf("CRC-8: 0x%02X\n", crc);
  return crc; // Retorna o CRC calculado
}

// Verifica se o CRC recebido é válido
//Retorna 1 para correto, 0 cc
int verifica_crc(int tam, void* buffer, unsigned char crc_recebido) {
  
  // Recalcula o CRC com os dados fornecidos
  unsigned char crc_calculado = monta_crc(tam, buffer);
  
  return crc_calculado == crc_recebido;
}

//--------------------FUNCOES BIBLIOTECA--------------------

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

    //Modo promíscuo
    struct packet_mreq mr = {0};
    mr.mr_ifindex = ifindex;
    mr.mr_type = PACKET_MR_PROMISC;
    // Não joga fora o que identifica como lixo
    if (setsockopt(soquete, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) == -1) {
      fprintf(stderr, "Erro ao fazer setsockopt: "
                      "Verifique se a interface de rede foi especificada corretamente.\n");
      exit(-1);
    }

    return soquete;
}

int verifica_marcador(unsigned char* buffer) {
    // Verifica se o marcador de início é válido
    if (buffer[0] != MARKER) {
        printf("Marcador de início inválido!\n");
        return 0;
    }
    return 1;
}

int recebe_pacote(int soquete, unsigned char* buffer) {

    // Receber pacotes
    ssize_t tamanho_recebido = recv(soquete, buffer, ETH_FRAME_LEN, 0);
    if (tamanho_recebido < 0) {
        perror("Erro ao receber pacote");
        close(soquete);
        exit(-1);
    }

    return verifica_marcador(buffer);
}

void desmontar_pacote(unsigned char* buffer, unsigned char* dados, unsigned char* tamanho, unsigned char* sequencia, unsigned char* tipo) {

    // Extrair tamanho (6 bits) e sequência (5 bits)
    *tamanho = (buffer[1] >> 2) & 0x3F;
    *sequencia = ((buffer[1] & 0x03) << 3) | (buffer[2] >> 5);

    // Extrair tipo (5 bits)
    *tipo = buffer[2] & 0x1F;

    //verifica crc
    unsigned char suposto_crc = '\0';
    suposto_crc = (((unsigned char*)buffer)[*tamanho + 3]);



    // Extrair dados (63 bytes)
    memcpy(dados, &buffer[3], *tamanho);
    
    if(verifica_crc(*tamanho+2, buffer, suposto_crc))
      printf("CRC OK\n");
    else
      printf("CRC Errado\n");

    //Exibe conteúdo do pacote decodificado
    // printf("Pacote recebido:\n");
    // printf("  Tamanho: %u\n", *tamanho);
    // printf("  Sequência: %u\n", *sequencia);
    // printf("  Tipo: %u\n", *tipo);
    // printf("CRC-8: 0x%02X\n", suposto_crc);
    
    // printf("  Dados: ");
    // for (int i = 0; i < *tamanho; i++) {
    //     printf("%02x ", dados[i]);
    // }
    // printf("\n");
}

int envia_pacote(void* pacote, char* interface, int soquete){

    //declarando estrutura para o endereço de destino
    struct sockaddr_ll destino = {0} ;  // Endereço do destinatário
    destino.sll_ifindex = if_nametoindex(interface);
    destino.sll_halen = ETH_ALEN;
    memset(destino.sll_addr, 0xFF, ETH_ALEN);  

    // Envia o pacote
    if (send(soquete, pacote, 4 + DATA_SIZE, 0) < 0) {
        perror("Erro ao enviar pacote");
        close(soquete);
        return 0;
    }
    return 1;
}

//prepara o pacote para envia-lo
//separa a dados em seus respectivos indices detrno do pacote e de toda a mensagem; 
void* monta_pacote(int tam, unsigned char sequencia, unsigned char tipo, void* dados){

  //tamanho pacote = 4 + tam
  void* pacote = malloc(tam + 4);
  if(!pacote)
    return NULL;
  
  ((char*) pacote)[0] = 126;

  compactar_valores(pacote, tam, sequencia, tipo);

  //De pacote[3] ate pacote[2 + tam] vao os dados
  memcpy(pacote + 3, dados, tam );
  
  //pacote tam+3 recebe o crc 
  ((char*) pacote)[tam + 3] = monta_crc(tam+2, pacote);
  return pacote;
}

