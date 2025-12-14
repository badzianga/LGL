#include "Error.h"

#ifdef ESP_PLATFORM
void ThrowError(ErrCode code, const char* file, int line) {
    for (;;)
}
#else
#include <stdio.h>
#include <stdlib.h>
void ThrowError(ErrCode code, const char* file, int line) {
    const char* message = "";
    switch (code) {
        case ERR_INVALID_PARAMS: {
            message = "Invalid parameter(s)";
        } break;
        case ERR_OUT_OF_MEMORY: {
            message = "Out of memory";
        } break;
        case ERR_FILE_NOT_FOUND: {
            message = "File not found";
        } break;
        case ERR_INTERNAL_ERROR: {
            message = "Internal error";
        }
        default: break;
    }
    fprintf(stderr, "[ERROR] %s:%d - %s\n", file, line, message);
    exit(1);
}
#endif
