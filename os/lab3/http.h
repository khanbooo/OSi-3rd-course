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

typedef struct response {
    int major_version;
    int minor_version;
    int status_code;
    int finished;

    llhttp_t parser;
    llhttp_settings_t settings;
} response_t;

void request_init(request_t *request);
void response_init(response_t *response);

int request_parse(request_t *request, char *buf, int len);
int response_parse(response_t *response, char *buf, int len);
