#ifndef _MCAM_GLOBALS_H
#define _MCAM_GLOBALS_H

#include "mcam.h"

extern	int	g_mca_running;

#define ECM_PIDS_MATRIX 20

typedef struct mca_ecmpid
{
	unsigned int	CAID;
	unsigned int	PROVID;
	unsigned int	ECM_PID;
	unsigned int	EMM_PID;
	unsigned int	dummy1;
	unsigned int	dummy2;
	unsigned int	checked; /* dummy3; */
	unsigned int	dummy4;
	unsigned int	dummy5;
	unsigned int	dummy6;
	unsigned int	dummy7;
} mca_ecmpid_t;

typedef struct mca_setting {
	struct {
		unsigned char model[128];
		unsigned short version;
	} host;

	struct {
		unsigned short version;
	} protocol;
	/* mca runtime settings */
	int		mode;
	int		sim;
	int		full_install;
	int		execute;
	/* cas flags */
	int		cw_mode;
	int		cardinfo_changed;
	int		filter_control;
	int		ecm_filter_changed;
	int		new_cam_sw;
	int		auto_reset;
	int		reset_desc;
	int		ecmpids_count;
	mca_ecmpid_t	ecmpids[ECM_PIDS_MATRIX];

	struct {
		unsigned char even[8];
		unsigned char odd[8];
	} dcw;
} mca_setting_t;

extern mca_setting_t	mca_settings;

#endif /* _MCAM_GLOBALS_H */
