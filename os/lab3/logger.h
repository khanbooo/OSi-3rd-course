#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define RED "\033[0;31m"
#define RESET "\033[0m"

typedef enum LogLevel{
    INFO = 1,
    WARNING,
    ERROR
} LogLevel;

typedef struct Logger{
    FILE * stream;
    LogLevel logging_level;
} Logger;

Logger* create_logger(FILE * stream, LogLevel level);

void destroy_logger(Logger* logger);

void logger_log(Logger* logger, LogLevel level, const char* message);

void logger_info(Logger* logger, const char* message);

void logger_warning(Logger* logger, const char* message);

void logger_error(Logger* logger, const char* message);

