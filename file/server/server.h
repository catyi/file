#ifndef _SERVER_H
#define _SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <poll.h>
#include "sqlite3.h"
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>

#define L 'L'
#define R 'R'
#define Q 'Q'
#define P 'P'
#define D 'D'

static sqlite3 *db = NULL;
typedef struct ms{
    char type;
    char name[10];
    char password[10];
    int flag;
}m,*mp;

typedef struct er{
    char test[20];
    int er_flag ;
}erm,*em;

int server_init();
int wait_client(int listen);
int sqlite_init();
int login(int fd,mp msg);
int regist(int connfd,mp msg);
int quit(int connfd,mp msg1);
int down(int connfd,mp msg1);
int updata(int connfd,mp msg1);


#endif
