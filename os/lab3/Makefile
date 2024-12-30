# TARGET = proxy
# SRCS =

TEST_HASHMAP = test_hashmap
SRCS_TEST_HASHMAP = main.c proxy.c http.c logger.c

CC=gcc
RM=rm
CFLAGS= -g -Wall
LIBS=./llhttp/build/libllhttp.a -lpthread
INCLUDE_DIR="."

# libllhttp: ./llhttp/build/libllhttp.a

# ./llhttp/build/libllhttp.a:
# 	(cd llhttp && npm ci && make)

all: ${TEST_HASHMAP} 

${TEST_HASHMAP}: ${SRCS_TEST_HASHMAP}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} -I./llhttp/build/ ${SRCS_TEST_HASHMAP} ${LIBS} -o ${TEST_HASHMAP}


clean:
	${RM} -f *.o ${TEST_HASHMAP}