
#include "mcam.h"
#include "mio.h"
#include "util.h"
#include "trace.h"

void signal_handler(int signum)
{
	switch(signum) {
	case SIGINT:	printf("trigger signal SIGINT\n"); break;
	case SIGKILL:	printf("trigger signal SIGKILL\n"); break;
	case SIGTERM:	printf("trigger signal SIGTERM\n"); break;
	default:	printf("trigger signal %d\n",signum); break;
	}

	g_mca_running = 0;

	exit(1);
}

int
main(int argc, char** argv)
{
	int i,n;
	FILE *fd;
	char ini_res[256] = {0};

	if (argc < 1)
		return 0;

	signal(SIGINT, signal_handler);
	signal(SIGTERM,signal_handler);
	signal(SIGKILL,signal_handler);

	mca_settings.mode = 1;

	for (i=1; i<argc; i++) {
		if (!strcmp(argv[i],"install")) {
			mca_settings.full_install = 1;
		} else
		if (!strcmp(argv[i],"mca")) {
			mprintf(0x2821,"starting MCA application !!!!!!\n");
			fd = fopen("/var/mca/install","r");
			mprintf(0x2821,"0 %d\n",fd);
			if (fd) {
				mprintf(0x2821,"1 %d\n",fd);
				fclose(fd);
				mca_settings.full_install = 1;
				mprintf(0x2821,"2\n");
				call_exe("ls -l /var/mca/install", call_exe_res, 1024, 1);
				mprintf(0x2821,"3\n");
				call_exe("rm -f /var/mca/install", call_exe_res, 1024, 1);
				mprintf(0x2821,"4\n");
				call_exe("sync", call_exe_res, 1024, 1);
				call_exe("ls -l /var/mca/install", call_exe_res, 1024, 0);
				mprintf(0x2821,"Full install mode\n");
			}
			mprintf(0x2821,"5\n");

			parse_ini_file("wifi:power", "100", NULL, ini_res);
			if ( !strcmp(ini_res, "ON") || !strcmp(ini_res, "OFF") || !strcmp(ini_res, "on") || !strcmp(ini_res, "off") ) {
				parse_ini_file("wifi:power", NULL, "100", NULL);
				wifi_set_power(100);
			} else {
				wifi_set_power(atoi(ini_res));
			}
			mprintf(0x2821,"2\n");

			mprintf(0x2821,"starting 0\n");

			mprintf(0x2821,"starting 1\n");

			mca_settings.execute = 0;
			parse_ini_file("system:execute", "0", NULL, ini_res);
			if ( !strcmp(ini_res, "0") ) {
				mca_settings.auto_reset = 0;
				parse_ini_file("system:auto_reset", "0", NULL, ini_res);
				if ( !strcmp(ini_res, "1") )
					mca_settings.auto_reset = 1;
			} else {
				mca_settings.execute = 1;
				/* execute code */
			}

			mprintf(0x2821,"started MCA application\n");
		} else
		if (!strcmp(argv[i],"t1")) {
			// test_cfg()
		} else
		if (!strcmp(argv[i],"test")) {
			parse_ini_file("system:flt", "OFF", NULL, ini_res);
			mca_settings.filter_control = 0;
			if ( !strcmp(ini_res, "ON") || !strcmp(ini_res, "on") )
				mca_settings.filter_control = 1;
			//parse_ini_file("system:max_num_of_list", "OFF", NULL, ini_res);
			//parse_ini_file("system:max_num_of_column", "OFF", NULL, ini_res);

			g_mca_running = 1;
			mio_init();
		} else
		if (!strcmp(argv[i],"s")) {
			mca_settings.sim = 1;
		}
	}

	while (g_mca_running) { 
		sleep(2); 
	}

	mio_term();

	mprintf(0x2821,"exit MCA application!!!!!!\n");

	return 0;
}
