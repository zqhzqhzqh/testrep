#include"s.h"

int block(const char *File,int pronum){
	int blocksize;
	int fd = open(File,O_RDONLY);
	blocksize = lseek(fd,0,SEEK_END);
	if(blocksize%pronum == 0)
		return blocksize/pronum;
	else
		return blocksize/pronum+1;
}
