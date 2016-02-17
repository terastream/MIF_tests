#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

// man page says that _GNU_SOURCE has to be defined in order to be
// able to use setns() system call. But, by inspecting the source
// it turns out __USE_GNU has to be defined instead.
#define __USE_GNU
#include <sched.h>

#define PORT1		80
#define HOST1		0xA135410B

#define NSFILE		"/var/run/netns/TESTNS"

#define GET_REQUEST	"GET / HTTP/1.0\r\n\r\n"

#define SLEEP_TIME	1

int main(int argc, char **argv)
{
	int ns1_s;
	struct sockaddr_in ns1_addr;

	int ns_fd, read_ret;
	char buffer[128];

	printf("My PID is: %d\n", getpid());

	/*
	 * Change namespace
	 */
	if ((ns_fd = open(NSFILE, O_RDONLY)) < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	if (setns(ns_fd, CLONE_NEWNET) < 0) {
		perror("setns");
		exit(EXIT_FAILURE);
	}

	/*
	 * Create a socket
	 */
	if ((ns1_s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	/*
	 * Wait for some time
	 */
	printf("Changed to a new namespace. Sleeping for %d seconds...", SLEEP_TIME);
	fflush(stdout);
	sleep(SLEEP_TIME);
	printf("\n");

	/*
	 * Connect to a remote host
	 */
	ns1_addr.sin_family = AF_INET;
	ns1_addr.sin_port = htons(PORT1);
	ns1_addr.sin_addr.s_addr = htonl(HOST1);

	if (connect(ns1_s, (struct sockaddr *)&ns1_addr, sizeof(struct sockaddr_in)) < 0) {
		perror("connect");
		exit(EXIT_FAILURE);
	}

	/*
	 * Try to communicate
	 */

	// Send GET request
	if (write(ns1_s, GET_REQUEST, sizeof(GET_REQUEST)) < 0) {
		perror("write");
		exit(EXIT_FAILURE);
	}

	// Read response
	while ((read_ret = read(ns1_s, buffer, sizeof(buffer) - 1)) > 0) {
		buffer[read_ret] = 0;
		printf("%s", buffer);
	}

	if (read_ret < 0) {
		perror("read");
		exit(EXIT_FAILURE);
	}

	close(ns_fd);
	close(ns1_s);

	return 0;
}
