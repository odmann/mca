#ifndef _MCAM_MMI_H_
#define _MCAM_MMI_H_

#include "globals.h"

typedef struct mmi_desc {
	int	menu;
	int	level;
	int	response;
	int	waiting_for_response;
	//
	int	(*show)		(struct mmi_desc* parent);
	int	(*dispatch)	(struct mmi_desc* parent);
	//
} mmi_desc_t;

int	mmi_dispatch	(void *data, int size);

void	mmi_reset	(void);
void	mmi_add_item	(unsigned char type, const char *item);
void	mmi_show	(void);
void	mmi_close	(void);

void	mmi_init	(void);
void	mmi_free	(void);


#endif // _MCAM_MMI_H_
