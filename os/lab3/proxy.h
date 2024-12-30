#include "http.h"
#include <unistd.h>
// #include <


#define BUFFER_SIZE 1024

extern Logger* logger;

typedef struct client_context {
    int client_socket;
} client_context;

typedef struct server_context {
    int dont_know;
} server_context;

void handle_client(client_context* context);
void *client_thread(void *arg);
// static int recieve_request(client_context* context, request_t* request);

// void server_handler(server_context* context);
// void *server_thread(void *arg);