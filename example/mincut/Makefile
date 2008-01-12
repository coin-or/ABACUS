
CC      = gcc
CFLAGS  = -O3

OBJECTS = PadbergRinaldi.o GoldbergTarjan.o initdistlabels.o reinitdistlabels.o

mincut.a: $(OBJECTS)
	ar rcv mincut.a $(OBJECTS)

clean:
	-rm mincut.a $(OBJECTS)
