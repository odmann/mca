#include "mmi_upgrade.h"

#include "mmi_main.h"

int
mmi_upgrade_show(struct mmi_desc *parent)
{
	mmi_reset();
	mmi_add_item(2, "MCA/Upgrade");
	mmi_add_item(3, "Upgrade");
	mmi_add_item(4, "Please select");
	mmi_show();
	parent->level++;
	parent->waiting_for_response = 1;
}

int
mmi_upgrade_dispatch(struct mmi_desc *parent)
{
	switch(parent->response) {
	case 0: // <-- back
		parent->level--;
		parent->show	= mmi_main_show;
		parent->dispatch= mmi_main_dispatch;
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	}
}
