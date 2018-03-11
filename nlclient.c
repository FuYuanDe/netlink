/*
* capture test 
* Author : Mason
* Date   : 2018/03/11
*/

#include <linux/netlink.h>
#include <linux/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "nlclient.h"

#define LOG(fmt,arg...) \
	printf("[netlink client] %s:%d  "fmt,__FUNCTION__,__LINE__,##arg)

#define NETLINK_MYNL_MODULE 25
#define NETLINK_ADD 17
#define NETLINK_DELETE 18

void main(int argc, char **argv)
{
	int fd;
	int index = 0;
    unsigned int taskid;
    int select;	
    int num;
    char flag;
    __be32 ip1_be;
    __be32 ip2_be;
    unsigned short remote_port;
    int space;
	struct sockaddr_nl src,dest;
	struct nlmsghdr *nlh = NULL;
	struct iovec iov;
	struct msghdr msg;
    struct task_info *task = NULL;

    char *iplocal = (char *)malloc(20);
    char *ipremote = (char *)malloc(20);
    if(!iplocal || !ipremote)
    {
        printf("nlcient malloc fail. \r\n");
        return ;
    }

    printf("Welcome !\n 1 : delete mgr \n 2 : add mgr \n Please enter your choice ! \n ");
    scanf("%d",&select);
    printf("Great! \n");

  
    fd = socket(AF_NETLINK,SOCK_DGRAM,NETLINK_MYNL_MODULE);
    if(fd == -1)
    {
        LOG("create socket fail\n");
        return;
    }

    memset(&src,0,sizeof(struct sockaddr_nl));
    memset(&dest,0,sizeof(struct sockaddr_nl));
    memset(&msg,0,sizeof(struct msghdr));   
    src.nl_family = AF_NETLINK;
    src.nl_pid = getpid(); 
    src.nl_groups = 0;  
    dest.nl_family = AF_NETLINK;
    dest.nl_pid = 0;    
    dest.nl_groups = 0;

    bind(fd, (struct sockaddr*)&src, sizeof(struct sockaddr_nl));    

    switch(select){
        case 1:
            printf("input taskid : \n");
            scanf("%u",&taskid);
            nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(sizeof(int)));
            if(nlh == NULL)
            {
                LOG("nlh malloc fail \r\n");
                goto end;
            }
            nlh->nlmsg_len = NLMSG_SPACE(sizeof(int));
            nlh->nlmsg_pid = getpid();
            nlh->nlmsg_type = NETLINK_DELETE;
            nlh->nlmsg_flags = NLM_F_REQUEST;
            memcpy(NLMSG_DATA(nlh),&taskid,sizeof(int));
            break;            

        case 2:
            printf("How many Range do you want? \n");
            scanf("%d",&num);
            space = sizeof(int) + sizeof(struct addr_nl) + (sizeof(struct filter_nl))*num;
            task = (struct task_info *)malloc(sizeof(int)+sizeof(struct addr_nl)+sizeof(struct filter_nl)*num);
            memset(task,0,space);
            printf("input taskid: \n");            
            scanf("%u",&task->taskid);
            printf("which remote port to send to?");
            scanf("%hu",&remote_port);        
            getchar();
            task->addr.local_port = 0;
            task->addr.remote_port = htons(remote_port);

            printf("please input local ip  :  ");
            gets(iplocal);     

            printf("please input remote ip :  ");
            gets(ipremote);
                        
            task->addr.local_ip.ip = inet_addr(iplocal);
            task->addr.remote_ip.ip = inet_addr(ipremote);                

            index = 0;
            while(index < num)
            {
                printf("input filter[%d].min,filter[%d].max: ",index,index);                
                scanf("%hu %hu",&task->filter[index].min,&task->filter[index].max);
                
                task->filter[index].protocol = 0xFFFF;
                index++;
            }           
            printf("Dont hurry! Let's make sure that everything is OK \n");
            printf("Your task id : %u \n",task->taskid);
            printf("Your address: local.port = %hu,local.ip = %s \n",ntohs(task->addr.local_port),iplocal);
            printf("Your address: remote.port = %hu,remote.ip = %s \n",ntohs(task->addr.remote_port),ipremote);

            index = 0;
            while(index < num)
            {   
                printf("filter[%d].min = %u,filter[%d].max = %u \n",index,task->filter[index].min,index,task->filter[index].max);
                index++;
            }
            getchar();
            printf("Everything OK? (Y|N) ");
            flag = getchar();
            if(flag == 'Y' || flag == 'y')
                printf("Great! \n");
            else
            {
                puts("Oh,No");
                free(task);
                return ;            
            }
            nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(space));
            if(nlh == NULL)
            {
                LOG("nlh malloc fail \r\n");
                goto end;
            }     
            nlh->nlmsg_len = NLMSG_SPACE(space);
            nlh->nlmsg_pid = getpid();
            nlh->nlmsg_type = NETLINK_ADD;
            nlh->nlmsg_flags = NLM_F_REQUEST;
            memcpy(NLMSG_DATA(nlh),task,space);                     
            break;
        default:
            printf("unrecognized choice \n");        
            return;
    }
        if(nlh != NULL)
	{
		iov.iov_base = (void *)nlh;
		iov.iov_len = nlh->nlmsg_len;
		msg.msg_name = (void *)&dest;
		msg.msg_namelen = sizeof(struct sockaddr_nl);
		msg.msg_iov = &iov;
	        msg.msg_iovlen = 1;
		sendmsg(fd, &msg, 0);
	}
end:
    if(nlh != NULL)
    {
    	free(nlh);
    	nlh = NULL;
    }
    if(task != NULL)
    {
    	free(task);
    	task = NULL;
    }
    LOG("msg send success \n");
	return;
}

