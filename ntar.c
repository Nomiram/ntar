#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#define MAX_BUF_SIZE 225
int writeFileContent(int fd, int fo);//return count of bytes written 
short getFileSize(int file);//2 Bytes
char* getFileName(int file);//30 Bytes
/*
	  file .ntar format
	     		FILE                        FOLDER
bytes  10       1    50   2    SIZE         1    50
	  META|... |TYPE|NAME|SIZE|CONTENT|..  |TYPE|NAME|..
	
*/

int main(int argc, char* argv[])
{
int fin;
int fout;
char* meta[10]={0};
meta = "ntar1.0";
char* fileinname[50]={0};
char* fileoutname[50]={0};
DIR* dir;
struct dirent* CurFile;
//TODO check posix stat()
//dir = opendir("")
if((fin=open("TEXT.TEST", O_RDONLY))==-1) {
	printf("Cannot open file.\n");
	exit(1);
}
//create .ntar file
if((fout=open("test.ntar", O_WRONLY | O_CREAT, 00666))==-1) {
	printf("Cannot open file to write.\n");
	exit(1);
}
printf("file size = %i\n", getFileSize(fin));
writeFileContent(fin,fout);
printf("archive file size = %i\n", getFileSize(fout));
close(fin);
close(fout);
return 0;
}
short getFileSize(int file){
	short size=0;
	int tempCur=0;
	
	tempCur = lseek(file, 0, SEEK_CUR);
	size = lseek(file, 0, SEEK_END);
	lseek(file, tempCur, SEEK_SET);
	return size;
}
int writeFileContent(int fi, int fo){
	char buffer[MAX_BUF_SIZE]={0};
	int countBytes=0;
	int count = read(fi, buffer, MAX_BUF_SIZE);
	if (count == 0)
	{printf ("Possible read error.\n");
	exit(1);
	}
	lseek(fo,0, SEEK_END);
	write(fo,buffer,count);
	printf("str: %s\n",buffer);
}