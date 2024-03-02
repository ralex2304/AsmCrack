#include "args_parser.h"

Status::Statuses args_parse(int argc, char* argv[], ArgsVars* args_vars,
                            const Argument args_dict[], const int args_dict_len) {
    assert(argv);
    assert(argc);
    assert(args_vars);

    bool exit = false;
    for (int i = 1; i < argc; i++) {
        bool is_found = false;

        for (int j = 0; j < args_dict_len; j++) {
            if (strcmp(argv[i], args_dict[j].arg) == 0) {
                switch (args_dict[j].func(args_dict, args_dict_len, &i, argc, argv, args_vars)) {
                    case ArgsMode::CONTINUE:
                        break;
                    case ArgsMode::EXIT:
                        exit = true;
                        break;
                    case ArgsMode::ERROR:
                        return Status::ARGS_ERROR;
                    default:
                        assert(0 && "args[j].func() returned wrong ArgsMode");
                        break;
                }
                is_found = true;
                break;
            }
        }
        if (is_found)
            continue;

        printf("Wrong console argument: %s\n"
               "Use -h for help\n", argv[i]);
        return Status::ARGS_ERROR;
    }
    if (exit)
        return Status::OK_EXIT;

    return Status::NORMAL_WORK;
}

ArgsMode print_help(const Argument args_dict[], const int args_dict_len,
                    int* arg_i, const int argc, char* argv[], ArgsVars* args_vars) {
    assert(args_dict);
    assert(arg_i);
    assert(argc);
    assert(argv);
    assert(args_vars);

    printf("# Asm programs patcher\n"
           "# Console args:\n");

    for (int i = 0; i < args_dict_len; i++)
        printf("%s", args_dict[i].description);

    printf("# Scheme file format:\n"
           "#     <file checksum>\n"
           "#     <addr in hex>: <old byte in hex> -> <new byte in hex>\n"
           "# Example:\n"
           "#     1263453430\n"
           "#     01b2: a0 -> b4\n");

    printf("# End of help\n");
    return ArgsMode::EXIT;
}

ArgsMode read_input_filename(const Argument args_dict[], const int args_dict_len,
                             int* arg_i, int argc, char* argv[], ArgsVars* args_vars) {
    (void) args_dict_len;

    assert(args_dict);
    assert(arg_i);
    assert(argv);
    assert(args_vars);

    if (++(*arg_i) >= argc) {
        fprintf(stderr, "No input file name found\n");
        return ArgsMode::ERROR;
    }

    args_vars->input_filename = argv[*arg_i];
    return ArgsMode::CONTINUE;
}

ArgsMode read_output_filename(const Argument args_dict[], const int args_dict_len,
                              int* arg_i, int argc, char* argv[], ArgsVars* args_vars) {
    (void) args_dict_len;

    assert(args_dict);
    assert(arg_i);
    assert(argv);
    assert(args_vars);

    if (++(*arg_i) >= argc) {
        fprintf(stderr, "No output file name found\n");
        return ArgsMode::ERROR;
    }

    args_vars->output_filename = argv[*arg_i];
    return ArgsMode::CONTINUE;
}

ArgsMode read_scheme_filename(const Argument args_dict[], const int args_dict_len,
                              int* arg_i, int argc, char* argv[], ArgsVars* args_vars) {
    (void) args_dict_len;

    assert(args_dict);
    assert(arg_i);
    assert(argv);
    assert(args_vars);

    if (++(*arg_i) >= argc) {
        fprintf(stderr, "No output file name found\n");
        return ArgsMode::ERROR;
    }

    args_vars->scheme_filename = argv[*arg_i];
    return ArgsMode::CONTINUE;
}

ArgsMode enable_checksum_mode(const Argument args_dict[], const int args_dict_len,
                              int* arg_i, int argc, char* argv[], ArgsVars* args_vars) {
    (void) args_dict_len;

    assert(args_dict);
    assert(arg_i);
    assert(argc);
    assert(argv);
    assert(args_vars);

    args_vars->checksum_mode = true;

    return ArgsMode::CONTINUE;
}

ArgsMode enable_force_mode(const Argument args_dict[], const int args_dict_len,
                           int* arg_i, int argc, char* argv[], ArgsVars* args_vars) {
    (void) args_dict_len;

    assert(args_dict);
    assert(arg_i);
    assert(argc);
    assert(argv);
    assert(args_vars);

    args_vars->force_mode = true;

    return ArgsMode::CONTINUE;
}

ArgsMode read_addr_offset(const Argument args_dict[], const int args_dict_len,
                          int* arg_i, int argc, char* argv[], ArgsVars* args_vars) {
    (void) args_dict_len;

    assert(args_dict);
    assert(arg_i);
    assert(argv);
    assert(args_vars);

    if (++(*arg_i) >= argc) {
        fprintf(stderr, "No program segment begin addr\n");
        return ArgsMode::ERROR;
    }

    int n_readed = 0;
    if (sscanf(argv[*arg_i], "%zx%n", &args_vars->addr_offset, &n_readed) <= 0 ||
        n_readed != (int)strlen(argv[*arg_i])) {

        fprintf(stderr, "Invalid program segment begin addr given\n");
        return ArgsMode::ERROR;
    }


    return ArgsMode::CONTINUE;
}
