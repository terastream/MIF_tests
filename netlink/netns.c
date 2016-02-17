#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <libnetlink.h>
#include <linux/rtnetlink.h>
#include <libnfnetlink/libnfnetlink.h>

struct iplink_req {
	struct nlmsghdr         n;
	struct ifinfomsg        i;
	char                    buf[1024];
};

int main(int argc, char **argv)
{
	unsigned int ifindex;
	int fd;
	struct iplink_req req;
	struct rtnl_handle rth;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <interface> <netnspath>\n", argv[0]);
		return EXIT_FAILURE;
	}

	memset(&req, 0, sizeof(req));

	req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
	req.n.nlmsg_flags = NLM_F_REQUEST;
	req.n.nlmsg_type = RTM_NEWLINK;
	req.i.ifi_family = AF_NETLINK;

	if ((fd = open(argv[2], O_RDONLY)) < 0) {
		perror("open");
		return EXIT_FAILURE;
	}
	addattr_l(&req.n, sizeof(req), IFLA_NET_NS_FD, &fd, 4);

	ifindex = if_nametoindex(argv[1]);
	if (!ifindex) {
		perror("if_nametoindex");
		return EXIT_FAILURE;
	}
	printf ("ifindex=%d\n", ifindex);
	req.i.ifi_index = ifindex;

	if (rtnl_open(&rth, 0) < 0) {
		perror("rtnl_open");
		return EXIT_FAILURE;
	}

	if (rtnl_talk(&rth, &req.n, NULL, 0) < 0)
		exit(EXIT_FAILURE);

	rtnl_close(&rth);
	
	close(fd);

	return EXIT_SUCCESS;
}
