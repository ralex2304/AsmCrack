#include "statuses.h"

Status::Statuses Status::raise(const Statuses status) {
    switch (status) {
        case NORMAL_WORK:
            assert(0 && "Status::raise(): NORMAL_WORK mustn't be raised");
            break;
        case ARGS_ERROR:
            fprintf(stderr, "Exiting. Args error\n");
            break;
        case INP_FILE_ERROR:
            fprintf(stderr, "Exiting. Input file error\n");
            break;
        case OUT_FILE_ERROR:
            fprintf(stderr, "Exiting. Output file error\n");
            break;
        case MEMORY_EXCEED:
            fprintf(stderr, "Exiting. Not enough memory\n");
            break;
        case GTK_ERROR:
            fprintf(stderr, "Exiting. GTK error\n");
            break;
        case OK_EXIT:
        case DEFAULT:
            break;
        default:
            assert(0 && "Error::raise(): wrong error");
            break;
    };
    return status;
}
