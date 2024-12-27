# TARGET = proxy
# SRCS =

TEST_HASHMAP = test_hashmap
SRCS_TEST_HASHMAP = hash.c hashmap.c main.c

CC=gcc
RM=rm
CFLAGS= -g -Wall
LIBS=-lpthread
INCLUDE_DIR="."

all: ${TEST_HASHMAP} 

${TARGET_1}: hash.h hashmap.h ${SRCS_TEST_HASHMAP}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_TEST_HASHMAP} ${LIBS} -o ${TEST_HASHMAP}


clean:
	${RM} -f *.o ${TEST_HASHMAP}