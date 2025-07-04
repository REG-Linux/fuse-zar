
#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef void* za_handle;
typedef void (*za_iter_cb)(const char* name, int is_dir, void* u);

za_handle za_open (const char* path);
void      za_close(za_handle h);

uint8_t za_is_dir (za_handle h, const char* path);
uint8_t za_is_file(za_handle h, const char* path);

uint64_t za_size(za_handle h, const char* path);
uint64_t za_read(za_handle h, const char* path,
                 uint64_t offset, uint64_t length, void* buffer);

void za_list(za_handle h, const char* path,
             za_iter_cb cb, void* user);
#ifdef __cplusplus
}
#endif
