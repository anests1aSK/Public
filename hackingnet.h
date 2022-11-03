int send_string(int sockfd, unsigned char *buffer) {
    int sent_bytes, bytes_to_send;
    bytes_to_send = strlen(buffer);
    while(bytes_to_send > 0) {
        sent_bytes = send(sockfd, buffer, bytes_to_send, 0);
        if(sent_bytes == -1)
            return 0;
        bytes_to_send -= sent_bytes;
        buffer += sent_bytes;
    }
    return 1;
}

int recv_line(int sockfd, unsigned char *dest_buffer) {
#define EOL "\r\n"
#define EOL_SIZE 2
    unsigned char *ptr;
    int eol_matched = 0;

    ptr = dest_buffer;
    while(recv(sockfd, ptr, 1, 0) == 1) {
        if(*ptr == EOL[eol_matched]) {
            eol_matched++;
            if(eol_matched == EOL_SIZE) {
                *(ptr+1-EOL_SIZE) = '\0';
                return strlen(dest_buffer);
            }
        } else {
            eol_matched = 0;
        }
        ptr++;
    }
    return 0;
}

#define ETHER_ADDR_LEN 6
#define ETHER_HDR_LEN 14

struct ether_hdr {
  unsigned char ether_dest_addr[ETHER_ADDR_LEN]; // Dest MAC
  unsigned char ether_src_addr[ETHER_ADDR_LEN];  // Source MAC address
  unsigned short ether_type; // Tipo de paquete
};

struct ip_hdr {
  unsigned char ip_version_and_header_length; // Version y header 
  unsigned char ip_tos;          // Tipo de Servicio
  unsigned short ip_len;         // Total length
  unsigned short ip_id;          // Numero de identificacion
  unsigned short ip_frag_offset; // fragment offset ejemplo si hay que fragmentar paquetes para el destino.
  unsigned char ip_ttl;          // Tiempo de vida
  unsigned char ip_type;         // Tipo de protocolo
  unsigned short ip_checksum;    // Checksum
  struct in_addr ip_src_addr;      // Source IP
  struct in_addr ip_dest_addr;     // Dest
};



struct tcp_hdr {
  unsigned short tcp_src_port;   // Source TCP port
  unsigned short tcp_dest_port;  // Dest TCP port
  unsigned int tcp_seq;          // TCP sequence, es el offset de bytes recorrido por cada paquete
  unsigned int tcp_ack;          // TCP acknowledgment aumenta en 1 por cada paquete
  unsigned char reserved:4;      // reservo 4 bits
  unsigned char tcp_offset:4;    // TCP data offset para little endian si fuera para big endian tendria que cambiar de posicion con reserved
  unsigned char tcp_flags;       // TCP flags
#define TCP_FIN   0x01
#define TCP_SYN   0x02
#define TCP_RST   0x04
#define TCP_PUSH  0x08
#define TCP_ACK   0x10
#define TCP_URG   0x20
  unsigned short tcp_window;     // TCP window size, decide el size de los paquetes entre las dos coneccciones
  unsigned short tcp_checksum;   // TCP checksum
  unsigned short tcp_urgent;     // TCP urgent pointer
};


