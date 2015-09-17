This directory contains source which was used to test behavior of a
socket that is opened while in one network namespace and then anohter
name space is selected. What happens in that case with the socket?
Is it still usable?

How to setup test environment
=============================

1. Create new namespace. The source assumes it is called TESTNS:

ip netns add TESTNS

2. Run the application as a root:

sudo ./test

The application will write its PID, change network space and
wait for 120 seconds.

3. In another window, again as root, run the following command:

ip netns identify <PID>

Instead of PID, write the number written by the test application.
It should output NETNS.

4. When waiting time finishes, the application will connect to
IP address 161.53.65.11 port 80 and issue GET request. 
