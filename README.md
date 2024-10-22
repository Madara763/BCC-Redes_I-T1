# BCC-Redes_I-T1Trabalho 1
Implantar um servidor de BackUp de arquivos usando RawSockets (ethi0, Io);
A implementação DEVE ter os seguintes comandos:
• backup <nome_arq>
  Faz o backup de um arquivo do cliente para o servidor!
• restaura <nome_arq>
  Restaura um arquivo do servidor para o cliente.
• verefica <nome_arq>
  Verefica se o arquivo do cliente e do servidor são iguais. (utlizar cksum() - verefica crc).

Seguir protocolo definido em sala:
• Não usar BitField;
• TimeOut obrigatório - existem 6 formas de fazer. Escolher um e colocar no
relatório o porquê da escolha.

Protocolo:
|-----------------------|--------------|--------------|---------|--------------|------|
   Marcador de início   | tamanho      | sequência    | tipo    | Dados        | crc  |
|-----------------------|--------------|--------------|---------|--------------|------|
8 bits                   6 bits         5 bits         5 bits    63 bytes       8bits

• Marcador de início: 0111 1110 - sera sempre Esse.
• Tamanho: tamanho do campo de dados.
• CRC de 8 bits: Tem no wikipedia pronto (porém tem um erro nele);
o CRC sobre o tamanho/sequência/tipo/dado
o Controle de fluxo: para-e-espera.

Tabela de Tipos
TIPOS DEFINIÇÂO
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

Um exemplo de funcionamento para o Backup:

C = cliente.
S = Servidor.

C: 00100 “nome-arq”
S: 0001 ou 00010 ou 11111 “cod_erro” -> criar uma tabela de códigos de erros

C: 01111 “tam” -> tamanho em bytes – tamanho pode ser usado para verificar se tem espaço.
S: 0001 ou 00010 ou 11111 “cod_erro”

C: 10001 – envia os dados
S: 00001 ou 00000 – se foi tudo bem com o envio dos dados ou não

C: 10001 – encerra transmissão
S: 00001 ou 00000

Observação: Lembrar de estruturar um TimeOut nas repostas!
- A interface pode ser feita de qualquer forma, não importa a beleza.
- O que interessa é se funciona – o que está passando na rede.
- Tratar erros locais!
- Funcionamento análogo para o verifica e restaura, vai mudar da adição de alguns
tipos a mais como ok+tam(servidor) e ok+checksum (checksum é rodado pelas duas



• Entrega - última semana do semestre.
• Valor – 4,0.máquinas para comparação).
• Relatório obrigatório – conter o porquê de certas escolhas tomadas para a
implementação do trabalho (tipo o timeout).
Observações: Serão 2 máquinas, um servidor e a outra cliente.
- Biblioteca e funções sobre o RawSockets no moodle!
