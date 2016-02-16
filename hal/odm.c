/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/

/*
 * ============================================================
 * include files
 * ============================================================
 */

#include "odm_precomp.h"
#include <../rtl8821au/reg.h>
#include <../rtl8821au/dm.h>
#include <../wifi.h>
#include <rtl8812a_cmd.h>

#define ODM_RT_ASSERT(x, ...)	do { } while (0);

const u16 dB_Invert_Table[8][12] = {
	{	1,		1,		1,		2,		2,		2,		2,		3,		3,		3,		4,		4},
	{	4,		5,		6,		6,		7,		8,		9,		10,		11,		13,		14,		16},
	{	18,		20,		22,		25,		28,		32,		35,		40,		45,		50,		56,		63},
	{	71,		79,		89,		100,	112,	126,	141,	158,	178,	200,	224,	251},
	{	282,	316,	355,	398,	447,	501,	562,	631,	708,	794,	891,	1000},
	{	1122,	1259,	1413,	1585,	1778,	1995,	2239,	2512,	2818,	3162,	3548,	3981},
	{	4467,	5012,	5623,	6310,	7079,	7943,	8913,	10000,	11220,	12589,	14125,	15849},
	{	17783,	19953,	22387,	25119,	28184,	31623,	35481,	39811,	44668,	50119,	56234,	65535}
	};

/*
 * 20100515 Joseph: Add global variable to keep temporary scan list for antenna switching test.
 * u8			tmpNumBssDesc;
 * RT_WLAN_BSS	tmpbssDesc[MAX_BSS_DESC];
 */

/*
 * ============================================================
 * EDCA Paramter for AP/ADSL   by Mingzhi 2011-11-22
 * ============================================================
 */

/*
 * ============================================================
 *  Global var
 * ============================================================
 */



/*
 * ============================================================
 * Local Function predefine.
 * ============================================================
 */

/* START------------COMMON INFO RELATED--------------- */
/*
void odm_FindMinimumRSSI(struct _rtw_dm *pDM_Odm);
void odm_IsLinked(struct _rtw_dm *pDM_Odm);
*/
/* END------------COMMON INFO RELATED--------------- */

/* START---------------DIG--------------------------- */
void odm_DIGInit(struct _rtw_dm *pDM_Odm);
void odm_AdaptivityInit(struct _rtw_dm *pDM_Odm);
/* END---------------DIG--------------------------- */

/* END---------BB POWER SAVE----------------------- */

/* START-----------------PSD----------------------- */
/* END-------------------PSD----------------------- */

void odm_RateAdaptiveMaskInit(struct _rtw_dm *pDM_Odm);
void odm_TXPowerTrackingThermalMeterInit(struct _rtw_dm *pDM_Odm);
void odm_TXPowerTrackingInit(struct _rtw_dm *pDM_Odm);
void odm_TXPowerTrackingCheckCE(struct _rtw_dm *pDM_Odm);

#define	RxDefaultAnt1		0x65a9
#define	RxDefaultAnt2		0x569a

/*
 * ============================================================
 * Export Interface
 * ============================================================
 */




/*
 * Init /.. Fixed HW value. Only init time.
 */


void ODM_CmnInfoHook(struct _rtw_dm *pDM_Odm, ODM_CMNINFO_E	CmnInfo, void *pValue)
{
	struct rtl_priv *rtlpriv = pDM_Odm->rtlpriv;
	struct rtl_mac *mac = rtl_mac(rtlpriv);
	/*
	 * Hook call by reference pointer.
	 */
	switch (CmnInfo) {
	/*
	 * Dynamic call by reference pointer.
	 */

	case	ODM_CMNINFO_WM_MODE:
		pDM_Odm->pWirelessMode = (u8 *)pValue;
		break;

	case	ODM_CMNINFO_SEC_MODE:
		pDM_Odm->pSecurity = (u8 *)pValue;
		break;

	case	ODM_CMNINFO_SCAN:
		mac->act_scanning = (bool *)pValue;
		break;

	case	ODM_CMNINFO_POWER_SAVING:
		pDM_Odm->pbPowerSaving = (bool *)pValue;
		break;

	case	ODM_CMNINFO_ANT_TEST:
		pDM_Odm->pAntennaTest =  (u8 *)pValue;
		break;

	case 	ODM_CMNINFO_FORCED_RATE:
		pDM_Odm->pForcedDataRate = (u16 *)pValue;
		break;

	/* To remove the compiler warning, must add an empty default statement to handle the other values. */
	default:
		/* do nothing */
		break;
		break;

	}

}

