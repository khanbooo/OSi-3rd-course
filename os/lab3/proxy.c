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

static void log_response(response_t *response) {
    char buffer[256];
    snprintf(
        buffer, 
        sizeof(buffer), 
        "response: HTTP/%d.%d %d",
        response->major_version,
        response->minor_version,
        response->status_code
    );
    logger_info(logger, buffer);
}


static void get_host_from_url(char *url, char *host){
    char *start = strstr(url, "://");
    if (start){
        start += 3;
    }
    else{
        start = url;
    }

    char *end = strchr(start, '/');
    if (end){
        strncpy(host, start, end - start);
        host[end - start] = '\0';
    }
    else{
        strcpy(host, start);
    }
}

static int connect_by_url(char *url){
    char host[256];
    get_host_from_url(url, host);

    struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
        .ai_protocol = IPPROTO_TCP,
    };
    struct addrinfo *result;
    int err;

    err = getaddrinfo(host, "http", &hints, &result);
    if (err){
        logger_error(logger, "getaddrinfo");
        return -1;
    }

    int remote = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (remote < 0){
        logger_error(logger, "socket");
        freeaddrinfo(result);
        return -1;
    }

    err = connect(remote, result->ai_addr, result->ai_addrlen);

    if (err){
        logger_error(logger, "connect");
        close(remote);
        freeaddrinfo(result);
        return -1;
    }

    freeaddrinfo(result);
    return remote;
}

void handle_client(client_context* context){
    logger_info(logger, "client connected");

    void *request_buffer = malloc(BUFFER_SIZE);
    request_t request;
    request_init(&request);
    
    int received = read(context->client_socket, request_buffer, BUFFER_SIZE);

    request_parse(&request, request_buffer, received);

    logger_info(logger, "recieved part of request");

    if (received < 0){
        logger_error(logger, "read");
        free(request_buffer);
        close(context->client_socket);
        return;
    }

    int remote = connect_by_url(request.url);
    
    if (remote < 0){
        logger_error(logger, "connect_by_url");
        free(request_buffer);
        close(context->client_socket);
        return;
    }

    logger_info(logger, "connected to remote");
    
    int sent = 0;

    while (received > sent || !request.finished){
        int len = write(remote, request_buffer + sent, received - sent);
        if (sent < 0){
            logger_error(logger, "write");
            free(request_buffer);
            close(remote);
            close(context->client_socket);
            return;
        }
        sent += len;

        if (!request.finished){
            int lenr = read(context->client_socket, request_buffer + received, BUFFER_SIZE - received);
            if (lenr < 0){
                logger_error(logger, "read");
                free(request_buffer);
                close(remote);
                close(context->client_socket);
                return;
            }
            received += lenr;
        }
        if (request_parse(&request, request_buffer, received) < 0){
            logger_error(logger, "request_parse");
            free(request_buffer);
            close(context->client_socket);
            return;
        }
    }

    free(request_buffer);

    sent = 0;
    received = 0;
    
    int response_buffer_size = BUFFER_SIZE;
    void *response_buffer = malloc(BUFFER_SIZE);
    response_t response;
    response_init(&response);

    while (sent < received || !response.finished || (sent == 0 && received == 0)){
        int len = read(remote, response_buffer + received, response_buffer_size - received);
        if (len < 0){
            logger_error(logger, "read");
            close(remote);
            close(context->client_socket);
            break;
        }

        received += len;

        int lenw = write(context->client_socket, response_buffer + sent, received - sent);
        if (lenw < 0){
            logger_error(logger, "write");
            close(remote);
            close(context->client_socket);
            break;
        }
        sent += lenw;

        if (response_parse(&response, response_buffer + received - len, len) < 0){
            logger_error(logger, "response_parse");
            close(remote);
            close(context->client_socket);
            continue;
        }
        if (received == sent && received == response_buffer_size){
            received = 0;
            sent = 0;
        }
    }

    logger_info(logger, "sent response to client");
    free(response_buffer);
    close(remote);
    close(context->client_socket);
}

void* client_thread(void * args){
    handle_client((client_context*)args);
    logger_info(logger, "client disconnected");
    return NULL;
}
