LDFLAGS += -lpthread -lconfuse -lasound -lnl-3 -lnl-route-3 -lnl-genl-3
CFLAGS += -I. -I/usr/include/libnl3
GFLAGS += -Wall -Wextra -Wshadow -pg


SRC=$(wildcard *.c routines/*.c targets/*.c)
OBJ=${SRC:.c=.o}

%.o: %.c
	@echo "compiling...$<"
	${CC} ${GFLAGS} -c ${CFLAGS} -o $@ $<

routines/%.o: routines/%.c
	@echo "compiling routines...$<"
	${CC} ${GFLAGS} -c ${CFLAGS} -o $@ $<

targets/%.o: targets/%.c
	@echo "compiling targets...$<"
	${CC} ${GFLAGS} -c ${CFLAGS} -o $@ $<

mstatus: ${OBJ}
	@echo "building..."
	@echo "${CC} -o $@"
	${CC} ${GFLAGS} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo "cleaning..."
	@rm -v mstatus ${OBJ}

all: mstatus
