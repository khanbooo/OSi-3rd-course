#ifndef PROXY_HTTP_H
#define PROXY_HTTP_H

#include <llhttp.h>
#include <string.h>
#include <stdlib.h>

#include "logger.h"

extern Logger* logger;

typedef struct request {
    int major_version;
    int minor_version;
    int method;
    int finished;
    char *url;

    llhttp_t parser;
    llhttp_settings_t settings;
} request_t;

void request_init(request_t *request);

int request_parse(request_t *request, char *buf, int len);

// char *http_host_from_url(char *url);

#endif /* PROXY_HTTP_H */