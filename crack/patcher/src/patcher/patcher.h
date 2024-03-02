#ifndef PATCH_H_
#define PATCH_H_

#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "../utils/macros.h"
#include "../utils/statuses.h"
#include "../utils/crc.h"

struct Buffer {
    char* data = nullptr;
    long size  = -1;

    bool ctor(long buf_size) {
        assert(buf_size > 0);
        assert(data == nullptr);

        data = (char*)calloc(buf_size, sizeof(char));

        if (data == nullptr)
            return false;

        size = buf_size;
        return true;
    };

    void dtor() {
        assert(data);

        FREE(data);
        size = -1;
    };
};

Status::Statuses patch(const Buffer scheme, Buffer program, const size_t addr_offset,
                       const bool force_mode);

#endif //< #ifndef PATCH_H_
