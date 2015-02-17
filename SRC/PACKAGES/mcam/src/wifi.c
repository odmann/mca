#include "wifi.h"
#include "util.h"

int		wifi_networks_size = 0;
wifi_network_t	wifi_networks[32] = {0};

void
wifi_set_power(int p)
{
	char s[64] = {0};
	snprintf(s,sizeof(s),"iwpriv ra0 set TxPower=%d", p);
	call_exe(s, call_exe_res, 4096, 0);
}

int
wifi_parse_info(char *str)
{
	char 	*p = str;
	int	cnt = count_line(str);
	char	line[256] = {0};
	char	sval[256] = {0};

	p = skip_line(p, 2);

	wifi_networks_size = 0;
	while ( *p ) {
		wifi_network_t *network = &wifi_networks[wifi_networks_size];
		memset(network,0,sizeof(wifi_network_t));

		p = get_line(p, line);
		get_mid(line, 0, 4, sval);
		trim(sval);
		network->channel = atoi(sval);
		get_mid(line, 4, 37, sval);
		trim(sval);
		strcpy(network->ssid, sval);
		get_mid(line, 37, 57, sval);
		trim(sval);
		strcpy(network->bssid, sval);
		get_mid(line, 57, 80, sval);
		trim(sval);
		strcpy(network->security, sval);
		get_mid(line, 80, 89, sval);
		trim(sval);
		network->signal = atoi(sval);
		get_mid(line, 89, 97, sval);
		trim(sval);
		strcpy(network->wmode, sval);
		get_mid(line, 97, 107, sval);
		trim(sval);
		strcpy(network->nt, sval);

		wifi_networks_size++;
	}
}
