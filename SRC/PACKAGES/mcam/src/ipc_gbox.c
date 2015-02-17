#include "ipc_gbox.h"
#include "globals.h"

typedef struct ipc_gbox {
	int	sd_out;
	int	sd_in;
	struct {
		int sd;
		int ecmpidID;
	} slots[16];
} ipc_gbox_t;

ipc_gbox_t	gipc = {0};

int	ipc_gbox_slot_open(int id);
int	ipc_gbox_send(unsigned char* buf, int size);
int	ipc_gbox_recv(unsigned char* buf, int size);

pthread_t	ipc_gbox_thread_handle	= 0;
pthread_attr_t	ipc_gbox_thread_attr		={{0}};
pthread_mutex_t	ipc_gbox_thread_lock		= PTHREAD_MUTEX_INITIALIZER;
int		ipc_gbox_thread_running	= 0;

void*
ipc_gbox_task(void* arg)
{
	int i,n,k,l,slotID;
	unsigned char buf[512] = {0};

	ipc_gbox_thread_running = 1;

	while (ipc_gbox_thread_running) {
		n = ipc_gbox_recv(buf,sizeof(buf));
		if (n>0) {
			if (buf[0] == 0x89) {
				if (n>16) {
					if (memcmp(buf+1,"\x00\x00\x00\x00\x00\x00\x00\x00",8)) {
						memcpy(&mca_settings.dcw.even,buf+1,8);
						mca_settings.cw_mode = 1;
					}
					if (memcmp(buf+9,"\x00\x00\x00\x00\x00\x00\x00\x00",8)) {
						memcpy(&mca_settings.dcw.odd,buf+9,8);
						mca_settings.cw_mode = 1;
					}
				}
			} else
			if (buf[0] == 0x8A) {
				for (i=0;i<16;i++) {
					if (gipc.slots[i].sd)
						close(gipc.slots[i].sd);
					gipc.slots[i].sd=0;
				}
				if (buf[1] != 0x00) {
					slotID = 0;
					k=buf[1];
					i = 2;
					while (k && i<n) {
						unsigned int pid = (buf[i+0]<<8)|buf[i+1];
						for (l=0;l<mca_settings.ecmpids_count;l++) {
							if (mca_settings.ecmpids[l].ECM_PID == pid) {
								gipc.slots[slotID].sd = ipc_gbox_slot_open(slotID);
								if (gipc.slots[slotID].sd) {
									gipc.slots[slotID].ecmpidID = l;
								}
								mca_settings.ecmpids[l].checked = 1;
							}
						}
						i+=2;
						k--;
						slotID++;
					}
					mca_settings.ecm_filter_changed = 1;
				}
			}
		}
	}

	pthread_exit(0);

	return 0;
}

unsigned int
gethostaddress(const char* hostname)
{
	unsigned int	ipv4   = 0;

	int		i;
	int		rc;

	char			tmp[46];
	struct addrinfo		hints;
	struct addrinfo		*res;
	struct addrinfo		*res0	= NULL;
	
	memset(&hints,0,sizeof(struct addrinfo));
	hints.ai_family		= PF_UNSPEC;
	hints.ai_socktype	= SOCK_DGRAM;

	/* resolve the domain name into a list of address */
	rc = getaddrinfo(hostname, NULL, &hints, &res0);
	if (rc != 0) {
		if (res0)
			freeaddrinfo(res0);
		
		return 0;
	}
	
	i = 0;
	/* loop over all returned results and do inverse lookup */
	for (res = res0; res != NULL; res = res->ai_next) {

		memset(tmp,0,46);
		switch(res->ai_family) {
		case AF_INET:
			inet_ntop(AF_INET,&((struct sockaddr_in *)res->ai_addr)->sin_addr,tmp,46);

			ipv4 = ntohl( ((struct sockaddr_in *)res->ai_addr)->sin_addr.s_addr );
			if (ipv4 == INADDR_LOOPBACK)
				ipv4 = 0;
			break;
		case AF_INET6:
			inet_ntop(AF_INET6,&((struct sockaddr_in6 *)res->ai_addr)->sin6_addr,tmp,46);
			break;
		}

		i++;
	}
	if (res0)
		freeaddrinfo(res0);

	return ipv4;
}

