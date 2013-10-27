/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file testlibevent.c
 * @author zhangjiyang01@baidu.com(zhangjiyang01@baidu.com)
 * @date 2013/10/27 17:03:58
 * @version $Revision$ 
 * @brief 
 *  
 **/

#include <stdio.h>
#include <event2/event.h>
#include "event.h"
#include "tcp_listen.h"

void on_accept(void *arg)
{
    puts("fff");
    return;
    //printf("%s", arg);

}

void input()
{
    printf("ssss\n");
 //   int n;
//    scanf("%d", &n);

}
int main(void)
{
    struct sockaddr_in servaddr;
    int    listenfd;

    listenfd = tcp_listen(&servaddr);
   
    struct event_base *base;
    base = event_base_new();
    if (NULL == base) {
        perror("base");
    }
    struct event listen_event;
    event_set(&listen_event, listenfd, EV_PERSIST|EV_READ, (void *)on_accept, "accept happen!");
    event_base_set(base, &listen_event);
    event_add(&listen_event, NULL);
   
#define EVLOOP_ONCE             0x01
#define EVLOOP_NONBLOCK         0x02
#define EVLOOP_NO_EXIT_ON_EMPTY 0x04
    
    struct event *input_event;
    
    input_event = event_new(base, 0, EV_READ|EV_PERSIST,(void *)input, NULL ); 
    //event_set(&input_event, 0, EV_PERSIST|EV_READ, (void *)input, NULL); 
    //event_base_set(base, &input_event);
    event_add(input_event, NULL);

    //event_base_loop(base, EVLOOP_NO_EXIT_ON_EMPTY );
    event_base_dispatch(base);
    printf("%d\n", listenfd);
    return 0;
}





















/* vim: set ts=4 sw=4 sts=4 tw=100 */
