#include "mmi_setup_wifi.h"
#include "mmi_setup.h"
#include "util.h"
#include "wifi.h"

int
mmi_setup_wifi_show(struct mmi_desc *parent)
{
	char item[256] = {0};
	char ssid[128] = {0};

	parse_ini_file("wifi:ssid", NULL, NULL, ssid);
	snprintf(item,sizeof(item),"Easy Mode:%s",ssid);

	mmi_reset();
	mmi_add_item(2, "MCA/Setup/.../Wireless");
	mmi_add_item(3, "Wireless");
	mmi_add_item(4, "Please select");
	mmi_add_item(5, item);
	mmi_add_item(5, "Advanced Mode");
	mmi_show();
	parent->level++;
	parent->waiting_for_response = 1;
}

int
mmi_setup_wifi_dispatch(struct mmi_desc *parent)
{
	switch(parent->response) {
	case 0: // <-- back
		parent->level--;
		parent->show	= mmi_setup_show;
		parent->dispatch= mmi_setup_dispatch;
		break;
	case 1:
		parent->show	= mmi_setup_wifi_survey_show;
		parent->dispatch= mmi_setup_wifi_survey_dispatch;
		break;
	case 2:
		parent->show	= mmi_setup_wifi_advanced_show;
		parent->dispatch= mmi_setup_wifi_advanced_dispatch;
		break;
	}
	parent->response = -1;
}

int
mmi_setup_wifi_survey_show(struct mmi_desc *parent)
{
	int i;
	char item[128];

	call_exe("iwpriv ra0 get_site_survey", call_exe_res, 4096, 0);
	wifi_parse_info(call_exe_res);

	mmi_reset();
	mmi_add_item(2, "MCA/Setup/.../Easy Mode");
	mmi_add_item(3, "Easy Mode");
	mmi_add_item(4, "Please select");

	for (i=0;i<wifi_networks_size;i++) {
		memset(item,0,sizeof(item));
		snprintf(item,sizeof(item),"%s(%d%%)",wifi_networks[i].ssid,wifi_networks[i].signal);
		mmi_add_item(5, item);
	}
	mmi_show();
	parent->level++;
	parent->waiting_for_response = 1;
}

int
mmi_setup_wifi_survey_dispatch(struct mmi_desc *parent)
{
	switch(parent->response) {
	case 0: // <-- back
		parent->level--;
		parent->show	= mmi_setup_wifi_show;
		parent->dispatch= mmi_setup_wifi_dispatch;
		break;
	default:
		if (parent->response < wifi_networks_size) {
			wifi_network_t *network = &wifi_networks[(parent->response)-1];
			
			parse_ini_file("wifi:ssid", 0, network->ssid, 0);
			if (strstr(network->security,"WPA2PSK")) {
				parse_ini_file("wifi:authmode", 0, "WPA2PSK", 0);
				if (strstr(network->security,"AES")) {
					parse_ini_file("wifi:encryptype", 0, "AES", 0);
				}
			}
			break;
		}
	}
	parent->response = -1;
}

int
mmi_setup_wifi_advanced_show(struct mmi_desc *parent)
{
}

int
mmi_setup_wifi_advanced_dispatch(struct mmi_desc *parent)
{
}