/*
 * 3============================================================
 * 3 DIG
 * 3============================================================
 */

/*-----------------------------------------------------------------------------
 * Function:	odm_DIGInit()
 *
 * Overview:	Set DIG scheme init value.
 *
 * Input:		NONE
 *
 * Output:		NONE
 *
 * Return:		NONE
 *
 * Revised History:
 *	When		Who		Remark
 *
 *---------------------------------------------------------------------------*/
void ODM_ChangeDynamicInitGainThresh(struct rtl_priv *rtlpriv, uint32_t DM_Type, uint32_t DM_Value)
{
	struct dig_t *pDM_DigTable = &(rtlpriv->dm_digtable);

	/* ULLI better switch/case ?? */
	if (DM_Type == DIG_TYPE_THRESH_HIGH) {
		pDM_DigTable->rssi_highthresh = DM_Value;
	} else if (DM_Type == DIG_TYPE_THRESH_LOW) {
		pDM_DigTable->rssi_lowthresh = DM_Value;
	} else if (DM_Type == DIG_TYPE_ENABLE) {
		pDM_DigTable->dig_enable_flag = true;
	} else if (DM_Type == DIG_TYPE_DISABLE) {
		pDM_DigTable->dig_enable_flag = false;
	} else if (DM_Type == DIG_TYPE_BACKOFF) {
		if (DM_Value > 30)
			DM_Value = 30;
		pDM_DigTable->backoff_enable_flag = (u8)DM_Value;
	} else if (DM_Type == DIG_TYPE_RX_GAIN_MIN) {
		if (DM_Value == 0)
			DM_Value = 0x1;
		pDM_DigTable->rx_gain_min = (u8)DM_Value;
	} else if (DM_Type == DIG_TYPE_RX_GAIN_MAX) {
		if (DM_Value > 0x50)
			DM_Value = 0x50;
		pDM_DigTable->rx_gain_max = (u8)DM_Value;
	}
}	/* DM_ChangeDynamicInitGainThresh */

int getIGIForDiff(int value_IGI)
{
	#define ONERCCA_LOW_TH		0x30
	#define ONERCCA_LOW_DIFF	8

	if (value_IGI < ONERCCA_LOW_TH) {
		if ((ONERCCA_LOW_TH - value_IGI) < ONERCCA_LOW_DIFF)
			return ONERCCA_LOW_TH;
		else
			return value_IGI + ONERCCA_LOW_DIFF;
	} else {
		return value_IGI;
	}
}




