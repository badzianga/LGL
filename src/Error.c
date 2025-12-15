#include "Error.h"

static const char* GetErrorMessage(ErrCode code) {
    switch (code) {
        case ERR_INVALID_PARAMS: return "Invalid parameter(s)";
        case ERR_OUT_OF_MEMORY:  return "Out of memory";
        case ERR_FILE_NOT_FOUND: return "File not found";
        case ERR_INTERNAL_ERROR: return "Internal error";
        case ERR_UNKNOWN_FORMAT: return "Unknown pixel format";
        default: return "";
    }
}

#ifdef ESP_PLATFORM
void ThrowError(ErrCode code, const char* file, int line) {
    for (;;)
}
#else
#include <stdio.h>
#include <stdlib.h>
void ThrowError(ErrCode code, const char* file, int line) {
    const char* message = GetErrorMessage(code);
    fprintf(stderr, "[ERROR] %s:%d %s\n", file, line, message);
    exit(1);
}
#endif
