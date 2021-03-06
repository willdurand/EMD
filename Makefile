.SUFFIXES:
.SUFFIXES:.cpp .o

EXEC=emd
SOURCES=main.cpp Euclidean.cpp

OBJETS=$(SOURCES:%.cpp=%.o)

CC=g++
CFLAGS=-Wall -ansi -pedantic -ffast-math -I /usr/X11R6/include -I ./CImg
LFLAGS= -L . -L /usr/X11R6/lib  -lpthread -lX11 -lXext -Dcimg_use_xshm  -lm

$(EXEC):$(OBJETS) Makefile
	$(CC) -o  $(EXEC) $(OBJETS) $(LFLAGS)
.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean: clear
	rm -f $(OBJETS)
clear:
	rm -f $(EXEC)
depend:
	sed -e "/^#DEPENDANCIES/,$$ d" Makefile >dependances
	echo "#DEPENDANCIES" >> dependances
	$(CC) -MM $(SOURCES) >> dependances
	cat dependances >Makefile
	rm dependances

#DEPENDANCIES
main.o: main.cpp CImg.h Euclidean.hpp
Euclidean.o: Euclidean.cpp Euclidean.hpp
