#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[]) {

	if (argc < 2) {
		// fprintf(2,...)-- writes to stderr using file descriptor 2
		fprintf(2, "Usage: sleep <num of ticks>\n");
		exit(1);
	}

	// we should parse argv[1] to make sure it is an integer value
	pause(atoi(argv[1]));
	exit(0);

}