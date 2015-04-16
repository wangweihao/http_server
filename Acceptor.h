/*
 * =====================================================================================
 *
 *       Filename:  Acceptor.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年04月16日 16时32分26秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (wangweihao), 578867817@qq.com
 *        Company:  xiyoulinuxgroup
 *
 * =====================================================================================
 */

#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_

#define _GUN_SOURCE

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>

#include <iostream>
#include <string>

class Acceptor final
{
    public:
        Acceptor() = default;
        Acceptor(int pt, std::string &ht):
            port(pt), host(ht)
            {
                int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
                if(listen_sock == -1)
                {
                    perror("create listen sockfd error\n");
                    exit(EXIT_FAILURE);
                }
                
                server_addr.sin_family = AF_INET;
                server_addr.sin_port = htons(port);
                inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr);
                
                if(bind(listen_sock, (struct sockaddr*)&server_addr, sizeof(sockaddr_in)) < -1)
                {
                    perror("bind error\n");
                    exit(EXIT_FAILURE);
                }
                if(listen(listen_sock, 5) < -1)
                {
                    perror("listen error\n");
                    exit(EXIT_FAILURE);
                }

            }
        ~Acceptor(){ close(sockfd); }

        int Accpet(int flag);

    private:
        int sockfd = 0;
        int port;
        std::string host;
        struct sockaddr_in server_addr;
};

int
Acceptor::Accpet(int flag)
{
    socklen_t server_len = sizeof(struct sockaddr_in);
    accept4(sockfd, (struct sockaddr*)&server_addr, &server_len, flag);
    //accept(sockfd, (struct sockaddr*)&server_addr, &server_len);
}

#endif
