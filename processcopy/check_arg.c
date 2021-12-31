#include "s.h"

int check_arg(const char *SFile,int arg_num,int pronum)
{
	if(access(SFile,F_OK)!=0){
		perror("check_arg error");
		exit(-1);
	}
	if(arg_num < 3){
		printf("check_arg error:传入参数太少，至少要传入3个参数\n");
		exit(-1);
	}
	if(pronum<=0||pronum>=100){
		printf("check_arg error:进程数量不能少于1并且不能大于100\n");
		exit(-1);
	}
	return 0;
}
