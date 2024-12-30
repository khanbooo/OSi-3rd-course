PROXY = proxy
SRCS_PROXY = main.c proxy.c http.c logger.c

CC=gcc
RM=rm
CFLAGS= -g -Wall
LIBS=./llhttp/build/libllhttp.a -lpthread
INCLUDE_DIR="."

all: ${PROXY} libllhttp

libllhttp: ./llhttp/build/libllhttp.a

./llhttp/build/libllhttp.a:
	(cd llhttp && npm ci && make)

${PROXY}: ${SRCS_PROXY}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} -I./llhttp/build/ ${SRCS_PROXY} ${LIBS} -o ${PROXY}

clean:
	${RM} -f *.o ${PROXY}