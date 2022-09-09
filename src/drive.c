#include <strings.h>
#include <string.h>
#include <assert.h>

#include "drive.h"

enum {
	FLAG_VERBOSE,
	FLAG_HELP,
	FLAG_VERSION,
	FLAG_FRONT,
	FLAG_BACK,
	FLAG_OUT,
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

	if (argc > matches_len) {
		driver_error("Too many arguments");
		return 1;
	}

	Opt_Result result;
	opt_result_init(&result, matches, matches_len);

	Opt_Info opts[FLAG_LAST];
	opt_info_init(&opts[FLAG_VERBOSE], "verbose", "v", "Enable verbose output", OPT_VALUE_NONE, NULL, OPT_INFO_MATCH_FIRST);
	opt_info_init(&opts[FLAG_HELP], "help", "h", "Show help information", OPT_VALUE_NONE, NULL, OPT_INFO_STOP_PARSER);
	opt_info_init(&opts[FLAG_VERSION], "version", "", "Show version information", OPT_VALUE_NONE, NULL, OPT_INFO_STOP_PARSER);
	opt_info_init(&opts[FLAG_FRONT], "frontend", "", "Set frontend", OPT_VALUE_STRING, NULL, OPT_INFO_MATCH_MISSING | OPT_INFO_MATCH_LAST);
	opt_info_init(&opts[FLAG_BACK], "backend", "", "Set backend", OPT_VALUE_STRING, NULL, OPT_INFO_MATCH_MISSING | OPT_INFO_MATCH_LAST);
	opt_info_init(&opts[FLAG_OUT], "", "o", "Set output file name", OPT_VALUE_STRING, NULL, OPT_INFO_MATCH_MISSING | OPT_INFO_MATCH_LAST);

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

	size_t back = 0;
	size_t front = 0;

	const char *in_path = NULL;
	const char *out_path = NULL;

	size_t match = 0;
	while (match++ < result.matches_len) {
		Opt_Match *matchi = &result.matches[match];

		if (matchi->kind == OPT_MATCH_SIMPLE) {
			// TODO: Improve simple match handling, also handle multiple files (?)
			in_path = matchi->simple;
		}

		bool missing = matchi->kind == OPT_MATCH_MISSING;
		size_t opt = missing ? matchi->missing_opt : matchi->option.opt;

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

			case FLAG_FRONT:
				if (!missing) {
					for (size_t i = 0; i < DRIVER_FRONTS; ++i) {
						assert(matchi->option.value.kind == OPT_VALUE_STRING);
						if (strcasecmp(matchi->option.value.vstring, drive->fronts[i].info->name)) {
							front = i;
							break;
						}
					}

					driver_error("Unknown frontend");
					return 1;
				}
				break;

			case FLAG_BACK:
				if (!missing) {
					for (size_t i = 0; i < DRIVER_BACKS; ++i) {
						assert(matchi->option.value.kind == OPT_VALUE_STRING);
						if (strcasecmp(matchi->option.value.vstring, drive->backs[i].info->name)) {
							back = i;
							break;
						}
					}

					driver_error("Unknown backend");
					return 1;
				}
				break;

			case FLAG_OUT:
				if (!missing) {
					assert(matchi->option.value.kind == OPT_VALUE_STRING);
					out_path = matchi->option.value.vstring;
				}
				break;

			default:
				break;
		}
	}

	assert(result.simple != 0 && in_path != NULL);
	if (out_path == NULL) {
		// TODO: Calculate a name based on the input file, eg test.bf -> test.asm/test.o
		out_path = "output";
	}

	// TODO: Check absolute path
	if (!strcmp(out_path, in_path)) {
		driver_error("Output file is the same as input file");
		return 1;
	}

	FILE *in_file = fopen(in_path, "rb");
	if (in_file == NULL) {
		driver_error("Failed to open input file");
		return 1;
	}

	In_Channel in_chan;
	in_init_file(&in_chan, in_file);

	const size_t instrs_len = 1024;
	Bfir_Instr instrs[instrs_len];

	Bfir_Pool pool;
	bfir_pool_init(&pool, instrs, instrs_len, NULL);

	Bfir_Entry entry;
	bfir_entry_init(&entry, "main", &pool);

	drive->fronts[front].info->parse_f(&in_chan, &entry, drive->fronts[front].aux);

	FILE *out_file = fopen(out_path, "wb");
	if (out_file == NULL) {
		driver_error("Failed to open output file");
		return 1;
	}

	Out_Channel out_chan;
	out_init_file(&out_chan, in_file);

	drive->backs[front].info->emit_f(&out_chan, &entry, drive->backs[back].aux);

	fclose(in_file);
	fclose(out_file);

	return 0;
}
