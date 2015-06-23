/*
 * =====================================================================================
 *
 *       Filename:  epoll.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年06月16日 18时12分53秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (wangweihao), 578867817@qq.com
 *        Company:  xiyoulinuxgroup
 *
 * =====================================================================================
 */

#include "Server.h"

int main(int argc, char *argv[])
{
    int port = atoi(argv[2]);
    std::string ip = argv[1];
	Server epoll_server(port, ip);
    epoll_server.ready_();
    epoll_server.run_();


	return EXIT_SUCCESS;
}


