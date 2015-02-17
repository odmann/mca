#ifndef _MCAM_MIO_H
#define _MCAM_MIO_H

#include "globals.h"

#if defined(DEBUG_MIO)
	#define	DEBUG_MIO_READ
	#define DEBUG_MIO_WRITE
#endif

int	mio_read	(void* data, int size);
int	mio_write	(void* data, int size);

void	mio_init	(void);
void	mio_term	(void);

#endif // _MCAM_MIO_H
