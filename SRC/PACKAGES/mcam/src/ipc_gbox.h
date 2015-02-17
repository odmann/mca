#ifndef _MCAM_IPC_GBOX_H
#define _MCAM_IPC_GBOX_H

#include "mcam.h"

void	ipc_gbox_init	(void);
void	ipc_gbox_free	(void);

int	ipc_gbox_pmt	(unsigned char* pmt, int size);
int	ipc_gbox_ecm	(unsigned char* ecm, int size, unsigned int caid, unsigned int pid);

#endif
