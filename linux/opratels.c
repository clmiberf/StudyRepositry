#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <stdlib.h>

#define FILEPATH_MAX (80)

// it is file,get file info directly
int display_file(char *path,char *filename,char *opration){
  struct stat stat1;      //stat
  int i;
  struct passwd *pw;      // usr info
  struct group *gp;       // usr group info
  struct tm *tm;          //local time info
  stat(path,&stat1);

//get file type
  // switch (stat1.st_mode & S_IFMT) {
  //   case S_IFREG:
  //     break;
  //
  //   case S_IFDIR:
  //     break;
  //
  //   case S_IFLNK:
  //     break;
  //
  //   case S_IFBLK:
  //     break;
  //
  //   case S_IFCHR:
  //     break;
  //
  //   case S_IFIFO:
  //     break;
  //
  //   case S_IFSOCK:
  //     break;
  // }

  //get file read and write permissions


  pw = getpwuid(stat1.st_uid);
  gp = getgrgid(stat1.st_gid);
  tm = localtime(&stat1.st_ctime);

  if (strcmp(opration,"-l") == 0 ) {
        for (i = 8; i >=0; i--) {
          if(stat1.st_mode & (1 << i))
          {
                switch(i%3)
                {
                  case 2: printf("r"); break;
                  case 1: printf("w"); break;
                  case 0: printf("x"); break;
                }
          } else
                printf("-");
        }

        printf("%2ld %s %s %4ld", stat1.st_nlink, pw->pw_name, gp->gr_name, stat1.st_size);

        printf(" %04d-%02d-%02d %02d:%02d",tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min);

        printf(" %s\n", filename);
  }else if (strcmp(opration,"-a") == 0) {
        printf(" %s\n", filename);
  }else if (strcmp(opration,"-s") == 0) {
        if (filename[0] != '.')
          printf(" %s   %4ld\n", filename,(stat1.st_size/1024));
  }else if (strcmp(opration,"-Q") == 0 ) {
        if (filename[0] != '.')
          printf("\"%s\" \n", filename);
  }else if (strcmp(opration,"-0") == 0) {
        if (filename[0] != '.')
          printf(" %s\n", filename);
  }else if (strcmp(opration,"-1") == 0) {
        if (filename[0] != '.')
            printf(" %s\n", filename);
  }else if (strcmp(opration,"-i") == 0 ){
        if (filename[0] != '.')
            printf("%s   %4ld\n", filename,stat1.st_ino);
  }else{
      printf("no such commond\n");
  }

  return 0;
}

//if it is dir,enter the dir to traverse

int display_dir(char *filename,char *opration){
  DIR *dir;
  struct dirent *dirent;
  struct stat stat2;
  char buffer[1024];

  dir = opendir(filename);

  while ((dirent = readdir(dir)) != NULL) {
    /* code */

    strcpy(buffer,filename);
    strcat(buffer,"/");
    strcat(buffer,dirent->d_name);
    if (stat(buffer,&stat2)) {
      perror("stat");
      return -1;
    }

    display_file(buffer,dirent->d_name,opration);
  }

}





int main(int argc,char **argv){
  struct stat st;
  char buffer[1024];

  //获得当前路径
  char *file_path_getcwd;
  file_path_getcwd=(char *)malloc(FILEPATH_MAX);
  getcwd(file_path_getcwd,FILEPATH_MAX);


  if (argc == 1) {
    argv[1]=file_path_getcwd;
    argv[2]="-0";
    printf("%s\n", argv[1]);
  }else if (argc == 2) {
    if (argv[1][0] == '-') {
      argv[2] = argv[1];
      argv[1] = file_path_getcwd;
    }else{
        argv[2]="-0";
    }

  }


  //printf("%s\n", argv[2]);
  if (stat(argv[1],&st) < 0) {
    perror("stat");
    return -1;
  }

  if ((st.st_mode & S_IFMT) == S_IFDIR) {
    display_dir(argv[1],argv[2]);
  }else{
    display_file(argv[1],argv[1],argv[2]);
  }

  printf("\n");
  return 0;


}
