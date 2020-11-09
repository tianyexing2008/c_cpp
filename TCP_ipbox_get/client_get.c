/////////////////////////////
//客户端处理
//1 连接服务端
//2 向服务端发送GET filename来获取文件
///////////////////////////

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define E_ERR 1
#define E_OK 0

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

typedef struct arg_info
{
    char ip[32];
    int port;
    char fileName[32];
}ARG_INFO;

int getOpt(char *arg, ARG_INFO *info);

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        printf("usage %s ip:port/fileName\n", argv[0]);
        return 1;
    }

    ARG_INFO info;
    memset(&info, 0, sizeof(info));
    if(getOpt(argv[1], &info) == E_ERR)
    {
        return 1;
    }
    
    // 1 创建socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0)
    {
        perror("socket");
        return 1;
    }

    //2 获取连接
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(info.ip);
    server_addr.sin_port = htons(info.port);
    
    int ret = connect(fd, (sockaddr*)&server_addr, sizeof(server_addr));
    if(ret < 0)
    {
        printf("connect to %s:%d fail\n", info.ip, info.port);
        perror("connect");
        return 1;
    }
    
    //向服务发送请求，带文件名
    char operation[32] = {0};
    snprintf(operation, sizeof(operation), "GET %s", info.fileName);
    write(fd, operation, strlen(operation));

    //新建文件用于接收数据
    FILE *file;
    if((file = fopen(info.fileName, "a")) == NULL)
    {
        perror("fopen");
        return 1;
    }
    printf("create file %s success!\n", info.fileName);

    // 进入循环
    while(1)
    {
        //a从标准输入读入字符串
        char buf[32] = {0};
        ssize_t read_size = read(fd, buf, sizeof(buf) - 1);
        if(read_size < 0)
        {
            perror("read");
            return 1;
        }
        if(read_size == 0)
        {
            printf("read done!\n");
            fclose(file);
            return 0;
        }
        
        //b)把读入的字符串发送给服务器
        fwrite(buf, 1, read_size, file);
    }
    fclose(file);
    return 0;
}

//param[in]: arg, e.g."client.out 192.168.2.108:9898/core"
//param[out]: info
//return 0:ok, 1:error
int getOpt(char *arg, ARG_INFO *info)
{
    if(arg == NULL || info == NULL)
    {
        return E_ERR;
    }

    //取ip地址
    int i = 0;
    char ip[32] = {0};
    const char *p = arg;
    int len = strlen(arg);

    while(*p != ':')
    {
        ip[i++] = *p++;
    }
    ip[i] = '\0';
    snprintf(info->ip, sizeof(info->ip), "%s", ip);

    struct in_addr addr;
    if(inet_aton(info->ip, &addr) == 0)
    {
        printf("invalid ip address: %s\n", info->ip);
        return E_ERR;
    }

    //取port
    int port = 0;
    char portStr[4] = {0};

    //指向：后的数字
    p += 1;
    port = atoi(p); //atoi 遇到第一个非数字字符时就会返回
    info->port = port;
    
    int separate = 0;
    while(*p != '\0')
    {
        p++;
        if(*p == '/' && *(p + 1) != '\0') //假如/后带有文件名
        {
            separate = 1;
            break;
        }
    }
    
    //如果/后还有字符才取
    if(separate == 1)
    {
        snprintf(info->fileName, sizeof(info->fileName), "%s", p + 1);
    }
    else
    {
        fprintf(stderr, "invalid para, no fileName\n");
        return E_ERR;
    }
    
    printf("ipaddr: %s, port: %d, fileName: %s\n", info->ip, info->port, info->fileName);

    return E_OK;
}

















