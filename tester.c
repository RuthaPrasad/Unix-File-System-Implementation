#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<limits.h>
#include<string.h>
#include "hashtable.h"

// to test if a C/Python program can write into the FS
// unmount from the file system first , and then run tester.c 
// mount the file system back to see the changes

int main(int argc, char* argv[])
{
	
	#if 1 //to write into a file from 0 to 512
	
	int fd=ht_open("/root/TestFile3");
	char buf[9];
	strcpy(buf,"hellowor");
	int i=0;
	for(i=0;i<64;i++)//write from 0 to 512
	{
		write(fd,&buf,strlen(buf));
	}
	ht_close(fd);
	
	#endif
	
	
	#if 0 //to remove lines 101 and beyond
	
	int fd=ht_open("/root/TestFile3");
	lseek(fd,0,101);
	char buf[9];
	strcpy(buf,"");
	int i=0;
	for(i=0;i<52;i++)//write from 101 to 512
	{
		write(fd,&buf,strlen(buf));
	}
	ht_close(fd);
	
	#endif
	
	
	#if 0 //to write from 101 to 1024
	
	int fd=ht_open("/root/TestFile3");
	lseek(fd,101,SEEK_CUR);
	char buf[8];
	strcpy(buf,"");
	int i=0;
	for(i=0;i<116;i++)//write from 101 to 1024
	{
		write(fd,&buf,strlen(buf));
	}
	ht_close(fd);
	
	#endif
	return 0;
}