void odm_Adaptivity(struct _rtw_dm *pDM_Odm, u8	IGI)
{
	struct rtl_priv *rtlpriv = pDM_Odm->rtlpriv;
	struct dig_t *dm_digtable = &(rtlpriv->dm_digtable);

	int32_t TH_H_dmc, TH_L_dmc;
	int32_t TH_H, TH_L, Diff, IGI_target;
	uint32_t value32;
	bool EDCCA_State;

	RT_TRACE(rtlpriv, COMP_DIG, DBG_LOUD, "odm_Adaptivity() =====> \n");

	if (rtlpriv->rtlhal.current_bandtype == BAND_ON_5G) {
		pDM_Odm->TH_H = 0xf4;	/* 0xf8; */
		pDM_Odm->TH_L = 0xf7;	/* 0xfb; */
	} else {
		pDM_Odm->TH_H = 0xf4;	/* 0xfa; */
		pDM_Odm->TH_L = 0xf7;	/* 0xfd; */
	}

	RT_TRACE(rtlpriv, COMP_DIG, DBG_LOUD, "pDM_Odm->ForceEDCCA=%d, IGI_Base=0x%x, TH_H=0x%x, TH_L=0x%x, AdapEn_RSSI = %d\n",
	pDM_Odm->ForceEDCCA, pDM_Odm->IGI_Base, pDM_Odm->TH_H, pDM_Odm->TH_L, pDM_Odm->AdapEn_RSSI);

	rtl_set_bbreg(pDM_Odm->rtlpriv, 0x800, BIT10, 0);		/* ADC_mask enable */

	if (rtlpriv->mac80211.link_state < MAC80211_LINKED) {
		return;
	}

	if (!pDM_Odm->ForceEDCCA) {
		if (dm_digtable->rssi_val_min > pDM_Odm->AdapEn_RSSI)
			EDCCA_State = 1;
		else if (dm_digtable->rssi_val < (pDM_Odm->AdapEn_RSSI - 5))
			EDCCA_State = 0;
	} else
		EDCCA_State = 1;
	/*
	 * if ((pDM_Odm->SupportICType & ODM_IC_11AC_SERIES) && (*pDM_Odm->pBandType == BAND_ON_5G))
	 * 	IGI_target = pDM_Odm->IGI_Base;
	 * else
	 */

	if (rtlpriv->phy.current_chan_bw == ODM_BW20M)	/*CHANNEL_WIDTH_20 */
		IGI_target = pDM_Odm->IGI_Base;
	else if (rtlpriv->phy.current_chan_bw == ODM_BW40M)
		IGI_target = pDM_Odm->IGI_Base + 2;
	else if (rtlpriv->phy.current_chan_bw == ODM_BW80M)
		IGI_target = pDM_Odm->IGI_Base + 6;
	else
		IGI_target = pDM_Odm->IGI_Base;


	pDM_Odm->IGI_target = IGI_target;

	if (pDM_Odm->TH_H & BIT7)
		TH_H = pDM_Odm->TH_H | 0xFFFFFF00;
	else
		TH_H = pDM_Odm->TH_H;
	if (pDM_Odm->TH_L & BIT7)
		TH_L = pDM_Odm->TH_L | 0xFFFFFF00;
	else
		TH_L = pDM_Odm->TH_L;

	RT_TRACE(rtlpriv, COMP_DIG, DBG_LOUD, "BandWidth=%s, IGI_target=0x%x, EDCCA_State=%d\n",
		(rtlpriv->phy.current_chan_bw == ODM_BW80M) ? "80M" : ((rtlpriv->phy.current_chan_bw == ODM_BW40M) ? "40M" : "20M"), IGI_target, EDCCA_State);

	if (EDCCA_State == 1) {
		if (IGI < IGI_target) {
			Diff = IGI_target - (int32_t)IGI;
			TH_H_dmc = TH_H + Diff;
			if (TH_H_dmc > 10)
				TH_H_dmc = 10;
			TH_L_dmc = TH_L + Diff;
			if (TH_L_dmc > 10)
				TH_L_dmc = 10;
		} else 	{
			Diff = (int32_t)IGI - IGI_target;
			TH_H_dmc = TH_H - Diff;
			TH_L_dmc = TH_L - Diff;
		}
		TH_H_dmc = (TH_H_dmc & 0xFF);
		TH_L_dmc = (TH_L_dmc & 0xFF);
	} else {
		TH_H_dmc = 0x7f;
		TH_L_dmc = 0x7f;
	}
	RT_TRACE(rtlpriv, COMP_DIG, DBG_LOUD, "IGI=0x%x, TH_H_dmc=0x%x, TH_L_dmc=0x%x\n",
		IGI, TH_H_dmc, TH_L_dmc);

	rtl_set_bbreg(pDM_Odm->rtlpriv, rFPGA0_XB_LSSIReadBack, 0xFFFF, (TH_H_dmc<<8) | TH_L_dmc);
}