void
ipc_gbox_init()
{
	int rc;
	struct sockaddr_in	sockaddr;

	gipc.sd_out = socket(PF_INET,SOCK_DGRAM,0);
	if (gipc.sd_out<0) {
		perror("socket()");
		return;
	}
	bzero(&sockaddr,sizeof(sockaddr));
	sockaddr.sin_family	= AF_INET;
	sockaddr.sin_port	= htons(8004);
	sockaddr.sin_addr.s_addr= htonl(gethostaddress("127.0.0.1"));
	
	rc = connect(gipc.sd_out,(struct sockaddr *)&sockaddr, sizeof(sockaddr));
	if (rc < 0) {
		perror("connect()");
		ipc_gbox_free();
		return;
	}

	gipc.sd_in = socket(PF_INET,SOCK_DGRAM,0);
	if (gipc.sd_in<0) {
		perror("socket()");
		return;
	}
	bzero(&sockaddr,sizeof(sockaddr));
	sockaddr.sin_family	= AF_INET;
	sockaddr.sin_port	= htons(8003);
	sockaddr.sin_addr.s_addr= htonl(INADDR_ANY);
	rc = bind(gipc.sd_in,(struct sockaddr *)&sockaddr, sizeof(sockaddr));
	if (rc < 0) {
		perror("bind()");
		ipc_gbox_free();
		return;
	}

	pthread_mutex_init(&ipc_gbox_thread_lock, NULL);
	pthread_attr_init(&ipc_gbox_thread_attr);
	pthread_create(&ipc_gbox_thread_handle, &ipc_gbox_thread_attr, ipc_gbox_task, (void*)"ipcgbox");

}

void
ipc_gbox_free()
{

	ipc_gbox_thread_running = 0;
	pthread_mutex_destroy(&ipc_gbox_thread_lock);

	if (gipc.sd_out)
		close(gipc.sd_out);
	if (gipc.sd_in)
		close(gipc.sd_in);

	gipc.sd_out = 0;
	gipc.sd_in = 0;
}

int
ipc_gbox_slot_open(int id)
{
	int sd;
	int rc;
	struct sockaddr_in	sockaddr;

	sd = socket(PF_INET,SOCK_DGRAM,0);
	if (sd<0) {
		perror("socket()");
		return 0;
	}
	bzero(&sockaddr,sizeof(sockaddr));
	sockaddr.sin_family	= AF_INET;
	sockaddr.sin_port	= htons((8005+id));
	sockaddr.sin_addr.s_addr= htonl(gethostaddress("127.0.0.1"));

	rc = connect(sd,(struct sockaddr *)&sockaddr, sizeof(sockaddr));
	if (rc < 0) {
		perror("connect()");
		close(sd);
		return 0;
	}

	return sd;
}

int
ipc_gbox_send(unsigned char* buf, int size)
{
	int i;
	if (gipc.sd_out)
		return send(gipc.sd_out,buf,size,0);
	return 0;
}

int
ipc_gbox_recv(unsigned char* buf, int size)
{
	struct sockaddr_in	sockaddr;
	int clilen = sizeof(sockaddr);
	if (gipc.sd_in)
		return recvfrom(gipc.sd_in, buf, size, 0, (struct sockaddr *)&sockaddr, &clilen);
	return 0;
}

