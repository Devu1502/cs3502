#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>

volatile sig_atomic_t shutdown_flag = 0;

void handle_sigint(int sig) {
	shutdown_flag = 1;
}

int main(int argc, char *argv[]) {
	struct sigaction sa;

        sa.sa_handler = handle_sigint;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sigaction(SIGINT, &sa, NULL);


	int opt;
	FILE *input = stdin;
	int buffer_size = 4096;

	while((opt = getopt(argc, argv, "f:b:")) != -1) {
		switch (opt) {
			case 'f':
				input = fopen(optarg, "r");
				if (!input) {
					perror("fopen");
					return 1;
				}
				break;
			case 'b':
				buffer_size = atoi(optarg);
				break;
			default:
				fprintf(stderr, "Usage: %s [-f file] [-b size]\n", argv[0]);
				return 1;
		}
	}

	char *buffer = malloc(buffer_size);
	if (!buffer) {
		perror("malloc");
		return 1;
	}

	size_t n;
	while (!shutdown_flag && (n = fread(buffer, 1, buffer_size, input)) > 0) {
		fwrite(buffer, 1, n, stdout);
	}
	free(buffer);
	if (input != stdin) fclose(input);
	return 0;
}
