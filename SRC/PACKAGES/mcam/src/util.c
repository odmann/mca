#include "util.h"

#include "dictionary.h"
#include "iniparser.h"

char*	ini_cur		= NULL;
char*	ini_default	= "/var/mca/default.ini";
char*	ini_name	= "/var/mca/mca.ini";
//char*	ini_default	= "default.ini";
//char*	ini_name	= "mca.ini";

char	call_exe_res[4096] = {0};

void
call_exe(const char* command, char* result, int rsize, int show_result)
{
	int fdd;
	int fd[2];
	fdd = dup(fileno(stdout));
	memset(result,0,rsize);
	pipe(fd);
	dup2(fd[1],fileno(stdout));
	close(fd[1]);
	int rc = system(command);
	wait(rc);
	dup2(fdd,fileno(stdout));
	close(fdd);
	int n = read(fd[0], result, rsize);
	rtrim(result);
	result[n-1] = 0;
	if (show_result)
		printf(result);
}

int
check_inifile()
{
	FILE	*fd;
	int	rc;

	rc = 1;

	ini_cur = ini_default;
	fd = fopen(ini_default, "r");
	if (fd) {
		rc = 2;
		fclose(fd);
	}

	ini_cur = ini_name;
	fd = fopen(ini_name, "r");
	if (fd) {
		rc = 3;
		fclose(fd);
	}

	ini_cur = "/mnt/sd/mcaboot.ini";
	fd = fopen("/mnt/sd/mcaboot.ini", "r");
	if (fd) {
		rc = 4;
		fclose(fd);
	}

	return rc;
}

int
iniparser_get_section_string(char *key, char* section)
{
	int i;
	for (i=0;i<256;i++){
		if (!key[i])
			break;
		if (key[i] == ':') {
			memcpy(section,key,i);
			section[i] = 0;
			return 1;
		}
	}
	return 0;
}

int
parse_ini_file(char *key, char *def, char *val, char *res)
{
	FILE		*fd;
	dictionary	*d;
	int		ini_update = 0;
	char		section[256] = {0};

	switch( check_inifile() ) {
	case 0:
		return 0;
	case 1:
		return 1;
	case 2:
		ini_cur = ini_default;
		d = iniparser_load(ini_default);
		if (d) {
			fd = fopen(ini_name, "w");
			if (fd) {
				iniparser_dump_ini(d, fd);
				fclose(fd);
				iniparser_freedict(d);
				ini_cur = ini_name;
				ini_update = 1;
				break;
			}
		} else {
			fprintf(stderr, "cannot parse file: %s\n",ini_cur);
		}
		return 0;
		break;
	case 3:
		ini_cur = ini_name;
		break;
	case 4:
		ini_cur = "/mnt/sd/mcaboot.ini";
		break;
	default:
		break;
	}

	d = iniparser_load(ini_cur);
	if (!d) {
		fprintf(stderr, "cannot parse file: %s\n", ini_cur);
		return -1;
	}

	if (key && val) {
		char *s = iniparser_getstring(d, key, NULL);
		if (s) {
			if (strcmp(s, val)) {
				iniparser_set(d, key, val);
				ini_update = 1;
			}
		} else {
			iniparser_get_section_string(key, section);
			iniparser_set(d, section, NULL);
			iniparser_set(d, key, val);
			ini_update = 1;
		}
	}

	if (key && res) {
		char *s = iniparser_getstring(d, key, def);
		strcpy(res, s);
	}

	if (ini_update) {
		fd = fopen(ini_name, "w");
		if (fd) {
			iniparser_dump_ini(d, fd);
			fflush(fd);
			fclose(fd);
		}
	}
	iniparser_freedict(d);
	return 0;
}

/* **************************************************************************** *
	Helper
 * **************************************************************************** */
void
ltrim(char *str)
{
	char *p = str;
	int n = strlen(str);
	int i,j;
	for (i=0;i<n && (isspace(*p) || (*p < 0x21));i++) { };
	if (i > n)
		memset(p, 0, n);
	for (j=0;j<(n-i);j++)
		p[j] = p[i+j];
	while (j<n)
		p[j++] = 0;
}

void
rtrim(char *str)
{
	char *end = str + strlen(str) - 1;
	while ( end > str && (isspace(*end) || !(*end)) ) {
		*end = 0;
		end--;
	}
}

void
trim(char *str)
{
	ltrim(str);
	rtrim(str);
}

void
task_delay(int dly)
{
	int i;
	for (i=dly; i>1; i-=2)
		usleep(2000);
	if (i)
		usleep(1000*i);
}

int
count_line(char *str)
{
	int cnt = 0;
	char *p = str;
	while ( *p ) {
		if ( *p == 0x0A)
			cnt++;
		++p;
	}
	return cnt;
}

char*
skip_line(char *str, int linetoskip)
{
	int line = 1;
	char *p = str;
	if (linetoskip < line)
		return p;
	while ( *p ) {
		if (*p == 0x0A) {
			line++;
			if (line > linetoskip)
				return (p + 1);
		}
		++p;
	}
	return p;
}

char*
get_line(char *str, char *line)
{
	char *p = str;
	char *l = line;
	while ( *p ) {
		*l++ = *p;
		if ( *p == 0x0A ) {
			*l = 0;
			return (p+1);
		}
		++p;
	}
	*l = 0;
	return p;
}

void
get_mid(char *str, int spos, int epos, char* mid)
{
	memcpy(mid, str+spos, epos - spos);
	mid[epos-spos] = 0;
}
