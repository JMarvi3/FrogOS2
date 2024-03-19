#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
	int fd = open("floppy.img", O_RDONLY);
	unsigned short sum=0;
	char buff[512];
	read(fd, buff, 512);
	for(int sector=1; sector<0x460; ++sector) {
		read(fd, buff, 512);
		unsigned short *words = (unsigned short *)buff;
		for(int i=0; i<128; ++i)
			sum += words[i];
	}
	printf("%x\n", sum);
}
