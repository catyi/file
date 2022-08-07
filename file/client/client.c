/*===============================================
 *   文件名称：client.c
 *   创 建 者：    songjuan 
 *   创建日期：2022年07月22日
 *   描    述：
 ================================================*/
#include "client.h"
//初始化
int socket_init(int argv,char *a)
{
    if(argv < 2 )
    {
        printf("agv[] [ip] too fewer ---\n");
        return -1;
    }

    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0)
    {
        perror("socket");
        return -1;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9551);
    addr.sin_addr.s_addr = inet_addr(a);
    if(connect(sockfd,(struct sockaddr* )&addr,sizeof(addr)) < 0)
    {
        perror("connect");
        close(sockfd);
        return -1;
    }
    printf("suceess--\n");
    return sockfd;
}
//登录界面1
void interface1()
{
    printf("###############################\n");
    printf("#   1.login 2.regist  3.quit  #\n");
    printf("#   please choose by number   #\n");
    printf("###############################\n");
}
//使用界面1
void interface2()
{
    printf("###############################\n");
    printf("#          1. upfile          #\n");
    printf("#          2. downfile        #\n");
    printf("#          3. quit            #\n");
    printf("#          4. show            #\n");
    printf("#   please choose by number   #\n");
    printf("###############################\n");

}
//登录
void login(int connfd,mp msg1)
{
    printf("wlecom to login\n");
    msg1->type = L;
    printf("please input your name:");
    scanf("%s",msg1->name);
    getchar();
   // printf("%s",msg1->name);
    printf("please input your password:");
    scanf("%s",msg1->password);
   // printf("%s\n",msg1->password);
   // printf("%ld\n",sizeof(m));
    write(connfd,msg1,sizeof(m));
    erm ermsg;
    read(connfd,&ermsg,sizeof(erm));
    printf("er1:%s\n",ermsg.test);
    if(ermsg.er_flag == -1)
    {
        login(connfd,msg1);
    }else
    {
    read(connfd,msg1,sizeof(m));
    }

}

