LDFLAGS=-lpthread -lconfuse -lasound

SRC=$(wildcard *.c routines/*.c targets/*.c)
OBJ=${SRC:.c=.o}

%.o: %.c
	@echo "compiling...$<"
	${CC} -g -c ${CFLAGS} -o $@ $<

routines/%.o: routines/%.c
	@echo "compiling routines...$<"
	${CC} -g -c ${CFLAGS} -I.  -o $@ $<

targets/%.o: targets/%.c
	@echo "compiling targets...$<"
	${CC} -g -c ${CFLAGS} -I.  -o $@ $<

mstatus: ${OBJ}
	@echo "building..."
	@echo "${CC} -o $@"
	${CC} -g -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo "cleaning..."
	@rm -v mstatus ${OBJ}

all: mstatus
