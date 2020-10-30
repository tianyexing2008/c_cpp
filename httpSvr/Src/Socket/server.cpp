/************************************************
* 				server
* 
* desc: socket server封装
* author: kwanson
* email: CSDN kwanson
*************************************************/
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "utils.h"
#include "sockopt.h"
#include "client.h"
#include "server.h"
#include "common.h"

namespace Socket
{
	
CStreamServer::CStreamServer(const std::string &addr, uint16_t port):
m_addr(addr), m_port(port), m_sockfd(INVALID_VALUE)
{
}
CStreamServer::~CStreamServer()
{
	close();	// ignore error
}
int CStreamServer::fd()
{
	Pthread::CGuard guard(m_socketmutex);
	return m_sockfd;
}
int CStreamServer::set_nonblock(bool nonblock)
{
	Pthread::CGuard guard(m_socketmutex);
	if(INVALID_FD(m_sockfd))	
	{
		return -1;
	}
		
	return Socket::set_nonblock(m_sockfd, nonblock);
}
int CStreamServer::start(size_t backlog)
{
	Pthread::CGuard guard(m_socketmutex);
	if(!INVALID_FD(m_sockfd))	
	{
		return 0;
	}
		
	struct sockaddr_in sockaddr;
	if(convert_inaddr(m_addr, m_port, sockaddr) < 0)
	{
		return E_ERR;
	}

	int sockfd = 0x00;
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		return E_ERR;
	}

	set_resuseport(sockfd, true);	// ignore error
	if(bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0)
	{
		::close(sockfd);
		return E_ERR;
	}
	
	if(listen(sockfd, backlog) < 0)
	{
		::close(sockfd);
		return E_ERR;
	}
	
	m_sockfd = sockfd;
	printf("create socket[%d] success, listen port[%d]\n", m_sockfd, m_port);

	return E_OK;	
}
int CStreamServer::close()
{
	Pthread::CGuard guard(m_socketmutex);
	if(INVALID_FD(m_sockfd))
	{
		return E_ERR;
	}

	if(::close(m_sockfd) < 0);
	{
		return E_ERR;
	}
		
	m_sockfd = INVALID_VALUE;
	return 0;
}
bool CStreamServer::isclose()
{
	Pthread::CGuard guard(m_socketmutex);
	if(INVALID_FD(m_sockfd))
	{
		return true;
	}
	return false;
}
IClient *CStreamServer::accept()
{
	Pthread::CGuard guard(m_socketmutex);
	if(INVALID_FD(m_sockfd))
		return NULL;

	struct sockaddr_in clientaddr;
	socklen_t clientaddrlen = sizeof(clientaddr);
	int clientfd = ::accept(m_sockfd, (struct sockaddr *)&clientaddr, &clientaddrlen);
	if(INVALID_FD(clientfd))
		return NULL;

	IClient *client = new CStreamClient(clientfd);
	assert(client != NULL);
	return client;
}

}
