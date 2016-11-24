LDFLAGS=-lpthread


SRC=main.c bar.c item.c util.c
OBJ=${SRC:.c=.o}

.c.o:
	@echo "compiling...$<"
	${CC} -g -c ${CFLAGS} $<

mstatus: ${OBJ}
	@echo "building..."
	@echo "${CC} -o $@"
	${CC} -g -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo "cleaning..."
	@rm -v mstatus ${OBJ}

all: mstatus
