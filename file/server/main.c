#include "server.h"
int main(int argc, char *argv[])
{ 
    int sockfd = server_init();
    if(sockfd < 0)
    {
        printf("server_init is failede");
        return -1;
    }
    sqlite_init();

    m  msg;

    struct pollfd pfd[1024];
    for(int i =0;i<1024;i++)
    {
        pfd[i].fd = -1;
        pfd[i].events = -1;
    }
    pfd[0].fd = sockfd;
    pfd[0].events =POLLIN;
    int count =1;
    while(1)
    {
        int ret = poll(pfd,count,10000);
        if(ret <= 0)
        {
            printf("poll error or timeout\n");
            continue;
        }
        for(int i=0;i<count;i++)
        {
            if(pfd[i].revents & POLLIN)
            {
                if(pfd[i].fd == sockfd)
                {
                    int connfd = wait_client(sockfd);
                    if(connfd < 0)
                    {
                        break;
                    }
                    pfd[count].fd = connfd;
                    pfd[count].events = POLLIN;
                    count++;
                }else{
                    int confd = pfd[i].fd;
                    memset(&msg, '\0', sizeof(m));
                    printf("ok");
                    int ret = read(confd,&msg,sizeof(m));
                    if(ret < 0)
                    {
                        printf("read msg failed--\n");
                        close(confd);
                    }else if(ret == 0){
                        close(confd);
                        pfd[i].fd = -1;
                        pfd[i].events = -1;
                    }else{
                        printf("connfd success--\n");
                        switch(msg.type)
                        {
                            case L:
                                printf("login--\n");
                                printf("name %s\n",msg.name);
                                login(confd,&msg);
                                break;
                            case R:
                                printf("regis-\n");
                                regist(confd,&msg);
                                break;
                            case Q:
                                printf("quit\n");
                                quit(confd,&msg);
                                close(confd);
                                pfd[i].fd = -1;
                                pfd[i].events = -1;
                                break;
                            case D:
                                printf("down\n");
                                down(confd,&msg);
                                break;
                            case P:
                                printf("updata\n");
                                updata(confd,&msg);
                                break;
                        }
                    }
                }
            }
        }
    }
    return 0;
} 
