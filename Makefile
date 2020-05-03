CC = gcc
CFLAGS = `pkg-config --cflags glib-2.0` -Wall -pthread
LDLIBS = `pkg-config --libs glib-2.0`

EXE = server

all: $(EXE)

$(foreach f, $(EXE), $(eval $(f):))

.PHONY: clean

clean:
	${RM} $(EXE)
