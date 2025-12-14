#ifndef LGL_ERROR_H
#define LGL_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef enum ErrCode {
    ERR_NONE           = 0,
    ERR_INVALID_PARAMS = 1,
    ERR_OUT_OF_MEMORY  = 2,
    ERR_FILE_NOT_FOUND = 3,
    ERR_INTERNAL_ERROR = 4,
} ErrCode;

void ThrowError(ErrCode code, const char* file, int line);

#define THROW_ERROR(code) ThrowError(code, __FILE__, __LINE__)

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // LGL_ERROR_H
