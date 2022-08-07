#include "server.h"
//��ʼ��������
int server_init()
{
    int sockfd =socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0){
        perror("socket");
        return -1;
    }
    struct sockaddr_in addr;
    addr.sin_family =AF_INET;
    addr.sin_port = htons(9551);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sockfd,(struct sockaddr* )&addr,sizeof(addr)) < 0 )
    {
        perror("bind");
        return -1;
    }
    if(listen(sockfd,5) < 0)
    {
        perror("listen");
        return -1;
    }
    printf("listen success --- \n");
    return sockfd;
}
//�ȴ�����
int wait_client(int listen)
{
    struct sockaddr_in caddr;
    socklen_t len = sizeof(caddr);
    int connfd = accept(listen,(struct sockaddr*)&caddr,&len);
    if(connfd < 0)
    {
        perror("accept");
        return -1;
    }
    return connfd;
}
//��ʼ�����ݿ�
int sqlite_init()
{
    if(sqlite3_open("./my.db",&db) < 0)
    {
        printf("sqlite_open is failed\n");
        return -1;
    }
    char sql[] ="create table if not exists user(name char,password char,flag int);";
    if(sqlite3_exec(db,sql,NULL,NULL,NULL) < 0)
    {
        printf("create table failed--\n");
        return -1;
    }
    printf("sqlite init success--\n");
    return 0;
}
//��¼
int login(int fd,mp msg)
{
    printf("ligin in---\n");
    erm ermsg;
    char **result;
    char sql[50]={0};
    sprintf(sql,"select * from user where name = '%s';",msg->name);
    puts(sql);
    int B,C;
	//��ѯ�������Ƿ����
    if(sqlite3_get_table(db,sql,&result,&B,&C,NULL) != SQLITE_OK)
    {
        printf("get_table is failed\n");
        return -1;
    }
	
	//���� result ���������� ��Ϊ��
    if(B*C == 0)
    {
        strcpy(ermsg.test,"user not exists");
        ermsg.er_flag = -1;
        write(fd,&ermsg,sizeof(erm));
        printf("user not exists\n");
        return 0;
    }
    printf("get\n");
    if(strcmp(result[4],msg->password) != 0)
    {
        strcpy(ermsg.test,"password not true");
        ermsg.er_flag = -1;
        write(fd,&ermsg,sizeof(erm));
        printf("password not correct-\n");
        return -1;
    }
    printf("password ---\n");
    msg->flag = atoi(result[5]);
    printf("flag %d",msg->flag);
    strcpy(ermsg.test,"login success--\n");
    ermsg.er_flag = 1;
    write(fd,&ermsg,sizeof(erm));
    printf("login success--\n");
    write(fd,msg,sizeof(m));
    return 0;

}
//ע��
int regist(int connfd,mp msg)
{
    printf("regist in---\n");
    erm ermsg1;
    char **result;
    char sql[50]={0};
    sprintf(sql,"select * from user where name = '%s';",msg->name);
    puts(sql);
    int B,C;
    if(sqlite3_get_table(db,sql,&result,&B,&C,NULL) != SQLITE_OK)
    {
        printf("get_table is failed\n");
        return -1;
    }
    if(B*C != 0)
    {
        strcpy(ermsg1.test,"user is exists");
        ermsg1.er_flag = -1;
        write(connfd,&ermsg1,sizeof(erm));
        printf("user is exists\n");
        return 0;
    }
    char sql1[70]={0};
    sprintf(sql1,"insert into user values( '%s','%s',%d);",msg->name,msg->password,msg->flag);
    puts(sql1);
    if(sqlite3_exec(db,sql1,NULL,NULL,NULL) != 0)
    {
        printf("add is failed\n");
        return -1;
    }
    strcpy(ermsg1.test,"regist success--\n");
    ermsg1.er_flag = 1;
    write(connfd,&ermsg1,sizeof(erm));
    printf("regist  succsess --server\n");
    return 0;
}
//�˳�
int quit(int connfd,mp msg1)
{
    printf("wlecome to quit--\n");
    erm ermsg2;
    strcpy(ermsg2.test,"quit success--\n");
    write(connfd,&ermsg2,sizeof(erm));
    return 0;
}
//����
int down(int connfd,mp msg1)
{
    printf("welcome to download---\n");
    erm ermsg3;
	//�鿴��������Դ
    DIR *dir = opendir(".");
    if(dir == NULL)
    {
        perror("opendir");
        return -1;
    }
    struct dirent *dirf = NULL;
    char buf[300] ={0};
    while(1)
    {
        dirf =readdir(dir);
        if(dirf == NULL)
        {
            break;
        }
        if(dirf->d_name[0] != '.')
        {
            struct stat file;
            if(lstat(dirf->d_name,&file) < 0)
            {
                perror("lstat");
                return -1;
            }
            sleep(1);
            sprintf(buf,"%-8s %ld\n",dirf->d_name,file.st_size);
            int ret = write(connfd,buf,strlen(buf)-1);
            if(ret == 0)
            {
                write(connfd,"file failed",strlen("file failed"));
                printf("frwrite failed\n");
                return -1;
            }
        }
    }
    sleep(1);
    write(connfd,"files are all",strlen("files are all"));
    char buf1[30]={0};
	//��ȡ��Ҫ���ص��ļ���
    printf("begin to down\n");
    read(connfd,buf1,sizeof(buf1));
    printf("%s\n",buf1);
	//�����ص��ļ�
    FILE *fp = fopen(buf1,"rb");
    if(fp == NULL)
    {
        perror("fopen");
        return -1;
    }
	//��ȡ���صĴ�С������� ���� ��ʱ��
    fseek(fp,0,SEEK_END);
    int len = ftell(fp);    	
    fseek(fp,0,SEEK_SET);
    sprintf(buf1,"%d",len); 
    write(connfd,buf1,strlen(buf1));
    printf("%s\n",buf1);	
    sleep(1);
	//��ͻ��˷���
    while(1)
    {
        
        memset(buf1,0,sizeof(buf1));
        int ret = fread(buf1,sizeof(char),sizeof(buf1),fp);
        if(ret == 0)
        {
            if(feof(fp))
            {
                printf("file end\n");
                break;
            }
            else
            {
                printf("fread failed\n");
                return -1;
            }
        }
     //   printf("%d",msg1->flag);
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
    }
    fclose(fp);
    closedir(dir);
    printf("file  succsess --server\n");
    return 0;

}
//�ϴ�
int updata(int connfd,mp msg1)
{
    printf("welcome to updata---\n");
    char buf[30]={0};
    read(connfd,buf,sizeof(buf));
    printf("%s\n",buf);
    int fp = open(buf,O_RDWR|O_CREAT,0664);
    if(fp == -1)
    {
        perror("open");
        return -1;
    }
    memset(buf,0,sizeof(buf));
    read(connfd,buf,sizeof(buf));
    int len = atoi(buf);
    printf("%d",len);
    while(1)
    {
        memset(buf,0,sizeof(buf));
        int ret = read(connfd,buf,sizeof(buf));
        if(ret < 0)
        {
            perror("read");
            return -1;
        }
        int ret1 =write(fp,buf,ret);
        if(ret1 == -1)
        {
            perror("write");
            return -1;
        }
        len=len-ret;
        if(len == 0)
        {
            printf("updata success--\n");
            break;
        }
    }
    close(fp);
    printf("updata over--\n");
    return 0;

}

