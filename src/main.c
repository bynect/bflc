#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "opt.h"

enum {
	FLAG_HELP,
	FLAG_VERSION,
	FLAG_VERBOSE,
	FLAG_ASM,
	FLAG_LAST,
};

int main(int argc, const char **argv) {
	const size_t matches_len = 64;
	Opt_Match matches[matches_len];

	Opt_Result result;
	opt_result_init(&result, matches, matches_len);

	Opt_Info opts[FLAG_LAST];
	opt_info_init(&opts[FLAG_HELP], "help", "h", "Show help information", OPT_VALUE_NONE, NULL, OPT_INFO_STOP_PARSER);
	opt_info_init(&opts[FLAG_VERSION], "version", "", "Show version information", OPT_VALUE_NONE, NULL, OPT_INFO_STOP_PARSER);
	opt_info_init(&opts[FLAG_VERBOSE], "verbose", "v", "Enable verbose output", OPT_VALUE_NONE, NULL, OPT_INFO_MATCH_FIRST);
	opt_info_init(&opts[FLAG_ASM], "asm", "S", "Output assembly", OPT_VALUE_NONE, NULL, OPT_INFO_MATCH_FIRST);

	Opt_Parser parser;
	opt_parser_init(&parser, opts, FLAG_LAST);

	Opt_Error error = opt_parser_run(&parser, &result, argv, argc);
	if (error.kind != OPT_ERROR_NONE) {
		return 1;
	}

	opt_result_sort(&result, true);

	const char *args[] = { "file" };
	Opt_Usage usage = {
		.name = result.bin_name,
		.args = args,
		.args_len = 1,
		.line_max = 90,
	};

	if (result.simple == 0) {
		opt_info_usage(opts, FLAG_LAST, &usage, stderr);
		return 1;
	}

	for (size_t i = 0; i < result.matches_len; ++i) {
		if (result.matches[i].kind == OPT_MATCH_OPTION) {
			switch (result.matches[i].option.opt) {
				case FLAG_HELP:
					opt_info_help(opts, FLAG_LAST, NULL, NULL, &usage, stdout);
					return 0;

				case FLAG_VERSION:
					printf("version x.x.x\n");
					return 0;

				default:
					break;
			}
		}
	}

	return 0;
}
