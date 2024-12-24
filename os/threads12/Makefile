TARGET = thread-example
SRCS = thread12f.c

CC=gcc
RM=rm
CFLAGS= -g -Wall
LIBS=-lpthread
INCLUDE_DIR="."

all: ${TARGET}

${TARGET}: ${SRCS}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS} ${LIBS} -o ${TARGET}

clean:
	${RM} -f *.o ${TARGET}
