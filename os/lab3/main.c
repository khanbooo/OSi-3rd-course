#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "hash.h"

#define DEFAULT_PORT 8080
#define DEFAULT_BACKLOG 128

typedef struct client_context {
    int client_socket;
} client_context;

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

void handle_client(client_context* context){
    printf("client_socket: %d\n", context->client_socket);
    free(context);
}

void* client_thread(void * args){
    __NOP(); __NOP(); __NOP(); __NOP();
    handle_client((client_context*)args);
    __NOP(); __NOP(); __NOP(); __NOP();
    return;
}

int main(){
    // char *str = "string\0";

    // uint64_t hash = MurmurOAAT64(str);

    int server_socket = configureServerSocket(DEFAULT_PORT, DEFAULT_BACKLOG);
    if (server_socket < 0){
        perror("configureServerSocket");
        exit(1);
    }

    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);

    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    while (1){
        int err;
        // int client_socket = accept(server_socket, NULL, NULL);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len);
        if (client_socket < 0){
            perror("accept");
            close(server_socket);
            exit(1);
        }

        client_context* context = (client_context*)malloc(sizeof(client_context));
        context->client_socket = client_socket;

        err = pthread_create(&tid, &attr, (void*(*)(void*))client_thread, (void*)context);
        if (err){
            perror("pthread_create");
            close(client_socket);
            close(server_socket);
            exit(1);
        }
    }
}