#include "mmi.h"
#include "mio.h"
#include "trace.h"

#include "mmi_main.h"

mmi_desc_t	g_mmi	= {0};

static struct {
	unsigned char	preamble[9];
	unsigned char	context[4096];
	int		length;
} mmi_msg;

int
mmi_dispatch(void* data, int size)
{
	int inputvalue;
	char *buf = data;
	mprintf(0x2C25,"main_menu_handler\n");

	memcpy(mmi_msg.preamble,data,9);

	if (!g_mmi.show)
		g_mmi.show = mmi_main_show;
	if (!g_mmi.dispatch)
		g_mmi.dispatch = mmi_main_dispatch;

	inputvalue = -1;
	if (buf[5] == 0x00 && buf[6] == 0x00) {
		g_mmi.show = mmi_main_show;
		g_mmi.dispatch = mmi_main_dispatch;
		g_mmi.waiting_for_response = 0;
	} else
	if (buf[5] == 0x20 && buf[6] == 0x03) {
		inputvalue = buf[7];
	} else
	if (buf[5] == 0x21 && buf[6] == 0x03) {
		inputvalue = buf[7];
	}

	if (g_mmi.waiting_for_response) {
		g_mmi.response = inputvalue;
		g_mmi.waiting_for_response = 0;

		/* dispatch input */
		if (g_mmi.dispatch)
			g_mmi.dispatch(&g_mmi);
	}

	printf("%s level %d -> ",__FUNCTION__,g_mmi.level);

	/* dispatch ui */
	if (g_mmi.show)
		g_mmi.show(&g_mmi);

	printf("%d\n",g_mmi.level);

	return 0;
}

void
mmi_reset()
{
	mmi_msg.length = 0;
}

void
mmi_add_item(unsigned char type, const char *item)
{
	if (mmi_msg.length == 0) {
		mmi_msg.context[mmi_msg.length++] = 0x00;
		mmi_msg.context[mmi_msg.length++] = 'M';
		mmi_msg.context[mmi_msg.length++] = 0x00;
	}
	int ilength = strlen(item);
	if (ilength < 1 || (mmi_msg.length+ilength+1)>4096)
		return;
	mmi_msg.context[mmi_msg.length++] = type;
	memcpy(&mmi_msg.context[mmi_msg.length], item, ilength);
	mmi_msg.length += ilength;
	mmi_msg.context[mmi_msg.length++] = 0;
}

void
mmi_show()
{
	mmi_msg.preamble[0] = 0x74;
	mmi_msg.preamble[6] = 0x51;
	mmi_msg.preamble[7] = 0x13;
	mmi_msg.preamble[8] = 0x01;
	mio_write(&mmi_msg,mmi_msg.length+13);
}

void
mmi_close()
{
	mmi_msg.preamble[0] = 0x74;
	mmi_msg.preamble[6] = 0x51;
	mmi_msg.preamble[7] = 0x11;
	mmi_msg.preamble[8] = 0x01;
	memset(mmi_msg.context,0,4);
	mio_write(&mmi_msg,13);
	memset(&g_mmi,0,sizeof(mmi_desc_t));
}

void
mmi_init()
{
	memset(&g_mmi,0,sizeof(mmi_desc_t));
	mmi_reset();
}

void
mmi_free()
{
}
