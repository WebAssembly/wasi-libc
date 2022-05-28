#ifndef __wasilibc___header_netinet_in_h
#define __wasilibc___header_netinet_in_h

#include <__struct_in_addr.h>
#include <__struct_in6_addr.h>
#include <__struct_sockaddr_in.h>
#include <__struct_sockaddr_in6.h>
#include <__struct_ipv6_mreq.h>
#include <__struct_ip_opts.h>
#include <__struct_ip_msfilter.h>
#include <__struct_ip_mreq.h>
#include <__struct_ip_mreqn.h>
#include <__struct_ip_mreq_source.h>
#include <__struct_group_req.h>
#include <__struct_group_source_req.h>
#include <__struct_group_filter.h>
#include <__struct_in_pktinfo.h>
#include <__struct_in6_pktinfo.h>
#include <__struct_in6_mtuinfo.h>

#define IPPROTO_IP __WASI_SOCK_PROTO_IP
#define IPPROTO_ICMP __WASI_SOCK_PROTO_ICMP
#define IPPROTO_TCP __WASI_SOCK_PROTO_TCP
#define IPPROTO_UDP __WASI_SOCK_PROTO_UDP
#define IPPROTO_IPV6 __WASI_SOCK_PROTO_IPV6
#define IPPROTO_RAW __WASI_SOCK_PROTO_RAW

#define IPV6_ADDRFORM           1
#define IPV6_2292PKTINFO        2
#define IPV6_2292HOPOPTS        3
#define IPV6_2292DSTOPTS        4
#define IPV6_2292RTHDR          5
#define IPV6_2292PKTOPTIONS     6
#define IPV6_CHECKSUM           7
#define IPV6_2292HOPLIMIT       8
#define IPV6_NEXTHOP            9
#define IPV6_AUTHHDR            10
#define IPV6_UNICAST_HOPS       16
#define IPV6_MULTICAST_IF       17
#define IPV6_MULTICAST_HOPS     18
#define IPV6_MULTICAST_LOOP     19
#define IPV6_JOIN_GROUP         20
#define IPV6_LEAVE_GROUP        21
#define IPV6_ROUTER_ALERT       22
#define IPV6_MTU_DISCOVER       23
#define IPV6_MTU                24
#define IPV6_RECVERR            25
#define IPV6_V6ONLY             26
#define IPV6_JOIN_ANYCAST       27
#define IPV6_LEAVE_ANYCAST      28
#define IPV6_MULTICAST_ALL      29
#define IPV6_ROUTER_ALERT_ISOLATE 30
#define IPV6_IPSEC_POLICY       34
#define IPV6_XFRM_POLICY        35
#define IPV6_HDRINCL            36

#define IPV6_RECVPKTINFO        49
#define IPV6_PKTINFO            50
#define IPV6_RECVHOPLIMIT       51
#define IPV6_HOPLIMIT           52
#define IPV6_RECVHOPOPTS        53
#define IPV6_HOPOPTS            54
#define IPV6_RTHDRDSTOPTS       55
#define IPV6_RECVRTHDR          56
#define IPV6_RTHDR              57
#define IPV6_RECVDSTOPTS        58
#define IPV6_DSTOPTS            59
#define IPV6_RECVPATHMTU        60
#define IPV6_PATHMTU            61
#define IPV6_DONTFRAG           62
#define IPV6_RECVTCLASS         66
#define IPV6_TCLASS             67
#define IPV6_AUTOFLOWLABEL      70
#define IPV6_ADDR_PREFERENCES   72
#define IPV6_MINHOPCOUNT        73
#define IPV6_ORIGDSTADDR        74
#define IPV6_RECVORIGDSTADDR    IPV6_ORIGDSTADDR
#define IPV6_TRANSPARENT        75
#define IPV6_UNICAST_IF         76
#define IPV6_RECVFRAGSIZE       77
#define IPV6_FREEBIND           78

#define IPV6_ADD_MEMBERSHIP     IPV6_JOIN_GROUP
#define IPV6_DROP_MEMBERSHIP    IPV6_LEAVE_GROUP
#define IPV6_RXHOPOPTS          IPV6_HOPOPTS
#define IPV6_RXDSTOPTS          IPV6_DSTOPTS