unsigned int
parse_length_field(unsigned char* buffer, int* length_field)
{
	int		i;
	unsigned char	size_indicator		= (buffer[0] >> 7) & 0x01;
	unsigned int	length_value		= 0;
	int		length_field_size	= 0;
	
	
	if (size_indicator == 0) {
		length_field_size	= 1;
		length_value		= buffer[0] & 0x7F;
	} else {
		length_field_size	= buffer[0] & 0x0F;
		for (i=0;i<length_field_size;i++)
			length_value = (length_value << 8) | buffer[i + 1];
		length_field_size++;
	}
	
	*length_field = length_field_size;
	return length_value;
}

int
ipc_gbox_pmt(unsigned char* pmt, int size)
{
	unsigned char buf[2048] = {0};
	int i,n;
	int offset = 3;
	int length_field_bytes = 0;
	int program_info_length = 0;
	unsigned int pmt_len = parse_length_field(pmt+3,&length_field_bytes);
	if (pmt_len > 4) {
		offset += length_field_bytes;
		memset(buf+4,0,12);
		buf[4] = pmt[offset+1];
		buf[5] = pmt[offset+2];
		buf[6] = pmt[offset+3];
		buf[11]= pmt[offset+4];
		buf[12]= pmt[offset+5];

		program_info_length = ((pmt[offset+4]<<8)|(pmt[offset+5]))&0x0FFF;
		offset += 6;

		n = pmt_len-offset-1;
		memcpy(buf+13,&pmt[offset+1],n);
		n += 13;
		buf[2] = (n>>8)&0xFF;
		buf[3] = (n>>0)&0xFF;
	}
	mca_settings.ecmpids_count = 0;
	if (program_info_length > 0) {
		offset++;
		for (i=0;i<program_info_length;) {
			if (pmt[offset+i+0] == 0x09 && pmt[offset+i+1] != 0x00) {
				mca_settings.ecmpids[mca_settings.ecmpids_count].CAID = (pmt[offset+i+2]<<8)|(pmt[offset+i+3]);
				mca_settings.ecmpids[mca_settings.ecmpids_count].PROVID = 0;
				mca_settings.ecmpids[mca_settings.ecmpids_count].ECM_PID = ((pmt[offset+i+4]<<8)|(pmt[offset+i+5]))&0x1FFF;
				mca_settings.ecmpids[mca_settings.ecmpids_count].EMM_PID = 0;
				mca_settings.ecmpids[mca_settings.ecmpids_count].dummy1 = 0xFE;
				mca_settings.ecmpids[mca_settings.ecmpids_count].dummy2 = 0xFE;
				mca_settings.ecmpids[mca_settings.ecmpids_count].checked = 0;
				mca_settings.ecmpids[mca_settings.ecmpids_count].dummy4 = 1;
				mca_settings.ecmpids[mca_settings.ecmpids_count].dummy5 = 0;
				mca_settings.ecmpids[mca_settings.ecmpids_count].dummy6 = 0;
				mca_settings.ecmpids[mca_settings.ecmpids_count].dummy7 = 0;
				mca_settings.ecmpids_count++;
				i+=pmt[offset+i+1];
			}
			i += 2;
		}
	}


	buf[0] = 0x87;
	buf[1] = 0x02;
	return ipc_gbox_send(buf,((buf[2]<<8)|buf[3]));
}

int
ipc_gbox_ecm(unsigned char* ecm, int size, unsigned int caid, unsigned int pid)
{
	int i;
	unsigned char buf[2048];
	
	for (i=0;i<16;i++) {
		if (gipc.slots[i].sd) {
			if (mca_settings.ecmpids[gipc.slots[i].ecmpidID].CAID == caid && mca_settings.ecmpids[gipc.slots[i].ecmpidID].ECM_PID == pid) {
				buf[0] = 0x88;
				buf[1] = (pid>>8)&0xFF;
				buf[2] = (pid>>0)&0xFF;
				buf[3] = 0xFF;
				memcpy(buf+4,ecm,size);
				return send(gipc.slots[i].sd,buf,4+size,0);
			}
		}
	}
	return 0;
}
