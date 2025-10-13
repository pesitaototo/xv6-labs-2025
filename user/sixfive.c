
#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define NULL 0
// allow up to MAX_DIGIT, this choice is arbitrary
#define MAX_DIGITS 64

// returns 1 if num is all numeric characters (0-9)
// 0 otherwise
int
is_numeric(char *num) {
	for (int i=0; num[i] != '\0'; i++) {
		if (num[i] < '0' || num[i] > '9') { return 0; }
	}

	return 1;
}

void
process_num(char *num) {
	// is_numeric(char*) checks that num is a string of only numeric char before calling atoi())
	// if (is_numeric(num)) {
		int num_val = atoi(num);
		if (num_val % 6 == 0 || num_val % 5 == 0) {
			fprintf(1, "%d\n", num_val);
		}
	// }
}

// adds buf to num if current_digit < MAX_DIGIT
// void append_num(char *num, char buf, int current_digit) {
// 	if (current_digit < MAX_DIGITS-1) {
// 		num[current_digit] = buf;
// 		num[current_digit+1] = '\0';
// 		current_digit++;
// 	} else {
// 		fprintf(2, "Number (%s) is too long: count= %d\n", num, current_digit);
// 		exit(1);
// 	}
// }

void
sixfive(int fd) {
	// read 1 character at a time 
	char buf;
	char num[MAX_DIGITS];
	// track the current place of the digit for the number we're reading
	int current_digit = 0;
	// skip when num overflows
	int skipping = 0;

	char *separator_chars = " -\r\t\n./";

	// read input file a character at a time
	while (read(fd, &buf, 1) > 0) {
		// use strchr to test for separator vs number. pch == NULL if not a separator character
		char *pch = strchr(separator_chars, buf);

		// we have a separator character
		if (pch) {
			if (!skipping && current_digit > 0) {
				process_num(num);
			}

			skipping = 0;
			num[0] = '\0';
			current_digit = 0;
			continue;
		}

		if (skipping)
			continue;

		if (current_digit < MAX_DIGITS-1) {
			num[current_digit++] = buf;
			num[current_digit] = '\0';
		} else {
			skipping = 1;
		}

	}

	if (current_digit > 0) {
		process_num(num);
	}
}

int
main(int argc, char *argv[]) {

	if (argc < 2) {
		fprintf(2, "Usage: sixfive <input-files>...\n");
		exit(1);
	}

	for (int i=1; i<argc; i++) {
		int fd = open(argv[i], O_RDONLY);
		sixfive(fd);
		close(fd);
	}

	exit(0);
}