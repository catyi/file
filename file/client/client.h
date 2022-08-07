#ifndef _CLIENT_H
#define _CLIENT_H
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <math.h>

#define L 'L'
#define R 'R'
#define Q 'Q'
#define P 'P'
#define D 'D'
typedef struct msg{
    char type;
    char name[10];
    char password[10];
    int flag;
}m,*mp;

typedef struct er{
    char test[20];
    int er_flag;
}erm,*em;

int sockfd;
int socket_init(int argv,char *a);
void interface1();
void interface2();
void login(int connfd,mp msg1);
void regist(int connfd,mp msg1);
void quit(int connfd,mp msg1);
int download(int connfd,mp msg1);
int show();
int updata(int connfd,mp msg1);



#endif