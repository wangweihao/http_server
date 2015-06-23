/*
 * =====================================================================================
 *
 *       Filename:  Server.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年04月16日 18时44分28秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (wangweihao), 578867817@qq.com
 *        Company:  xiyoulinuxgroup
 *
 * =====================================================================================
 */

#ifndef _SERVER_H_
#define _SERVER_H_

enum { Max_conn = 10000 };

#include "Acceptor.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>

#include <vector>
#include <string>
#include <map>

class Server
{
    public:
        Server(int pt, std::string &ht):
            port(pt), host(ht)
            {
                bzero(&server_addr, sizeof(server_addr));
                server_addr.sin_family = AF_INET;
                server_addr.sin_port = htons(port);
                inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr);
                listen_fd = socket(PF_INET, SOCK_STREAM, 0);
                if(listen_fd < 0)
                {
                    perror("socket error\n");
                    exit(EXIT_FAILURE);
                }
                int ret = bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
                if(ret == -1)
                {
                    perror("binf error\n");
                    exit(EXIT_FAILURE);
                }
                int ret2 = listen(listen_fd, 5);
                if(ret == -1)
                {
                    perror("listen error\n");
                    exit(EXIT_FAILURE);
                }
            }
        ~Server() 
        {
            close(listen_fd);
            close(epollfd); 
        }
        /* ready_函数：执行初始化操作 */
        int ready_();
        /* run_运行函数 */
        int run_();


    private:
        /* register_函数：登记新加入的连接事件 */
        int register_(int fd);
        /* 设置非阻塞套接字 */
        int setnonblocking(int fd);

    private:
        /* 端口号和ip */
        int port;
        std::string host;
        
        int listen_fd = 0;
        int epollfd = 0;
        //将来打算每个连接封装为Acceptor对象
        std::vector<Acceptor>all_accpet;
        
        struct sockaddr_in server_addr;
        struct epoll_event events[Max_conn];
        struct epoll_event ev;
};

int
Server::run_()
{
    while(1)
    {
        /* 返回已经准备好的事件 */
        int nfds = epoll_wait(epollfd, events, Max_conn, -1);
        //std::cout << nfds << std::endl;
        //std::cout << "----------" << std::endl;
        if(nfds == -1)
        {
            perror("epoll_wait error\n");
            exit(EXIT_FAILURE);
        }
        for(int i = 0; i < nfds; ++i)
        {
            std::cout << "---------------" << std::endl;
            /* 新加入的连接 */
            if(events[i].data.fd == listen_fd)
            {
                std::cout << "加入连接" << std::endl;
                register_(events[i].data.fd);
                sleep(1);
            }
            /* 读事件 */
            else if(events[i].events & EPOLLIN)
            {
                std::cout << "读事件" << std::endl;
                char buffer_in[1024];
                recv(events[i].data.fd, buffer_in, 1024, 0);
                std::cout << buffer_in << std::endl;
                sleep(100);
            }
            /* 写事件 */
            else if(events[i].events & EPOLLOUT)
            {
                std::cout << "写事件" << std::endl;
                //char buffer_out[1024];
                //send(events[i].data.fd, buffer_out, 1024, 0);
                sleep(100);
            }
            /* 被挂断 */
            else if(events[i].events & EPOLLHUP)
            {
                std::cout << "挂断事件" << std::endl;
                sleep(100);
            }
            /* 错误 */
            else if(events[i].events & EPOLLERR)
            {
                std::cout << "错误事件" << std::endl;
                sleep(100);
            }
        }
    }
}

/* 改进时可以设定一个阻塞队列，用epoll新特性批量添加 */
int 
Server::register_(int fd)
{
    struct epoll_event ev;
    struct sockaddr_in client_addr;
    
    ev.events = EPOLLIN | EPOLLOUT;
    socklen_t addrlen = sizeof(struct sockaddr_in);

    int conn_sock = accept4(listen_fd, (struct sockaddr*)&client_addr, &addrlen, SOCK_NONBLOCK);
    //int conn_sock = accept(listen_fd, (struct sockaddr*)&client_addr, &addrlen);
    std::cout << "conn_sock:" << conn_sock << std::endl;
    if(conn_sock == -1){
        perror("accept error");
        exit(EXIT_FAILURE);
    }
    ev.data.fd = conn_sock;
    int ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev);
    //setnonblocking(conn_sock);
    if(ret == -1)
    {
        perror("epoll_ctl error\n");
        exit(EXIT_FAILURE);
    }

}

int 
Server::ready_()
{
    epollfd = epoll_create(Max_conn);
    if(epollfd == -1)
    {
        perror("epoll create default\n");
        exit(EXIT_FAILURE);
    }
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = listen_fd;
    int ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_fd, &ev);
    setnonblocking(listen_fd);
    if(ret == -1){
        perror("epoll_ctl error\n");
        exit(EXIT_FAILURE);
    }
    std::cout << "ready_ success" << std::endl;
    return 0;
}

int 
Server::setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

#endif