void ODM_Write_DIG(struct _rtw_dm *pDM_Odm, u8 CurrentIGI)
{
	struct rtl_priv *rtlpriv = pDM_Odm->rtlpriv;
	struct dig_t *pDM_DigTable = &(rtlpriv->dm_digtable);

	if (pDM_Odm->StopDIG) {
		RT_TRACE(rtlpriv, COMP_DIG, DBG_LOUD, "Stop Writing IGI\n");
		return;
	}

	RT_TRACE(rtlpriv, COMP_DIG, DBG_LOUD, "ODM_REG(IGI_A,pDM_Odm)=0x%x, ODM_BIT(IGI,pDM_Odm)=0x%x \n",
		ODM_REG(IGI_A, pDM_Odm), ODM_BIT(IGI, pDM_Odm));

	if (pDM_DigTable->cur_igvalue != CurrentIGI) {	/*if (pDM_DigTable->PreIGValue != CurrentIGI) */
		rtl_set_bbreg(pDM_Odm->rtlpriv, ODM_REG_IGI_A_11AC, ODM_BIT_IGI_11AC, CurrentIGI);
		if (pDM_Odm->rtlpriv->phy.rf_type != ODM_1T1R)
			rtl_set_bbreg(pDM_Odm->rtlpriv, ODM_REG_IGI_B_11AC, ODM_BIT_IGI_11AC, CurrentIGI);

		RT_TRACE(rtlpriv, COMP_DIG, DBG_LOUD, "CurrentIGI(0x%02x). \n", CurrentIGI);
		/* pDM_DigTable->PreIGValue = pDM_DigTable->CurIGValue; */
		pDM_DigTable->cur_igvalue = CurrentIGI;
	}
	RT_TRACE(rtlpriv, COMP_DIG, DBG_LOUD, "ODM_Write_DIG():CurrentIGI=0x%x \n", CurrentIGI);

}

void odm_DIGbyRSSI_LPS(struct _rtw_dm *pDM_Odm)
{
	struct rtl_priv *rtlpriv = pDM_Odm->rtlpriv;
	struct dig_t *dm_digtable = &(rtlpriv->dm_digtable);

	/* struct rtl_priv *rtlpriv =pDM_Odm->rtlpriv; */
	/* pDIG_T	pDM_DigTable = &pDM_Odm->DM_DigTable; */
	struct false_alarm_statistics *pFalseAlmCnt = &(rtlpriv->falsealm_cnt);

	u8	RSSI_Lower = DM_DIG_MIN_NIC;   	/* 0x1E or 0x1C */
	u8	CurrentIGI = dm_digtable->rssi_val;

	CurrentIGI = CurrentIGI+RSSI_OFFSET_DIG;

	/* RT_TRACE(rtlpriv,COMP_DIG_LPS, DBG_LOUD, ("odm_DIG()==>\n")); */

	/* Using FW PS mode to make IGI */

	RT_TRACE(rtlpriv, COMP_DIG, DBG_LOUD, "---Neil---odm_DIG is in LPS mode\n");
	/* Adjust by  FA in LPS MODE */
	if (pFalseAlmCnt->cnt_all > DM_DIG_FA_TH2_LPS)
		CurrentIGI = CurrentIGI+2;
	else if (pFalseAlmCnt->cnt_all > DM_DIG_FA_TH1_LPS)
		CurrentIGI = CurrentIGI+1;
	else if (pFalseAlmCnt->cnt_all < DM_DIG_FA_TH0_LPS)
		CurrentIGI = CurrentIGI-1;

	/* Lower bound checking */

	/* RSSI Lower bound check */
	if ((dm_digtable->rssi_val_min) > DM_DIG_MIN_NIC)
		RSSI_Lower = (dm_digtable->rssi_val_min-10);
	else
		RSSI_Lower = DM_DIG_MIN_NIC;

	/* Upper and Lower Bound checking */
	 if (CurrentIGI > DM_DIG_MAX_NIC)
		CurrentIGI = DM_DIG_MAX_NIC;
	 else if (CurrentIGI < RSSI_Lower)
		CurrentIGI = RSSI_Lower;

	ODM_Write_DIG(pDM_Odm, CurrentIGI);/* ODM_Write_DIG(pDM_Odm, pDM_DigTable->CurIGValue); */

}


