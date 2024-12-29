#include "logger.h"

Logger* create_logger(FILE * stream, LogLevel level){
    Logger* logger = (Logger*)malloc(sizeof(Logger));
    // logger->stream = fopen(filename, "w");
    logger->stream = stream;
    logger->logging_level = level;
    return logger;
}

void destroy_logger(Logger* logger){
    fclose(logger->stream);
    free(logger);
}

void logger_log(Logger* logger, LogLevel level, const char* message){
    char *color;
    char *level_str;

    if (level >= logger->logging_level){
        switch (level){
            case INFO:
                color = GREEN;
                level_str = "INFO";
                break;
            case WARNING:
                color = YELLOW;
                level_str = "WARNING";
                break;
            case ERROR:
                color = RED;
                level_str = "ERROR";
                break;
        }
    }

    time_t rawtime;
    time(&rawtime);
    struct tm* timeinfo = localtime(&rawtime);
    char time_str[128];
    asctime_r(timeinfo, time_str);
    time_str[strlen(time_str) - 1] = '\0';

    char *reset = RESET;
    if (logger->stream != stdout && logger->stream != stderr) {
        color = "";
        reset = "";
    }

    fprintf(logger->stream, "[%s%s] %s: %s%s\n", color, level_str, time_str, message, reset);
}

void logger_info(Logger* logger, const char* message){
    logger_log(logger, INFO, message);
}

void logger_warning(Logger* logger, const char* message){
    logger_log(logger, WARNING, message);
}

void logger_error(Logger* logger, const char* message){
    logger_log(logger, ERROR, message);
}

