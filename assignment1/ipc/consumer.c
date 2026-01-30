#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>

volatile sig_atomic_t shutdown_flag = 0;
volatile sig_atomic_t stats_flag = 0;

void handle_sigint(int sig) {
	shutdown_flag = 1;
}

void handle_sigusr1(int sig) {
	stats_flag = 1;
}

int main(int argc, char *argv[]) {
	struct sigaction sa_int, sa_usr1;

	sa_int.sa_handler = handle_sigint;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;
	sigaction(SIGINT, &sa_int, NULL);

	sa_usr1.sa_handler = handle_sigusr1;
	sigemptyset(&sa_usr1.sa_mask);
	sa_usr1.sa_flags = 0;
	sigaction(SIGUSR1, &sa_usr1, NULL);

	int opt;
	int max_lines = -1;
	int verbose = 0;

	while ((opt = getopt(argc, argv, "n:v")) != -1) {
		switch(opt) {
			case 'n':
				max_lines = atoi(optarg);
				break;
			case 'v':
				verbose = 1;
				break;
			default:
				fprintf(stderr, "Usage: %s [-n max] [-v]\n", argv[0]);
				return 1;
		}
	}

	char line[1024];
	int lines = 0;
	int chars = 0;

	while (!shutdown_flag && fgets(line, sizeof(line), stdin)) {
		lines++;
		chars += strlen(line);

		if (stats_flag) {
			fprintf(stderr, "[SIGUSR1] Lines: %d, Characters: %d\n", lines, chars);
			stats_flag = 0;
		}

		if (verbose) {
			fprintf(stdout, "%s", line);
		}


		if (max_lines > 0 && lines >= max_lines) {
			break;
		}
	}

	fprintf(stderr, "Lines: %d\n", lines);
	fprintf(stderr, "Characters: %d\n", chars);

	return 0;
}