/*
 * 3============================================================
 * 3 CCK Packet Detect Threshold
 * 3============================================================
 */


void ODM_Write_CCK_CCA_Thres(struct rtl_priv *rtlpriv, u8 CurCCK_CCAThres)
{
	struct dig_t *pDM_DigTable = &(rtlpriv->dm_digtable);

	if (pDM_DigTable->cur_cck_cca_thres != CurCCK_CCAThres) {	/* modify by Guo.Mingzhi 2012-01-03 */
		rtl_write_byte(rtlpriv, ODM_REG_CCK_CCA_11AC, CurCCK_CCAThres);
	}
	pDM_DigTable->pre_cck_cca_thres = pDM_DigTable->cur_cck_cca_thres;
	pDM_DigTable->cur_cck_cca_thres = CurCCK_CCAThres;

}

/*
 * 3============================================================
 * 3 RATR MASK
 * 3============================================================
 * 3============================================================
 * 3 Rate Adaptive
 * 3============================================================
 */



uint32_t ODM_Get_Rate_Bitmap(struct _rtw_dm *pDM_Odm, uint32_t macid,
	uint32_t ra_mask, u8 rssi_level)
{
	struct rtl_priv *rtlpriv = pDM_Odm->rtlpriv;
	struct rtl_hal  *rtlhal = rtl_hal(rtlpriv);

	struct sta_info *pEntry;
	uint32_t 	rate_bitmap = 0;
	u8 	WirelessMode;
	/* u8 	WirelessMode =*(pDM_Odm->pWirelessMode); */

	pEntry = pDM_Odm->pODM_StaInfo[macid];
	if (!IS_STA_VALID(pEntry))
		return ra_mask;

	WirelessMode = pEntry->wireless_mode;

	switch (WirelessMode) {
	case WIRELESS_MODE_B:
		if (ra_mask & 0x0000000c)		/* 11M or 5.5M enable */
			rate_bitmap = 0x0000000d;
		else
			rate_bitmap = 0x0000000f;
		break;

	case WIRELESS_MODE_G:
	case WIRELESS_MODE_A:
		if (rssi_level == DM_RATR_STA_HIGH)
			rate_bitmap = 0x00000f00;
		else
			rate_bitmap = 0x00000ff0;
		break;

	case WIRELESS_MODE_B | WIRELESS_MODE_G:
		if (rssi_level == DM_RATR_STA_HIGH)
			rate_bitmap = 0x00000f00;
		else if (rssi_level == DM_RATR_STA_MIDDLE)
			rate_bitmap = 0x00000ff0;
		else
			rate_bitmap = 0x00000ff5;
		break;

	case WIRELESS_MODE_B | WIRELESS_MODE_G | WIRELESS_MODE_N_24G:
	case WIRELESS_MODE_B | WIRELESS_MODE_N_24G:
	case WIRELESS_MODE_A | WIRELESS_MODE_N_5G:
		if (pDM_Odm->rtlpriv->phy.rf_type == ODM_1T2R || pDM_Odm->rtlpriv->phy.rf_type == ODM_1T1R) {
			if (rssi_level == DM_RATR_STA_HIGH) {
				rate_bitmap = 0x000f0000;
			} else if (rssi_level == DM_RATR_STA_MIDDLE) {
				rate_bitmap = 0x000ff000;
			} else {
				if (rtlpriv->phy.current_chan_bw == ODM_BW40M)
					rate_bitmap = 0x000ff015;
				else
					rate_bitmap = 0x000ff005;
			}
		} else {
			if (rssi_level == DM_RATR_STA_HIGH) {
				rate_bitmap = 0x0f8f0000;
			} else if (rssi_level == DM_RATR_STA_MIDDLE) {
				rate_bitmap = 0x0f8ff000;
			} else {
				if (rtlpriv->phy.current_chan_bw == ODM_BW40M)
					rate_bitmap = 0x0f8ff015;
				else
					rate_bitmap = 0x0f8ff005;
			}
		}
		break;
	case WIRELESS_MODE_AC_5G | WIRELESS_MODE_A:
	case WIRELESS_MODE_AC_5G | WIRELESS_MODE_G:
		if (pDM_Odm->rtlpriv->phy.rf_type == RF_1T1R) {
			if (IS_HARDWARE_TYPE_8811AU(rtlhal) ||
				(IS_HARDWARE_TYPE_8812AU(rtlhal) && IS_NORMAL_CHIP(rtlhal->version))) {
				if (IS_HARDWARE_TYPE_8821U(rtlhal)
					&& (rtlpriv->phy.current_channel >= 149)) {
					if (rssi_level == 1)				/* add by Gary for ac-series */
						rate_bitmap = 0x001f8000;
					else if (rssi_level == 2)
						rate_bitmap = 0x001ff000;
					else
						rate_bitmap = 0x001ff010;
				} else 	{
					if (rssi_level == 1)				/* add by Gary for ac-series */
						rate_bitmap = 0x003f8000;
					else if (rssi_level == 2)
						rate_bitmap = 0x003ff000;
					else
						rate_bitmap = 0x003ff010;
				}
			} else {
				rate_bitmap = 0x000ff010;
		       }
		} else {
			if (IS_NORMAL_CHIP(rtlhal->version)) {
				if (rssi_level == 1)			/* add by Gary for ac-series */
					rate_bitmap = 0xfe3f8000;       /* VHT 2SS MCS3~9 */
				else if (rssi_level == 2)
					rate_bitmap = 0xfffff000;       /* VHT 2SS MCS0~9 */
				else
					rate_bitmap = 0xfffff010;       /* All */
			} else
				rate_bitmap = 0x3fcff010;
		}
		break;

	default:
		if (pDM_Odm->rtlpriv->phy.rf_type == RF_1T2R)
			rate_bitmap = 0x000fffff;
		else
			rate_bitmap = 0x0fffffff;
		break;

	}

	/* printk("%s ==> rssi_level:0x%02x, WirelessMode:0x%02x, rate_bitmap:0x%08x \n",__FUNCTION__,rssi_level,WirelessMode,rate_bitmap); */
	RT_TRACE(rtlpriv, COMP_RATE, DBG_LOUD, " ==> rssi_level:0x%02x, WirelessMode:0x%02x, rate_bitmap:0x%08x \n", rssi_level, WirelessMode, rate_bitmap);

	return (ra_mask&rate_bitmap);

}

