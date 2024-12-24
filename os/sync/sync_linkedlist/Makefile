TARGET_1 = queue-example
SRCS_1 = queue.c queue-example.c

TARGET_2 = queue-threads
SRCS_2 = queue.c queue-threads.c

TARGET_3 = queue-spin
SRCS_3 = queue-spin.c queue-threads.c spin.c

TARGET_4 = queue-mutex
SRCS_4 = queue-mutex.c queue-threads.c mutex.c

TARGET_5 = queue-cond
SRCS_5 = queue-cond.c queue-threads.c

TARGET_6 = queue-sem
SRCS_6 = queue-sem.c queue-threads.c

CC=gcc
RM=rm
CFLAGS= -g -Wall
LIBS=-lpthread
INCLUDE_DIR="."

all: ${TARGET_1} ${TARGET_2} ${TARGET_3} ${TARGET_4} ${TARGET_5} ${TARGET_6}

${TARGET_1}: queue.h ${SRCS_1}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_1} ${LIBS} -o ${TARGET_1}

${TARGET_2}: queue.h ${SRCS_2}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_2} ${LIBS} -o ${TARGET_2}

${TARGET_3}: queue-spin.h spin.h ${SRCS_3}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_3} ${LIBS} -o ${TARGET_3}

${TARGET_4}: queue-mutex.h mutex.h ${SRCS_4}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_4} ${LIBS} -o ${TARGET_4}

${TARGET_5}: queue-cond.h ${SRCS_5}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_5} ${LIBS} -o ${TARGET_5}

${TARGET_6}: queue-sem.h ${SRCS_6}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_6} ${LIBS} -o ${TARGET_6}

clean:
	${RM} -f *.o ${TARGET_1} ${TARGET_2} ${TARGET_3} ${TARGET_4} ${TARGET_5} ${TARGET_6}
