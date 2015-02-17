#include "mmi_main.h"
#include "mmi_setup.h"
#include "mmi_utility.h"
#include "mmi_upgrade.h"
#include "mmi_filter.h"

int
mmi_main_show(struct mmi_desc *parent)
{
	mmi_reset();
	mmi_add_item(2, "MCA");
	mmi_add_item(3, "Main Menu");
	mmi_add_item(4, "Please select");
	mmi_add_item(5, "Setup");
	mmi_add_item(5, "Utility");
	mmi_add_item(5, "Easy FTP Upgrade");
	mmi_add_item(5, "Filter");
	mmi_show();
	parent->level++;
	parent->waiting_for_response = 1;
}

int
mmi_main_dispatch(struct mmi_desc *parent)
{
	switch(parent->response) {
	case 0: // <-- back
		parent->level--;
		break;
	case 1:
		parent->show	= mmi_setup_show;
		parent->dispatch= mmi_setup_dispatch;
		break;
	case 2:
		parent->show	= mmi_utility_show;
		parent->dispatch= mmi_utility_dispatch;
		break;
	case 3:
		parent->show	= mmi_upgrade_show;
		parent->dispatch= mmi_upgrade_dispatch;
		break;
	case 4:
		parent->show	= mmi_filter_show;
		parent->dispatch= mmi_filter_dispatch;
		break;
	}
}
