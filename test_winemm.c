#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/ioctl.h>
#include "winemm.h"

//#define DEBUG
static void  test_allocation(long long len)
{
	int fd = open("/dev/winemm-dev", O_RDWR);
	if (fd <= 0) {
		printf("error, can't open winemm-dev\n");
		return -1;
	}
	//printf("Test allocation: len (0x%llx)\n",len);
	void *vadr = mmap(0, len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (vadr == MAP_FAILED) {
		printf("error(%d),  mmap failure\n", errno);
		return -1;
	}

	//printf("Check memset(0xAA), pid(%d): \n", getpid());
	memset(vadr, 0xAA, len);
	if (*((unsigned char *)vadr) != 0xaa ||
	    *((unsigned char *)vadr + len/2) != 0xaa ||
	    *((unsigned char *)vadr + len-1) != 0xaa) {
		printf("Error, Check memset NG\n");
		return -1;
	}
	munmap(vadr, len);
	close(fd);
}

static int autotest()
{
	int mem_size[] = {70, 80, 100, 250, 300, 172, 200, 271, 300, 400,  0};
	int mem_count[] = {100, 40, 30, 50, 100, 180, 200, 271, 300, 400,  0};

	int i=0,j=0,k=0;
	int len;
	while((len = mem_size[i]) > 0) {
		for(int i=0; i< mem_ount[i]; i++)
			test_allocation(len);
	}
	return 0;
}

int main(int argc, char* argv[])
{
	char unit;
	int factor = 1;
	if (argc == 1)
		return autotest();

	if (strcmp(argv[1], "-h") == 0) {
		printf("Usage: test_winemm  SIZE (k,m): \n");
		printf("        test_winemm 2k\n");
		printf("        test_winemm 200\n");
		return -1;
	}

	printf("test: %s\n", argv[1]);
	unit = argv[1][strlen(argv[1])-1];
	if (unit == 'k' || unit == 'K') {
		argv[1][strlen(argv[1])-1] = '\0';
		factor = 1024;
	} else if (unit == 'M' | unit == 'm') {
		argv[1][strlen(argv[1])-1] = '\0';
		factor = 1024*1024;
	}
	long len = atoll(argv[1]) * factor;
	test_allocation(len);
	//printf("Pause, pid:%d, press any key to exit.\n", getpid());
	//getchar(); //Pause, cat /proc/pid/xx
	return 0;
}
