#ifndef __NLCLIENT_H__
#define __NLCLIENT_H__

#define MAX_NAME_LEN 40

union inet_addr_nl {
    unsigned int all[4];
    __be32 ip;
    __be32 ip6[4];
};

struct addr_nl {
    __be16 local_port;
    __be16 remote_port;
    union inet_addr_nl local_ip;
    union inet_addr_nl remote_ip;
};

struct filter_nl {
    unsigned short min;
    unsigned short max;
    unsigned int protocol;
};

struct task_info {
    unsigned int taskid;
    struct addr_nl addr;
    struct filter_nl filter[1];
};

//28
struct task_info_1 {
    unsigned int taskid;
    struct addr_nl addr;
    struct filter_nl filter;
};

#endif