//注册
void regist(int connfd,mp msg1)
{
    printf("wlecome to regsit\n");
    msg1->type = R;
    msg1->flag = -1;
    printf("please input your name:");
    scanf("%s",msg1->name);
    getchar();
   // printf("%s",msg1->name);
    printf("please input your password:");
    scanf("%s",msg1->password);
    getchar();
   // printf("%s\n",msg1->password);
   // printf("%ld\n",sizeof(m));
    printf("please choose to be a members ? ( y/n )\n");
    char ch;
    scanf("%c",&ch);
    if( ch =='y' )
    {
        printf("yes\n");
        msg1->flag = 1;
    }
  //  printf("flag %d",msg1->flag);
    write(connfd,msg1,sizeof(m));

    erm ermsg1;
    read(connfd,&ermsg1,sizeof(erm));
    printf("%s\n",ermsg1.test);
    if(ermsg1.er_flag == -1)
    {
        regist(connfd,msg1);
    }else{

    login(connfd,msg1);
    }
}
//退出
void quit(int connfd,mp msg1)
{
    printf("welcome to quit\n");
    msg1->type = Q;
    write(connfd,msg1,sizeof(m));
    erm ermsg2;
    read(connfd,&ermsg2,sizeof(erm));
   // printf("%s\n",ermsg2.test);
    printf("Welcome to you next time!\n");
    close(connfd);
}
//下载
int download(int connfd,mp msg1)
{
    printf("welcome to download\n");
    msg1->type = D;
    write(connfd,msg1,sizeof(m));
    printf("show server file\n");
    char buf[30]={0};
	//接收并打印服务器的文件
	while(1)
    {
        read(connfd,buf,sizeof(buf));
        printf("%s\n",buf);
        if(strcmp(buf,"files are all") == 0)
        {
            memset(buf,0,sizeof(buf));
            break;
        }
        if(strcmp(buf,"file failed") == 0)
        {
            printf("%s\n",buf);
            return -1;
        }
        memset(buf,0,sizeof(buf));
    }
    memset(buf,0,sizeof(buf));
  //  printf("file ok\n");
  //进入用户选择项
    printf("userbame:  %s **",msg1->name);
    printf("please choose the file which you want to down \n");
    char buf1[30]={0};
    fgets(buf,sizeof(buf),stdin);
    buf[strlen(buf)-1] = '\0';
    printf("please input you want to name--\n");
    scanf("%s",buf1);
    getchar();
    write(connfd,buf,strlen(buf));
    int fp = open(buf1,O_RDWR | O_CREAT,0664);
    if(fp == -1)
    {
        perror("open");
        return -1;
    }
    memset(buf,0,sizeof(buf));
    read(connfd,buf,sizeof(buf));
    int len = atoi(buf);
    int l = (30 +len -1)/30;
    int num =0;
  //  printf("%d",msg1->flag);
  //VIP 和普通会员的时间区别
    if(msg1->flag == 1)
    {
        int time = l *50;
        printf("vip user maybe need time :%d us\n",time);
    }else{
        int time = l *500;
        printf("ordinary user maybe need time :%d us\n",time);
    }
    // printf("len = %d\n",len);
    // printf("need time %d\n",l);
	
	//下载开始
    while(1)
    {
        int ret = read(sockfd,buf,sizeof(buf));
        if(ret < 0)
        {
            perror("read");
            return -1;
        }
        int ret1 = write(fp,buf,ret);
        if(ret1 == -1)
        {
            perror("write");
            return -1;
        }
        len = len - ret;
		//进度条，通过服务器发过来的长度 结合每次接收的大小
		//靠接受次数和总次数的商  作为真实的进度条
        if(num != l)
        {
            int m = (num*100)/l;
            printf("loading----------%d%%\r",m);
            fflush(stdout);
            num++;
            usleep(500);
        }else{
            printf("download complate---\n");
            fflush(stdout);
            break;
        }
    }
    close(fp);
    return 0;
}
//打印自己的目录
int show()
{
    DIR *d = opendir(".");
    if(d == NULL)
    {
        perror("opendir");
        return -1;
    }
    struct dirent *dirf = NULL;
    while(1)
    {
        dirf = readdir(d);
        if(dirf == NULL)
            break;
        if(dirf->d_name[0] != '.')
        {

            struct stat file;
            if(-1 == lstat(dirf->d_name,&file))
            {
                perror("lstat");
                return -1;
            }
            sleep(1);
            printf("%-8s %ld\n",dirf->d_name,file.st_size);
        }
    }
    closedir(d);
}
//上传
int updata(int connfd,mp msg1)
{
    msg1->type = P;
    write(connfd,msg1,sizeof(m));
    char buf1[30]={0};
   // printf("begin to up\n");
    printf("username:%s **",msg1->name);
    printf("choose what do you want to up\n");
    scanf("%s",buf1);
    getchar();
    write(connfd,buf1,sizeof(buf1));
   // printf("%s\n",buf1);
    FILE *fp = fopen(buf1,"rb");
    if(fp == NULL)
    {
        perror("fopen");
        return -1;
    }
	//得到文件的大小 算出需要发送次数
    fseek(fp,0,SEEK_END);
    int len = ftell(fp);    	
    fseek(fp,0,SEEK_SET);
    printf("%d",len);
    memset(buf1,0,sizeof(buf1));
    sprintf(buf1,"%d",len);
    write(connfd,buf1,strlen(buf1));
	//向上取整
    int l = (30 +len -1)/30;
    int num =0;
   // printf("%d",msg1->flag);
   //告诉用户需要花费的时间
    if(msg1->flag == 1)
    {
        int time = l *50;
        printf("vip user maybe need time :%d us\n",time);
    }else{
        int time = l *500;
        printf("ordinary user maybe need time :%d us\n",time);
    }
	//上传开始
    while(1)
    {
        memset(buf1,0,sizeof(buf1));
        int ret = fread(buf1,sizeof(char),sizeof(buf1),fp);
        printf("%d",msg1->flag);
		//VIP 和 普通用户的使用区别
        if(msg1->flag == 1)
        {
            usleep(50);	
        }else{
            usleep(500);
        }
        int ret1 = write(connfd,buf1,ret);
        if(ret1 < 0)
        {
            perror("write");
            return -1;
        }
		//进度条的处理 和下载一致
        if(num != l)
        {
            int m = (num*100)/l;
            printf("updata1----------%d%%\r",m);
            fflush(stdout);
            num++;
            usleep(500);
        }else{
            printf("updata file over----\n");
            fflush(stdout);
            break;
        }
    }
   // printf("updata--over-\n");
    fclose(fp);
    return 0;
}

