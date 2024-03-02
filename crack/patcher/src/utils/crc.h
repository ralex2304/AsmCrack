#ifndef CRC_H_
#define CRC_H_

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

uint32_t memcrc(const unsigned char* data, size_t size);

inline uint32_t memcrc(const char* data, size_t size) {
    return memcrc((const unsigned char*)data, size);
}

#endif //< #ifndef CRC_H_
