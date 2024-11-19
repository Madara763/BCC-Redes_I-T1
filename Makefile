CFLAGS = -Wall -g  # gerar "warnings" detalhados e infos de depuração
 
objs = cliente.o libbckapp.o librede.o

# regra default (primeira regra)
all: cliente
 
# regras de ligacao
cliente:	$(objs)
 
# regras de compilação
cliente.o:	cliente.c
libbckapp.o:	libbckapp.c
librede.o:	librede.c 

# remove arquivos temporários
clean:
	-rm -f $(objs) *~
 
# remove tudo o que não for o código-fonte
purge: clean
	-rm -f vinapp