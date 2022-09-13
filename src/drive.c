#include <strings.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include "drive.h"
#include "opt.h"

enum {
	DRIVE_OPT_DEBUG,
	DRIVE_OPT_VERBOSE,
	DRIVE_OPT_HELP,
	DRIVE_OPT_VERSION,
	DRIVE_OPT_FRONT,
	DRIVE_OPT_BACK,
	DRIVE_OPT_OUT,
	DRIVE_OPT_READ_SYSCALL,
	DRIVE_OPT_WRITE_SYSCALL,
	DRIVE_OPT_SYSCALL,
	DRIVE_OPT_CELLN,
	DRIVE_OPT_LAST,
};

static void driver_help(Driver *drive, Opt_Info *opts, size_t opts_len, Opt_Usage *usage) {
	opt_info_help(opts, opts_len, NULL, NULL, usage, stdout);

	if (drive->verbose) {
		size_t line_pad = 46; // TODO: Improve here

		printf("\nAvailable frontends:\n");
		for (size_t i = 0; i < DRIVE_FRONT_LAST; ++i) {
			size_t line_curr = printf("  %-12s (sign: 0x%016lx)", drive->fronts[i].names[0], drive->fronts[i].info->sign.quad);
			for (size_t i = line_curr; i < line_pad; ++i) putchar(' ');
			printf("%s\n", drive->fronts[i].desc);
		}

		printf("\nAvailable backends:\n");
		for (size_t i = 0; i < DRIVE_BACK_LAST; ++i) {
			size_t line_curr = printf("  %-12s (sign: 0x%016lx)", drive->backs[i].names[0], drive->backs[i].info->sign.quad);
			for (size_t i = line_curr; i < line_pad; ++i) putchar(' ');
			printf("%s\n", drive->backs[i].desc);
		}
	}
}

static void driver_version(Driver *drive) {
	printf("bflc %s\n", VERSION);
}

static void driver_print_opt(FILE *stream, size_t opt, Opt_Info *opts, size_t opts_len) {
	assert(opt < opts_len);
	if (opts[opt].long_len != 0) {
		fprintf(stream, "--%s", opts[opt].long_name);
		if (opts[opt].short_len != 0) fprintf(stream, ", ");
	}

	if (opts[opt].short_len != 0) fprintf(stream, "-%s", opts[opt].short_name);
}

