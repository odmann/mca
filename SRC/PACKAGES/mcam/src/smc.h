#ifndef _MCAM_SMC_H_
#define _MCAM_SMC_H_

#include "globals.h"

typedef struct smc_msg {
	unsigned char	command;
	unsigned char	data[512];
} smc_msg_t;

typedef struct smc_ecm {
	unsigned int	caid;
	unsigned int	pid;
	unsigned char	dvb_msg[504];
} smc_ecm_t;

int	smc_dispatch	(void *data, int size);

void	smc_reset	(void);
void	smc_set_led	(unsigned char state);

void	smc_init	(void);
void	smc_free	(void);

#endif
