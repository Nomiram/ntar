#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/io.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define MAX_BUF_SIZE 255
#define MAX_PATH_SIZE 255
#define NAME_SIZE 50
#define META_SIZE 10
#define T_IS_FILE 1
#define T_IS_FOLDER 2
int writeFolder(char* ndir, int fout);
int writeFileContent(int fd, int fo);//return count of bytes written 
short getFileSize(int file);//2 Bytes
int CreateArchive(char* ndir, char* arch);
/*
      file .ntar format (ntar1.0)
                FILE                         FOLDER
bytes  10        1    50   2    SIZE          1    50
      META|... |TYPE|NAME|SIZE|CONTENT|...  |TYPE|NAME|...
*/

int main(int argc, char* argv[])
{
int fout;
char WorkDir [MAX_PATH_SIZE]={0};
char CurDir [MAX_PATH_SIZE]={0};
int fin;
struct dirent *file;
DIR* dir;
char* fileinname[NAME_SIZE]={0};
char* fileoutname[NAME_SIZE]={0};
char meta[META_SIZE]="ntar1.0";

struct dirent* CurFile;
if( argc > 1 ){
CreateArchive(argv[1],"re.ntar");
}

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
int CreateArchive(char* ndir, char* arch){
   
    int fout;
    char META[META_SIZE]="ntar1.0";
    
    char* fileinname[NAME_SIZE]={0};
    char* fileoutname[NAME_SIZE]={0};
    struct dirent* CurFile;
    //create .ntar file

    if((fout = creat(arch,00666))==-1) {
    	printf("Cannot open file to write.\n");
    	exit(1);
    }
    ///WRITE META
    write(fout, META, META_SIZE);
    ///
    writeFolder(ndir,fout);
    printf("archive file size = %i\n", getFileSize(fout));
close(fout);
}
int writeFolder(char* ndir, int fout){
    char WorkDir [MAX_PATH_SIZE]={0};
    char CurDir [MAX_PATH_SIZE]={0};
    int fin;
    struct dirent *file;
    getcwd(WorkDir,sizeof(WorkDir));
    chdir(ndir);
    getcwd(CurDir,sizeof(CurDir));
    DIR *dir = opendir(CurDir);
    if (dir == NULL)
        return 1;
    //printf("%s",dir);
    while (file = readdir(dir)) {
        struct stat filestat;
        stat(file->d_name, &filestat);
        printf("\t%10s\t%x\t%ld\n",file->d_name, filestat.st_mode, filestat.st_size);
        if(S_ISDIR(filestat.st_mode)&&strcmp(file->d_name,".")&&strcmp(file->d_name,"..")){
        printf("is folder\n");
            writeFolder(file->d_name,fout);
            chdir("..");
            continue;
        }
        if(S_ISREG(filestat.st_mode)){
        if((fin=open(file->d_name, O_RDONLY))!=-1) {
            printf("file size = %i\n", getFileSize(fin));
            ///WRITE FILE
            char TYPE = T_IS_FILE;
            char writefilename [NAME_SIZE] = {0};
            char *tempfilename;
            char writefilesize [2] = {0};
            short tempsize=getFileSize(fin);
            memmove(writefilename, file->d_name, (strlen(file->d_name)<NAME_SIZE?strlen(file->d_name):NAME_SIZE));
            memmove(writefilesize,&(tempsize) , sizeof(short));
            lseek(fout, 0, SEEK_END);
            write(fout, &TYPE, sizeof(TYPE));
            write(fout, writefilename, NAME_SIZE);
            write(fout,writefilesize , sizeof(short));
            writeFileContent(fin,fout);
            ///
        }else{printf("Cannot open file. %s\n",file->d_name);}
        }
    }
    closedir(dir);
    close(fin);
}
int writeFile(int fi,int fo){
    char filename [NAME_SIZE] = {0};
    
}
int writeFileContent(int fi, int fo){

	char buffer[MAX_BUF_SIZE] = {0};
    int size = 0;
    lseek(fo, 0, SEEK_END);
	int count = 0;
    while(count = read(fi, buffer, MAX_BUF_SIZE)){
	if (count == 0)
	{
        printf ("Possible read error.\n");
	    exit(1);
	}
	write(fo, buffer, count);
	//printf("%s\n", buffer);
    }
    return size;
}