#include <stdio.h>

#include "utils/args_parser.h"
#include "file/file.h"
#include "utils/crc.h"
#include "patcher/patcher.h"
#include "utils/statuses.h"
#include "gui/gui.h"

/**
 * @brief This array contains console options, their functions and descriptions
 */
const Argument ARGS_DICT[] = {
    {"-h", print_help,           "#   -h - prints help information\n"},

    {"-s", read_scheme_filename, "#   -s - specify scheme file name after this\n"},

    {"-i", read_input_filename,  "#   -i - specify input binary file name after this\n"},

    {"-o", read_output_filename, "#   -o - specify output binary file name after this\n"},

    {"-c", enable_checksum_mode, "#   -c - print input file checksum\n"},

    {"-f", enable_force_mode,    "#   -f - force mode - disable all checks\n"},

    {"-a", read_addr_offset,     "#   -m - set program segment begin addr (default 0x0100)\n"},
};

const int ARGS_DICT_LEN = sizeof(ARGS_DICT) / sizeof(ARGS_DICT[0]);

int main(int argc, char* argv[]) {

    /// Parsing console arguments
    ArgsVars args_vars = {};
    args_vars.scheme_filename  = "scheme.txt";              //< default value
    args_vars.input_filename   = "../original_prog.com";    //< default value
    args_vars.output_filename  = "../cracked_prog.com";     //< default value
    args_vars.addr_offset      = 0x0100;                    //< default value

    STATUS_CHECK_RAISE(args_parse(argc, argv, &args_vars, ARGS_DICT, ARGS_DICT_LEN));
    /// Parsing console arguments end

    Buffer input_buf = {};
    STATUS_CHECK_RAISE(file_open_read_close(args_vars.input_filename, &input_buf.data, 0,
                                                                      &input_buf.size));
    if (args_vars.checksum_mode) {
        printf("File checksum: %u\n", memcrc(input_buf.data, (size_t)input_buf.size));

        input_buf.dtor();
        return Status::NORMAL_WORK;
    }

    Buffer scheme_buf = {};
    STATUS_CHECK_RAISE(file_open_read_close(args_vars.scheme_filename, &scheme_buf.data, 1,
                                                                       &scheme_buf.size),
                                                                            input_buf.dtor());

    STATUS_CHECK_RAISE(patch(scheme_buf, input_buf, args_vars.addr_offset, args_vars.force_mode),
                                                                            input_buf.dtor(),
                                                                            scheme_buf.dtor());

    STATUS_CHECK_RAISE(file_open_write_bytes_close(args_vars.output_filename, input_buf.data,
                                                                      (size_t)input_buf.size),
                                                                            input_buf.dtor(),
                                                                            scheme_buf.dtor());
    input_buf.dtor();
    scheme_buf.dtor();

    if (!start_gtk_window())
        return Status::GTK_ERROR;

    return Status::OK_EXIT;
}
