#include "trace.h"

void
mprintf(unsigned int m, const char *fmt, ...)
{
	char	buf[1024];
	va_list	va;
	
	va_start(va, fmt);
	snprintf(buf,1024,"\x1B[%dm\x1B[%dm%s%s",(m>>8)&0xFFFF,(m&0xFF),fmt,"\x1B[0m");
	vprintf(buf, va);
	va_end(va);
}

