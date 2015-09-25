#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
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

#define CMD_NSCREATE	"ip netns add "
#define CMD_NSDELETE	"ip netns delete "
#define CMD_NSLIST	    "ip netns list"
#define CMD_NSIDENTIFY	"ip netns identify "

#define NSDIR	"/var/run/netns/"
#define NS1		"PvD_1"
#define NS2		"PvD_2"

#define SLEEP_TIME  5

int main(int argc, char **argv)
{
    char cmd_ns[50];
    char ns1_fn[50], ns2_fn[50];
	int ns1_fd, ns2_fd, nsw_fd;
    int counter = 0;
    char spid[10];

	printf("My PID is: %d\n\n", getpid());

	/*
	 * Create new namespaces
	 */
	printf("Available namespaces:\n");
    memset(cmd_ns, '\0', sizeof(cmd_ns));
    strcat(cmd_ns, CMD_NSLIST);
    system(cmd_ns);
    printf("\n");

	printf("Creating namespace %s...\n", NS1);
    memset(cmd_ns, '\0', sizeof(cmd_ns));
    strcat(cmd_ns, CMD_NSCREATE);
    strcat(cmd_ns, NS1);
    system(cmd_ns);
    printf("\n");

	printf("Available namespaces:\n");
    memset(cmd_ns, '\0', sizeof(cmd_ns));
    strcat(cmd_ns, CMD_NSLIST);
    system(cmd_ns);
	printf("\n");

	printf("Creating namespace %s...\n", NS2);
    memset(cmd_ns, '\0', sizeof(cmd_ns));
    strcat(cmd_ns, CMD_NSCREATE);
    strcat(cmd_ns, NS2);
    system(cmd_ns);
    printf("\n");

	printf("Available namespaces:\n");
    memset(cmd_ns, '\0', sizeof(cmd_ns));
    strcat(cmd_ns, CMD_NSLIST);
    system(cmd_ns);
	printf("\n");

	/*
	 * Open namespace descriptors
	 */
    memset(ns1_fn, '\0', sizeof(ns1_fn));
    strcat(ns1_fn, NSDIR);
    strcat(ns1_fn, NS1);
    memset(ns2_fn, '\0', sizeof(ns2_fn));
    strcat(ns2_fn, NSDIR);
    strcat(ns2_fn, NS2);

	if ((ns1_fd = open(ns1_fn, O_RDONLY)) < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	}
	if ((ns2_fd = open(ns2_fn, O_RDONLY)) < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	/*
	 * Switch namespace several times
	 */
    memset(cmd_ns, '\0', sizeof(cmd_ns));
    strcat(cmd_ns, CMD_NSIDENTIFY);
    sprintf(spid, "%d", getpid());
    strcat(cmd_ns, spid);

	sleep(SLEEP_TIME);
	while (1) {
        nsw_fd = ns1_fd;
        if (counter++ % 2 != 0) {
            nsw_fd = ns2_fd;
        }
		printf("Changing namespace...\n");
		if (setns(nsw_fd, CLONE_NEWNET) < 0) {
			perror("setns");
			exit(EXIT_FAILURE);
		}
        else {
            printf("Now in ");
        	fflush(stdout);
            system(cmd_ns);
            printf("\n");
        }
    	sleep(SLEEP_TIME);
	}

	printf("\n");

	close(ns1_fd);
	close(ns2_fd);

	return 0;
}