/*-----------------------------------------------------------------------------
 * Function:	odm_RefreshRateAdaptiveMask()
 *
 * Overview:	Update rate table mask according to rssi
 *
 * Input:		NONE
 *
 * Output:		NONE
 *
 * Return:		NONE
 *
 * Revised History:
 *	When		Who		Remark
 *	05/27/2009	hpfan	Create Version 0.
 *
 *---------------------------------------------------------------------------*/

/*
 * Return Value: bool
 * - true: RATRState is changed.
 */
static bool ODM_RAStateCheck(struct rtl_priv *rtlpriv, u32 RSSI,
	bool bForceUpdate, u8 *pRATRState)
{
	struct rate_adaptive *p_ra = &(rtlpriv->ra);

	const u8 GoUpGap = 5;
	u8 HighRSSIThreshForRA = p_ra->high_rssi_thresh_for_ra;
	u8 LowRSSIThreshForRA = p_ra->low2high_rssi_thresh_for_ra40m;
	u8 RATRState;

	/*
	 * Threshold Adjustment:
	 * when RSSI state trends to go up one or two levels, make sure RSSI is high enough.
	 * Here GoUpGap is added to solve the boundary's level alternation issue.
	 */
	switch (*pRATRState) {
	case DM_RATR_STA_INIT:
	case DM_RATR_STA_HIGH:
		break;
	case DM_RATR_STA_MIDDLE:
		HighRSSIThreshForRA += GoUpGap;
		break;
	case DM_RATR_STA_LOW:
		HighRSSIThreshForRA += GoUpGap;
		LowRSSIThreshForRA += GoUpGap;
		break;

	default:
		ODM_RT_ASSERT(pDM_Odm, false, ("wrong rssi level setting %d !", *pRATRState));
		break;
	}

	/* Decide RATRState by RSSI. */
	if (RSSI > HighRSSIThreshForRA)
		RATRState = DM_RATR_STA_HIGH;
	else if (RSSI > LowRSSIThreshForRA)
		RATRState = DM_RATR_STA_MIDDLE;
	else
		RATRState = DM_RATR_STA_LOW;
	/* printk("==>%s,RATRState:0x%02x ,RSSI:%d \n",__FUNCTION__,RATRState,RSSI); */

	if (*pRATRState != RATRState || bForceUpdate) {
		RT_TRACE(rtlpriv, COMP_RATE, DBG_LOUD, "RSSI Level %d -> %d\n", *pRATRState, RATRState);
		*pRATRState = RATRState;
		return true;
	}

	return false;
}

