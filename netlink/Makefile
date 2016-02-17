netns: netns.c
	gcc `pkg-config --cflags --libs libnl-3.0` -o netns netns.c -lnetlink

clean:
	rm -f netns
