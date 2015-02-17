#include "smc.h"
#include "mio.h"

#include "ipc_gbox.h"

int
smc_dispatch(void *data, int size)
{
	int	i,n;
	smc_msg_t* msg = data;

	if (size <= 0)
		return 0;

	switch(msg->data[3]) {
	case 0x01: { // cam model info
		n = strlen(&msg->data[6]) + 1;
		if (n>(sizeof(mca_settings.host.model)-1))
			n = (sizeof(mca_settings.host.model)-1);
		if (n>size)
			n = size;
		memcpy(mca_settings.host.model,&msg->data[6],n);
		mca_settings.host.model[n] = '\0';
		mca_settings.host.version = (msg->data[6+n]<<8)|msg->data[6+n+1];
		msg->command = 0x74;
		msg->data[5] = 0x01;
		msg->data[6] = 0x00;
		msg->data[7] = 0x01;
		mio_write(msg,0x0E);
		//
		break;
	}
	case 0x02: { // state
		// process message
		mca_settings.protocol.version = msg->data[4];
		// response create and send
		msg->command = 0x74;
		msg->data[5] = 0x02;
		msg->data[6] = 0x00;
		msg->data[7] = 0x01;
		// set flags
		msg->data[8] = 0;
		msg->data[8] |= ((mca_settings.cw_mode&1)<<0);
		msg->data[8] |= ((mca_settings.cardinfo_changed&1)<<1);
		msg->data[8] |= ((mca_settings.filter_control&1)<<2);
		msg->data[8] |= ((mca_settings.new_cam_sw&1)<<3);
		msg->data[8] |= ((mca_settings.ecm_filter_changed&1)<<4);
		msg->data[8] |= ((mca_settings.auto_reset&1)<<5);
		msg->data[8] |= ((mca_settings.reset_desc&1)<<6);
		mca_settings.reset_desc = 0;
		//
		if (msg->data[8]&1) {
			mca_settings.cw_mode = 0;
			msg->data[9] = 0;
			msg->data[10]= 0;
			msg->data[11]= 0;
			memcpy(msg->data+12,&mca_settings.dcw,16);
			if (memcmp(mca_settings.dcw.even,"\x00\x00\x00\x00\x00\x00\x00\x00",8))
				msg->data[10]= 1;
			if (memcmp(mca_settings.dcw.odd,"\x00\x00\x00\x00\x00\x00\x00\x00",8))
				msg->data[11]= 1;
			if (msg->data[10] == 1 ||msg->data[11] == 1) {
				mio_write(msg,0x21);
				break;
			}
			msg->data[8] &= 0xFE;
		}
		mio_write(msg,0x0E);
		//
		break;
	}
	case 0x34: { // pmt data
		//
		if (msg->data[6] == 0x9F && msg->data[7] == 0x80 && msg->data[8] == 0x32) {
			// process pmt data
			#if defined(_MCAM_IPC_GBOX_H)
			ipc_gbox_pmt(msg->data+6,size-7);
			#endif
		}

		// response create and send
		msg->command = 0x74;
		msg->data[5] = msg->data[3];
		msg->data[6] = msg->data[4];
		msg->data[7] = 3;
		msg->data[8] = 0;
		mio_write(msg,14);
		//
		break;
	}
	case 0x35:  { // ecm data
		//
		// process ecm data
		#if defined(_MCAM_IPC_GBOX_H)
		ipc_gbox_ecm(msg->data+14,msg->data[16]+3,(msg->data[6]<<8)|msg->data[7],(msg->data[10]<<8)|msg->data[11]);
		#endif
		//usleep(80000);
		// response create and send
		msg->command = 0x74;
		msg->data[5] = msg->data[3];
		msg->data[6] = msg->data[4];
		msg->data[7] = 3;
		msg->data[8] = 0;
		mio_write(msg,14);
		//
		break;
	}
	case 0x37: { // dcw request
		//
		// response create and send
		msg->command = 0x74;
		msg->data[5] = msg->data[3];
		msg->data[6] = 0;
		msg->data[7] = 1;
		msg->data[8] = 0;
		msg->data[9] = 0;
		mio_write(msg,15);
		//
		break;
	}
	case 0x38: { // pid filter
		msg->command = 0x74;
		msg->data[5] = 0x02;
		msg->data[6] = 0x00;
		msg->data[7] = 0x01;
		msg->data[8] = 0x00;
		if (mca_settings.ecm_filter_changed) {
			mca_settings.ecm_filter_changed = 0;
			msg->data[6] = 0x02;
			msg->data[9] = 0x00;
			int nbytes = 10;
			for (i=0;i<mca_settings.ecmpids_count;i++) {
				if (mca_settings.ecmpids[i].checked) {
					memcpy(msg->data+nbytes, &mca_settings.ecmpids[i], sizeof(mca_ecmpid_t));
					nbytes += sizeof(mca_ecmpid_t);
					msg->data[9] = msg->data[9] + 1;
				} 
			}
			mio_write(msg,nbytes+3);
			break;
		}
		mio_write(msg,13);
		break;
	}
	} // switch()
	return 0;
}

void
smc_reset()
{
	unsigned char cmd[] = { 0x72, 0x00, 0x00, 0x00 };
	mio_write(cmd,1);
}

void
smc_set_led(unsigned char state)
{
	unsigned char cmd[] = { 0x00, 0x00, 0x00, 0x00 };
	cmd[0] = state;
	mio_write(cmd,1);
}

void
smc_init()
{
	smc_set_led(0x75);
	smc_reset();
	#if defined(_MCAM_IPC_GBOX_H)
	ipc_gbox_init();
	#endif
}

void
smc_free()
{
	#if defined(_MCAM_IPC_GBOX_H)
	ipc_gbox_free();
	#endif
}
