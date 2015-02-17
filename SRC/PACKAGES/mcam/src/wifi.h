#ifndef _MCAM_WIFI_H
#define _MCAM_WIFI_H

#include "globals.h"

typedef struct wifi_network {
	int	channel;
	char	ssid[48];
	char	bssid[20];
	char	security[20];
	int	signal;
	char	wmode[16];
	char	nt[16];
} wifi_network_t;

extern int		wifi_networks_size;
extern wifi_network_t	wifi_networks[32];

void		wifi_set_power	(int p);
int		wifi_parse_info	(char *str);

#endif
