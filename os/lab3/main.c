#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include "hash.h"
#include "proxy.h"

#define DEFAULT_PORT 1080
#define DEFAULT_BACKLOG 128

Logger* logger;

int interrupted = 0;

void signal_handler(int signal){
    interrupted = 1;
}

static void set_signal_handler(){
    struct sigaction act = {0};
    act.sa_handler = signal_handler;
    sigaction(SIGINT, &act, NULL);
}

void print(uint64_t number){
    printf("number: %lu\n", number);
}

int configureServerSocket(int port, int backlog){
    int err;

    int server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket < 0){
        perror("socket");
        exit(1);
    }

    int opt = 1;

    err = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (err){
        perror("setsockopt");
        exit(1);
    }

    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = INADDR_ANY
    };

    err = bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    
    if (err == -1){
        perror("bind");
        close(server_socket);
        exit(1);
    }

    err = listen(server_socket, backlog);

    return server_socket;
}


int main(){
    // char *str = "string\0";

    // uint64_t hash = MurmurOAAT64(str);

    logger = create_logger(stdout, INFO);

    set_signal_handler();

    int server_socket = configureServerSocket(DEFAULT_PORT, DEFAULT_BACKLOG);
    if (server_socket < 0){
        // perror("configureServerSocket");
        logger_error(logger, "configureServerSocket");
        exit(1);
    }

    logger_info(logger, "server started");

    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);

    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    while (!interrupted){
        int err;
        // int client_socket = accept(server_socket, NULL, NULL);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len);
        if (client_socket < 0){
            // perror("accept");
            if (errno == EINTR) {
                continue;
            }
            logger_error(logger, "accept");
            continue;
        }

        client_context* context = (client_context*)malloc(sizeof(client_context));
        context->client_socket = client_socket;

        err = pthread_create(&tid, &attr, (void*(*)(void*))client_thread, (void*)context);
        if (err){
            logger_error(logger, "pthread_create");

            continue;
        }
    }
    
    pthread_attr_destroy(&attr);
    close(server_socket);
    logger_info(logger, "server stopped");
    destroy_logger(logger);
}