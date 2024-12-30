#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "http.h"

#define BUFFER_SIZE 1024 * 64

extern Logger* logger;

typedef struct client_context {
    int client_socket;
} client_context;

typedef struct server_context {
    int server_socket;
    int client_socket;
} server_context;

void handle_client(client_context* context);
void *client_thread(void *arg);

void handle_server(server_context* context);
void *server_thread(void *arg);