void odm_RefreshRateAdaptiveMask(struct rtl_priv *rtlpriv)
{
	struct rate_adaptive *p_ra = &(rtlpriv->ra);
	u8	i;
	struct _rtw_hal *pHalData = GET_HAL_DATA(rtlpriv);
	struct _rtw_dm *	pDM_Odm = &(pHalData->odmpriv);
	PODM_RATE_ADAPTIVE	pRA = &pDM_Odm->RateAdaptive;

	if (rtlpriv->bDriverStopped) {
		RT_TRACE(rtlpriv, COMP_RATE, DBG_TRACE, "<---- odm_RefreshRateAdaptiveMask(): driver is going to unload\n");
		return;
	}

	/* printk("==> %s \n",__FUNCTION__); */

	for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++) {
		struct sta_info *pstat = pDM_Odm->pODM_StaInfo[i];
		if (IS_STA_VALID(pstat)) {
			if (pstat->rssi_stat.UndecoratedSmoothedPWDB < p_ra->ldpc_thres) {
				p_ra->use_ldpc = true;
				pRA->bLowerRtsRate = true;
				Set_RA_LDPC_8812(pstat, true);
				/* DbgPrint("RSSI=%d, bUseLdpc = true\n", pHalData->UndecoratedSmoothedPWDB); */
			} else if (pstat->rssi_stat.UndecoratedSmoothedPWDB > (p_ra->ldpc_thres-5)) {
				p_ra->use_ldpc = false;
				pRA->bLowerRtsRate = false;
				Set_RA_LDPC_8812(pstat, false);
				/* DbgPrint("RSSI=%d, bUseLdpc = false\n", pHalData->UndecoratedSmoothedPWDB); */
			}

			if (true == ODM_RAStateCheck(rtlpriv, pstat->rssi_stat.UndecoratedSmoothedPWDB, false , &pstat->rssi_level)) {
				RT_TRACE(rtlpriv, COMP_RATE, DBG_LOUD, "RSSI:%d, RSSI_LEVEL:%d\n", pstat->rssi_stat.UndecoratedSmoothedPWDB, pstat->rssi_level);
				/* printk("RSSI:%d, RSSI_LEVEL:%d\n", pstat->rssi_stat.UndecoratedSmoothedPWDB, pstat->rssi_level); */
				rtw_hal_update_ra_mask(pstat->rtlpriv, pstat, pstat->rssi_level);
			}
		}
	}

}
