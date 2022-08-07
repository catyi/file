#include "client.h"
int main(int argc, char *argv[])
{ 
    int connfd = socket_init(argc,argv[1]);
    if(connfd < 0)
    {
        printf("socket_init is failed--\n");
        return -1;
    }
    interface1();
    m msg1;
    int num;
BB: scanf("%d",&num);
    getchar();
    switch(num)
    {
        case 1:
            login(connfd,&msg1);
            printf("case login--\n");
            break;
        case 2:
            regist(connfd,&msg1);
            break;
        case 3:
            quit(connfd,&msg1);
            return 0;
        default:
            printf("input num again--\n");
            goto BB;
    }
   // printf("please wait--\n");
    sleep(1);
   // printf("new switch\n");
    while(1)
    {
        printf("please input your choose--\n");
        interface2();
        int n;
AA: scanf("%d",&n);
    getchar();
    switch(n)
    {
        case 1:
           // printf("updata\n");
            updata(connfd,&msg1);
            break;
        case 2:
           // printf("download\n");
            download(connfd,&msg1);
            break;
        case 3:
           // printf("quit\n");
            quit(connfd,&msg1);
            return 0;
        case 4:
            //printf("show\n");
            show();
            break;
        default:
            printf("input err,please input agin\n");
            goto AA;

    }

    }
} 
