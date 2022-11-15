/*
 * afe.c
 *
 *  Created on: Nov 15, 2022
 *      Author: pujak
 */
#include "afe.h"
#include "main.h"
#include "uart/gui-driver/gui_evt.h"
#include <stdint.h>
#include <string.h>

#define X_SIZE	1024
#define Y_SIZE 	2048

/* Private typedef */
typedef enum {
	EvtID_Status = 0x1,
	EvtID_TchEnter,
	EvtID_TchMove,
	EvtID_TchLeave,
	EvtID_Debug = 0xD,
	EvtID_Error,
}eEvtID_t;

/* private function definitions */
static void afe_rptFrm(void)
{
	static int16_t s16_frm[4][TX_LEN*RX_LEN];
	uint8_t u8_val;
	uint8_t u8_i;

	u8_val = ((uint8_t) s16_frm[0][0] + 1);

	/* generate fake frame section process */
	for(u8_i=0; u8_i<sizeof(s16_frm)/sizeof(s16_frm[0]); u8_i++)
	{
		memset(&(s16_frm[u8_i]), u8_val + u8_i, sizeof(s16_frm[u8_i]));
		vGuiEvt_pushRptFrm(u8_i + RptFrmTyp_MS_Raw, &(s16_frm[u8_i]));
	}
}

static void afe_rptTch(void)
{
	static sRptTchInfo_t rptTchInfo[2];
	uint8_t u8_i;

	/* generate fake touch */
	rptTchInfo[0].u16_coorX += 5;
	rptTchInfo[0].u16_coorY += 10;
	rptTchInfo[1].u16_coorX = X_SIZE - rptTchInfo[0].u16_coorX;
	rptTchInfo[1].u16_coorY = Y_SIZE - rptTchInfo[0].u16_coorY;

	for(u8_i=0; u8_i<sizeof(rptTchInfo)/sizeof(rptTchInfo[0]); u8_i++)
	{
		rptTchInfo[u8_i].u8_coorZ = 0;
		rptTchInfo[u8_i].u8_tchID = u8_i;
		rptTchInfo[u8_i].u8_evtID = (EvtID_TchEnter > rptTchInfo[u8_i].u8_evtID)
				? EvtID_TchEnter : EvtID_TchMove;

		vGuiEvt_pushRptTch(&(rptTchInfo[u8_i]));
	}
}

/* public function definitions */
void afe_scan(void)
{
	afe_rptFrm();
	afe_rptTch();

	vGuiEvt_finishReport();
}
