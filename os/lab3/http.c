#include "http.h"

static int on_method_complete(llhttp_t *parser) {
    request_t *request = parser->data;
    request->method = llhttp_get_method(parser);
    logger_info(logger, "on_method_complete");
    return 0;
}

static int on_version_complete_req(llhttp_t *parser) {
    
    request_t *request = parser->data;
    request->major_version = llhttp_get_http_major(parser);
    request->minor_version = llhttp_get_http_minor(parser);
    logger_info(logger, "on_version_complete_req");
    return 0;
}

static int on_url(llhttp_t *parser, const char *at, size_t length) {
    request_t *request = parser->data;

    if (request->url) {
        request->url = realloc(request->url, strlen(request->url) + length + 1);
        strncat(request->url, at, length);
    }
    else{
        request->url = strndup(at, length);
    }
    logger_info(logger, "on_url");
    return 0;
}

static int on_message_complete_req(llhttp_t *parser) {
    request_t *request = parser->data;
    request->finished = 1;
    logger_info(logger, "on_message_complete_req");
    return 0;
}

void request_init(request_t *request) {
    memset(request, 0, sizeof(request_t));
    llhttp_settings_init(&request->settings);
    request->settings.on_url = on_url;
    request->settings.on_method_complete = on_method_complete;
    request->settings.on_version_complete = on_version_complete_req;
    request->settings.on_message_complete = on_message_complete_req;


    llhttp_init(&request->parser, HTTP_REQUEST, &request->settings);
    request->parser.data = request;
}

int request_parse(request_t *request, char *buf, int len) {
    llhttp_errno_t err = llhttp_execute(&request->parser, buf, len);
    if (err != HPE_OK) {
        logger_error(logger, "llhttp_execute");
        return -1;
    }
    return 0;
}

// Response

static int on_status_complete(llhttp_t *parser) {
    response_t *response = parser->data;
    response->status_code = llhttp_get_status_code(parser);
    logger_info(logger, "on_status_complete");
    return 0;
}

static int on_version_complete_res(llhttp_t *parser) {
    response_t *response = parser->data;
    response->major_version = llhttp_get_http_major(parser);
    response->minor_version = llhttp_get_http_minor(parser);
    logger_info(logger, "on_version_complete_res");
    return 0;
}   

static int on_message_complete_res(llhttp_t *parser) {
    response_t *response = parser->data;
    response->finished = 1;
    logger_info(logger, "on_message_complete_res");
    return 0;
}

void response_init(request_t *request) {
    memset(request, 0, sizeof(request_t));
    llhttp_settings_init(&request->settings);
    request->settings.on_status_complete = on_status_complete;
    request->settings.on_version_complete = on_version_complete_res;
    request->settings.on_message_complete = on_message_complete_res;

    llhttp_init(&request->parser, HTTP_RESPONSE, &request->settings);
    request->parser.data = request;
}

int response_parse(request_t *request, char *buf, int len) {
    llhttp_errno_t err = llhttp_execute(&request->parser, buf, len);
    if (err != HPE_OK) {
        logger_error(logger, "llhttp_execute");
        return -1;
    }
    return 0;
}