#define IPV6_PMTUDISC_DONT      0
#define IPV6_PMTUDISC_WANT      1
#define IPV6_PMTUDISC_DO        2
#define IPV6_PMTUDISC_PROBE     3
#define IPV6_PMTUDISC_INTERFACE 4
#define IPV6_PMTUDISC_OMIT      5

#define IPV6_PREFER_SRC_TMP            0x0001
#define IPV6_PREFER_SRC_PUBLIC         0x0002
#define IPV6_PREFER_SRC_PUBTMP_DEFAULT 0x0100
#define IPV6_PREFER_SRC_COA            0x0004
#define IPV6_PREFER_SRC_HOME           0x0400
#define IPV6_PREFER_SRC_CGA            0x0008
#define IPV6_PREFER_SRC_NONCGA         0x0800

#define IPV6_RTHDR_LOOSE        0
#define IPV6_RTHDR_STRICT       1

#define IPV6_RTHDR_TYPE_0       0

#define __UAPI_DEF_IN_ADDR      0
#define __UAPI_DEF_IN_IPPROTO   0
#define __UAPI_DEF_IN_PKTINFO   0
#define __UAPI_DEF_IP_MREQ      0
#define __UAPI_DEF_SOCKADDR_IN  0
#define __UAPI_DEF_IN_CLASS     0
#define __UAPI_DEF_IN6_ADDR     0
#define __UAPI_DEF_IN6_ADDR_ALT 0
#define __UAPI_DEF_SOCKADDR_IN6 0
#define __UAPI_DEF_IPV6_MREQ    0
#define __UAPI_DEF_IPPROTO_V6   0
#define __UAPI_DEF_IPV6_OPTIONS 0
#define __UAPI_DEF_IN6_PKTINFO  0
#define __UAPI_DEF_IP6_MTUINFO  0

#define TCP_NODELAY 1
#define TCP_MAXSEG	 2
#define TCP_CORK	 3
#define TCP_KEEPIDLE	 4
#define TCP_KEEPINTVL	 5
#define TCP_KEEPCNT	 6
#define TCP_SYNCNT	 7
#define TCP_LINGER2	 8
#define TCP_DEFER_ACCEPT 9
#define TCP_WINDOW_CLAMP 10
#define TCP_INFO	 11
#define	TCP_QUICKACK	 12
#define TCP_CONGESTION	 13
#define TCP_MD5SIG	 14
#define TCP_THIN_LINEAR_TIMEOUTS 16
#define TCP_THIN_DUPACK  17
#define TCP_USER_TIMEOUT 18
#define TCP_REPAIR       19
#define TCP_REPAIR_QUEUE 20
#define TCP_QUEUE_SEQ    21
#define TCP_REPAIR_OPTIONS 22
#define TCP_FASTOPEN     23
#define TCP_TIMESTAMP    24
#define TCP_NOTSENT_LOWAT 25
#define TCP_CC_INFO      26
#define TCP_SAVE_SYN     27
#define TCP_SAVED_SYN    28
#define TCP_REPAIR_WINDOW 29
#define TCP_FASTOPEN_CONNECT 30
#define TCP_ULP          31
#define TCP_MD5SIG_EXT   32
#define TCP_FASTOPEN_KEY 33
#define TCP_FASTOPEN_NO_COOKIE 34
#define TCP_ZEROCOPY_RECEIVE   35
#define TCP_INQ          36
#define TCP_TX_DELAY     37

#define TCP_CM_INQ TCP_INQ

#define TCP_ESTABLISHED  1
#define TCP_SYN_SENT     2
#define TCP_SYN_RECV     3
#define TCP_FIN_WAIT1    4
#define TCP_FIN_WAIT2    5
#define TCP_TIME_WAIT    6
#define TCP_CLOSE        7
#define TCP_CLOSE_WAIT   8
#define TCP_LAST_ACK     9
#define TCP_LISTEN       10
#define TCP_CLOSING      11

#define IN6ADDR_ANY_INIT { { \
    0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00  \
} }

#define IN6ADDR_LOOPBACK_INIT { { \
    0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x01  \
} }

#endif
