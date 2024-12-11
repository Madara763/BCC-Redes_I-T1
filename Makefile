CFLAGS = -Wall -g  # gerar "warnings" detalhados e infos de depuração
 
objc = cliente.o libbckapp.o librede.o
objs = servidor.o libbckapp.o librede.o

# regra default (primeira regra)
all: cliente servidor
 
# regras de ligacao
cliente:	$(objc)
servidor:	$(objs)
 
# regras de compilação
cliente.o:	cliente.c
servidor.o:	servidor.c
libbckapp.o:	libbckapp.c
librede.o:	librede.c


# remove arquivos temporários
clean:
	-rm -f $(objs) $(objc) *~
 
# remove tudo o que não for o código-fonte
purge: clean
	-rm -f cliente 