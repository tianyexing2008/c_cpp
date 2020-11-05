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
#define DROP_TRIGGER_SIZE 1024 * 1024 * 100 //100M时触发一次缓存清空
#define MODE_MAX_NUM 32

#define E_OK 0
#define E_ERR 1

#define BLOCK_SIZE 1024 * 1024 * 400  //分块大小
typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

enum OPERATION
{
    OPE_GET,
    OPE_MOD,
};

struct REQUEST_MODE
{
    enum OPERATION opt;
    char optStr[32];
    char buf[32];
};

struct REQUEST_MODE requestMode[MODE_MAX_NUM] = {
    {OPE_GET, "GET", 0},
    {OPE_MOD, "UPLOAD", 0}
};

int HandleRequest(const int socket, const char *request);
void *ProcessConnect(void *arg);
enum OPERATION GetOptMode(const char *opt, int len);
int DealOperate(int sock, enum OPERATION mode, const char *fileName);
void getFile(int sockfd, const char *fileName);
void dropCache(int drop);

int g_isSameFile = 0;
int g_totalSize = 0;
int g_readSize = 0;
char g_fileName[32] = {0};

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        printf("Usage: %s [ip] [port]\n", argv[0]);
        return 1;
    }

    //1.创建 socket
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock < 0)
    {
        perror("socket");
        return 1;
    }

    //2.绑定端口号
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(atoi(argv[2]));

    //设置地址复用
    int on = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    int ret = bind(server_sock, (sockaddr*)&server, sizeof(server));
    if(ret < 0)
    {
        perror("bind");
        return 1;
    }

    //3.使用listen允许服务器被客户端连接
    ret = listen(server_sock, 5);
    if(ret < 0)
    {
        perror("listen");
        return 1;
    }

    //4.服务器初始化完成，进入事件循环
    printf("Server Init ok!\n");
    pthread_t pthread_fd;
    while(1)
    {
        sockaddr_in client;
        socklen_t len = sizeof(client);
        int new_sock = accept(server_sock, (sockaddr*)&client, &len);
        if(new_sock < 0)
        {
            perror("accept");
            continue;
        }
        printf("new connect socket %d from %s\n", new_sock, inet_ntoa(client.sin_addr));
        pthread_create(&pthread_fd, NULL, ProcessConnect, (void *)&new_sock);
        pthread_join(pthread_fd, NULL);

        // printf("drop all cache: 3\n");
        // system("sync");
        // dropCache(3);
    }

    return 0;
}

void *ProcessConnect(void *arg)
{ 
    int new_sock = *((int*)arg);

    //a)从客户端读取数据
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
    
    char operation[4] = {0};
    char fileName[32] = {0};
    const char *p = buf;
    int i = 0;
    
    /* 获取操作类型 */
    while(*p != ' ')
    {
        operation[i++] = *p++;
    }
    operation[i] = '\0';

    /* 获取文件名 */
    snprintf(fileName, sizeof(fileName), "%s", p + 1);
    tracef("operation: %s, filename: %s\n", operation, fileName);

    if(DealOperate(new_sock, OPE_GET, fileName))
    {
        printf("DealOperate error !\n");
        return NULL;
    }
    close(new_sock);
    return NULL;
}


int DealOperate(int sock, enum OPERATION mode, const char *fileName)
{
    switch (mode)
    {
        case OPE_GET:
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
        printf("no such file of %s\n", fileName);
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

void dropCache(int drop)
{
    int ret = 0;
    int fd = open("/proc/sys/vm/drop_caches", O_RDWR);

    char dropData[8] = {0};
    int dropSize = snprintf(dropData, sizeof(dropData), "%d", drop);
    ret = write(fd, dropData, dropSize);
    close(fd);
}
