int driver_run(Driver *drive, int argc, const char **argv) {
	const size_t matches_len = 64;
	Opt_Match matches[matches_len];

	// TODO: Handle in a better way
	if (argc > matches_len) {
		driver_error("Too many arguments");
		return 1;
	}

	Opt_Result result;
	opt_result_init(&result, matches, matches_len);

	Opt_Info opts[DRIVE_OPT_LAST];
	opt_info_init(&opts[DRIVE_OPT_DEBUG], "debug", "", "Enable debug output", OPT_VALUE_NONE, NULL, OPT_INFO_MATCH_FIRST);
	opt_info_init(&opts[DRIVE_OPT_VERBOSE], "verbose", "v", "Enable verbose output", OPT_VALUE_NONE, NULL, OPT_INFO_MATCH_FIRST);
	opt_info_init(&opts[DRIVE_OPT_HELP], "help", "h", "Show help information", OPT_VALUE_NONE, NULL, OPT_INFO_STOP_PARSER);
	opt_info_init(&opts[DRIVE_OPT_VERSION], "version", "", "Show version information", OPT_VALUE_NONE, NULL, OPT_INFO_STOP_PARSER);

	opt_info_init(&opts[DRIVE_OPT_FRONT], "frontend", "", "Set frontend", OPT_VALUE_STRING, NULL, OPT_INFO_MATCH_MISSING | OPT_INFO_MATCH_LAST);
	opt_info_init(&opts[DRIVE_OPT_BACK], "backend", "", "Set backend", OPT_VALUE_STRING, NULL, OPT_INFO_MATCH_MISSING | OPT_INFO_MATCH_LAST);
	opt_info_init(&opts[DRIVE_OPT_OUT], "", "o", "Set output file name", OPT_VALUE_STRING, "FILE", OPT_INFO_MATCH_MISSING | OPT_INFO_MATCH_LAST);

	opt_info_init(&opts[DRIVE_OPT_READ_SYSCALL], "", "fread", "Use read syscall", OPT_VALUE_NONE, NULL, OPT_INFO_MATCH_FIRST);
	opt_info_init(&opts[DRIVE_OPT_WRITE_SYSCALL], "", "fwrite", "Use write syscall", OPT_VALUE_NONE, NULL, OPT_INFO_MATCH_FIRST);
	opt_info_init(&opts[DRIVE_OPT_SYSCALL], "", "fsyscall", "Use syscalls", OPT_VALUE_NONE, NULL, OPT_INFO_MATCH_FIRST);
	opt_info_init(&opts[DRIVE_OPT_CELLN], "", "fcell", "Set cell number", OPT_VALUE_INT, NULL, OPT_INFO_STOP_DUPLICATE);

	Opt_Parser parser;
	opt_parser_init(&parser, opts, DRIVE_OPT_LAST);

	const char *value_kinds[] = {
		"",
		"string",
		"int",
		"float"
		"bool",
	};

	const char *args[] = { "file" };
	Opt_Usage usage = {
		.name = argv[0],
		.args = args,
		.args_len = 1,
		.line_max = 90,
	};

	Opt_Error error = opt_parser_run(&parser, &result, argv, argc);
	switch (error.kind) {
		case OPT_ERROR_NONE:
		case OPT_ERROR_STOPPED:
			break;

		case OPT_ERROR_UNKNOWN_OPTION:
			driver_error("Unknown option '%s'\n", error.unknown_opt);
			opt_info_usage(opts, DRIVE_OPT_LAST, &usage, stderr);
			return 1;

		case OPT_ERROR_DUPLICATE_OPTION:
			driver_error("Invalid duplicate option '");
			driver_print_opt(stderr, error.duplicate.opt, opts, DRIVE_OPT_LAST);
			fprintf(stderr, "'\n");
			return 1;

		case OPT_ERROR_MISSING_VALUE:
			driver_error("Missing required %s value for option '", value_kinds[error.missing.expected_value]);
			driver_print_opt(stderr, error.duplicate.opt, opts, DRIVE_OPT_LAST);
			fprintf(stderr, "'\n");
			return 1;

		case OPT_ERROR_INVALID_VALUE:
			driver_error("Invalid value '%s', %s was expected\n", error.invalid.base, value_kinds[error.invalid.expected_value]);
			return 1;

		default:
			driver_error("Failed to parse arguments\n");
			return 1;
	}

	opt_result_sort(&result, true);

	size_t back = 0;
	size_t front = 0;

	const char *in_path = NULL;
	const char *out_path = NULL;

	size_t match = 0;
	while (match < result.matches_len) {
		Opt_Match *matchi = &result.matches[match++];

		if (matchi->kind == OPT_MATCH_SIMPLE) {
			// TODO: Improve simple match handling, also handle multiple files (?)
			in_path = matchi->simple;
			if (drive->debug) printf("Set in_path to '%s'\n", in_path);
			continue;
		}

		bool missing = matchi->kind == OPT_MATCH_MISSING;
		size_t opt = missing ? matchi->missing_opt : matchi->option.opt;

		switch (opt) {
			case DRIVE_OPT_DEBUG:
				drive->debug = true;
				drive->verbose = true;
				break;

			case DRIVE_OPT_VERBOSE:
				drive->verbose = true;
				break;

			case DRIVE_OPT_HELP:
				driver_help(drive, opts, DRIVE_OPT_LAST, &usage);
				return 0;

			case DRIVE_OPT_VERSION:
				driver_version(drive);
				return 0;

			case DRIVE_OPT_FRONT:
				if (!missing) {
					assert(matchi->option.value.kind == OPT_VALUE_STRING);
					if (drive->verbose) printf("Searching '%s' frontend\n", matchi->option.value.vstring);

					bool found = false;
					for (size_t i = 0; i < DRIVE_FRONT_LAST && !found; ++i) {
						size_t name = 0;
						while (drive->fronts[i].names[name] != NULL) {
							if (!strcasecmp(matchi->option.value.vstring, drive->fronts[i].names[name])) {
								if (drive->debug) printf("Set frontend as %zu\n", i);
								front = i;
								found = true;
								break;
							}
							++name;
						}
					}

					if (found) break;
					driver_error("Unknown frontend '%s'\n", matchi->option.value.vstring);
					if (drive->verbose) {
						fprintf(stderr, "Available frontends: ");
						for (size_t i = 0; i < DRIVE_FRONT_LAST - 1; ++i) fprintf(stderr, "%s, ", drive->fronts[i].names[0]);
						fprintf(stderr, "%s\n", drive->fronts[DRIVE_FRONT_LAST - 1].names[0]);
					}
					return 1;
				}
				break;

			case DRIVE_OPT_BACK:
				if (!missing) {
					assert(matchi->option.value.kind == OPT_VALUE_STRING);
					if (drive->verbose) printf("Searching '%s' backend\n", matchi->option.value.vstring);

					bool found = false;
					for (size_t i = 0; i < DRIVE_BACK_LAST && !found; ++i) {
						size_t name = 0;
						while (drive->backs[i].names[name] != NULL) {
							if (!strcasecmp(matchi->option.value.vstring, drive->backs[i].names[name])) {
								if (drive->debug) printf("Set backend as %zu\n", i);
								back = i;
								found = true;
								break;
							}
							++name;
						}
					}

					if (found) break;
					driver_error("Unknown backend '%s'\n", matchi->option.value.vstring);
					if (drive->verbose) {
						fprintf(stderr, "Available backends: ");
						for (size_t i = 0; i < DRIVE_BACK_LAST - 1; ++i) fprintf(stderr, "%s, ", drive->backs[i].names[0]);
						fprintf(stderr, "%s\n", drive->backs[DRIVE_BACK_LAST - 1].names[0]);
					}
					return 1;
				}
				break;

			case DRIVE_OPT_OUT:
				if (!missing) {
					assert(matchi->option.value.kind == OPT_VALUE_STRING);
					out_path = matchi->option.value.vstring;
					if (drive->debug) printf("Set out_path as '%s'\n", out_path);
				}
				break;

			case DRIVE_OPT_READ_SYSCALL:
				assert(!missing);
				drive->backs[back].flag_f(drive->backs[back].aux, DRIVE_FLAG_READ_SYSCALL, matchi->option.value);
				if (drive->debug) printf("Use read syscall\n");
				break;

			case DRIVE_OPT_WRITE_SYSCALL:
				assert(!missing);
				drive->backs[back].flag_f(drive->backs[back].aux, DRIVE_FLAG_WRITE_SYSCALL, matchi->option.value);
				if (drive->debug) printf("Use write syscall\n");
				break;

			case DRIVE_OPT_SYSCALL:
				assert(!missing);
				drive->backs[back].flag_f(drive->backs[back].aux, DRIVE_FLAG_READ_SYSCALL, matchi->option.value);
				drive->backs[back].flag_f(drive->backs[back].aux, DRIVE_FLAG_WRITE_SYSCALL, matchi->option.value);
				if (drive->debug) printf("Use read and write syscalls\n");
				break;

			case DRIVE_OPT_CELLN:
				assert(!missing);
				assert(matchi->option.value.kind == OPT_VALUE_INT);
				drive->cell_n = matchi->option.value.vint;
				if (drive->debug) printf("Set cell_n as '%zu'\n", drive->cell_n);
				break;

			default:
				break;
		}
	}

	if (result.simple == 0 || in_path == NULL) {
		driver_error("No input file specified\n");
		opt_info_usage(opts, DRIVE_OPT_LAST, &usage, stderr);
		return 1;
	}

	if (out_path == NULL) {
		// TODO: Calculate a name based on the input file, eg test.bf -> test.asm/test.o
		out_path = "output";
		if (drive->debug) printf("No out_path specified, falling back to '%s'\n", out_path);
	}

	// TODO: Check absolute path
	if (!strcmp(out_path, in_path)) {
		driver_error("Output and input file are the same '%s'\n", in_path);
		return 1;
	}

	FILE *in_file = fopen(in_path, "rb");
	if (in_file == NULL) {
		driver_error("Failed to open input file '%s'\n", in_path);
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
	fclose(in_file);

	FILE *out_file = fopen(out_path, "wb");
	if (out_file == NULL) {
		driver_error("Failed to open output file '%s'\n", out_path);
		return 1;
	}

	Out_Channel out_chan;
	out_init_file(&out_chan, out_file);

	drive->backs[back].info->emit_f(&out_chan, &entry, drive->backs[back].aux);
	fclose(out_file);

	return 0;
}

void driver_error(const char *fmt, ...) {
	const char *red = "\x1b[31m";
	const char *reset = "\x1b[0m";
	fprintf(stderr, "%serror%s: ", red, reset);

	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
}
