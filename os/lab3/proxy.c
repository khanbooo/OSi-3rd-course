#include "proxy.h"

extern Logger* logger;

static void log_request(request_t *request) {
    char buffer[256];
    snprintf(
        buffer, 
        sizeof(buffer), 
        "request: %s %s HTTP/%d.%d",
        llhttp_method_name(request->method),
        request->url,
        request->major_version,
        request->minor_version
    );
    logger_info(logger, buffer);
}

static int recieve_request(request_t *request, void **buffer, int *len, int client){
    int request_buffer_size = BUFFER_SIZE;
    void *request_buffer = malloc(request_buffer_size);
    int request_len = 0;

    while (!request->finished){
        int len = read(client, request_buffer + request_len, request_buffer_size - request_len);
        printf("len: %d\n", len);
        printf("buffer: %s\n", (char *)request_buffer);
        logger_info(logger, "client is working");
        if (len < 0){
            logger_error(logger, "read");
            free(request_buffer);
            return -1;
        }
        
        if (len == 0){
            logger_info(logger, "client breaks");
            break;
        }
        
        request_len += len;
        if (request_len == request_buffer_size){
            request_buffer_size *= 2;
            request_buffer = realloc(request_buffer, request_buffer_size);
        }
        
        if (request_parse(request, request_buffer, request_len) < 0){
            logger_error(logger, "request_parse");
            free(request_buffer);
            return -1;
        }
    }

    *buffer = request_buffer;
    *len = request_len;

    return 0;
}

void handle_client(client_context* context){
    // printf("client_socket: %d\n", context->client_socket);
    logger_info(logger, "client connected");

    int err;
    void *request_buffer = malloc(BUFFER_SIZE);
    int request_len = 0;
    request_t request;
    request_init(&request);

    

    // err = recieve_request(context, &request);
    err = recieve_request(&request, &request_buffer, &request_len, context->client_socket);

    if (err < 0){
        logger_error(logger, "recieve_request");
        free(request_buffer);
        return;
    }

    log_request(&request);



    free(context);
}

void* client_thread(void * args){
    asm volatile(
        "nop"
    );
    handle_client((client_context*)args);
    asm volatile(
        "nop"
    );
    // printf("done\n");
    logger_info(logger, "client disconnected");
    return NULL;
}

// void handle_server(server_context* context){
//     // printf("server_socket: %d\n", context->server_socket);
//     logger_info(logger, "server connected");
//     free(context);
// }

// void* server_thread(void * args){
//     asm volatile(
//         "nop"
//     );
//     handle_server((server_context*)args);
//     asm volatile(
//         "nop"
//     );
//     // printf("done\n");
//     logger_info(logger, "server disconnected");
//     return NULL;
// }

