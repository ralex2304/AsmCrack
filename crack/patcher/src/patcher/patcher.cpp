#include "patcher.h"

static Status::Statuses read_and_check_checksum_(const Buffer scheme, Buffer program, size_t* pos,
                                                 const bool force_mode);

#define SKIP_SPACES_NO_ERROR_()                                                         \
            while ((long)pos < scheme.size) {                                           \
                if (isspace(scheme.data[pos])) {                                        \
                    pos++;                                                              \
                    continue;                                                           \
                }                                                                       \
                                                                                        \
                if (scheme.data[pos] == ';') {                                          \
                    while ((long)pos < scheme.size && scheme.data[pos] != '\n') pos++;  \
                    continue;                                                           \
                }                                                                       \
                break;                                                                  \
            }


Status::Statuses patch(const Buffer scheme, Buffer program, const size_t addr_offset,
                       const bool force_mode) {
    assert(scheme.data);
    assert(scheme.size >= 0);
    assert(program.data);
    assert(program.size >= 0);

    size_t pos = 0;

    STATUS_CHECK(read_and_check_checksum_(scheme, program, &pos, force_mode));

    SKIP_SPACES_NO_ERROR_();

    while ((long)pos < scheme.size) {

        size_t addr = 0;
        char old_byte = 0;
        char new_byte = 0;
        int readed_n = 0;

        if (sscanf(scheme.data + pos, "%zx: %hhx -> %hhx%n", &addr, &old_byte,
                                                             &new_byte, &readed_n) != 3) {
            printf("Scheme file syntax error\n");
            return Status::INP_FILE_ERROR;
        }
        pos += readed_n;

        if ((long)addr >= program.size || addr < addr_offset) {
            printf("Error. Address 0x%.4zx is out of range (allowed 0x%.4lx - 0x%.4lx)\n",
                                     addr, addr_offset,  scheme.size - 1 + addr_offset);
            return Status::INP_FILE_ERROR;
        }
        addr -= addr_offset;

        if (!force_mode && old_byte != program.data[addr]) {
            printf("Expected 0x%.2x instead of 0x%.2x at 0x%.4zx\n",
                             old_byte, program.data[addr], addr + addr_offset);
            return Status::INP_FILE_ERROR;
        }

        program.data[addr] = new_byte;

        SKIP_SPACES_NO_ERROR_();
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses read_and_check_checksum_(const Buffer scheme, Buffer program, size_t* pos,
                                                 const bool force_mode) {
    assert(scheme.data);
    assert(scheme.size >= 0);
    assert(program.data);
    assert(program.size >= 0);
    assert(pos);

    uint32_t expected_checksum = 0;
    int readed = 0;

    if (sscanf(scheme.data, "%u%n", &expected_checksum, &readed) <= 0) {
        printf("Error in scheme file. Checksum not found\n");
        return Status::INP_FILE_ERROR;
    }
    *pos += readed;

    if (force_mode)
        return Status::NORMAL_WORK;

    uint32_t real_checksum = memcrc(program.data, program.size);
    if (real_checksum != expected_checksum) {
        printf("Checksum error. Expected %u instead of %u\n", expected_checksum, real_checksum);
        return Status::INP_FILE_ERROR;
    }

    return Status::NORMAL_WORK;
}
