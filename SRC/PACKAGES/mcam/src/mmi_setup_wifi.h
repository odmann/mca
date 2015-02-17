#ifndef _MCAM_MMI_SETUP_WIFI_H
#define _MCAM_MMI_SETUP_WIFI_H

#include "mmi.h"

int	mmi_setup_wifi_show(struct mmi_desc *parent);
int	mmi_setup_wifi_dispatch(struct mmi_desc *parent);

int	mmi_setup_wifi_survey_show(struct mmi_desc *parent);
int	mmi_setup_wifi_survey_dispatch(struct mmi_desc *parent);

int	mmi_setup_wifi_advanced_show(struct mmi_desc *parent);
int	mmi_setup_wifi_advanced_dispatch(struct mmi_desc *parent);

#endif
