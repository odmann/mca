#include "mmi_setup.h"
#include "mmi_setup_wifi.h"

#include "mmi_main.h"

int
mmi_setup_show(struct mmi_desc *parent)
{
	mmi_reset();
	mmi_add_item(2, "MCA/Setup");
	mmi_add_item(3, "Setup");
	mmi_add_item(4, "Please select");
	mmi_add_item(5, "Wireless");
	mmi_add_item(5, "Network");
	mmi_add_item(5, "System");
	mmi_show();
	parent->level++;
	parent->waiting_for_response = 1;
}

int
mmi_setup_dispatch(struct mmi_desc *parent)
{
	switch(parent->response) {
	case 0: // <-- back
		parent->level--;
		parent->show	= mmi_main_show;
		parent->dispatch= mmi_main_dispatch;
		break;
	case 1:
		parent->show	= mmi_setup_wifi_show;
		parent->dispatch= mmi_setup_wifi_dispatch;
		break;
	case 2:
		break;
	case 3:
		break;
	}
}
