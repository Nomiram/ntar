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
int readFolder(char* ndir, int fout);
int writeFolder(char* ndir, int fout);
int writeFileContent(int fd, int fo);//return count of bytes written 
short getFileSize(int file);//2 Bytes
int UnpackArchive(char* ndir, char* arch);
int CreateArchive(char* ndir, char* arch);
/*
      file .ntar format (ntar1.0)
                FILE                                   FOLDER
bytes  10        1     1     50   2    SIZE          1     1     50
      META|... |TYPE|RECLVL|NAME|SIZE|CONTENT|...  |TYPE|RECLVL|NAME|...
         2
RECLVL-level of recursion(вложенность)
*/

int main(int argc, char* argv[])
{
struct dirent* CurFile;
if( argc > 1 ){
    if(strcmp(argv[1],"-h")==0){
        printf("ntar - an archiving utility\n\
        usage: ntar c FOLDER ARCHIVE - create archive\n\
               ntar x FOLDER ARCHIVE - extract files from an archive\n");
    }
    else if((strcmp(argv[1],"c")==0)&&(argc > 2))
        CreateArchive(argv[2],argv[3]);
    else if((strcmp(argv[1],"x")==0)&&(argc > 2))
        UnpackArchive(argv[2],argv[3]);
    else printf("ntar -h for get help\n");
}
    else
printf("ntar -h for get help\n");
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
    static char RECLVL=0;
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
    ///WRITE FOLDER
    struct stat folderstat;
    stat(CurDir, &folderstat);
    char TYPE = T_IS_FOLDER;
    char* foldername=strrchr(CurDir,'/');
    if(foldername==NULL){
    char* foldername=strrchr(CurDir,'\\');    
    }
    char writefoldername [NAME_SIZE] = {0};
    memmove(writefoldername, foldername+1, (strlen(foldername+1)<NAME_SIZE?strlen(foldername+1):NAME_SIZE));
    write(fout, &TYPE, sizeof(TYPE));
    write(fout, &RECLVL, sizeof(RECLVL));
    write(fout, writefoldername, NAME_SIZE);
    ///
    while (file = readdir(dir)) {
        struct stat filestat;
        stat(file->d_name, &filestat);
        printf("\t%10s\t%x\t%ld\n",file->d_name, filestat.st_mode, filestat.st_size);
        if(S_ISDIR(filestat.st_mode) && strcmp(file->d_name,".")!=0 && strcmp(file->d_name,"..")!=0){
            //printf("is folder\n");
            RECLVL++;
            writeFolder(file->d_name,fout);
            chdir("..");
            RECLVL--;
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
            write(fout, &RECLVL, sizeof(RECLVL));
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
/////////////////////////////////////////////////////
int UnpackArchive(char* ndir, char* arch){
   
    int fout;
    char META[META_SIZE]={0};//"ntar1.0";
    int fin;
    char* fileinname[NAME_SIZE]={0};
    char* fileoutname[NAME_SIZE]={0};
    struct dirent* CurFile;
    //create .ntar file
    if((fin=open(arch, O_RDONLY))==-1){
    	printf("Cannot open file to write.\n");
    	exit(1);
    }
    ///READ META
    read(fin, META, 10);
    if(strcmp(META,"ntar1.0")==0)
    {printf("ntar1.0\n");
        printf("archive file size = %i\n", getFileSize(fin));
    }else
    {
        printf("file is not archive\n");
    }
    ///
    readFolder(ndir,fin);
close(fin);
}
int readFolder(char* ndir, int fout){
    static char RECLVL=-1;
    char WorkDir [MAX_PATH_SIZE]={0};
    char CurDir [MAX_PATH_SIZE]={0};
    int fin;
    struct dirent *file;
    chdir(ndir);
    getcwd(WorkDir,sizeof(WorkDir));
    printf("%s\n",WorkDir);
    char TYPE = 0;
    while(read(fout, &TYPE, sizeof(TYPE))){
        //printf("DIR\n");
        getcwd(CurDir,sizeof(CurDir));
        //printf("Curdir %s\n",CurDir);
        if(TYPE==T_IS_FOLDER){
        char newRECLVL={0};
        char writefoldername [NAME_SIZE] = {0};
        read(fout, &newRECLVL, sizeof(RECLVL));   
        //printf("REC %d %d \n",newRECLVL, RECLVL);
       while(newRECLVL<RECLVL){
            chdir("..");
            //printf("DIRCHANGED\n");
            RECLVL--;
            getcwd(CurDir,sizeof(CurDir));
            ///printf("%s\n",CurDir);
        }
        read(fout, writefoldername, NAME_SIZE);
        printf("%s\n", writefoldername);
        if(mkdir(writefoldername,(0777))!=0){
            printf("err create dir %s\n",strerror(errno));
        }
        sleep(1);
        RECLVL++;
            strcat(CurDir,"/");
            strcat(CurDir,writefoldername);
            //printf("CD %s\n",CurDir);
			chdir(CurDir);
        //printf("chdir %d\n",);
        continue;
        }
        if(TYPE==T_IS_FILE){
            //printf("FILE\n");
            char newRECLVL={0};
            char writefilename [NAME_SIZE] = {0};
            char writefilesize [2] = {0};
        read(fout, &newRECLVL, sizeof(RECLVL));  
        //printf("REC %d %d \n",newRECLVL, RECLVL);
        while(newRECLVL<RECLVL){
            chdir("..");
            //printf("DIRCHANGED\n");
            RECLVL--;
            getcwd(CurDir,sizeof(CurDir));
            ///printf("%s\n",CurDir);
        }
        
            read(fout, writefilename, NAME_SIZE);
        printf("file %s\n",writefilename);
        strcat(CurDir,"/");
        strcat(CurDir,writefilename);
        //printf("CD %s\n",CurDir);
        if((fin = creat(CurDir,00666))==-1) {
	    printf("Cannot open file to write. %s\n",strerror(errno));
        exit(1);
        }
        //todo 
        read(fout,writefilesize , sizeof(short));
            //printf("sizef %s\n",writefilesize);
        short size = 0;
           memmove(&(size),writefilesize , sizeof(short));
        //todo while
        printf("size %d\n",size);
        char buffer[MAX_BUF_SIZE] = {0};
    //int size = 0;
    //lseek(fo, 0, SEEK_END);
	int count = size;
	int cnt = size;
    while(count){
    cnt = read(fout, buffer, MAX_BUF_SIZE<count?MAX_BUF_SIZE:count);
	if (cnt == 0)
	{
        printf ("Possible read error.\n");
	    exit(1);
	}
    //printf("%s",buffer);
    count-=cnt;    
	write(fin, buffer, cnt);
	//printf("%s\n", buffer);
    }
        }
    }
	close(fin);
}
