#include "drive.h"

enum {
	FLAG_VERBOSE,
	FLAG_HELP,
	FLAG_VERSION,
	FLAG_LAST,
};

static void driver_error(const char *message) {
	const char *red = "\x1b[31m";
	const char *reset = "\x1b[0m";
	fprintf(stderr, "%serror%s: %s\n", red, reset, message);
}

static void driver_help(Driver *drive, Opt_Info *opts, size_t opts_len, Opt_Usage *usage) {
	opt_info_help(opts, opts_len, NULL, NULL, usage, stdout);
	if (drive->verbose) {
		printf("\nAvailable frontends:\n");
		for (size_t i = 0; i < DRIVER_FRONTS; ++i) printf("  %s (sign: 0x%016lx)\n", drive->fronts[i].info->name, drive->fronts[i].info->sign.quad);

		printf("\nAvailable backends:\n");
		for (size_t i = 0; i < DRIVER_BACKS; ++i) printf("  %s (sign: 0x%016lx)\n", drive->backs[i].info->name, drive->backs[i].info->sign.quad);
	}
}

static void driver_version(Driver *drive) {
	printf("version x.x.x\n");
}

int driver_run(Driver *drive, int argc, const char **argv) {

	const size_t matches_len = 64;
	Opt_Match matches[matches_len];

	Opt_Result result;
	opt_result_init(&result, matches, matches_len);

	Opt_Info opts[FLAG_LAST];
	opt_info_init(&opts[FLAG_VERBOSE], "verbose", "v", "Enable verbose output", OPT_VALUE_NONE, NULL, OPT_INFO_MATCH_FIRST);
	opt_info_init(&opts[FLAG_HELP], "help", "h", "Show help information", OPT_VALUE_NONE, NULL, OPT_INFO_STOP_PARSER);
	opt_info_init(&opts[FLAG_VERSION], "version", "", "Show version information", OPT_VALUE_NONE, NULL, OPT_INFO_STOP_PARSER);

	Opt_Parser parser;
	opt_parser_init(&parser, opts, FLAG_LAST);

	Opt_Error error = opt_parser_run(&parser, &result, argv, argc);
	if (error.kind != OPT_ERROR_NONE && error.kind != OPT_ERROR_STOPPED) {
		driver_error("Failed to parse arguments");
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

	if (result.simple == 0 && error.kind != OPT_ERROR_STOPPED) {
		driver_error("Missing input file");
		opt_info_usage(opts, FLAG_LAST, &usage, stderr);
		return 1;
	}

	for (size_t i = 0; i < result.matches_len; ++i) {
		if (result.matches[i].kind == OPT_MATCH_SIMPLE) continue;

		bool missing = result.matches[i].kind == OPT_MATCH_MISSING;
		size_t opt = missing ? result.matches[i].missing_opt : result.matches[i].option.opt;
		switch (opt) {
			case FLAG_VERBOSE:
				drive->verbose = true;
				break;

			case FLAG_HELP:
				driver_help(drive, opts, FLAG_LAST, &usage);
				return 0;

			case FLAG_VERSION:
				driver_version(drive);
				return 0;

			default:
				break;
		}
	}

	return 0;
}
