/**********************************************
** 服务端，ARM版本，在arm板上运行，用于给客户端进行
** 连接，并下载arm板上的文件
** 编译器：arm-linux-gnueabihf-g++
**********************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <fcntl.h>

#include "log.h"

#define READ_BUF_SIZE 512
#define DROP_TRIGGER_SIZE 1024 * 1024 * 100 //100M 时触发一次缓存清空
#define TYPE_OPT_NUM 3

#define E_OK 0
#define E_ERR 1

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

enum OPERATION
{
    OPT_GET = 0,
    OPT_POST,
    OPT_NULL
};

struct OPT_INFO
{
    char *typeOpt;
    enum OPERATION opt;
};

struct OPT_INFO optInfo[OPT_NULL] = {
    {"GET", OPT_GET},
    {"POST", OPT_POST}
};


int socketStart(const char *ip, int port);
int handelRequest(const int socket, const char *request);
void *processConnect(void *arg);
enum OPERATION getOperate(const char *opt);
int dealOperate(int sock, enum OPERATION mode, const char *fileName);
void getFile(int sockfd, const char *fileName);
void dropCache(int drop);

// const char *typeOpt[TYPE_OPT_NUM] = {
//     "GET",
//     "POST",
//     NULL
// };


int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        printf("Usage: %s [ip] [port]\n", argv[0]);
        return 1;
    }

    int server_sock = socketStart(argv[1], atoi(argv[2]));
    if(server_sock < 0)
    {
        return 1;
    }

    printf("Server Init ok!\n");
    pthread_t pthread_fd;
    sockaddr_in client;
    socklen_t len = sizeof(client);
    while(1)
    {
        int new_sock = accept(server_sock, (sockaddr*)&client, &len);
        if(new_sock < 0)
        {
            perror("accept");
            continue;
        }
        printf("new connect socket %d from %s\n", new_sock, inet_ntoa(client.sin_addr));
        pthread_create(&pthread_fd, NULL, processConnect, (void *)&new_sock);
        pthread_join(pthread_fd, NULL);
    }

    return 0;
}

int socketStart(const char *ip, int port)
{
    //1.创建 socket
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock < 0)
    {
        perror("socket");
        return -1;
    }

    //2.绑定端口号
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons(port);

    //设置地址复用
    int on = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    int ret = bind(server_sock, (sockaddr*)&server, sizeof(server));
    if(ret < 0)
    {
        perror("bind");
        return -1;
    }

    //3.使用listen允许服务器被客户端连接
    ret = listen(server_sock, 5);
    if(ret < 0)
    {
        perror("listen");
        return -1;
    }

    return server_sock;
}

void *processConnect(void *arg)
{ 
    //获取入参，sock
    int new_sock = *((int*)arg);

    //从客户端读取数据
    char buf[128] = {0};
    ssize_t read_size = read(new_sock, buf, sizeof(buf) - 1);
    if(read_size < 0)
    {
        perror("read");
        return NULL;
    }

    if(read_size == 0)
    {
        //TCP中，如果read的返回值是0，说明对端关闭了连接
        printf("[client %d] disconnect!\n", new_sock);
        close(new_sock);
        return NULL;
    }
    buf[read_size] = '\0';
    
    //客户端发过来的信息，需要固定格式，形如：GET filename
    char operation[8] = {0};
    char fileName[32] = {0};
    const char *p = buf;
    int i = 0;
    
    /* 获取操作类型字符串 */
    while(*p != ' ')
    {
        operation[i++] = *p++;
    }
    operation[i] = '\0';

    /* 获取文件名 */
    snprintf(fileName, sizeof(fileName), "%s", p + 1);
    tracef("operation: %s, filename: %s\n", operation, fileName);

    /* 匹配操作类型 */
    enum OPERATION opt = getOperate(operation);
    if(dealOperate(new_sock, opt, fileName))
    {
        printf("dealOperate error !\n");
        return NULL;
    }
    close(new_sock);
    return NULL;
}

enum OPERATION getOperate(const char *opt)
{
    if(opt == NULL)
    {
        return OPT_GET;
    }

    int i;
    for(i = 0; i < OPT_NULL; i++)
    {
        if(strcmp(optInfo[i].typeOpt, opt) == 0)
        {
            return optInfo[i].opt;
        }
    }

    //如果无匹配，默认GET
    return OPT_GET;
}

int dealOperate(int sock, enum OPERATION mode, const char *fileName)
{
    switch (mode)
    {
        case OPT_GET:
            getFile(sock, fileName);
            break;
        default:
            break;
    }
    return E_OK;
}

void getFile(int sockfd, const char *fileName)
{
    /* 打开指定文件 */
    FILE *fp = NULL;
    fp = fopen(fileName, "rb");
    if(fp == NULL)
    {
        perror("fopen");
        return;
    }
    
    /* 获取文件大小, byte */
    int fileLen = 0;
    int readSize = 0;
    char readBuf[READ_BUF_SIZE] = {0};
 
    fseek(fp, 0, SEEK_END);
    fileLen = ftell(fp);
    if(fileLen == -1)
    {
        printf("ftell error !\n");
        return;
    }
    fseek(fp, 0, SEEK_SET);
    
    int dropSize = 0;
    while(fileLen > 0)
    {
        readSize = fread(readBuf, 1, READ_BUF_SIZE - 1, fp);
        if(readSize <= 0)
        {
            break;
        }
        fileLen -= readSize;
        dropSize += readSize;
        write(sockfd, readBuf, readSize);

        if(dropSize > DROP_TRIGGER_SIZE)
        {
            dropSize = 0;
            sleep(1);
            printf("drop all cache: 3\n");
            system("sync");
            dropCache(3);
        }
    }
    
    fclose(fp);
}


// param[in] :  
// 1: 释放页面缓存
// 2: 释放目录文件和inodes 
// 3: 释放所有缓存(页面缓存，目录文件和inodes)
void dropCache(int drop)
{
    int ret = 0;
    int fd = open("/proc/sys/vm/drop_caches", O_RDWR);

    char dropData[4] = {0};
    int dropSize = snprintf(dropData, sizeof(dropData), "%d", drop);
    ret = write(fd, dropData, dropSize);
    close(fd);
}
















