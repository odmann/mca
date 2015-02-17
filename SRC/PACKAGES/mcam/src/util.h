#ifndef _MCAM_UTIL_H
#define _MCAM_UTIL_H

#include "globals.h"

extern char call_exe_res[4096];

void	call_exe	(const char* command, char* result, int rsize, int show_result);
int	parse_ini_file	(char *key, char *def, char *val, char *res);

/* Helper */
void	ltrim		(char *str);
void	rtrim		(char *str);
void	trim		(char *str);
void	task_delay	(int dly);
int	count_line	(char *str);
char	*skip_line	(char *str, int linetoskip);
char	*get_line	(char *str, char *line);
void	get_mid		(char *str, int spos, int epos, char *mid);


#endif

