LDFLAGS=-lpthread -lconfuse -lasound

SRC=$(wildcard *.c items/*.c)
OBJ=${SRC:.c=.o}

%.o: %.c
	@echo "compiling...$<"
	${CC} -g -c ${CFLAGS} -o $@ $<

items/%.o: items/%.c
	@echo "compiling items...$<"
	${CC} -g -c ${CFLAGS} -I.  -o $@ $<

mstatus: ${OBJ}
	@echo "building..."
	@echo "${CC} -o $@"
	${CC} -g -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo "cleaning..."
	@rm -v mstatus ${OBJ}

all: mstatus
