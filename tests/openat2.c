#define _GNU_SOURCE
#include <sys/syscall.h>  /* SYS_openat2, */
#include <linux/openat2.h> /* struct open_how, RESOLVE_*, */
#include <unistd.h>       /* syscall(2), close(2), */
#include <fcntl.h>        /* O_RDONLY, O_PATH, O_DIRECTORY, */
#include <stdio.h>        /* printf(3), puts(3), */
#include <stdlib.h>       /* exit(3), EXIT_*, */
#include <string.h>       /* memset(3), */

int main()
{
	struct open_how how;
	int dirfd;
	int fd;
	int exit_status = EXIT_SUCCESS;

	/* Test 1: openat2 with RESOLVE_IN_ROOT resolving ".."
	 * With RESOLVE_IN_ROOT, ".." should be confined to the
	 * dirfd root, exercising non-trivial path resolution. */
	printf("%-45s : ", "openat2 RESOLVE_IN_ROOT with ..");
	fflush(stdout);

	dirfd = open(".", O_PATH | O_DIRECTORY);
	if (dirfd < 0) {
		puts("skip (can't open cwd)");
		return 125;
	}

	memset(&how, 0, sizeof(how));
	how.flags = O_RDONLY | O_DIRECTORY;
	how.resolve = RESOLVE_IN_ROOT;

	fd = syscall(SYS_openat2, dirfd, "..", &how, sizeof(how));
	if (fd >= 0) {
		close(fd);
		puts("OK");
	} else {
		perror("FAILED");
		exit_status = EXIT_FAILURE;
	}

	close(dirfd);

	/* Test 2: openat2 with RESOLVE_BENEATH on ".".
	 * RESOLVE_BENEATH prevents escaping the dirfd via "..". */
	printf("%-45s : ", "openat2 RESOLVE_BENEATH with .");
	fflush(stdout);

	dirfd = open(".", O_PATH | O_DIRECTORY);
	if (dirfd < 0) {
		puts("skip (can't open cwd)");
		return 125;
	}

	memset(&how, 0, sizeof(how));
	how.flags = O_RDONLY | O_DIRECTORY;
	how.resolve = RESOLVE_BENEATH;

	fd = syscall(SYS_openat2, dirfd, ".", &how, sizeof(how));
	if (fd >= 0) {
		close(fd);
		puts("OK");
	} else {
		perror("FAILED");
		exit_status = EXIT_FAILURE;
	}

	close(dirfd);

	/* Test 3: openat2 without resolve flags. */
	printf("%-45s : ", "openat2 without resolve flags");
	fflush(stdout);

	memset(&how, 0, sizeof(how));
	how.flags = O_RDONLY | O_DIRECTORY;

	fd = syscall(SYS_openat2, AT_FDCWD, ".", &how, sizeof(how));
	if (fd >= 0) {
		close(fd);
		puts("OK");
	} else {
		perror("FAILED");
		exit_status = EXIT_FAILURE;
	}

	exit(exit_status);
}
