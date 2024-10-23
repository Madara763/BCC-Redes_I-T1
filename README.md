# PRIMEIRO TRABALHO DE REDES 1
## ESTRUTURAÇÃO DO SISTEMA
Esse trabalho consiste na criação de um sistema de backup com aplicações do lado do cliente e do servidor, além de um protocolo de comunicação novo.
### Aplicação servidora:
Esse programa, instalado e sempre em execução no servidor recebe, trata e retorna a solicitações do cliente, sendo essas:
  - Backup: Servidor recebe um arquivo do cliente, o servidor armazena o arquivo e registra que ele está salvo, e retorna uma mensagem se deu certo.
  - Restaura: Cliente solitita um arquivo ao servidor, servidor consulta a existencia do arquivo, se existir retorna o arquivo e o cliente retorna que deu certo, caso contrário retorna uma mensagem de erro.
  - Consulta: Cliente envia pergunta se um arquivo com nome 'x' existe no servidor de backup, servidor retorna se existe ou não.

### Aplicação Cliente
Esse programa é executado quando o usuário chama a execução, lê e escreve arquivos na máquina do cliente e faz requisicões a aplicação no servidor:
  - Backup: Cliente recebe um arquivo do usuário envia um arquivo ao servidor, o servidor armazena o arquivo e registra que ele está salvo, e retorna uma mensagem se deu certo.
  - Restaura: Cliente solitita um arquivo ao servidor, servidor consulta a existencia do arquivo, se existir retorna o arquivo e o cliente retorna que deu certo, caso contrário retorna uma mensagem de erro, Cliente rebebe o arquivo e salva no diretório indicado.
  - Consulta: Cliente envia pergunta se um arquivo com nome 'x' existe no servidor de backup, servidor retorna se existe ou não, clinete exibe ao usuário.

### Comunicação na Rede
As mensagens trocadas entre as aplicações cliente e servidora são definidas por um protocolo próprio o Tartaruga.

#### Formato da Mensagem:

   Marcador de início   | Tamanho      | Sequência    | Tipo    | Dados        | CRC  |
|-----------------------|--------------|--------------|---------|--------------|------|
  8 bits                |   6 bits     |    5 bits    |  5 bits |  63 bytes    | 8bits

#### Marcador de inicio:
  - Marcador de início: 0111 1110 

#### Tamanho:
  - Quantidade de bytes no campo de dados

#### CRC:
  - CRC de 8 bits: Tem no wikipedia pronto (porém tem um erro nele) (DEFINIDO NO FUTURO) 
  - CRC sobre o tamanho/sequência/tipo/dado

#### Controle de Fluxo:
  - Controle de fluxo: para-e-espera

#### Tipos:
Alguns tipos das mensagens trocadas entre cliente e servidora.
00000 ACK – tudo ok, manda o próximo pedaço/arquivo
00001 NACK – problema de crc, não entendi e mando dnv, se ocorrer
manda dnv
00010 OK – Tudo certo.
00100 backup – inidica realização de backup
00101 restaura – indica realização de restaura
00110 verefica – indica realização de vereficação
11111 Erro – um código de erro
10000 Dados – enviando dados
01111 Tamanho
10001 Fim da transmissão
01110 ok + tamanho
01101 ok+checksum – cksum() função de verificação de crc


