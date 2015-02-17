#include "mio.h"
#include "smc.h"
#include "mmi.h"
#include "trace.h"

pthread_t	mio_thread_handle	= 0;
pthread_attr_t	mio_thread_attr		={{0}};
pthread_mutex_t	mio_thread_lock		= PTHREAD_MUTEX_INITIALIZER;

void*
mio_task(void* arg)
{
	int n;
	unsigned char buf[4096];

	mprintf(0x2821, "starting command handler\n");

	mmi_init();
	smc_init();

	memset(buf,0,sizeof(buf));
	while (g_mca_running) {
		n = mio_read(buf,4095);
		if (n>0) {
			if (buf[4] == 0x51 /*&& buf[5] == 0x00 && buf[6] == 0x00*/)
				mmi_dispatch(buf,n);
			else
				smc_dispatch(buf,n);
			memset(buf,0,n+1);
		}
	}

	smc_free();
	mmi_free();

	mprintf(0x2821, "exit command handler\n");

	pthread_exit(0);

	return 0;
}


/* ****************************************************************************************** *
 * MIO Functions
 * ****************************************************************************************** */
int
mio_read(void* data, int size)
{
	FILE* fd_mio;
	int n;
	unsigned char* buf = data;

	n = 0;
	if (size < 4)
		return n;

	fd_mio = fopen("/proc/mio","rb");
	if (fd_mio) {
		n = fread(buf,1,4,fd_mio);
		if (n==4) {
			n = (buf[2]<<8)|buf[3];
			if ((n+4)>size)
				n = size-4;
			n = fread(buf+4,1,n,fd_mio) + 4;
		} else {
			n = 0;
		}
		fclose(fd_mio);
	}
#if defined(DEBUG_MIO_READ)
	if (n>0) {
		int i;
		printf("[r] <%02X>",buf[0]);
		for (i=0;i<(n-1);i++) {
			printf("%02X",buf[1+i]);
			if (i==3)
				printf(" ");
		}
		printf("\n");
	}
#endif
	return n;
}

int
mio_write(void* data, int size)
{
	FILE* fd_mio;
	int n;
	unsigned char* buf = data;
	
	n = 0;
	fd_mio = fopen("/proc/mio","wb");
	if (fd_mio) {
		n = fwrite(data,size,1,fd_mio);
		fclose(fd_mio);
	}
#if defined(DEBUG_MIO_WRITE)
	if (n>0) {
		int i;
		printf("[w] <%02X>",buf[0]);
		for (i=0;i<(size-1);i++) {
			printf("%02X",buf[1+i]);
			if (i==3)
				printf(" ");
		}
		printf("\n");
	}
#endif
	return n;
}

void
mio_init()
{
	pthread_mutex_init(&mio_thread_lock, NULL);
	pthread_attr_init(&mio_thread_attr);
	pthread_create(&mio_thread_handle, &mio_thread_attr, mio_task, (void*)"mcam_mio_task");
}

void
mio_term()
{
	if (mio_thread_handle)
		pthread_join(mio_thread_handle, NULL);
	pthread_mutex_destroy(&mio_thread_lock);
}
