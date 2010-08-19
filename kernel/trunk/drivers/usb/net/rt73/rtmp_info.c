/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2006, Ralink Technology, Inc.
 *
 * This program is free software; you can redistribute it and/or modify  * 
 * it under the terms of the GNU General Public License as published by  * 
 * the Free Software Foundation; either version 2 of the License, or     * 
 * (at your option) any later version.                                   * 
 *                                                                       * 
 * This program is distributed in the hope that it will be useful,       * 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         * 
 * GNU General Public License for more details.                          * 
 *                                                                       * 
 * You should have received a copy of the GNU General Public License     * 
 * along with this program; if not, write to the                         * 
 * Free Software Foundation, Inc.,                                       * 
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             * 
 *                                                                       * 
 ************************************************************************

	Module Name:
	rtmp_info.c

	Abstract:
	IOCTL related subroutines

	Revision History:
	Who 		When		  What
	--------	----------	  ----------------------------------------------
	Rory Chen	01-03-2003	  created
	Rory Chen	02-14-2005	  modify to support RT61
*/

#include	"rt_config.h"
#include <net/iw_handler.h>

#ifdef DBG
extern ULONG	RTDebugLevel;
#endif

#ifndef IW_ESSID_MAX_SIZE
/* Maximum size of the ESSID and NICKN strings */
#define IW_ESSID_MAX_SIZE	32
#endif

extern UCHAR	CipherWpa2Template[];
extern UCHAR	CipherWpa2TemplateLen;
extern UCHAR	CipherWpaPskTkip[];
extern UCHAR	CipherWpaPskTkipLen;

#define NR_WEP_KEYS 4
//#define WEP_SMALL_KEY_LEN (40/8)  //move to rtmp_def.h
//#define WEP_LARGE_KEY_LEN (104/8)


struct iw_priv_args privtab[] = {
{ RTPRIV_IOCTL_SET, 
  IW_PRIV_TYPE_CHAR | 1024, 0,
  "set"},
  
#ifdef DBG

//{ RTPRIV_IOCTL_BBP,
//  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024,
//  "bbp"},
//{ RTPRIV_IOCTL_MAC,
//  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024,
//  "mac"},

#ifdef RALINK_ATE
{ RTPRIV_IOCTL_E2P,
  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024,
  "e2p"},
#endif  /* RALINK_ATE */
  
#endif

{ RTPRIV_IOCTL_STATISTICS,
  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024,
  "stat"}, 
{ RTPRIV_IOCTL_GSITESURVEY,
  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024 ,
  "get_site_survey"},
{ RTPRIV_IOCTL_GETRAAPCFG,  IW_PRIV_TYPE_CHAR | 1024, 0,  
  "get_RaAP_Cfg"},
#if 1 
{ RTPRIV_IOCTL_AUTH, 
  IW_PRIV_TYPE_INT	|1, 0,	
  "auth"},
{ RTPRIV_IOCTL_WEPSTATUS, 
  IW_PRIV_TYPE_INT	|1, 0,	
  "enc"},  
{ RTPRIV_IOCTL_WPAPSK, 
  IW_PRIV_TYPE_CHAR |64, 0,  
  "wpapsk"},
{ RTPRIV_IOCTL_PSM, 
 IW_PRIV_TYPE_INT  |1, 0,  
 "psm"},
#endif
};

static struct {
	CHAR *name;
	INT (*set_proc)(PRTMP_ADAPTER pAdapter, PUCHAR arg);
} *PRTMP_PRIVATE_SET_PROC, RTMP_PRIVATE_SUPPORT_PROC[] = {
	{"DriverVersion",				Set_DriverVersion_Proc},
	{"CountryRegion",				Set_CountryRegion_Proc},	
	{"CountryRegionABand",			Set_CountryRegionABand_Proc},	   
	{"SSID",						Set_SSID_Proc}, 
	{"WirelessMode",				Set_WirelessMode_Proc}, 	  
	{"TxRate",						Set_TxRate_Proc},			  
	{"Channel",						Set_Channel_Proc},			  
	{"BGProtection",				Set_BGProtection_Proc},
	{"TxPreamble",					Set_TxPreamble_Proc},		  
	{"RTSThreshold",				Set_RTSThreshold_Proc}, 	  
	{"FragThreshold",				Set_FragThreshold_Proc},	  
	{"TxBurst",						Set_TxBurst_Proc},
	{"AdhocOfdm",					Set_AdhocModeRate_Proc},
#ifdef AGGREGATION_SUPPORT
	{"PktAggregate",				Set_PktAggregate_Proc}, 	  
#endif
	{"TurboRate",					Set_TurboRate_Proc},		  
#if 0
	{"WmmCapable",					Set_WmmCapable_Proc},		  		   
#endif	
	{"IEEE80211H",					Set_IEEE80211H_Proc},
	{"NetworkType", 				Set_NetworkType_Proc},		  
	{"AuthMode",					Set_AuthMode_Proc}, 		  
	{"EncrypType",					Set_EncrypType_Proc},		  
	{"DefaultKeyID",				Set_DefaultKeyID_Proc}, 	  
	{"Key1",						Set_Key1_Proc}, 			  
	{"Key2",						Set_Key2_Proc}, 			  
	{"Key3",						Set_Key3_Proc}, 			  
	{"Key4",						Set_Key4_Proc}, 			  
	{"WPAPSK",						Set_WPAPSK_Proc},
	{"ResetCounter",				Set_ResetStatCounter_Proc},
	{"PSMode",						Set_PSMode_Proc},
#ifdef DBG
	{"Debug",						Set_Debug_Proc},			 
#endif
#ifdef RALINK_ATE   
	{"ATE",							Set_ATE_Proc},              // set ATE Mode to: STOP, TXCONT, TXCARR, TXFRAME, RXFRAME          
	{"ATEDA",						Set_ATE_DA_Proc},           // set ATE TxFrames ADDR1, DA
	{"ATESA",						Set_ATE_SA_Proc},           // set ATE TxFrames ADDR2, SA
	{"ATEBSSID",					Set_ATE_BSSID_Proc},        // set ATE TxFrames ADDR3, BSSID
	{"ATECHANNEL",					Set_ATE_CHANNEL_Proc},      // set ATE Channel
	{"ATETXPOW",					Set_ATE_TX_POWER_Proc},     // set ATE TxPower
	{"ATETXFREQOFFSET",				Set_ATE_TX_FREQOFFSET_Proc}, //set ATE RF frequency offset
	{"ATETXLEN",					Set_ATE_TX_LENGTH_Proc},    // set ATE TxLength
	{"ATETXCNT",					Set_ATE_TX_COUNT_Proc},     // set ATE TxCount
	{"ATETXRATE",					Set_ATE_TX_RATE_Proc},      // set ATE TxRate
#endif	// RALINK_ATE
	{NULL,}
};

char * rtstrchr(const char * s, int c)
{
	for(; *s != (char) c; ++s)
		if (*s == '\0')
			return NULL;
	return (char *) s;
}

/*
This is required for LinEX2004/kernel2.6.7 to provide iwlist scanning function
*/

int
rt_ioctl_giwname(struct net_device *dev,
		   struct iw_request_info *info,
		   char *name, char *extra)
{
	strncpy(name, "RT73 WLAN", IFNAMSIZ);
	return 0;
}

int rt_ioctl_siwfreq(struct net_device *dev,
			struct iw_request_info *info,
			struct iw_freq *freq, char *extra)
{
	PRTMP_ADAPTER pAdapter = (PRTMP_ADAPTER) dev->priv;
	int 	chan = -1;
	
    //check if the interface is down
    if (pAdapter->RTUSBCmdThr_pid < 0)
        return -ENETDOWN;

	if (freq->e > 1)
		return -EINVAL;

	if((freq->e == 0) && (freq->m <= 1000))
		chan = freq->m;	// Setting by channel number 
	else
		MAP_KHZ_TO_CHANNEL_ID( (freq->m /100) , chan); // Setting by frequency - search the table , like 2.412G, 2.422G, 
	pAdapter->PortCfg.Channel = chan;
	DBGPRINT(RT_DEBUG_TRACE, "==>rt_ioctl_siwfreq::SIOCSIWFREQ[cmd=0x%x] (Channel=%d)\n", SIOCSIWFREQ, pAdapter->PortCfg.Channel);


	return 0;
}
int rt_ioctl_giwfreq(struct net_device *dev,
		   struct iw_request_info *info,
		   struct iw_freq *freq, char *extra)
{
	PRTMP_ADAPTER pAdapter = (PRTMP_ADAPTER) dev->priv;
	ULONG	m;

    //check if the interface is down
    if (pAdapter->RTUSBCmdThr_pid < 0)
        return -ENETDOWN;

	DBGPRINT(RT_DEBUG_TRACE,"==>rt_ioctl_giwfreq  %d\n",pAdapter->PortCfg.Channel);
	
	MAP_CHANNEL_ID_TO_KHZ(pAdapter->PortCfg.Channel, m);
	freq->m = m * 100;
	freq->e = 1;

	return 0;
}

int rt_ioctl_siwmode(struct net_device *dev,
		   struct iw_request_info *info,
		   __u32 *mode, char *extra)
{
	PRTMP_ADAPTER pAdapter = (PRTMP_ADAPTER) dev->priv;

    //check if the interface is down
    if (pAdapter->RTUSBCmdThr_pid < 0)
        return -ENETDOWN;

	switch (*mode)
	{
		case IW_MODE_ADHOC:
			if (pAdapter->PortCfg.BssType != BSS_ADHOC)
			{	
				// Config has changed
				if (INFRA_ON(pAdapter))
				{
					RTUSBEnqueueInternalCmd(pAdapter, RT_OID_LINK_DOWN);
					// First cancel linkdown timer
					DBGPRINT(RT_DEBUG_TRACE, "NDIS_STATUS_MEDIA_DISCONNECT Event BB!\n");							
				}

			}
			pAdapter->PortCfg.BssType = BSS_ADHOC;
			DBGPRINT(RT_DEBUG_TRACE, "===>rt_ioctl_siwmode::SIOCSIWMODE (AD-HOC)\n");
			break;
		case IW_MODE_INFRA:
			if (pAdapter->PortCfg.BssType != BSS_INFRA)
			{
				// Config has changed
				if (ADHOC_ON(pAdapter)) 						  
					RTUSBEnqueueInternalCmd(pAdapter, RT_OID_LINK_DOWN);
			}
			pAdapter->PortCfg.BssType = BSS_INFRA;
			DBGPRINT(RT_DEBUG_TRACE, "===>rt_ioctl_siwmode::SIOCSIWMODE (INFRA)\n");
			break;
		default:
			DBGPRINT(RT_DEBUG_TRACE, "===>rt_ioctl_siwmode::SIOCSIWMODE (unknown)\n");
			return -EINVAL;
	}
	
	// Reset Ralink supplicant to not use, it will be set to start when UI set PMK key
	pAdapter->PortCfg.WpaState = SS_NOTUSE;

	return 0;
}

int rt_ioctl_giwmode(struct net_device *dev,
		   struct iw_request_info *info,
		   __u32 *mode, char *extra)
{
	PRTMP_ADAPTER pAdapter = (PRTMP_ADAPTER) dev->priv;

    //check if the interface is down
    if (pAdapter->RTUSBCmdThr_pid < 0)
        return -ENETDOWN;
        
	if (ADHOC_ON(pAdapter))
		*mode = IW_MODE_ADHOC;
	else if (INFRA_ON(pAdapter))
		*mode = IW_MODE_INFRA;
	else
		*mode = IW_MODE_AUTO;

	DBGPRINT(RT_DEBUG_TRACE,"==>rt_ioctl_giwmode\n");		
	return 0;
}

int rt_ioctl_siwsens(struct net_device *dev,
		   struct iw_request_info *info,
		   char *name, char *extra)
{
    PRTMP_ADAPTER pAdapter = (PRTMP_ADAPTER) dev->priv;

    //check if the interface is down
    if (pAdapter->RTUSBCmdThr_pid < 0)
        return -ENETDOWN;

	return 0;
}

int rt_ioctl_giwsens(struct net_device *dev,
		   struct iw_request_info *info,
		   char *name, char *extra)
{
    PRTMP_ADAPTER pAdapter = (PRTMP_ADAPTER) dev->priv;

    //check if the interface is down
    if (pAdapter->RTUSBCmdThr_pid < 0)
        return -ENETDOWN;
        
	return 0;
}

int rt_ioctl_giwrange(struct net_device *dev,
		   struct iw_request_info *info,
		   struct iw_point *data, char *extra)
{
	PRTMP_ADAPTER pAdapter = (PRTMP_ADAPTER) dev->priv;
	struct iw_range *range = (struct iw_range *) extra;
	u16 val;
	int i;

    //check if the interface is down
    if (pAdapter->RTUSBCmdThr_pid < 0)
        return -ENETDOWN;

	DBGPRINT(RT_DEBUG_TRACE,"===>rt_ioctl_giwrange\n");		
	data->length = sizeof(struct iw_range);
	memset(range, 0, sizeof(struct iw_range));

	range->txpower_capa = IW_TXPOW_DBM;

	if (INFRA_ON(pAdapter)||ADHOC_ON(pAdapter))
	{
		range->min_pmp = 1 * 1024;
		range->max_pmp = 65535 * 1024;
		range->min_pmt = 1 * 1024;
		range->max_pmt = 1000 * 1024;
		range->pmp_flags = IW_POWER_PERIOD;
		range->pmt_flags = IW_POWER_TIMEOUT;
		range->pm_capa = IW_POWER_PERIOD | IW_POWER_TIMEOUT |
			IW_POWER_UNICAST_R | IW_POWER_ALL_R;
	}

	range->we_version_compiled = WIRELESS_EXT;
	range->we_version_source = 14;

	range->retry_capa = IW_RETRY_LIMIT;
	range->retry_flags = IW_RETRY_LIMIT;
	range->min_retry = 0;
	range->max_retry = 255;

	range->num_channels =  pAdapter->ChannelListNum;

	val = 0;
	for (i = 1; i <= range->num_channels; i++) {
		u32 m;
		range->freq[val].i = pAdapter->ChannelList[i-1].Channel;
		MAP_CHANNEL_ID_TO_KHZ(pAdapter->ChannelList[i-1].Channel, m);
		range->freq[val].m = m * 100; /* HZ */
		
		range->freq[val].e = 1;
		val++;
		if (val == IW_MAX_FREQUENCIES)
			break;
	}
	range->num_frequency = val;

	range->max_qual.qual = 100; /* what is correct max? This was not
                                * documented exactly. At least
					            * 69 has been observed. */
	range->max_qual.level = 0; /* dB */
	range->max_qual.noise = 0; /* dB */

	/* What would be suitable values for "average/typical" qual? */
	range->avg_qual.qual = 20;
	range->avg_qual.level = -60;
	range->avg_qual.noise = -95;
	range->sensitivity = 3;

	range->max_encoding_tokens = NR_WEP_KEYS;
	range->num_encoding_sizes = 2;
	range->encoding_size[0] = 5;
	range->encoding_size[1] = 13;

#if 0
	over2 = 0;
	len = prism2_get_datarates(dev, rates);
	range->num_bitrates = 0;
	for (i = 0; i < len; i++) {
		if (range->num_bitrates < IW_MAX_BITRATES) {
			range->bitrate[range->num_bitrates] =
				rates[i] * 500000;
			range->num_bitrates++;
		}
		if (rates[i] == 0x0b || rates[i] == 0x16)
			over2 = 1;
	}
	/* estimated maximum TCP throughput values (bps) */
	range->throughput = over2 ? 5500000 : 1500000;
#endif
	range->min_rts = 0;
	range->max_rts = 2347;
	range->min_frag = 256;
	range->max_frag = 2346;

	return 0;
}

int rt_ioctl_giwap(struct net_device *dev,
			  struct iw_request_info *info,
			  struct sockaddr *ap_addr, char *extra)
{

	PRTMP_ADAPTER pAdapter = (PRTMP_ADAPTER) dev->priv;

    //check if the interface is down
    if (pAdapter->RTUSBCmdThr_pid < 0)
        return -ENETDOWN;

	if (INFRA_ON(pAdapter) || ADHOC_ON(pAdapter))
	{
		ap_addr->sa_family = ARPHRD_ETHER;
		memcpy(ap_addr->sa_data, &pAdapter->PortCfg.Bssid, ETH_ALEN);
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, "IOCTL::SIOCGIWAP(=EMPTY)\n");
		return -ENOTCONN;
	}

	return 0;
}

/*
 * Units are in db above the noise floor. That means the
 * rssi values reported in the tx/rx descriptors in the
 * driver are the SNR expressed in db.
 *
 * If you assume that the noise floor is -95, which is an
 * excellent assumption 99.5 % of the time, then you can
 * derive the absolute signal level (i.e. -95 + rssi). 
 * There are some other slight factors to take into account
 * depending on whether the rssi measurement is from 11b,
 * 11g, or 11a.   These differences are at most 2db and
 * can be documented.
 *
 * NB: various calculations are based on the orinoco/wavelan
 *	   drivers for compatibility
 */
static void set_quality(PRTMP_ADAPTER pAdapter,
                        struct iw_quality *iq, 
                        u_int rssi)
{
    u32 ChannelQuality, NorRssi;

       
    // Normalize Rssi
    if (rssi > 0x50)
        NorRssi = 100;
    else if (rssi  < 0x20)
        NorRssi = 0;
    else
        NorRssi = (rssi  - 0x20) * 2;
		
    // ChannelQuality = W1*RSSI + W2*TxPRR + W3*RxPER	 (RSSI 0..100), (TxPER 100..0), (RxPER 100..0)
    ChannelQuality = (RSSI_WEIGHTING * NorRssi + 
					    TX_WEIGHTING * (100 - 0) + 
				        RX_WEIGHTING* (100 - 0)) / 100;

    if (ChannelQuality >= 100)
        ChannelQuality = 100;
        
    iq->qual = ChannelQuality;
    
#ifdef RTMP_EMBEDDED
    iq->level = rt_abs(rssi);   // signal level (dBm)
#else
    iq->level = abs(rssi);      // signal level (dBm)
#endif    
    iq->level += 256 - pAdapter->BbpRssiToDbmDelta;
    iq->noise = (pAdapter->BbpWriteLatch[17] > pAdapter->BbpTuning.R17UpperBoundG) ? pAdapter->BbpTuning.R17UpperBoundG : ((ULONG) pAdapter->BbpWriteLatch[17]); 	// noise level (dBm)
    iq->noise += 256 - 143;		
    iq->updated = pAdapter->iw_stats.qual.updated;
		
}

int rt_ioctl_iwaplist(struct net_device *dev,
			struct iw_request_info *info,
			struct iw_point *data, char *extra)
{

	PRTMP_ADAPTER pAdapter = (PRTMP_ADAPTER) dev->priv;
	struct sockaddr addr[IW_MAX_AP];
	struct iw_quality qual[IW_MAX_AP];
	int i;
	
    //check if the interface is down
    if (pAdapter->RTUSBCmdThr_pid < 0)
        return -ENETDOWN;

	for (i = 0; i <IW_MAX_AP ; i++)
	{
		if (i >=  pAdapter->ScanTab.BssNr)
			break;
		addr[i].sa_family = ARPHRD_ETHER;
			memcpy(addr[i].sa_data, &pAdapter->ScanTab.BssEntry[i].Bssid, MAC_ADDR_LEN);
		set_quality(pAdapter, &qual[i], pAdapter->ScanTab.BssEntry[i].Rssi);
	}
	data->length = i;
	memcpy(extra, &addr, i*sizeof(addr[0]));
	data->flags = 1;		/* signal quality present (sort of) */
	memcpy(extra + i*sizeof(addr[0]), &qual, i*sizeof(qual[i]));

	return 0;
}

#ifdef SIOCGIWSCAN
int rt_ioctl_siwscan(struct net_device *dev,
			struct iw_request_info *info,
			struct iw_point *data, char *extra)
{
	ULONG								Now;
	PRTMP_ADAPTER pAdapter = (PRTMP_ADAPTER) dev->priv;
	int Status = NDIS_STATUS_SUCCESS;
	//BOOLEAN		StateMachineTouched = FALSE;

    //check if the interface is down
    if (pAdapter->RTUSBCmdThr_pid < 0)
        return -ENETDOWN;

	if (RTMP_TEST_FLAG(pAdapter, fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS))
		return 0;
	do{
		Now = jiffies;

		if ((OPSTATUS_TEST_FLAG(pAdapter, fOP_STATUS_MEDIA_STATE_CONNECTED)) &&
			((pAdapter->PortCfg.AuthMode == Ndis802_11AuthModeWPA) || 
			(pAdapter->PortCfg.AuthMode == Ndis802_11AuthModeWPAPSK) ||
			(pAdapter->PortCfg.AuthMode == Ndis802_11AuthModeWPA2) ||
			(pAdapter->PortCfg.AuthMode == Ndis802_11AuthModeWPA2PSK)) &&
			(pAdapter->PortCfg.PortSecured == WPA_802_1X_PORT_NOT_SECURED)
			)
		{
			DBGPRINT(RT_DEBUG_TRACE, "!!! Link UP, Port Not Secured! ignore this set::OID_802_11_BSSID_LIST_SCAN\n");
			Status = NDIS_STATUS_SUCCESS;
			break;
		}
	
		if (pAdapter->Mlme.CntlMachine.CurrState != CNTL_IDLE  && (pAdapter->MLMEThr_pid > 0))
		{
			MlmeEnqueue(pAdapter, 
                        MLME_CNTL_STATE_MACHINE,
                        RT_CMD_RESET_MLME,
                        0,
                        NULL);
		}

		// Reset Missed scan number
		pAdapter->PortCfg.ScanCnt = 0;
		pAdapter->PortCfg.LastScanTime = Now;

		MlmeEnqueue(pAdapter, 
					MLME_CNTL_STATE_MACHINE, 
					OID_802_11_BSSID_LIST_SCAN, 
					0, 
					NULL);
		RTUSBMlmeUp(pAdapter);		 

		Status = NDIS_STATUS_SUCCESS;
		//StateMachineTouched = TRUE;
		
	}while(0);
	return 0;
}

#define MAX_CUSTOM_LEN 64
int
rt_ioctl_giwscan(struct net_device *dev,
			struct iw_request_info *info,
			struct iw_point *data, char *extra)
{

	PRTMP_ADAPTER pAdapter = (PRTMP_ADAPTER) dev->priv;
	int i=2, j;
	char *current_ev = extra, *previous_ev = extra;
	char *end_buf = extra + IW_SCAN_MAX_DATA;   // some of platforms restricted on IW_SCAN_MAX_DATA 
	char *current_val;
    struct iw_event iwe;

#if 0   // support bit rate, extended rate, quality and last beacon timing
    //-------------------------------------------
	char custom[MAX_CUSTOM_LEN];
	char *p;	
	char SupRateLen, ExtRateLen;
	char rate, max_rate;
    int  k;
	//-------------------------------------------
#endif	
	
    //check if the interface is down
    if (pAdapter->RTUSBCmdThr_pid < 0)
        return -ENETDOWN;

	if (RTMP_TEST_FLAG(pAdapter, fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS)){
		/*
		 * Still scanning, indicate the caller should try again.
		 */
		return -EAGAIN;
	}
	for (i = 0; i < pAdapter->ScanTab.BssNr; i++) 
	{
		if (current_ev >= end_buf)
			break;
		
		//MAC address
		//================================
		memset(&iwe, 0, sizeof(iwe));
		iwe.cmd = SIOCGIWAP;
		iwe.u.ap_addr.sa_family = ARPHRD_ETHER;
        memcpy(iwe.u.ap_addr.sa_data, &pAdapter->ScanTab.BssEntry[i].Bssid, ETH_ALEN);

        previous_ev = current_ev;
        current_ev = iwe_stream_add_event(current_ev,end_buf, &iwe, IW_EV_ADDR_LEN);
        if (current_ev == previous_ev)
            break;
            
		//ESSID	
		//================================
		memset(&iwe, 0, sizeof(iwe));
		iwe.cmd = SIOCGIWESSID;
		iwe.u.data.length = pAdapter->ScanTab.BssEntry[i].SsidLen;
		iwe.u.data.flags = 1;
 
        previous_ev = current_ev;
		current_ev = iwe_stream_add_point(current_ev,end_buf, &iwe, pAdapter->ScanTab.BssEntry[i].Ssid);
        if (current_ev == previous_ev)
            break;
            
		//Network Type 
		//================================
		memset(&iwe, 0, sizeof(iwe));
		iwe.cmd = SIOCGIWMODE;
		if (pAdapter->ScanTab.BssEntry[i].BssType == Ndis802_11IBSS)
		{
			iwe.u.mode = IW_MODE_ADHOC;
		}
		else if (pAdapter->ScanTab.BssEntry[i].BssType == Ndis802_11Infrastructure)
		{
			iwe.u.mode = IW_MODE_INFRA;
		}
		else
		{
			iwe.u.mode = IW_MODE_AUTO;
		}
		iwe.len = IW_EV_UINT_LEN;

        previous_ev = current_ev;
		current_ev = iwe_stream_add_event(current_ev, end_buf, &iwe,  IW_EV_UINT_LEN);
        if (current_ev == previous_ev)
            break;
            
		//Channel and Frequency
		//================================
		memset(&iwe, 0, sizeof(iwe));
		iwe.cmd = SIOCGIWFREQ;
		if (INFRA_ON(pAdapter) || ADHOC_ON(pAdapter))
			iwe.u.freq.m = pAdapter->ScanTab.BssEntry[i].Channel;
		else
			iwe.u.freq.m = pAdapter->ScanTab.BssEntry[i].Channel;
		iwe.u.freq.e = 0;
		iwe.u.freq.i = 0;
		
		previous_ev = current_ev;
		current_ev = iwe_stream_add_event(current_ev,end_buf, &iwe, IW_EV_FREQ_LEN);
        if (current_ev == previous_ev)
            break;
            
		//Encyption key
		//================================
		memset(&iwe, 0, sizeof(iwe));
		iwe.cmd = SIOCGIWENCODE;
		if (CAP_IS_PRIVACY_ON (pAdapter->ScanTab.BssEntry[i].CapabilityInfo ))
			iwe.u.data.flags =IW_ENCODE_ENABLED | IW_ENCODE_NOKEY;
		else
			iwe.u.data.flags = IW_ENCODE_DISABLED;

        previous_ev = current_ev;		
        current_ev = iwe_stream_add_point(current_ev, end_buf,&iwe,  (char *)pAdapter->SharedKey[(iwe.u.data.flags & IW_ENCODE_INDEX)-1].Key);
        if (current_ev == previous_ev)
            break;
            
#if 1	// support bit rate
		//Bit Rate
		//================================
		memset(&iwe, 0, sizeof(iwe));
		iwe.cmd = SIOCGIWRATE;		
		current_val = current_ev + IW_EV_LCP_LEN;
		//for (j = 0; j < pAdapter->ScanTab.BssEntry[i].RatesLen;j++)
		for (j = 0; j < 1;j++)
		{
			iwe.u.bitrate.value = RateIdToMbps[pAdapter->ScanTab.BssEntry[i].SupRate[i]/2] * 1000000;
			iwe.u.bitrate.disabled = 0;        
			current_val = iwe_stream_add_value(current_ev,
				current_val, end_buf, &iwe,
				IW_EV_PARAM_LEN);
		}

        
        if((current_val-current_ev)>IW_EV_LCP_LEN)
            current_ev = current_val;
        else
            break;
            

#else	// support bit rate, extended rate, quality and last beacon timing
        // max. of displays used IW_SCAN_MAX_DATA are about 22~24 cells
		//Bit Rate
		//================================
		memset(&iwe, 0, sizeof(iwe));
		iwe.cmd = SIOCGIWRATE;
		current_val = current_ev + IW_EV_LCP_LEN;
		
		SupRateLen = pAdapter->ScanTab.BssEntry[i].SupRateLen;
		ExtRateLen = pAdapter->ScanTab.BssEntry[i].ExtRateLen;
		
		max_rate = 0;	
		p = custom;
		p += snprintf(p, MAX_CUSTOM_LEN - (p - custom), " Rates (Mb/s): ");
		for (k = 0, j = 0; k < SupRateLen; ) 
		{
			if (j < ExtRateLen && 
			((pAdapter->ScanTab.BssEntry[i].ExtRate[j] & 0x7F) < 
				(pAdapter->ScanTab.BssEntry[i].SupRate[k] & 0x7F)))
			{
				rate = pAdapter->ScanTab.BssEntry[i].ExtRate[j++] & 0x7F;
			}	
			else
			{
				rate = pAdapter->ScanTab.BssEntry[i].SupRate[k++] & 0x7F;				
			}
			
			if (rate > max_rate)
				max_rate = rate;
			p += snprintf(p, MAX_CUSTOM_LEN - (p - custom), 
					  "%d%s ", rate >> 1, (rate & 1) ? ".5" : "");			
		}
			
		for (; j < ExtRateLen; j++) 
		{
			rate = pAdapter->ScanTab.BssEntry[i].ExtRate[j] & 0x7F;
			p += snprintf(p, MAX_CUSTOM_LEN - (p - custom), 
					  "%d%s ", rate >> 1, (rate & 1) ? ".5" : "");
			if (rate > max_rate)
				max_rate = rate;
		}		
		iwe.u.bitrate.value = max_rate * 500000;
		iwe.u.bitrate.disabled = 0;
		current_val = iwe_stream_add_value(current_ev,
			current_val, end_buf, &iwe,
			IW_EV_PARAM_LEN);
		if((current_val-current_ev)>IW_EV_LCP_LEN)
			current_ev = current_val;
		else
		    break;

		//Extended Rate	
		//================================
		memset(&iwe, 0, sizeof(iwe));
		iwe.cmd = IWEVCUSTOM;
		iwe.u.data.length = p - custom;
		if (iwe.u.data.length)
		{
		    previous_ev = current_ev;
			current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe, custom);
            if (current_ev == previous_ev)
                break;
        }
		//Quality Statistics
		//================================
		memset(&iwe, 0, sizeof(iwe));
		iwe.cmd = IWEVQUAL;
		set_quality(pAdapter, &iwe.u.qual, pAdapter->ScanTab.BssEntry[i].Rssi);

		previous_ev = current_ev;
		current_ev = iwe_stream_add_event(current_ev, end_buf, &iwe, IW_EV_QUAL_LEN);
        if (current_ev == previous_ev)
            break;

		//Age to display seconds since last beacon
		//================================
		memset(&iwe, 0, sizeof(iwe));		
		iwe.cmd = IWEVCUSTOM;
		p = custom;
		p += snprintf(p, MAX_CUSTOM_LEN - (p - custom),
				  " Last beacon: %lums ago", (jiffies - pAdapter->ScanTab.BssEntry[i].LastBeaconRxTime) / (HZ / 100));
		iwe.u.data.length = p - custom;
		if (iwe.u.data.length)
		{   
		    previous_ev = current_ev;
			current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe, custom);
            if (current_ev == previous_ev)
                break;
        }
#endif

		//================================
		memset(&iwe, 0, sizeof(iwe));

	}
	data->length = current_ev - extra;
	DBGPRINT(RT_DEBUG_TRACE,"===>rt_ioctl_giwscan. %d(%d) BSS returned\n",i , pAdapter->ScanTab.BssNr);					
	return 0;
}
#endif

int rt_ioctl_siwessid(struct net_device *dev,
			 struct iw_request_info *info,
			 struct iw_point *data, char *essid)
{

	PRTMP_ADAPTER pAdapter = (PRTMP_ADAPTER) dev->priv;
	NDIS_802_11_SSID					Ssid, *pSsid=NULL;

    //check if the interface is down
    if (pAdapter->RTUSBCmdThr_pid < 0)
        return -ENETDOWN;

    memset(&Ssid, 0x00, sizeof(NDIS_802_11_SSID));

	if (data->flags)
	{
		if (data->length > IW_ESSID_MAX_SIZE)
		{
			return -E2BIG;
		}

		memcpy(Ssid.Ssid, essid, (data->length - 1));
		Ssid.SsidLength = data->length - 1;	//minus null character.
	}
	else
	{
		Ssid.SsidLength = 0;  // ANY ssid 
        memcpy(Ssid.Ssid, "", 0);		    
    }
    
	pSsid = &Ssid;
	if ((pAdapter->Mlme.CntlMachine.CurrState != CNTL_IDLE) && (pAdapter->MLMEThr_pid > 0))
	{
		MlmeEnqueue(pAdapter, 
                    MLME_CNTL_STATE_MACHINE,
                    RT_CMD_RESET_MLME,
                    0,
                    NULL);	
    }

	// tell CNTL state machine to call NdisMSetInformationComplete() after completing
	// this request, because this request is initiated by NDIS.

	// To prevent some kernel thread is very low priority ...so essid copy immediately for later wpapsk counting.
	if (pAdapter->PortCfg.AuthMode >= Ndis802_11AuthModeWPA)
	{
		NdisMoveMemory(pAdapter->MlmeAux.Ssid, Ssid.Ssid, Ssid.SsidLength);
		pAdapter->MlmeAux.SsidLen = Ssid.SsidLength;
	}

	DBGPRINT(RT_DEBUG_TRACE,"===>rt_ioctl_siwessid:: (Ssid.SsidLength = %d, %s)\n",Ssid.SsidLength, Ssid.Ssid);		

	if (pAdapter->MLMEThr_pid > 0)
	{
		MlmeEnqueue(pAdapter, 
					MLME_CNTL_STATE_MACHINE, 
					OID_802_11_SSID,
					sizeof(NDIS_802_11_SSID),
					(VOID *)pSsid);
	}			
	RTUSBMlmeUp(pAdapter);
		
	return 0;
}

int rt_ioctl_giwessid(struct net_device *dev,
			 struct iw_request_info *info,
			 struct iw_point *data, char *essid)
{

	PRTMP_ADAPTER pAdapter = (PRTMP_ADAPTER) dev->priv;

    //check if the interface is down
    if (pAdapter->RTUSBCmdThr_pid < 0)
        return -ENETDOWN;

	data->flags = 1;		/* active */
	if (OPSTATUS_TEST_FLAG(pAdapter, fOP_STATUS_MEDIA_STATE_CONNECTED))
	{
		DBGPRINT(RT_DEBUG_TRACE,"MediaState is connected\n");
		data->length = pAdapter->PortCfg.SsidLen;
		memcpy(essid, pAdapter->PortCfg.Ssid, pAdapter->PortCfg.SsidLen);
		pAdapter->PortCfg.Ssid[pAdapter->PortCfg.SsidLen] = '\0';
		DBGPRINT(RT_DEBUG_TRACE,"pAdapter->PortCfg.Ssid=%s , Ssidlen = %d\n",pAdapter->PortCfg.Ssid, pAdapter->PortCfg.SsidLen);
	}
	else
	{//the ANY ssid was specified
		data->length  = 0;
		DBGPRINT(RT_DEBUG_TRACE,"MediaState is not connected, ess\n");
	}
	DBGPRINT(RT_DEBUG_TRACE, "===>rt_ioctl_giwessid:: (Len=%d, ssid=%s...)\n", pAdapter->PortCfg.SsidLen, pAdapter->PortCfg.Ssid);

	return 0;

}

int rt_ioctl_siwnickn(struct net_device *dev,
			 struct iw_request_info *info,
			 struct iw_point *data, char *nickname)
{
	PRTMP_ADAPTER pAdapter = (PRTMP_ADAPTER) dev->priv;

    //check if the interface is down
    if (pAdapter->RTUSBCmdThr_pid < 0)
        return -ENETDOWN;

	if (data->length > IW_ESSID_MAX_SIZE)
		return -EINVAL;

	memset(pAdapter->nickn, 0, IW_ESSID_MAX_SIZE);
	memcpy(pAdapter->nickn, nickname, data->length);


	return 0;
}

int rt_ioctl_giwnickn(struct net_device *dev,
			 struct iw_request_info *info,
			 struct iw_point *data, char *nickname)
{
	PRTMP_ADAPTER pAdapter = (PRTMP_ADAPTER) dev->priv;

    //check if the interface is down
    if (pAdapter->RTUSBCmdThr_pid < 0)
        return -ENETDOWN;

	if (data->length > strlen(pAdapter->nickn) + 1)
		data->length = strlen(pAdapter->nickn) + 1;
	if (data->length > 0) {
		memcpy(nickname, pAdapter->nickn, data->length-1);
		nickname[data->length-1] = '\0';
	}
	return 0;
}

int rt_ioctl_siwrts(struct net_device *dev,
			   struct iw_request_info *info,
			   struct iw_param *rts, char *extra)
{
	PRTMP_ADAPTER pAdapter = (PRTMP_ADAPTER) dev->priv;
	u16 val;

    //check if the interface is down
    if (pAdapter->RTUSBCmdThr_pid < 0)
        return -ENETDOWN;

	if (rts->disabled)
		val = MAX_RTS_THRESHOLD;
	else if (rts->value < 0 || rts->value > MAX_RTS_THRESHOLD)
		return -EINVAL;
	else if (rts->value == 0)
		val = MAX_RTS_THRESHOLD;
	else
		val = rts->value;
	
	if (val != pAdapter->PortCfg.RtsThreshold)
		pAdapter->PortCfg.RtsThreshold = val;

	return 0;
}

int rt_ioctl_giwrts(struct net_device *dev,
			   struct iw_request_info *info,
			   struct iw_param *rts, char *extra)
{
	PRTMP_ADAPTER pAdapter = (PRTMP_ADAPTER) dev->priv;

    //check if the interface is down
    if (pAdapter->RTUSBCmdThr_pid < 0)
        return -ENETDOWN;

	rts->value = pAdapter->PortCfg.RtsThreshold;
	rts->disabled = (rts->value == MAX_RTS_THRESHOLD);
	rts->fixed = 1;

	return 0;
}

int rt_ioctl_siwfrag(struct net_device *dev,
			struct iw_request_info *info,
			struct iw_param *rts, char *extra)
{
	PRTMP_ADAPTER pAdapter = (PRTMP_ADAPTER) dev->priv;
	u16 val;

    //check if the interface is down
    if (pAdapter->RTUSBCmdThr_pid < 0)
        return -ENETDOWN;

	if (rts->disabled)
		val = MAX_FRAG_THRESHOLD;
	else if (rts->value >= MIN_FRAG_THRESHOLD || rts->value <= MAX_FRAG_THRESHOLD)
		val = __cpu_to_le16(rts->value & ~0x1); /* even numbers only */
	else if (rts->value == 0)
		val = MAX_FRAG_THRESHOLD;
	else
		return -EINVAL;

	pAdapter->PortCfg.FragmentThreshold = val;
	return 0;
}

int rt_ioctl_giwfrag(struct net_device *dev,
			struct iw_request_info *info,
			struct iw_param *rts, char *extra)
{
	PRTMP_ADAPTER pAdapter = (PRTMP_ADAPTER) dev->priv;

    //check if the interface is down
    if (pAdapter->RTUSBCmdThr_pid < 0)
        return -ENETDOWN;

	rts->value = pAdapter->PortCfg.FragmentThreshold;
	rts->disabled = (rts->value == 2346);
	rts->fixed = 1;

	return 0;
}

int rt_ioctl_siwencode(struct net_device *dev,
			  struct iw_request_info *info,
			  struct iw_point *erq, char *keybuf)
{
	PRTMP_ADAPTER pAdapter = (PRTMP_ADAPTER) dev->priv;
	union {
		char buf[sizeof(NDIS_802_11_WEP)+MAX_LEN_OF_KEY- 1];
		NDIS_802_11_WEP keyinfo;
	} WepKey;
	int 	index, i, len;
	CHAR	kid = 0;

    //check if the interface is down
    if (pAdapter->RTUSBCmdThr_pid < 0)
        return -ENETDOWN;

	NdisZeroMemory(&WepKey, sizeof(WepKey));
	
	if (erq->flags & IW_ENCODE_DISABLED)
	{
		pAdapter->PortCfg.PairCipher = Ndis802_11WEPDisabled;
		pAdapter->PortCfg.GroupCipher = Ndis802_11WEPDisabled;
		pAdapter->PortCfg.WepStatus = Ndis802_11WEPDisabled;
	}
	else
	{
		pAdapter->PortCfg.PairCipher = Ndis802_11WEPEnabled;
		pAdapter->PortCfg.GroupCipher = Ndis802_11WEPEnabled;
		pAdapter->PortCfg.WepStatus = Ndis802_11WEPEnabled;
	}

	if (erq->flags & IW_ENCODE_RESTRICTED)
		pAdapter->PortCfg.AuthMode = Ndis802_11AuthModeShared;
	else 
		pAdapter->PortCfg.AuthMode = Ndis802_11AuthModeOpen;

	if(pAdapter->PortCfg.WepStatus == Ndis802_11WEPDisabled)
		pAdapter->PortCfg.AuthMode = Ndis802_11AuthModeOpen;
	
	if ((erq->flags & IW_ENCODE_DISABLED) == 0) 
	{
		/* Enable crypto. */
		if (erq->length > IFNAMSIZ)
			return -EINVAL;

		/* Old solution to take  default key  */
		index = (erq->flags & IW_ENCODE_INDEX) ;
		if((index < 0) || (index > NR_WEP_KEYS))
			return -EINVAL; 	
		DBGPRINT(RT_DEBUG_TRACE," erq->flags = %x\n",erq->flags);
		
		if (index != 0)
		{
			pAdapter->PortCfg.DefaultKeyId = index -1;
		}

		if ((erq->length == 1) && (index == 0))
		{
			/* New solution to take  default key  when old way not work, not change KeyMaterial*/
			NdisMoveMemory(&kid, keybuf, 1 );
			if((index < 0) || (index >= NR_WEP_KEYS))
				return -EINVAL; 	
			
			DBGPRINT(RT_DEBUG_TRACE,"kid = %d , erq->length = %d\n",kid, erq->length);
			if (kid > 0)
				pAdapter->PortCfg.DefaultKeyId = kid-1;
			else 
				pAdapter->PortCfg.DefaultKeyId = 0;
		}
		else
		{	
			DBGPRINT(RT_DEBUG_TRACE,"DefaultKeyId = %d , erq->length = %d, flags 0x%x\n",pAdapter->PortCfg.DefaultKeyId, erq->length,erq->flags);
			len = erq->length;
			if(len > WEP_LARGE_KEY_LEN)
				len = WEP_LARGE_KEY_LEN;
				
			// If this instruction default key
			NdisZeroMemory(pAdapter->SharedKey[pAdapter->PortCfg.DefaultKeyId].Key,  MAX_LEN_OF_KEY);		  
			NdisMoveMemory(pAdapter->SharedKey[pAdapter->PortCfg.DefaultKeyId].Key, keybuf, len);  
			NdisMoveMemory(WepKey.keyinfo.KeyMaterial, keybuf, len);
			WepKey.keyinfo.KeyIndex = 0x80000000 + pAdapter->PortCfg.DefaultKeyId; 
			WepKey.keyinfo.KeyLength = len;
			pAdapter->SharedKey[pAdapter->PortCfg.DefaultKeyId].KeyLen =(UCHAR) (len <= WEP_SMALL_KEY_LEN ? WEP_SMALL_KEY_LEN : WEP_LARGE_KEY_LEN);
			DBGPRINT(RT_DEBUG_TRACE,"SharedKey	");
			for (i=0; i < 5;i++)
				DBGPRINT_RAW(RT_DEBUG_TRACE,"	%x ", pAdapter->SharedKey[pAdapter->PortCfg.DefaultKeyId].Key[i]);
			DBGPRINT(RT_DEBUG_TRACE,   "\n");
			// need to enqueue cmd to thread
			RTUSBEnqueueCmdFromNdis(pAdapter, OID_802_11_ADD_WEP, TRUE, &WepKey, sizeof(WepKey.keyinfo) + len - 1);

		}
		
	}
	DBGPRINT(RT_DEBUG_TRACE, "==>rt_ioctl_siwencode::AuthMode=%x\n",pAdapter->PortCfg.AuthMode);
	DBGPRINT(RT_DEBUG_TRACE, "==>rt_ioctl_siwencode::DefaultKeyId=%x, KeyLen = %d\n",pAdapter->PortCfg.DefaultKeyId , pAdapter->SharedKey[pAdapter->PortCfg.DefaultKeyId].KeyLen);
	DBGPRINT(RT_DEBUG_TRACE, "==>rt_ioctl_siwencode::WepStatus=%x\n",pAdapter->PortCfg.WepStatus);
	return 0;
}

int
rt_ioctl_giwencode(struct net_device *dev,
			  struct iw_request_info *info,
			  struct iw_point *erq, char *key)
{
	PRTMP_ADAPTER pAdapter = (PRTMP_ADAPTER) dev->priv;
	int kid;
	
    //check if the interface is down
    if (pAdapter->RTUSBCmdThr_pid < 0)
        return -ENETDOWN;

	kid = erq->flags & IW_ENCODE_INDEX;
	DBGPRINT(RT_DEBUG_TRACE, "===>rt_ioctl_giwencode %d\n", erq->flags & IW_ENCODE_INDEX);

	if (pAdapter->PortCfg.WepStatus == Ndis802_11WEPDisabled)
	{
		erq->length = 0;
		erq->flags = IW_ENCODE_DISABLED;
	} 
	else if ((kid > 0) && (kid <=4))
	{
		// copy wep key
		erq->flags = kid ;			/* NB: base 1 */
		if (erq->length > pAdapter->SharedKey[kid-1].KeyLen)
			erq->length = pAdapter->SharedKey[kid-1].KeyLen;
		memcpy(key, pAdapter->SharedKey[kid-1].Key, erq->length);
		//if ((kid == pAdapter->PortCfg.DefaultKeyId))
		//erq->flags |= IW_ENCODE_ENABLED;	/* XXX */
		if (pAdapter->PortCfg.AuthMode == Ndis802_11AuthModeShared)
			erq->flags |= IW_ENCODE_RESTRICTED;		/* XXX */
		else
			erq->flags |= IW_ENCODE_OPEN;		/* XXX */
		
	}
	else if (kid == 0)
	{
		if (pAdapter->PortCfg.AuthMode == Ndis802_11AuthModeShared)
			erq->flags |= IW_ENCODE_RESTRICTED;		/* XXX */
		else
			erq->flags |= IW_ENCODE_OPEN;		/* XXX */
		erq->length = pAdapter->SharedKey[pAdapter->PortCfg.DefaultKeyId].KeyLen;
		memcpy(key, pAdapter->SharedKey[pAdapter->PortCfg.DefaultKeyId].Key, erq->length);
		// copy default key ID
		if (pAdapter->PortCfg.AuthMode == Ndis802_11AuthModeShared)
			erq->flags |= IW_ENCODE_RESTRICTED;		/* XXX */
		else
			erq->flags |= IW_ENCODE_OPEN;		/* XXX */
		erq->flags = pAdapter->PortCfg.DefaultKeyId + 1;			/* NB: base 1 */
		erq->flags |= IW_ENCODE_ENABLED;	/* XXX */
	}
		
	return 0;

}

static int
rt_ioctl_setparam(struct net_device *dev, struct iw_request_info *info,
			 void *w, char *extra)
{
	PRTMP_ADAPTER pAdapter = (PRTMP_ADAPTER) dev->priv;
	char *this_char = extra;
	char *value;
	int  Status=0;

    //check if the interface is down
    if (pAdapter->RTUSBCmdThr_pid < 0)
        return -ENETDOWN;

	if (!*this_char)																											
		return Status;																													
																																
	if ((value = rtstrchr(this_char, '=')) != NULL) 																			
		*value++ = 0;																											

	if (!value) 																									 
		return Status;																													

	// reject setting nothing besides ANY ssid(ssidLen=0)
	if (!*value && (strcmp(this_char, "SSID") != 0))
		return Status;																													

	for (PRTMP_PRIVATE_SET_PROC = RTMP_PRIVATE_SUPPORT_PROC; PRTMP_PRIVATE_SET_PROC->name; PRTMP_PRIVATE_SET_PROC++)			
	{																															
		if (strcmp(this_char, PRTMP_PRIVATE_SET_PROC->name) == 0)																
		{																														
			if(!PRTMP_PRIVATE_SET_PROC->set_proc(pAdapter, value))																
			{	//FALSE:Set private failed then return Invalid argument 														
				Status = -EINVAL;																								
			}																													
			break;	//Exit for loop.																						
		}																														
	}																															
																																
	if(PRTMP_PRIVATE_SET_PROC->name == NULL)																					
	{  //Not found argument 																									
		Status = -EINVAL;																										
		DBGPRINT(RT_DEBUG_TRACE, "===>rt_ioctl_setparam:: (iwpriv) Not Support Set Command [%s=%s]\n", this_char, value);																												  
	}																															
  
	return Status;
}

static const iw_handler rt_handler[] =
{
	(iw_handler) NULL,						/* SIOCSIWCOMMIT */
	(iw_handler) rt_ioctl_giwname,			/* SIOCGIWNAME	1 */
	(iw_handler) NULL,						/* SIOCSIWNWID */
	(iw_handler) NULL,						/* SIOCGIWNWID */
	(iw_handler) rt_ioctl_siwfreq,			/* SIOCSIWFREQ */
	(iw_handler) rt_ioctl_giwfreq,			/* SIOCGIWFREQ	5 */
	(iw_handler) rt_ioctl_siwmode,			/* SIOCSIWMODE */
	(iw_handler) rt_ioctl_giwmode,			/* SIOCGIWMODE */
	(iw_handler) NULL,						/* SIOCSIWSENS */
	(iw_handler) NULL,						/* SIOCGIWSENS */
	(iw_handler) NULL /* not used */,		/* SIOCSIWRANGE */
	(iw_handler) rt_ioctl_giwrange,			/* SIOCGIWRANGE 11 */
	(iw_handler) NULL /* not used */,		/* SIOCSIWPRIV */
	(iw_handler) NULL /* kernel code */,	/* SIOCGIWPRIV */
	(iw_handler) NULL /* not used */,		/* SIOCSIWSTATS */
	(iw_handler) NULL /* kernel code */,	/* SIOCGIWSTATS f */
	(iw_handler) NULL,						/* SIOCSIWSPY */
	(iw_handler) NULL,						/* SIOCGIWSPY */
	(iw_handler) NULL,						/* -- hole -- */
	(iw_handler) NULL,						/* -- hole -- */
	(iw_handler) NULL,						/* SIOCSIWAP */
	(iw_handler) rt_ioctl_giwap,			/* SIOCGIWAP	0x15*/
	(iw_handler) NULL,						/* -- hole --	0x16 */
	(iw_handler) rt_ioctl_iwaplist,			/* SIOCGIWAPLIST */
#ifdef SIOCGIWSCAN
	(iw_handler) rt_ioctl_siwscan,			/* SIOCSIWSCAN	0x18*/
	(iw_handler) rt_ioctl_giwscan,			/* SIOCGIWSCAN */
#else
	(iw_handler) NULL,						/* SIOCSIWSCAN */
	(iw_handler) NULL,						/* SIOCGIWSCAN */
#endif /* SIOCGIWSCAN */
	(iw_handler) rt_ioctl_siwessid,			/* SIOCSIWESSID */
	(iw_handler) rt_ioctl_giwessid,			/* SIOCGIWESSID */
	(iw_handler) rt_ioctl_siwnickn,			/* SIOCSIWNICKN */
	(iw_handler) rt_ioctl_giwnickn,			/* SIOCGIWNICKN 1d*/
	(iw_handler) NULL,						/* -- hole -- */
	(iw_handler) NULL,						/* -- hole -- */
	(iw_handler) NULL,						/* SIOCSIWRATE	20*/
	(iw_handler) NULL,						/* SIOCGIWRATE */
	(iw_handler) rt_ioctl_siwrts,			/* SIOCSIWRTS */
	(iw_handler) rt_ioctl_giwrts,			/* SIOCGIWRTS */
	(iw_handler) rt_ioctl_siwfrag,			/* SIOCSIWFRAG */
	(iw_handler) rt_ioctl_giwfrag,			/* SIOCGIWFRAG	25*/
	(iw_handler) NULL,						/* SIOCSIWTXPOW */
	(iw_handler) NULL,						/* SIOCGIWTXPOW */
	(iw_handler) NULL,						/* SIOCSIWRETRY */
	(iw_handler) NULL,						/* SIOCGIWRETRY  29*/
	(iw_handler) rt_ioctl_siwencode,		/* SIOCSIWENCODE 2a*/
	(iw_handler) rt_ioctl_giwencode,		/* SIOCGIWENCODE 2b*/
	(iw_handler) NULL,						/* SIOCSIWPOWER  2c*/
	(iw_handler) NULL,						/* SIOCGIWPOWER  2d*/
};

static const iw_handler rt_priv_handlers[] = {
	(iw_handler) NULL,					/* SIOCWFIRSTPRIV+0 */
	(iw_handler) rt_ioctl_setparam,		/* SIOCWFIRSTPRIV+2 */
};

const struct iw_handler_def rt73_iw_handler_def =
{
#define	N(a)	(sizeof (a) / sizeof (a[0]))
	.standard	= (iw_handler *) rt_handler,
	.num_standard	= sizeof(rt_handler) / sizeof(iw_handler),
	.private	= (iw_handler *) rt_priv_handlers,
	.num_private		= N(rt_priv_handlers),
	.private_args	= (struct iw_priv_args *) privtab,
	.num_private_args	= N(privtab),
#if WIRELESS_EXT > 15
//	.spy_offset	= offsetof(struct hostap_interface, spy_data),
#endif /* WIRELESS_EXT > 15 */
};

INT RTMPSetInformation(
	IN	PRTMP_ADAPTER pAdapter,
	IN	OUT struct ifreq	*rq,
	IN	INT 				cmd)
{
	struct iwreq						*wrq = (struct iwreq *) rq;
	NDIS_802_11_SSID					Ssid, *pSsid=NULL;
	NDIS_802_11_MAC_ADDRESS 			Bssid;
	RT_802_11_PHY_MODE					PhyMode;
	RT_802_11_STA_CONFIG				StaConfig, *pStaConfig=NULL;
	NDIS_802_11_RATES					aryRates;
	RT_802_11_PREAMBLE					Preamble;
	NDIS_802_11_WEP_STATUS				WepStatus;
	NDIS_802_11_AUTHENTICATION_MODE 	AuthMode;
	NDIS_802_11_NETWORK_INFRASTRUCTURE	BssType;
	NDIS_802_11_RTS_THRESHOLD			RtsThresh;
	NDIS_802_11_FRAGMENTATION_THRESHOLD FragThresh;
	NDIS_802_11_POWER_MODE				PowerMode;
	NDIS_802_11_TX_POWER_LEVEL			TxPowerLevel;
	PNDIS_802_11_KEY					pKey = NULL;
	PNDIS_802_11_REMOVE_KEY 			pRemoveKey = NULL;
	NDIS_802_11_CONFIGURATION			Config, *pConfig = NULL;
	NDIS_802_11_NETWORK_TYPE			NetType;
	ULONG								Now;
	ULONG								KeyIdx; 
	INT 								Status = NDIS_STATUS_SUCCESS;
	ULONG								AntDiv;
	BOOLEAN 							RadioState;
    BOOLEAN							    StateMachineTouched = FALSE;

#if WPA_SUPPLICANT_SUPPORT    
    PNDIS_802_11_WEP			        pWepKey =NULL;
    PNDIS_802_11_PMKID                  pPmkId = NULL;
    BOOLEAN				                IEEE8021xState;
    BOOLEAN				                IEEE8021x_required_keys;
    BOOLEAN                             wpa_supplicant_enable;
#endif      

	switch(cmd & 0x7FFF) {
		case RT_OID_802_11_COUNTRY_REGION:
			if (wrq->u.data.length < sizeof(UCHAR))
				Status = -EINVAL;
			else if (!(pAdapter->PortCfg.CountryRegion & 0x80) && !(pAdapter->PortCfg.CountryRegionForABand & 0x80))	// Only avaliable when EEPROM not programming
			{
				ULONG	Country;
				UCHAR	TmpPhy;

				Status = copy_from_user(&Country, wrq->u.data.pointer, wrq->u.data.length);   
				pAdapter->PortCfg.CountryRegion = (UCHAR)(Country & 0x000000FF);
				pAdapter->PortCfg.CountryRegionForABand = (UCHAR)((Country >> 8) & 0x000000FF);

				TmpPhy = pAdapter->PortCfg.PhyMode;
				pAdapter->PortCfg.PhyMode = 0xff;
				// Build all corresponding channel information
				Status = RTUSBEnqueueCmdFromNdis(pAdapter, RT_OID_802_11_SET_PHY_MODE, TRUE, &TmpPhy, sizeof(TmpPhy));
				DBGPRINT(RT_DEBUG_TRACE, "Set::RT_OID_802_11_SET_COUNTRY_REGION (A:%d  B/G:%d)\n", pAdapter->PortCfg.CountryRegionForABand, 
					pAdapter->PortCfg.CountryRegion);
			}
			break;	 
		case OID_802_11_BSSID_LIST_SCAN:
			Now = jiffies;
			DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_BSSID_LIST_SCAN, TxCnt = %d \n", pAdapter->BulkLastOneSecCount);

#ifdef RALINK_ATE			
			if(pAdapter->ate.Mode != ATE_STASTART)
			{
				DBGPRINT(RT_DEBUG_TRACE, "The driver is in ATE mode now\n");
				break;
			}	
#endif				

			if (pAdapter->BulkLastOneSecCount > 100)
			{
				DBGPRINT(RT_DEBUG_TRACE, "!!! Link UP, ignore this set::OID_802_11_BSSID_LIST_SCAN\n");
				//Status = NDIS_STATUS_SUCCESS;
				pAdapter->PortCfg.ScanCnt = 99;		// Prevent auto scan triggered by this OID
				break;
			}
			
			if ((OPSTATUS_TEST_FLAG(pAdapter, fOP_STATUS_MEDIA_STATE_CONNECTED)) &&
				((pAdapter->PortCfg.AuthMode == Ndis802_11AuthModeWPA) || 
				(pAdapter->PortCfg.AuthMode == Ndis802_11AuthModeWPAPSK) ||
				(pAdapter->PortCfg.AuthMode == Ndis802_11AuthModeWPA2) ||
				(pAdapter->PortCfg.AuthMode == Ndis802_11AuthModeWPA2PSK)
#if WPA_SUPPLICANT_SUPPORT
			  	 || (pAdapter->PortCfg.IEEE8021X == TRUE)		
#endif 					
				) &&
				(pAdapter->PortCfg.PortSecured == WPA_802_1X_PORT_NOT_SECURED))
			{
				DBGPRINT(RT_DEBUG_TRACE, "!!! Link UP, Port Not Secured! ignore this set::OID_802_11_BSSID_LIST_SCAN\n");
				Status = NDIS_STATUS_SUCCESS;
				pAdapter->PortCfg.ScanCnt = 99;		// Prevent auto scan triggered by this OID
				break;
			}

			Status = RTUSBEnqueueCmdFromNdis(pAdapter, OID_802_11_BSSID_LIST_SCAN, TRUE, NULL, 0);
			break;
		case OID_802_11_SSID: 
			if (wrq->u.data.length != sizeof(NDIS_802_11_SSID))
				Status = -EINVAL;
			else
			{
				Status = copy_from_user(&Ssid, wrq->u.data.pointer, wrq->u.data.length);
				pSsid = &Ssid;

				if (pSsid->SsidLength > MAX_LEN_OF_SSID)
					Status = -EINVAL;
				else
				{
					// reset SSID to null
					if (pSsid->SsidLength == 0)
					{
						memcpy(pSsid->Ssid, "", 0);
					}

					RTUSBEnqueueCmdFromNdis(pAdapter, OID_802_11_SSID, TRUE, pSsid, sizeof(NDIS_802_11_SSID));
					DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_SSID (Len=%d,Ssid=%s)\n", pSsid->SsidLength, pSsid->Ssid);
				}
			}
			break;
		case OID_802_11_BSSID:
#ifdef RALINK_ATE			
			if(pAdapter->ate.Mode != ATE_STASTART)
			{
				DBGPRINT(RT_DEBUG_TRACE, "The driver is in ATE mode now\n");
				Status = NDIS_STATUS_RESOURCES;
				break;
			}	
#endif				
		
			if (wrq->u.data.length != sizeof(NDIS_802_11_MAC_ADDRESS))
				Status	= -EINVAL;
			else
			{
				Status = copy_from_user(&Bssid, wrq->u.data.pointer, wrq->u.data.length);
		   
				RTUSBEnqueueCmdFromNdis(pAdapter, RT_OID_802_11_BSSID, TRUE, &Bssid, wrq->u.data.length);
				DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_BSSID %02x:%02x:%02x:%02x:%02x:%02x\n",
										Bssid[0], Bssid[1], Bssid[2], Bssid[3], Bssid[4], Bssid[5]);
			}
			break;
		case RT_OID_802_11_RADIO:
			if (wrq->u.data.length != sizeof(BOOLEAN))
				Status	= -EINVAL;
			else
			{
				Status = copy_from_user(&RadioState, wrq->u.data.pointer, wrq->u.data.length);
				DBGPRINT(RT_DEBUG_TRACE, "Set::RT_OID_802_11_RADIO (=%d)\n", RadioState);
				if (pAdapter->PortCfg.bSwRadio != RadioState)
				{
					pAdapter->PortCfg.bSwRadio = RadioState;
					if (pAdapter->PortCfg.bRadio != (pAdapter->PortCfg.bHwRadio && pAdapter->PortCfg.bSwRadio))
					{
						pAdapter->PortCfg.bRadio = (pAdapter->PortCfg.bHwRadio && pAdapter->PortCfg.bSwRadio);
						RTUSBEnqueueCmdFromNdis(pAdapter, RT_OID_SET_RADIO, TRUE, NULL, 0);
					}
				}
			}
			break;
		case RT_OID_802_11_PHY_MODE:
			if (wrq->u.data.length != sizeof(RT_802_11_PHY_MODE))
				Status	= -EINVAL;
			else
			{
				Status = copy_from_user(&PhyMode, wrq->u.data.pointer, wrq->u.data.length);
				RTUSBEnqueueCmdFromNdis(pAdapter, RT_OID_802_11_PHY_MODE, TRUE, &PhyMode, sizeof(RT_802_11_PHY_MODE));
				DBGPRINT(RT_DEBUG_TRACE, "Set::RT_OID_802_11_PHY_MODE (=%d)\n", PhyMode);
			}
			break; 
		case RT_OID_802_11_STA_CONFIG:
			if (wrq->u.data.length != sizeof(RT_802_11_STA_CONFIG))
				Status	= -EINVAL;
			else
			{	
				Status = copy_from_user(&StaConfig, wrq->u.data.pointer, wrq->u.data.length);
				pStaConfig = &StaConfig;
				RTUSBEnqueueCmdFromNdis(pAdapter, RT_OID_802_11_STA_CONFIG, TRUE, pStaConfig, sizeof(RT_802_11_STA_CONFIG));

				DBGPRINT(RT_DEBUG_TRACE, "Set::RT_OID_802_11_SET_STA_CONFIG (Burst=%d,BGprot=%d,ShortSlot=%d,Adhoc=%d\n",
					pStaConfig->EnableTxBurst,
					pStaConfig->UseBGProtection,
					pStaConfig->UseShortSlotTime,
					pStaConfig->AdhocMode);
			}
			break;
		case OID_802_11_DESIRED_RATES:
			if (wrq->u.data.length != sizeof(NDIS_802_11_RATES))
				Status	= -EINVAL;
			else
			{
				Status = copy_from_user(&aryRates, wrq->u.data.pointer, wrq->u.data.length);
				NdisZeroMemory(pAdapter->PortCfg.DesireRate, MAX_LEN_OF_SUPPORTED_RATES);
				NdisMoveMemory(pAdapter->PortCfg.DesireRate, &aryRates, sizeof(NDIS_802_11_RATES));
				DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_DESIRED_RATES (%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x)\n",
					pAdapter->PortCfg.DesireRate[0],pAdapter->PortCfg.DesireRate[1],
					pAdapter->PortCfg.DesireRate[2],pAdapter->PortCfg.DesireRate[3],
					pAdapter->PortCfg.DesireRate[4],pAdapter->PortCfg.DesireRate[5],
					pAdapter->PortCfg.DesireRate[6],pAdapter->PortCfg.DesireRate[7] );
					// Changing DesiredRate may affect the MAX TX rate we used to TX frames out
					RTUSBEnqueueCmdFromNdis(pAdapter, RT_OID_UPDATE_TX_RATE, TRUE, NULL, 0);
			}
			break;
		case RT_OID_802_11_PREAMBLE:
			if (wrq->u.data.length != sizeof(RT_802_11_PREAMBLE))
				Status	= -EINVAL;
			else
			{
				Status = copy_from_user(&Preamble, wrq->u.data.pointer, wrq->u.data.length);
				if ((Preamble == Rt802_11PreambleShort) || (Preamble == Rt802_11PreambleLong) || (Preamble == Rt802_11PreambleAuto))
				{
					RTUSBEnqueueCmdFromNdis(pAdapter, RT_OID_802_11_PREAMBLE, TRUE, &Preamble, sizeof(RT_802_11_PREAMBLE));
				}
				else
				{
					Status = -EINVAL;
					break;
				}
				DBGPRINT(RT_DEBUG_TRACE, "Set::RT_OID_802_11_SET_PREAMBLE (=%d)\n", Preamble);
			}
			break;
		case OID_802_11_WEP_STATUS:
			if (wrq->u.data.length != sizeof(NDIS_802_11_WEP_STATUS))
				Status	= -EINVAL;
			else
			{
				Status = copy_from_user(&WepStatus, wrq->u.data.pointer, wrq->u.data.length);
				// Since TKIP, AES, WEP are all supported. It should not have any invalid setting
				if (WepStatus <= Ndis802_11Encryption3KeyAbsent)
				{
					if (pAdapter->PortCfg.WepStatus != WepStatus)
					{
						// Config has changed
						pAdapter->bConfigChanged = TRUE;
					}
					//Status = RTUSBEnqueueCmdFromNdis(pAdapter, OID_802_11_WEP_STATUS, TRUE, &WepStatus, sizeof(NDIS_802_11_WEP_STATUS));
					pAdapter->PortCfg.WepStatus 	= WepStatus;
					pAdapter->PortCfg.OrigWepStatus = WepStatus;
					pAdapter->PortCfg.PairCipher	= WepStatus;
					pAdapter->PortCfg.GroupCipher	= WepStatus;	
				}
				else
				{
					Status	= -EINVAL;
					break;
				}
				DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_WEP_STATUS (=%d)\n",WepStatus);
			}
			break;		
		case OID_802_11_AUTHENTICATION_MODE:
			if (wrq->u.data.length != sizeof(NDIS_802_11_AUTHENTICATION_MODE)) 
				Status	= -EINVAL;
			else
			{
				Status = copy_from_user(&AuthMode, wrq->u.data.pointer, wrq->u.data.length);
				if (AuthMode > Ndis802_11AuthModeMax)
				{
					Status	= -EINVAL;
					break;
				}
				else
				{
					if (pAdapter->PortCfg.AuthMode != AuthMode)
					{
						// Config has changed
						pAdapter->bConfigChanged = TRUE;
					}
					pAdapter->PortCfg.AuthMode = AuthMode;
				}
				pAdapter->PortCfg.PortSecured = WPA_802_1X_PORT_NOT_SECURED;
				DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_AUTHENTICATION_MODE (=%d) \n",pAdapter->PortCfg.AuthMode);
			}
			break;
		case OID_802_11_INFRASTRUCTURE_MODE:
			if (wrq->u.data.length != sizeof(NDIS_802_11_NETWORK_INFRASTRUCTURE))
				Status	= -EINVAL;
			else
			{
				Status = copy_from_user(&BssType, wrq->u.data.pointer, wrq->u.data.length);
				if (BssType == Ndis802_11IBSS) 
				{
					if (pAdapter->PortCfg.BssType != BSS_ADHOC)
					{
						// Config has changed
						if (INFRA_ON(pAdapter))
						{
							RTUSBEnqueueInternalCmd(pAdapter, RT_OID_LINK_DOWN);
							// First cancel linkdown timer
							DBGPRINT(RT_DEBUG_TRACE, "NDIS_STATUS_MEDIA_DISCONNECT Event BB!\n");							
						}
						//pAdapter->bConfigChanged = TRUE;
					}
					pAdapter->PortCfg.BssType = BSS_ADHOC;
					DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_INFRASTRUCTURE_MODE (AD-HOC)\n");
				}
				else if (BssType == Ndis802_11Infrastructure) 
				{
					if (pAdapter->PortCfg.BssType != BSS_INFRA)
					{
						// Config has changed
						//pAdapter->bConfigChanged = TRUE;
						if (ADHOC_ON(pAdapter))
							RTUSBEnqueueInternalCmd(pAdapter, RT_OID_LINK_DOWN);
					}
					pAdapter->PortCfg.BssType = BSS_INFRA;
					DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_INFRASTRUCTURE_MODE (INFRA)\n");
				}
				else
				{
					Status	= -EINVAL;
					DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_INFRASTRUCTURE_MODE (unknown)\n");
				}
			}
			// Reset Ralink supplicant to not use, it will be set to start when UI set PMK key
			pAdapter->PortCfg.WpaState = SS_NOTUSE;
			break;
	    case OID_802_11_REMOVE_WEP:
            DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_REMOVE_WEP\n");
            if (wrq->u.data.length != sizeof(NDIS_802_11_KEY_INDEX)){
		        Status = -EINVAL;
            }
            else 
            {
		        KeyIdx = *(NDIS_802_11_KEY_INDEX *) wrq->u.data.pointer;

		        if (KeyIdx & 0x80000000)
		        {
			        // Should never set default bit when remove key
			        Status = -EINVAL;
		        }
		        else
		        {
			        KeyIdx = KeyIdx & 0x0fffffff;
			        if (KeyIdx >= 4){
				        Status = -EINVAL;
			        }
			        else
			        {
				        pAdapter->SharedKey[KeyIdx].KeyLen = 0;
				        pAdapter->SharedKey[KeyIdx].CipherAlg = CIPHER_NONE;
				        AsicRemoveSharedKeyEntry(pAdapter, 0, (UCHAR)KeyIdx);
			        }
		        }
            }
            break;
		case RT_OID_802_11_RESET_COUNTERS:
			NdisZeroMemory(&pAdapter->WlanCounters, sizeof(COUNTER_802_11));
			NdisZeroMemory(&pAdapter->Counters8023, sizeof(COUNTER_802_3));
			NdisZeroMemory(&pAdapter->RalinkCounters, sizeof(COUNTER_RALINK));
			pAdapter->Counters8023.RxNoBuffer	= 0;
			pAdapter->Counters8023.GoodReceives = 0;
			pAdapter->Counters8023.RxNoBuffer	= 0;
			DBGPRINT(RT_DEBUG_TRACE, "Set::RT_OID_802_11_RESET_COUNTERS (=%d)\n", pAdapter->Counters8023.GoodReceives);
			break;		  
		case OID_802_11_RTS_THRESHOLD:
			if (wrq->u.data.length != sizeof(NDIS_802_11_RTS_THRESHOLD))
				Status	= -EINVAL;
			else
			{
				Status = copy_from_user(&RtsThresh, wrq->u.data.pointer, wrq->u.data.length);
				if (RtsThresh > MAX_RTS_THRESHOLD)
					Status	= -EINVAL;
				else
					pAdapter->PortCfg.RtsThreshold = (USHORT)RtsThresh;
			}
			DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_RTS_THRESHOLD (=%d)\n",RtsThresh);
			break;
		case OID_802_11_FRAGMENTATION_THRESHOLD:
			if (wrq->u.data.length != sizeof(NDIS_802_11_FRAGMENTATION_THRESHOLD))
				Status	= -EINVAL;
			else
			{
				Status = copy_from_user(&FragThresh, wrq->u.data.pointer, wrq->u.data.length);
				pAdapter->PortCfg.bFragmentZeroDisable = FALSE;
				if (FragThresh > MAX_FRAG_THRESHOLD || FragThresh < MIN_FRAG_THRESHOLD)
				{
					if (FragThresh == 0)
					{
						pAdapter->PortCfg.FragmentThreshold = MAX_FRAG_THRESHOLD;
						pAdapter->PortCfg.bFragmentZeroDisable = TRUE;
					}
					else
						Status	= -EINVAL;
				}
				else
					pAdapter->PortCfg.FragmentThreshold = (USHORT)FragThresh;
			}
			DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_FRAGMENTATION_THRESHOLD (=%d) \n",FragThresh);
			break;
		case OID_802_11_POWER_MODE:
			if (wrq->u.data.length != sizeof(NDIS_802_11_POWER_MODE))
				Status = -EINVAL;
			else
			{
				Status = copy_from_user(&PowerMode, wrq->u.data.pointer, wrq->u.data.length);
				// save user's policy here, but not change PortCfg.Psm immediately
				if (PowerMode == Ndis802_11PowerModeCAM) 
				{
					// clear PSM bit immediately
					MlmeSetPsmBit(pAdapter, PWR_ACTIVE);
					
					OPSTATUS_SET_FLAG(pAdapter, fOP_STATUS_RECEIVE_DTIM); 
					if (pAdapter->PortCfg.bWindowsACCAMEnable == FALSE)
						pAdapter->PortCfg.WindowsPowerMode = PowerMode;
					pAdapter->PortCfg.WindowsBatteryPowerMode = PowerMode;
				}
				else if (PowerMode == Ndis802_11PowerModeMAX_PSP) 
				{
					// do NOT turn on PSM bit here, wait until MlmeCheckForPsmChange()
					// to exclude certain situations.
					//	   MlmeSetPsmBit(pAdapter, PWR_SAVE);
					if (pAdapter->PortCfg.bWindowsACCAMEnable == FALSE)
						pAdapter->PortCfg.WindowsPowerMode = PowerMode;
					pAdapter->PortCfg.WindowsBatteryPowerMode = PowerMode;
					OPSTATUS_SET_FLAG(pAdapter, fOP_STATUS_RECEIVE_DTIM);
					pAdapter->PortCfg.DefaultListenCount = 5;
				}
				else if (PowerMode == Ndis802_11PowerModeFast_PSP) 
				{
					// do NOT turn on PSM bit here, wait until MlmeCheckForPsmChange()
					// to exclude certain situations.
					//	   MlmeSetPsmBit(pAdapter, PWR_SAVE);
					OPSTATUS_SET_FLAG(pAdapter, fOP_STATUS_RECEIVE_DTIM);
					if (pAdapter->PortCfg.bWindowsACCAMEnable == FALSE)
						pAdapter->PortCfg.WindowsPowerMode = PowerMode;
					pAdapter->PortCfg.WindowsBatteryPowerMode = PowerMode;
					pAdapter->PortCfg.DefaultListenCount = 3;
				}
				else
					Status = -EINVAL;
			}
			DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_POWER_MODE (=%d)\n",PowerMode);
			break;
		case OID_802_11_TX_POWER_LEVEL:
			if (wrq->u.data.length != sizeof(NDIS_802_11_TX_POWER_LEVEL))
				Status	= -EINVAL;
			else
			{
				Status = copy_from_user(&TxPowerLevel, wrq->u.data.pointer, wrq->u.data.length);
				if (TxPowerLevel > MAX_TX_POWER_LEVEL)
					Status	= -EINVAL;
				else
					pAdapter->PortCfg.TxPower = (UCHAR)TxPowerLevel;
			}
			DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_TX_POWER_LEVEL (=%d) \n",TxPowerLevel);
			break;
		case RT_OID_802_11_TX_POWER_LEVEL_1:
			if (wrq->u.data.length	< sizeof(ULONG))
				Status = -EINVAL;
			else
			{
				ULONG	PowerTemp;
				
				Status = copy_from_user(&PowerTemp, wrq->u.data.pointer, wrq->u.data.length);
				if (PowerTemp > 100)
					PowerTemp = 0xffffffff;  // AUTO
				pAdapter->PortCfg.TxPowerDefault = PowerTemp; //keep current setting.

				// Only update TxPowerPercentage if the value is smaller than current AP setting
// TODO: 2005-03-08 john removed the following line. 
//				if (pAdapter->PortCfg.TxPowerDefault < pAdapter->PortCfg.TxPowerPercentage)
					pAdapter->PortCfg.TxPowerPercentage = pAdapter->PortCfg.TxPowerDefault;			
				DBGPRINT(RT_DEBUG_TRACE, "Set::RT_OID_802_11_TX_POWER_LEVEL_1 (=%d)\n", pAdapter->PortCfg.TxPowerPercentage);
			}	
			break;
		case OID_802_11_PRIVACY_FILTER:
			if (wrq->u.data.length != sizeof(NDIS_802_11_PRIVACY_FILTER))
				Status = -EINVAL;
			else 
			{	
				NDIS_802_11_PRIVACY_FILTER	Filter;
				
				Status = copy_from_user(&Filter, wrq->u.data.pointer, wrq->u.data.length);
				if ((Filter == Ndis802_11PrivFilterAcceptAll) || (Filter == Ndis802_11PrivFilter8021xWEP))
					pAdapter->PortCfg.PrivacyFilter = Filter;
				else
					Status = -EINVAL;
			}
			DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_PRIVACY_FILTER (=%d) \n",pAdapter->PortCfg.PrivacyFilter);
			break;
		case OID_802_11_NETWORK_TYPE_IN_USE:  
			if (wrq->u.data.length != sizeof(NDIS_802_11_NETWORK_TYPE))
				Status = -EINVAL;
			else 
			{
				Status = copy_from_user(&NetType, wrq->u.data.pointer, wrq->u.data.length);
				RTUSBEnqueueCmdFromNdis(pAdapter, OID_802_11_NETWORK_TYPE_IN_USE, TRUE, &NetType, wrq->u.data.length);
				DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_NETWORK_TYPE_IN_USE (=%d)\n",NetType);
			}		
			break;
	
		case OID_802_11_RX_ANTENNA_SELECTED:   
			if (wrq->u.data.length != sizeof(NDIS_802_11_ANTENNA))
				Status = -EINVAL;
			else 
			{  
				Status = copy_from_user(&AntDiv, wrq->u.data.pointer, wrq->u.data.length);
				RTUSBEnqueueCmdFromNdis(pAdapter, OID_802_11_RX_ANTENNA_SELECTED, FALSE, &AntDiv, wrq->u.data.length);
			}
			break;
		case OID_802_11_TX_ANTENNA_SELECTED:	   
			if (wrq->u.data.length != sizeof(NDIS_802_11_ANTENNA))
				Status = -EINVAL;
			else 
			{
				Status = copy_from_user(&AntDiv, wrq->u.data.pointer, wrq->u.data.length);
				RTUSBEnqueueCmdFromNdis(pAdapter, OID_802_11_TX_ANTENNA_SELECTED, FALSE, &AntDiv, wrq->u.data.length);
			}
			break;
		// For WPA PSK PMK key
		case RT_OID_802_11_ADD_WPA:
			DBGPRINT(RT_DEBUG_ERROR, "!!!!!!!!!!!!!!!!!!!!!Set::RT_OID_802_11_ADD_WPA !!\n");
			pKey = kmalloc(wrq->u.data.length, MEM_ALLOC_FLAG);
			if(pKey == NULL)
			{
				Status = -ENOMEM;
				break;
			}
			
			Status = copy_from_user(pKey, wrq->u.data.pointer, wrq->u.data.length);
			if (pKey->Length != wrq->u.data.length)
			{
				Status	= -EINVAL;
				DBGPRINT(RT_DEBUG_TRACE, "Set::RT_OID_802_11_ADD_WPA, Failed!!\n");
			}
			else
			{
				if ((pAdapter->PortCfg.AuthMode != Ndis802_11AuthModeWPAPSK) &&\
					(pAdapter->PortCfg.AuthMode != Ndis802_11AuthModeWPA2PSK) &&\
					(pAdapter->PortCfg.AuthMode != Ndis802_11AuthModeWPANone) )
				{	 
					Status = -EOPNOTSUPP;
					DBGPRINT(RT_DEBUG_TRACE, "Set::RT_OID_802_11_ADD_WPA, Failed!! [AuthMode != WPAPSK/WPA2PSK/WPANONE]\n");
				}
				else if ((pAdapter->PortCfg.AuthMode == Ndis802_11AuthModeWPAPSK) ||\
						(pAdapter->PortCfg.AuthMode == Ndis802_11AuthModeWPA2PSK) ) 	// Only for WPA PSK mode
				{
					INT i;

					pAdapter->PortCfg.PskKey.KeyLen = (UCHAR) pKey->KeyLength;
					NdisMoveMemory(pAdapter->PortCfg.PskKey.Key, &pKey->KeyMaterial, pKey->KeyLength);
					// Use RaConfig as PSK agent.
					// Start STA supplicant state machine
					pAdapter->PortCfg.WpaState = SS_START;

					DBGPRINT(RT_DEBUG_TRACE,"PskKey =  Len = %d \n ",pKey->KeyLength);
					for (i = 0; i < 32; i++)
					{
						DBGPRINT_RAW(RT_DEBUG_TRACE,"%02x:", pAdapter->PortCfg.PskKey.Key[i]);
					}
					DBGPRINT(RT_DEBUG_TRACE,"\n");						
					// Use RaConfig as PSK agent.
					// Start STA supplicant state machine
					pAdapter->PortCfg.WpaState = SS_START;
					DBGPRINT(RT_DEBUG_TRACE, "Set::RT_OID_802_11_ADD_WPA (id=0x%x, Len=%d-byte)\n", pKey->KeyIndex, pKey->KeyLength);
				}
				else
				{
					Status = RTUSBEnqueueCmdFromNdis(pAdapter, OID_802_11_ADD_KEY, TRUE, pKey, pKey->Length);
				}
			}
			kfree(pKey);
			break;	
		case OID_802_11_REMOVE_KEY:
			pRemoveKey = kmalloc(wrq->u.data.length, MEM_ALLOC_FLAG);
			if(pRemoveKey == NULL)
			{
				Status = -ENOMEM;
				break;
			}
			
			Status = copy_from_user(pRemoveKey, wrq->u.data.pointer, wrq->u.data.length);
			if (pRemoveKey->Length != wrq->u.data.length)
			{
				Status = -EINVAL;
				DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_REMOVE_KEY, Failed!!\n");
			}
			else
			{
				Status = RTMPRemoveKeySanity(pAdapter, pRemoveKey);
				if (Status == NDIS_STATUS_SUCCESS)
				{
					RTUSBEnqueueCmdFromNdis(pAdapter, OID_802_11_REMOVE_KEY, TRUE, pRemoveKey, wrq->u.data.length);
				}
			}	
			DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_REMOVE_KEY (id=0x%x, Len=%d-byte)\n", pRemoveKey->KeyIndex, pRemoveKey->Length);	 
			kfree(pRemoveKey);
			break;		  
		// New for WPA
		case OID_802_11_ADD_KEY: 
			DBGPRINT(RT_DEBUG_ERROR, "!!!!!!!!!!!!!!!!!!!!!Set::OID_802_11_ADD_KEY !!\n");
			pKey = kmalloc(wrq->u.data.length, MEM_ALLOC_FLAG);
			if(pKey == NULL)
			{
				Status = -ENOMEM;
				break;
			}
			
			Status = copy_from_user(pKey, wrq->u.data.pointer, wrq->u.data.length);
			if (pKey->Length != wrq->u.data.length)
			{
				Status	= -EINVAL;
				DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_ADD_KEY, Failed!!\n");
			}
			else
			{		
				if (pAdapter->PortCfg.AuthMode >= Ndis802_11AuthModeWPA)
				{
					// Probably PortCfg.Bssid reset to zero as linkdown, 
				    // Set pKey.BSSID to Broadcast bssid in order to ensure AsicAddSharedKeyEntry done 
				    if(pAdapter->PortCfg.AuthMode == Ndis802_11AuthModeWPANone)
				    {
				        NdisMoveMemory(pKey->BSSID, BROADCAST_ADDR, 6);
				    }
				
					RTUSBEnqueueCmdFromNdis(pAdapter, OID_802_11_ADD_KEY, TRUE, pKey, pKey->Length);
				}
				else	// Old WEP stuff
				{
					Status = RTMPWPAWepKeySanity(pAdapter, pKey);
					if (Status == NDIS_STATUS_SUCCESS)
						RTUSBEnqueueCmdFromNdis(pAdapter, OID_802_11_ADD_KEY_WEP, TRUE, pKey, wrq->u.data.length);		
				}
				
				KeyIdx = pKey->KeyIndex & 0x0fffffff;
				DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_ADD_KEY (id=0x%x, Len=%d-byte)\n", KeyIdx, pKey->KeyLength);		  
			}
			kfree(pKey);
			break;
#if WPA_SUPPLICANT_SUPPORT 
		case OID_802_11_SET_IEEE8021X:
			if (wrq->u.data.length != sizeof(BOOLEAN))
                Status  = -EINVAL;
            else
            {
                Status = copy_from_user(&IEEE8021xState, wrq->u.data.pointer, wrq->u.data.length);                				
				pAdapter->PortCfg.IEEE8021X = IEEE8021xState;									


		        // set WEP key to ASIC for static wep mode
                if(pAdapter->PortCfg.IEEE8021X == FALSE && pAdapter->PortCfg.AuthMode < Ndis802_11AuthModeWPA )
                {
                     int idx;
      
                     idx = pAdapter->PortCfg.DefaultKeyId;
                     //for (idx=0; idx < 4; idx++)
                     {
                          DBGPRINT_RAW(RT_DEBUG_TRACE, "Set WEP key to Asic for static wep mode =>\n");
          
                          if(pAdapter->PortCfg.DesireSharedKey[idx].KeyLen != 0)
                          {
                                 pAdapter->SharedKey[idx].KeyLen = pAdapter->PortCfg.DesireSharedKey[idx].KeyLen;
                                 NdisMoveMemory(pAdapter->SharedKey[idx].Key, pAdapter->PortCfg.DesireSharedKey[idx].Key, pAdapter->SharedKey[idx].KeyLen);
           
                                 pAdapter->SharedKey[idx].CipherAlg = pAdapter->PortCfg.DesireSharedKey[idx].CipherAlg;
       
                                 AsicAddSharedKeyEntry(pAdapter, 0, (UCHAR)idx, pAdapter->SharedKey[idx].CipherAlg, pAdapter->SharedKey[idx].Key, NULL, NULL); 
                          }
                    }              
                }
    

				DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_SET_IEEE8021X (=%d)\n", pAdapter->PortCfg.IEEE8021X);									
			}	
			break;	
		case OID_802_11_SET_IEEE8021X_REQUIRE_KEY:	
			if (wrq->u.data.length != sizeof(BOOLEAN))
				 Status  = -EINVAL;
            else
            {
                Status = copy_from_user(&IEEE8021x_required_keys, wrq->u.data.pointer, wrq->u.data.length);                				
				pAdapter->PortCfg.IEEE8021x_required_keys = IEEE8021x_required_keys;				
				DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_SET_IEEE8021X_REQUIRE_KEY (%d)\n", pAdapter->PortCfg.IEEE8021x_required_keys);									
			}	
			break;
	    // For WPA_SUPPLICANT to set dynamic wep key	    
	    case OID_802_11_ADD_WEP:
			DBGPRINT(RT_DEBUG_ERROR, "!!!!!!!!!!!!!!!!!!!!!Set::OID_802_11_ADD_WEP !!\n");
	        pWepKey = kmalloc(wrq->u.data.length, MEM_ALLOC_FLAG);

	        if(pWepKey == NULL)
            {
                Status = -ENOMEM;
				DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_ADD_WEP, Failed!!\n");
                break;
            }
            Status = copy_from_user(pWepKey, wrq->u.data.pointer, wrq->u.data.length);
            if (pWepKey->Length != wrq->u.data.length)
            {
                Status  = -EINVAL;
                DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_ADD_WEP, Failed (length mismatch)!!\n");
            }
            else
            {
	
		        KeyIdx = pWepKey->KeyIndex & 0x0fffffff;
		    	
                // KeyIdx must be smaller than 4
                if (KeyIdx > 4)
		        {
                    Status = -EINVAL;
                    DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_ADD_WEP, Failed (KeyIdx must be smaller than 4)!!\n");						
                }
                else
                {
                    // After receiving eap-success in 802.1x mode, PortSecured will be TRUE.
			        // At this moment, wpa_supplicant will set dynamic wep key to driver.
			        // Otherwise, driver only records it, not set to Asic. 
 			        if(pAdapter->PortCfg.PortSecured == WPA_802_1X_PORT_SECURED)
			        {
                        UCHAR CipherAlg;
                        
                        pAdapter->SharedKey[KeyIdx].KeyLen = (UCHAR) pWepKey->KeyLength;
                        NdisMoveMemory(pAdapter->SharedKey[KeyIdx].Key, &pWepKey->KeyMaterial, pWepKey->KeyLength);

                        if (pWepKey->KeyLength == 5)
                            CipherAlg = CIPHER_WEP64;
                        else
                            CipherAlg = CIPHER_WEP128;
                        
                        pAdapter->SharedKey[KeyIdx].CipherAlg = CipherAlg;
				
                        if (pWepKey->KeyIndex & 0x80000000)
                        {
                            // Default key for tx (shared key)
                            pAdapter->PortCfg.DefaultKeyId = (UCHAR) KeyIdx;
                        }
                        
			            AsicAddSharedKeyEntry(pAdapter, 0, (UCHAR)KeyIdx, CipherAlg, pAdapter->SharedKey[KeyIdx].Key, NULL, NULL);
                    	
			            DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_ADD_WEP (id=0x%x, Len=%d-byte), Port Secured\n", pWepKey->KeyIndex, pWepKey->KeyLength);

	                }
			        else // PortSecured is NOT SECURED
			        {
			            UCHAR CipherAlg;
							
				        pAdapter->PortCfg.DesireSharedKey[KeyIdx].KeyLen = (UCHAR) pWepKey->KeyLength;
                        NdisMoveMemory(pAdapter->PortCfg.DesireSharedKey[KeyIdx].Key, &pWepKey->KeyMaterial, pWepKey->KeyLength);
		    
				        if (pWepKey->KeyLength == 5)
                            CipherAlg = CIPHER_WEP64;
                        else
                            CipherAlg = CIPHER_WEP128;
                        
                            pAdapter->PortCfg.DesireSharedKey[KeyIdx].CipherAlg = CipherAlg;
				
                        if (pWepKey->KeyIndex & 0x80000000)
                        {
                            // Default key for tx (shared key)
                            pAdapter->PortCfg.DefaultKeyId = (UCHAR) KeyIdx;
                        }
	
				        DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_ADD_WEP (id=0x%x, Len=%d-byte), Port Not Secured\n", pWepKey->KeyIndex, pWepKey->KeyLength);																
			        }
		        }		    		 				

	        }  		
	        kfree(pWepKey);
	        break;
#endif	    	    
		case OID_802_11_CONFIGURATION:
			if (wrq->u.data.length != sizeof(NDIS_802_11_CONFIGURATION))
				Status	= -EINVAL;
			else
			{
				Status = copy_from_user(&Config, wrq->u.data.pointer, wrq->u.data.length);
				pConfig = &Config;

				if ((pConfig->BeaconPeriod >= 20) && (pConfig->BeaconPeriod <=400))
					pAdapter->PortCfg.BeaconPeriod = (USHORT) pConfig->BeaconPeriod;
				
				pAdapter->PortCfg.AtimWin = (USHORT) pConfig->ATIMWindow;
				MAP_KHZ_TO_CHANNEL_ID(pConfig->DSConfig, pAdapter->PortCfg.Channel);
				//
				// Save the channel on MlmeAux for CntlOidRTBssidProc used.
				//
				pAdapter->MlmeAux.Channel = pAdapter->PortCfg.Channel;
				
				DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_CONFIGURATION (BeacnPeriod=%d,AtimW=%d,Ch=%d)\n",
					pConfig->BeaconPeriod, pConfig->ATIMWindow, pAdapter->PortCfg.Channel);
				// Config has changed
				pAdapter->bConfigChanged = TRUE;
			}
			break;
        case OID_802_11_DISASSOCIATE:
#ifdef RALINK_ATE			
			if(pAdapter->ate.Mode != ATE_STASTART)
			{
				DBGPRINT(RT_DEBUG_TRACE, "The driver is in ATE mode now\n");
				Status = NDIS_STATUS_SUCCESS;
				break;
			}	
#endif		
        
		    // Set to immediately send the media disconnect event
		    pAdapter->MlmeAux.CurrReqIsFromNdis = TRUE; 			
		
		    DBGPRINT(RT_DEBUG_TRACE, "Set::OID_802_11_DISASSOCIATE \n");
		
		    if (INFRA_ON(pAdapter))
		    {
		        if (pAdapter->Mlme.CntlMachine.CurrState != CNTL_IDLE)
			    {
				    MlmeEnqueue(pAdapter, 
				                MLME_CNTL_STATE_MACHINE,
				                RT_CMD_RESET_MLME,
				                0,
				                NULL);	
			    }
		
			    MlmeEnqueue(pAdapter, 
						    MLME_CNTL_STATE_MACHINE, 
						    OID_802_11_DISASSOCIATE, 
						    0, 
						    NULL);
		
		        StateMachineTouched = TRUE;
		    }
		    break;
#if WPA_SUPPLICANT_SUPPORT	    
		case OID_SET_COUNTERMEASURES: 
			if (wrq->u.param.value)
		    {
		        pAdapter->PortCfg.bBlockAssoc = TRUE;
		        DBGPRINT(RT_DEBUG_TRACE, "Set::OID_SET_COUNTERMEASURES bBlockAssoc=TRUE \n");
			}
			else
			{
		        // WPA MIC error should block association attempt for 60 seconds
		        pAdapter->PortCfg.bBlockAssoc = FALSE;
		        DBGPRINT(RT_DEBUG_TRACE, "Set::OID_SET_COUNTERMEASURES bBlockAssoc=FALSE \n");
		    }    	
		    break;
		case OID_802_11_PMKID:
		    pPmkId = kmalloc(wrq->u.data.length, MEM_ALLOC_FLAG);
		
		    if(pPmkId == NULL) {
		        Status = -ENOMEM;
		        break;
		    }
		    Status = copy_from_user(pPmkId, wrq->u.data.pointer, wrq->u.data.length);
		  
		    DBGPRINT(RT_DEBUG_OFF, "Set::OID_802_11_PMKID \n");
		  
		    // check the PMKID information
		    if (pPmkId->BSSIDInfoCount == 0)
			    break;
		    else
		    {
			    PBSSID_INFO	pBssIdInfo;
			    ULONG		BssIdx;
			    ULONG		CachedIdx;
		
			    for (BssIdx = 0; BssIdx < pPmkId->BSSIDInfoCount; BssIdx++)
			    {
				    // point to the indexed BSSID_INFO structure
				    pBssIdInfo = (PBSSID_INFO) ((PUCHAR) pPmkId + 2 * sizeof(ULONG) + BssIdx * sizeof(BSSID_INFO));
				    // Find the entry in the saved data base.
				    for (CachedIdx = 0; CachedIdx < pAdapter->PortCfg.SavedPMKNum; CachedIdx++)
				    {
					    // compare the BSSID
					    if (NdisEqualMemory(pBssIdInfo->BSSID, pAdapter->PortCfg.SavedPMK[CachedIdx].BSSID, sizeof(NDIS_802_11_MAC_ADDRESS)))
						    break;			
				    }
		
				    // Found, replace it
				    if (CachedIdx < PMKID_NO)
				    {
					    DBGPRINT(RT_DEBUG_OFF, "Update OID_802_11_PMKID, idx = %d\n", CachedIdx);
					    NdisMoveMemory(&pAdapter->PortCfg.SavedPMK[CachedIdx], pBssIdInfo, sizeof(BSSID_INFO));
					    pAdapter->PortCfg.SavedPMKNum++;
				    }
				    // Not found, replace the last one
				    else
				    {
					    // Randomly replace one
					    CachedIdx = (pBssIdInfo->BSSID[5] % PMKID_NO);
					    DBGPRINT(RT_DEBUG_OFF, "Update OID_802_11_PMKID, idx = %d\n", CachedIdx);
					    NdisMoveMemory(&pAdapter->PortCfg.SavedPMK[CachedIdx], pBssIdInfo, sizeof(BSSID_INFO));
				    }				
			    }
		   }
		   break;
		case RT_OID_WPA_SUPPLICANT_SUPPORT:
		    if (wrq->u.data.length != sizeof(BOOLEAN))
		        Status  = -EINVAL;
		    else
		    {
		        Status = copy_from_user(&wpa_supplicant_enable, wrq->u.data.pointer, wrq->u.data.length);                				
		        pAdapter->PortCfg.WPA_Supplicant = wpa_supplicant_enable;									
			    DBGPRINT(RT_DEBUG_TRACE, "Set::RT_OID_WPA_SUPPLICANT_SUPPORT (=%d)\n", pAdapter->PortCfg.WPA_Supplicant);									
		    }
		    break;
#endif
		default:
			DBGPRINT(RT_DEBUG_TRACE, "Set::unknown IOCTL's subcmd = 0x%08x\n", cmd);
			Status = -EOPNOTSUPP;
            break;
	}
	return Status;

}

INT RTMPQueryInformation(
	IN	PRTMP_ADAPTER pAdapter,
	IN	OUT struct ifreq	*rq,
	IN	INT 				cmd)
{
	struct iwreq						*wrq = (struct iwreq *) rq;
	NDIS_802_11_BSSID_LIST_EX			*pBssidList = NULL;
	PNDIS_WLAN_BSSID_EX 				pBss;
	NDIS_802_11_SSID					Ssid;
	NDIS_802_11_CONFIGURATION			Configuration;
	RT_802_11_LINK_STATUS				LinkStatus;
	RT_802_11_STA_CONFIG				StaConfig;
	NDIS_802_11_STATISTICS				Statistics;
	NDIS_802_11_RTS_THRESHOLD			RtsThresh;
	NDIS_802_11_FRAGMENTATION_THRESHOLD FragThresh;
	NDIS_802_11_POWER_MODE				PowerMode;
	NDIS_802_11_NETWORK_INFRASTRUCTURE	BssType;
	RT_802_11_PREAMBLE					PreamType;
	NDIS_802_11_AUTHENTICATION_MODE 	AuthMode;
	NDIS_802_11_WEP_STATUS				WepStatus;
	RT_VERSION_INFO 					DriverVersionInfo; 
	NDIS_MEDIA_STATE					MediaState;
	ULONG								BssBufSize;
	ULONG								BssLen;
	ULONG								ulInfo = 0;
	PUCHAR								pBuf = NULL;
	PUCHAR								pPtr;
	INT 								Status = NDIS_STATUS_SUCCESS;
	UCHAR								Padding;
	UINT								i;
	BOOLEAN 							RadioState;
	ULONG								NetworkTypeList[4];
    	UINT                                we_version_compiled;

	switch(cmd) {
		case RT_OID_DEVICE_NAME:
			DBGPRINT(RT_DEBUG_INFO, "Query::RT_OID_DEVICE_NAME\n");
			wrq->u.data.length = sizeof(NIC_DEVICE_NAME);
			Status = copy_to_user(wrq->u.data.pointer, NIC_DEVICE_NAME, wrq->u.data.length);
			break;		  
		case RT_OID_VERSION_INFO:
			DBGPRINT(RT_DEBUG_INFO, "Query::RT_OID_VERSION_INFO \n");		
			DriverVersionInfo.DriverVersionW = DRV_MAJORVERSION;
			DriverVersionInfo.DriverVersionX = DRV_MINORVERSION;
			DriverVersionInfo.DriverVersionY = DRV_SUBVERSION;
			DriverVersionInfo.DriverVersionZ = DRV_TESTVERSION;
			DriverVersionInfo.DriverBuildYear	= DRV_YEAR;
			DriverVersionInfo.DriverBuildMonth	= DRV_MONTH;
			DriverVersionInfo.DriverBuildDay	= DRV_DAY;
			wrq->u.data.length = sizeof(RT_VERSION_INFO);
			Status = copy_to_user(wrq->u.data.pointer, &DriverVersionInfo, wrq->u.data.length);
			break;
		case OID_802_11_BSSID_LIST:
			DBGPRINT(RT_DEBUG_TRACE, "Query::OID_802_11_BSSID_LIST (%d BSS returned)\n",pAdapter->ScanTab.BssNr);
			// Claculate total buffer size required
			BssBufSize = sizeof(ULONG);
			
			for (i = 0; i < pAdapter->ScanTab.BssNr; i++) 
			{
				// Align pointer to 4 bytes boundary.
				Padding = 4 - (pAdapter->ScanTab.BssEntry[i].VarIELen & 0x0003);
				if (Padding == 4)
					Padding = 0;
				// 'packed' alignment to the previous one 	
				BssBufSize += (sizeof(NDIS_WLAN_BSSID_EX) - 1 + sizeof(NDIS_802_11_FIXED_IEs) + pAdapter->ScanTab.BssEntry[i].VarIELen + Padding);
			}

			// For safety issue, we add 256 bytes just in case
			BssBufSize += 256;
			// Allocate the same size as passed from higher layer
			pBuf = kmalloc(BssBufSize, MEM_ALLOC_FLAG);
			if(pBuf == NULL)
			{
				Status = -ENOMEM;
				break;
			}
			// Init 802_11_BSSID_LIST_EX structure
			NdisZeroMemory(pBuf, BssBufSize);
			pBssidList = (PNDIS_802_11_BSSID_LIST_EX) pBuf;
			pBssidList->NumberOfItems = pAdapter->ScanTab.BssNr;
			
			// Calculate total buffer length
			BssLen = 4; // Consist of NumberOfItems
			// Point to start of NDIS_WLAN_BSSID_EX
			// pPtr = pBuf + sizeof(ULONG);
			pPtr = (PUCHAR) &pBssidList->Bssid[0];
			for (i = 0; i < pAdapter->ScanTab.BssNr; i++) 
			{
				pBss = (PNDIS_WLAN_BSSID_EX) pPtr;
				NdisMoveMemory(&pBss->MacAddress, &pAdapter->ScanTab.BssEntry[i].Bssid, MAC_ADDR_LEN);
				if ((pAdapter->ScanTab.BssEntry[i].Hidden == 1) && (pAdapter->PortCfg.bShowHiddenSSID == FALSE))
				{
					pBss->Ssid.SsidLength = 0;
				}
				else
				{
					pBss->Ssid.SsidLength = pAdapter->ScanTab.BssEntry[i].SsidLen;
					NdisMoveMemory(pBss->Ssid.Ssid, pAdapter->ScanTab.BssEntry[i].Ssid, pAdapter->ScanTab.BssEntry[i].SsidLen);
				}
				pBss->Privacy = pAdapter->ScanTab.BssEntry[i].Privacy;
				pBss->Rssi = pAdapter->ScanTab.BssEntry[i].Rssi - pAdapter->BbpRssiToDbmDelta;

				pBss->NetworkTypeInUse = NetworkTypeInUseSanity(pAdapter->ScanTab.BssEntry[i].Channel, 
																pAdapter->ScanTab.BssEntry[i].SupRate,
																pAdapter->ScanTab.BssEntry[i].SupRateLen,
																pAdapter->ScanTab.BssEntry[i].ExtRate,
																pAdapter->ScanTab.BssEntry[i].ExtRateLen);
																
				pBss->Configuration.Length = sizeof(NDIS_802_11_CONFIGURATION);
				pBss->Configuration.BeaconPeriod = pAdapter->ScanTab.BssEntry[i].BeaconPeriod;	
				pBss->Configuration.ATIMWindow = pAdapter->ScanTab.BssEntry[i].AtimWin;

				MAP_CHANNEL_ID_TO_KHZ(pAdapter->ScanTab.BssEntry[i].Channel, pBss->Configuration.DSConfig);

				if (pAdapter->ScanTab.BssEntry[i].BssType == BSS_INFRA) 
					pBss->InfrastructureMode = Ndis802_11Infrastructure;
				else
					pBss->InfrastructureMode = Ndis802_11IBSS;

				NdisMoveMemory(pBss->SupportedRates, pAdapter->ScanTab.BssEntry[i].SupRate, pAdapter->ScanTab.BssEntry[i].SupRateLen);
// TODO: 2004-09-13 john -	should we copy ExtRate into this array? if not, some APs annouced all 8 11g rates
// in ExtRateIE which may be mis-treated as 802.11b AP by ZeroConfig
				NdisMoveMemory(pBss->SupportedRates + pAdapter->ScanTab.BssEntry[i].SupRateLen,
							   pAdapter->ScanTab.BssEntry[i].ExtRate,
							   pAdapter->ScanTab.BssEntry[i].ExtRateLen);

				DBGPRINT(RT_DEBUG_TRACE,"BSS#%d - %s, Ch %d = %d Khz, Sup+Ext rate# = %d\n",
					i,pBss->Ssid.Ssid,
					pAdapter->ScanTab.BssEntry[i].Channel,
					pBss->Configuration.DSConfig,
					pAdapter->ScanTab.BssEntry[i].SupRateLen + pAdapter->ScanTab.BssEntry[i].ExtRateLen);

					
				if (pAdapter->ScanTab.BssEntry[i].VarIELen == 0)
				{
					pBss->IELength = sizeof(NDIS_802_11_FIXED_IEs);
					NdisMoveMemory(pBss->IEs, &pAdapter->ScanTab.BssEntry[i].FixIEs, sizeof(NDIS_802_11_FIXED_IEs));
					// 'packed' alignment to the previous one
					pPtr = pPtr + sizeof(NDIS_WLAN_BSSID_EX) - 1 + sizeof(NDIS_802_11_FIXED_IEs);
				}
				else
				{
					pBss->IELength = sizeof(NDIS_802_11_FIXED_IEs) + pAdapter->ScanTab.BssEntry[i].VarIELen;
					// 'packed' alignment to the previous one
					pPtr = pPtr + sizeof(NDIS_WLAN_BSSID_EX) - 1 + sizeof(NDIS_802_11_FIXED_IEs);
					NdisMoveMemory(pBss->IEs, &pAdapter->ScanTab.BssEntry[i].FixIEs, sizeof(NDIS_802_11_FIXED_IEs));
					NdisMoveMemory(pPtr, pAdapter->ScanTab.BssEntry[i].VarIEs, pAdapter->ScanTab.BssEntry[i].VarIELen);
					pPtr += pAdapter->ScanTab.BssEntry[i].VarIELen;
				}
				// Align pointer to 4 bytes boundary.
				Padding = 4 - (pAdapter->ScanTab.BssEntry[i].VarIELen & 0x0003);
				if (Padding == 4)
					Padding = 0;
				pPtr += Padding;
				// 'packed' alignment to the previous one
				pBss->Length = sizeof(NDIS_WLAN_BSSID_EX) - 1 + sizeof(NDIS_802_11_FIXED_IEs) + pAdapter->ScanTab.BssEntry[i].VarIELen + Padding;
				BssLen += pBss->Length;
			}
			wrq->u.data.length = BssLen;
			Status = copy_to_user(wrq->u.data.pointer, pBssidList, wrq->u.data.length);
			kfree(pBssidList);
			break;
		case OID_802_3_CURRENT_ADDRESS:
			wrq->u.data.length = MAC_ADDR_LEN;
			Status = copy_to_user(wrq->u.data.pointer, &pAdapter->CurrentAddress, wrq->u.data.length);
			DBGPRINT(RT_DEBUG_INFO, "Query::OID_802_3_CURRENT_ADDRESS \n");
			break;
		case OID_GEN_MEDIA_CONNECT_STATUS:
			DBGPRINT(RT_DEBUG_INFO, "Query::OID_GEN_MEDIA_CONNECT_STATUS \n");
			if (OPSTATUS_TEST_FLAG(pAdapter, fOP_STATUS_MEDIA_STATE_CONNECTED))
				MediaState = NdisMediaStateConnected;
			else
				MediaState = NdisMediaStateDisconnected;
				
			wrq->u.data.length = sizeof(NDIS_MEDIA_STATE);
			Status = copy_to_user(wrq->u.data.pointer, &MediaState, wrq->u.data.length);
			break;	 
		case OID_802_11_BSSID:
			if (INFRA_ON(pAdapter) || ADHOC_ON(pAdapter))
			{
				wrq->u.data.length = sizeof(NDIS_802_11_MAC_ADDRESS);
				Status = copy_to_user(wrq->u.data.pointer, &pAdapter->PortCfg.Bssid, wrq->u.data.length);

				DBGPRINT(RT_DEBUG_INFO, "IOCTL::SIOCGIWAP(=%02x:%02x:%02x:%02x:%02x:%02x)\n",
						pAdapter->PortCfg.Bssid[0],pAdapter->PortCfg.Bssid[1],pAdapter->PortCfg.Bssid[2],
						pAdapter->PortCfg.Bssid[3],pAdapter->PortCfg.Bssid[4],pAdapter->PortCfg.Bssid[5]);

			}
			else
			{
				DBGPRINT(RT_DEBUG_TRACE, "Query::OID_802_11_BSSID(=EMPTY)\n");
				Status = -ENOTCONN;
			}
			break;
		case OID_802_11_SSID:
			Ssid.SsidLength = pAdapter->PortCfg.SsidLen;
			NdisZeroMemory(Ssid.Ssid, MAX_LEN_OF_SSID);
			NdisMoveMemory(Ssid.Ssid, pAdapter->PortCfg.Ssid, Ssid.SsidLength);
			wrq->u.data.length = sizeof(NDIS_802_11_SSID);
			Status = copy_to_user(wrq->u.data.pointer, &Ssid, wrq->u.data.length);
			DBGPRINT(RT_DEBUG_TRACE, "Query::OID_802_11_SSID (Len=%d, ssid=%s)\n", Ssid.SsidLength,Ssid.Ssid);
			break;
		case RT_OID_802_11_QUERY_LINK_STATUS:
			LinkStatus.CurrTxRate = RateIdTo500Kbps[pAdapter->PortCfg.TxRate];	 // unit : 500 kbps
			LinkStatus.ChannelQuality = pAdapter->Mlme.ChannelQuality;
			LinkStatus.RxByteCount = pAdapter->RalinkCounters.ReceivedByteCount;
			LinkStatus.TxByteCount = pAdapter->RalinkCounters.TransmittedByteCount;
			wrq->u.data.length = sizeof(RT_802_11_LINK_STATUS);
			Status = copy_to_user(wrq->u.data.pointer, &LinkStatus, wrq->u.data.length);
			DBGPRINT(RT_DEBUG_TRACE, "Query::RT_OID_802_11_QUERY_LINK_STATUS\n");
			break;
		case OID_802_11_CONFIGURATION:
			Configuration.Length = sizeof(NDIS_802_11_CONFIGURATION);
			Configuration.BeaconPeriod = pAdapter->PortCfg.BeaconPeriod;
			Configuration.ATIMWindow = pAdapter->PortCfg.AtimWin;
			MAP_CHANNEL_ID_TO_KHZ(pAdapter->PortCfg.Channel, Configuration.DSConfig);
			wrq->u.data.length = sizeof(NDIS_802_11_CONFIGURATION);
			Status = copy_to_user(wrq->u.data.pointer, &Configuration, wrq->u.data.length);
			DBGPRINT(RT_DEBUG_TRACE, "Query::OID_802_11_CONFIGURATION(BeaconPeriod=%d,AtimW=%d,Channel=%d) \n", 
									Configuration.BeaconPeriod, Configuration.ATIMWindow, pAdapter->PortCfg.Channel);
			break;
		case OID_802_11_RSSI_TRIGGER:
			ulInfo = pAdapter->PortCfg.LastRssi - pAdapter->BbpRssiToDbmDelta;
			wrq->u.data.length = sizeof(ulInfo);
			Status = copy_to_user(wrq->u.data.pointer, &ulInfo, wrq->u.data.length);
			DBGPRINT(RT_DEBUG_TRACE, "Query::OID_802_11_RSSI_TRIGGER(=%d)\n", ulInfo);
			break;
		case OID_802_11_STATISTICS:
			DBGPRINT(RT_DEBUG_TRACE, "Query::OID_802_11_STATISTICS \n");

			// add the most up-to-date h/w raw counters into software counters
			NICUpdateRawCounters(pAdapter);
			
			// Sanity check for calculation of sucessful count
			if (pAdapter->WlanCounters.TransmittedFragmentCount.QuadPart < pAdapter->WlanCounters.RetryCount.QuadPart)
				pAdapter->WlanCounters.TransmittedFragmentCount.QuadPart = pAdapter->WlanCounters.RetryCount.QuadPart;

			Statistics.TransmittedFragmentCount.QuadPart = pAdapter->WlanCounters.TransmittedFragmentCount.QuadPart;
			Statistics.MulticastTransmittedFrameCount.QuadPart = pAdapter->WlanCounters.MulticastTransmittedFrameCount.QuadPart;
			Statistics.FailedCount.QuadPart = pAdapter->WlanCounters.FailedCount.QuadPart;
			Statistics.RetryCount.QuadPart = pAdapter->WlanCounters.RetryCount.QuadPart;
			Statistics.MultipleRetryCount.QuadPart = pAdapter->WlanCounters.MultipleRetryCount.QuadPart;
			Statistics.RTSSuccessCount.QuadPart = pAdapter->WlanCounters.RTSSuccessCount.QuadPart;
			Statistics.RTSFailureCount.QuadPart = pAdapter->WlanCounters.RTSFailureCount.QuadPart;
			Statistics.ACKFailureCount.QuadPart = pAdapter->WlanCounters.ACKFailureCount.QuadPart;
			Statistics.FrameDuplicateCount.QuadPart = pAdapter->WlanCounters.FrameDuplicateCount.QuadPart;
			Statistics.ReceivedFragmentCount.QuadPart = pAdapter->WlanCounters.ReceivedFragmentCount.QuadPart;
			Statistics.MulticastReceivedFrameCount.QuadPart = pAdapter->WlanCounters.MulticastReceivedFrameCount.QuadPart;
#ifdef DBG	
			Statistics.FCSErrorCount = pAdapter->RalinkCounters.RealFcsErrCount;
#else
			Statistics.FCSErrorCount.QuadPart = pAdapter->WlanCounters.FCSErrorCount.QuadPart;
			Statistics.FrameDuplicateCount.vv.LowPart = pAdapter->WlanCounters.FrameDuplicateCount.vv.LowPart / 100;
#endif
			wrq->u.data.length = sizeof(NDIS_802_11_STATISTICS);
			Status = copy_to_user(wrq->u.data.pointer, &Statistics, wrq->u.data.length);
			break;			 
		case OID_GEN_RCV_OK:
			DBGPRINT(RT_DEBUG_INFO, "Query::OID_GEN_RCV_OK \n");
			ulInfo = pAdapter->Counters8023.GoodReceives;
			wrq->u.data.length = sizeof(ulInfo);
			Status = copy_to_user(wrq->u.data.pointer, &ulInfo, wrq->u.data.length);
			break;
		case OID_GEN_RCV_NO_BUFFER:
			DBGPRINT(RT_DEBUG_INFO, "Query::OID_GEN_RCV_NO_BUFFER \n");
			ulInfo = pAdapter->Counters8023.RxNoBuffer;
			wrq->u.data.length = sizeof(ulInfo);
			Status = copy_to_user(wrq->u.data.pointer, &ulInfo, wrq->u.data.length);
			break;
		case RT_OID_802_11_PHY_MODE:
			ulInfo = (ULONG)pAdapter->PortCfg.PhyMode;
			wrq->u.data.length = sizeof(ulInfo);
			Status = copy_to_user(wrq->u.data.pointer, &ulInfo, wrq->u.data.length);
			DBGPRINT(RT_DEBUG_TRACE, "Query::RT_OID_802_11_PHY_MODE (=%d)\n", ulInfo);
			break;
		case RT_OID_802_11_STA_CONFIG:
			DBGPRINT(RT_DEBUG_TRACE, "Query::RT_OID_802_11_STA_CONFIG\n");
			StaConfig.EnableTxBurst = pAdapter->PortCfg.bEnableTxBurst;
			StaConfig.EnableTurboRate = 0; //pAdapter->PortCfg.EnableTurboRate;
			StaConfig.UseBGProtection = pAdapter->PortCfg.UseBGProtection;
			StaConfig.UseShortSlotTime = pAdapter->PortCfg.UseShortSlotTime;
			StaConfig.AdhocMode = pAdapter->PortCfg.AdhocMode;
			StaConfig.HwRadioStatus = (pAdapter->PortCfg.bHwRadio == TRUE) ? 1 : 0;
			StaConfig.Rsv1 = 0;
			StaConfig.SystemErrorBitmap = pAdapter->SystemErrorBitmap;
			wrq->u.data.length = sizeof(RT_802_11_STA_CONFIG);
			Status = copy_to_user(wrq->u.data.pointer, &StaConfig, wrq->u.data.length);
			break;
		case OID_802_11_RTS_THRESHOLD:
			RtsThresh = pAdapter->PortCfg.RtsThreshold;
			wrq->u.data.length = sizeof(RtsThresh);
			Status = copy_to_user(wrq->u.data.pointer, &RtsThresh, wrq->u.data.length);
			DBGPRINT(RT_DEBUG_TRACE, "Query::OID_802_11_RTS_THRESHOLD(=%d)\n", RtsThresh);
			break;
		case OID_802_11_FRAGMENTATION_THRESHOLD:
			FragThresh = pAdapter->PortCfg.FragmentThreshold;
			if (pAdapter->PortCfg.bFragmentZeroDisable == TRUE)
				FragThresh = 0;
			wrq->u.data.length = sizeof(FragThresh);
			Status = copy_to_user(wrq->u.data.pointer, &FragThresh, wrq->u.data.length);
			DBGPRINT(RT_DEBUG_TRACE, "Query::OID_802_11_FRAGMENTATION_THRESHOLD(=%d)\n", FragThresh);
			break;
		case OID_802_11_POWER_MODE:
			PowerMode = pAdapter->PortCfg.WindowsPowerMode;
			wrq->u.data.length = sizeof(PowerMode);
			Status = copy_to_user(wrq->u.data.pointer, &PowerMode, wrq->u.data.length);
			DBGPRINT(RT_DEBUG_TRACE, "Query::OID_802_11_POWER_MODE(=%d)\n", PowerMode);
			break;
		case RT_OID_802_11_RADIO:
			RadioState = (BOOLEAN) pAdapter->PortCfg.bSwRadio;
			wrq->u.data.length = sizeof(RadioState);
			Status = copy_to_user(wrq->u.data.pointer, &RadioState, wrq->u.data.length);
			DBGPRINT(RT_DEBUG_TRACE, "Query::RT_OID_802_11_QUERY_RADIO (=%d)\n", RadioState);
			break;
		case OID_802_11_INFRASTRUCTURE_MODE:
			if (ADHOC_ON(pAdapter))
				BssType = Ndis802_11IBSS;
			else if (INFRA_ON(pAdapter))
				BssType = Ndis802_11Infrastructure;
			else
				BssType = Ndis802_11AutoUnknown;

			wrq->u.data.length = sizeof(BssType);
			Status = copy_to_user(wrq->u.data.pointer, &BssType, wrq->u.data.length);
			DBGPRINT(RT_DEBUG_TRACE, "Query::OID_802_11_INFRASTRUCTURE_MODE(=%d)\n", BssType);
			break;
		case RT_OID_802_11_PREAMBLE:
			PreamType = pAdapter->PortCfg.TxPreamble;
			wrq->u.data.length = sizeof(PreamType);
			Status = copy_to_user(wrq->u.data.pointer, &PreamType, wrq->u.data.length);
			DBGPRINT(RT_DEBUG_TRACE, "Query::RT_OID_802_11_PREAMBLE(=%d)\n", PreamType);
			break;
		case OID_802_11_AUTHENTICATION_MODE:
			AuthMode = pAdapter->PortCfg.AuthMode;
			wrq->u.data.length = sizeof(AuthMode);
			Status = copy_to_user(wrq->u.data.pointer, &AuthMode, wrq->u.data.length);
			DBGPRINT(RT_DEBUG_TRACE, "Query::OID_802_11_AUTHENTICATION_MODE(=%d)\n", AuthMode);
			break;
		case OID_802_11_WEP_STATUS:
			WepStatus = pAdapter->PortCfg.WepStatus;
			wrq->u.data.length = sizeof(WepStatus);
			Status = copy_to_user(wrq->u.data.pointer, &WepStatus, wrq->u.data.length);
			DBGPRINT(RT_DEBUG_TRACE, "Query::OID_802_11_WEP_STATUS(=%d)\n", WepStatus);
			break;
		case OID_802_11_TX_POWER_LEVEL:
			wrq->u.data.length = sizeof(ULONG);
			Status = copy_to_user(wrq->u.data.pointer, &pAdapter->PortCfg.TxPower, wrq->u.data.length);
			DBGPRINT(RT_DEBUG_TRACE, "Query::OID_802_11_TX_POWER_LEVEL %x\n",pAdapter->PortCfg.TxPower);
			break;
		case RT_OID_802_11_TX_POWER_LEVEL_1:
			wrq->u.data.length = sizeof(ULONG);
			Status = copy_to_user(wrq->u.data.pointer, &pAdapter->PortCfg.TxPowerPercentage, wrq->u.data.length);
			DBGPRINT(RT_DEBUG_TRACE, "Query::RT_OID_802_11_TX_POWER_LEVEL_1 (=%d)\n", pAdapter->PortCfg.TxPowerPercentage);
			break;
		case OID_802_11_NETWORK_TYPES_SUPPORTED:
			if ((pAdapter->RfIcType == RFIC_5226) || (pAdapter->RfIcType == RFIC_5225))
			{
				NetworkTypeList[0] = 3; 				// NumberOfItems = 3
				NetworkTypeList[1] = Ndis802_11DS;		// NetworkType[1] = 11b
				NetworkTypeList[2] = Ndis802_11OFDM24;	// NetworkType[2] = 11g
				NetworkTypeList[3] = Ndis802_11OFDM5;	// NetworkType[3] = 11a
				wrq->u.data.length = 16;
				Status = copy_to_user(wrq->u.data.pointer, &NetworkTypeList[0], wrq->u.data.length);
			}
			else
			{
				NetworkTypeList[0] = 2; 				// NumberOfItems = 2
				NetworkTypeList[1] = Ndis802_11DS;		// NetworkType[1] = 11b
				NetworkTypeList[2] = Ndis802_11OFDM24;	// NetworkType[2] = 11g
				wrq->u.data.length = 12;
				Status = copy_to_user(wrq->u.data.pointer, &NetworkTypeList[0], wrq->u.data.length);
			}
			DBGPRINT(RT_DEBUG_TRACE, "Query::OID_802_11_NETWORK_TYPES_SUPPORTED\n");
			break;
		case OID_802_11_NETWORK_TYPE_IN_USE:
			wrq->u.data.length = sizeof(ULONG);
			if (pAdapter->PortCfg.PhyMode == PHY_11A)
				ulInfo = Ndis802_11OFDM5;
			else if ((pAdapter->PortCfg.PhyMode == PHY_11BG_MIXED) || (pAdapter->PortCfg.PhyMode == PHY_11G))
				ulInfo = Ndis802_11OFDM24;
			else
				ulInfo = Ndis802_11DS;
			Status = copy_to_user(wrq->u.data.pointer, &ulInfo, wrq->u.data.length);
			DBGPRINT(RT_DEBUG_INFO, "Query::OID_802_11_NETWORK_TYPE_IN_USE(=%d)\n", ulInfo);
			break;
		case RT_OID_802_11_QUERY_LAST_RX_RATE:
			ulInfo = (ULONG)pAdapter->LastRxRate;
			wrq->u.data.length = sizeof(ulInfo);
			Status = copy_to_user(wrq->u.data.pointer, &ulInfo, wrq->u.data.length);
			DBGPRINT(RT_DEBUG_TRACE, "Query::RT_OID_802_11_QUERY_LAST_RX_RATE (=%d)\n", ulInfo);
			break;
		case RT_OID_802_11_QUERY_EEPROM_VERSION:
			wrq->u.data.length = sizeof(ULONG);
			Status = copy_to_user(wrq->u.data.pointer, &pAdapter->EepromVersion, wrq->u.data.length);
			DBGPRINT(RT_DEBUG_INFO, "Query::RT_OID_802_11_QUERY_EEPROM_VERSION (=%d)\n", pAdapter->EepromVersion);
			break;
		case RT_OID_802_11_QUERY_FIRMWARE_VERSION:
			wrq->u.data.length = sizeof(ULONG);
			Status = copy_to_user(wrq->u.data.pointer, &pAdapter->FirmwareVersion, wrq->u.data.length);
			DBGPRINT(RT_DEBUG_INFO, "Query::RT_OID_802_11_QUERY_FIRMWARE_VERSION (=%d)\n", pAdapter->FirmwareVersion);
			break;
		case RT_OID_802_11_QUERY_NOISE_LEVEL:
			// TODO: how to measure NOISE LEVEL now?????
			wrq->u.data.length = sizeof(UCHAR);
			Status = copy_to_user(wrq->u.data.pointer, &pAdapter->BbpWriteLatch[17], wrq->u.data.length);
			DBGPRINT(RT_DEBUG_TRACE, "Query::RT_OID_802_11_QUERY_NOISE_LEVEL (=%d)\n", pAdapter->BbpWriteLatch[17]);
			break;
		case RT_OID_802_11_EXTRA_INFO:
			wrq->u.data.length = sizeof(ULONG);
			Status = copy_to_user(wrq->u.data.pointer, &pAdapter->ExtraInfo, wrq->u.data.length);
			DBGPRINT(RT_DEBUG_TRACE, "Query::RT_OID_802_11_EXTRA_INFO (=%d)\n", pAdapter->ExtraInfo);
			break;
		case RT_OID_802_11_QUERY_PIDVID:
			wrq->u.data.length = sizeof(ULONG);
			Status = copy_to_user(wrq->u.data.pointer, &pAdapter->VendorDesc, wrq->u.data.length);
			DBGPRINT(RT_DEBUG_INFO, "Query::RT_OID_802_11_QUERY_PIDVID (=%d)\n", pAdapter->VendorDesc);
			break;
	    case RT_OID_WE_VERSION_COMPILED:
	        wrq->u.data.length = sizeof(UINT);
	        we_version_compiled = WIRELESS_EXT;
	        Status = copy_to_user(wrq->u.data.pointer, &we_version_compiled, wrq->u.data.length);
	        DBGPRINT(RT_DEBUG_INFO, "Query::RT_OID_WE_VERSION_COMPILED (=%d)\n", we_version_compiled);
	        break;

		default:
			DBGPRINT(RT_DEBUG_TRACE, "Query::unknown IOCTL's subcmd = 0x%08x\n", cmd);
			Status = -EOPNOTSUPP;
			break;
	}
	return Status;
}

INT rt73_ioctl(
	IN	struct net_device	*net_dev, 
	IN	OUT	struct ifreq	*rq, 
	IN	INT					cmd)
{
	RTMP_ADAPTER						*pAd = net_dev->priv;
	struct iwreq						*wrq = (struct iwreq *) rq;
	struct iw_point 					*erq = NULL;
	struct iw_freq						*frq = NULL;
	NDIS_802_11_SSID					Ssid, *pSsid=NULL;
	NDIS_802_11_NETWORK_INFRASTRUCTURE	BssType = Ndis802_11Infrastructure;
	NDIS_802_11_RTS_THRESHOLD			RtsThresh;
	NDIS_802_11_FRAGMENTATION_THRESHOLD FragThresh;
	NDIS_802_11_MAC_ADDRESS 			Bssid;
	BOOLEAN 							StateMachineTouched = FALSE;
	INT 								Status = NDIS_STATUS_SUCCESS;
	USHORT								subcmd;
	int 								chan=-1, index=0, len=0, i=0;


	if ( (RTMP_TEST_FLAG(pAd , fRTMP_ADAPTER_NIC_NOT_EXIST)) || 
		 (RTMP_TEST_FLAG(pAd , fRTMP_ADAPTER_RESET_IN_PROGRESS)))
	{
		DBGPRINT(RT_DEBUG_TRACE, "IOCTL::remove in progress!\n");
		return -ENETDOWN;
	}
	if (pAd->RTUSBCmdThr_pid < 0)
		return -ENETDOWN;
		
	switch(cmd)
	{			
		case SIOCGIFHWADDR:     //get  MAC addresses
			DBGPRINT(RT_DEBUG_TRACE, "IOCTL::SIOCGIFHWADDR\n");
			memcpy(wrq->u.name, pAd->CurrentAddress, ETH_ALEN);
            DBGPRINT(RT_DEBUG_TRACE, "IOCTL::SIOCGIFHWADDR(=%02x:%02x:%02x:%02x:%02x:%02x)\n",
		        pAd->CurrentAddress[0],pAd->CurrentAddress[1],pAd->CurrentAddress[2],
		        pAd->CurrentAddress[3],pAd->CurrentAddress[4],pAd->CurrentAddress[5]);
			break;
		case SIOCGIWNAME:
			DBGPRINT(RT_DEBUG_TRACE, "IOCTL::SIOCGIWNAME\n");
			strcpy(wrq->u.name, "RT73 WLAN");
			break;
		case SIOCSIWESSID:	//Set ESSID
			erq = &wrq->u.essid;
			memset(&Ssid, 0x00, sizeof(NDIS_802_11_SSID));
			if (erq->flags)
			{
				if (erq->length > IW_ESSID_MAX_SIZE)
				{
					Status = -E2BIG;
					break;
				}

				Status = copy_from_user(Ssid.Ssid, erq->pointer, (erq->length - 1));
				Ssid.SsidLength = erq->length - 1;	//minus null character.
			}
			else
			{	
				Ssid.SsidLength = 0;  // ANY ssid 
				memcpy(pSsid->Ssid, "", 0);	    
			}
			pSsid = &Ssid;
			if (pAd->Mlme.CntlMachine.CurrState != CNTL_IDLE)
			{
				MlmeEnqueue(pAd, 
				            MLME_CNTL_STATE_MACHINE,
				            RT_CMD_RESET_MLME,
				            0,
				            NULL);	
			}
			
			// To prevent some kernel thread is very low priority ...so essid copy immediately for later wpapsk counting.
			if (pAd->PortCfg.AuthMode >= Ndis802_11AuthModeWPA)
			{
				NdisMoveMemory(pAd->MlmeAux.Ssid, Ssid.Ssid, Ssid.SsidLength);
				pAd->MlmeAux.SsidLen = Ssid.SsidLength;
			}
				
			MlmeEnqueue(pAd, 
						MLME_CNTL_STATE_MACHINE, 
						OID_802_11_SSID,
						sizeof(NDIS_802_11_SSID),
						(VOID *)pSsid);
					
			Status = NDIS_STATUS_SUCCESS;
			StateMachineTouched = TRUE;

			DBGPRINT(RT_DEBUG_TRACE, "IOCTL::SIOCSIWESSID[cmd=0x%x] (Len=%d,Ssid=%s)\n", SIOCSIWESSID, pSsid->SsidLength, pSsid->Ssid);
			break;
		case SIOCGIWESSID:	//Get ESSID
			erq = &wrq->u.essid;

			 if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED))
			{
				erq->flags=1;
				erq->length = pAd->PortCfg.SsidLen;
				Status = copy_to_user(erq->pointer, pAd->PortCfg.Ssid, erq->length);
			}
			else
			{//the ANY ssid was specified
				erq->flags=0;
				erq->length=0;
			}
			DBGPRINT(RT_DEBUG_TRACE, "IOCTL::SIOCGIWESSID (Len=%d, ssid=%lu...)\n", erq->length, (unsigned long)erq->pointer);
			break;	   
		case SIOCSIWNWID:	// set network id (the cell)
		case SIOCGIWNWID:	// get network id
			Status = -EOPNOTSUPP;
			break;
		case SIOCSIWFREQ:	//set channel/frequency (Hz)
			frq = &wrq->u.freq;
			if((frq->e == 0) && (frq->m <= 1000))
				chan = frq->m;	// Setting by channel number 
			else
				MAP_KHZ_TO_CHANNEL_ID( (frq->m /100) , chan); // Setting by frequency - search the table , like 2.412G, 2.422G, 
			pAd->PortCfg.Channel = chan;
			DBGPRINT(RT_DEBUG_TRACE, "IOCTL::SIOCSIWFREQ[cmd=0x%x] (Channel=%d)\n", SIOCSIWFREQ, pAd->PortCfg.Channel);
			break;
		case SIOCGIWFREQ:	// get channel/frequency (Hz)
			wrq->u.freq.m = pAd->PortCfg.Channel;
			wrq->u.freq.e = 0;
			wrq->u.freq.i = 0;
			break;	
		case SIOCSIWNICKN: //set node name/nickname
			erq = &wrq->u.data;
			if (erq->flags)
			{
				if (erq->length <= IW_ESSID_MAX_SIZE)
					Status = copy_from_user(pAd->nickn, erq->pointer, erq->length);
				else
					Status = -E2BIG;
			}
			break;	
		case SIOCGIWNICKN: //get node name/nickname
			erq = &wrq->u.data;
			erq->length = strlen(pAd->nickn);
			Status = copy_to_user(erq->pointer, pAd->nickn, erq->length);
			break;
		
		case SIOCGIWRATE:	//get default bit rate (bps)
			wrq->u.bitrate.value = RateIdTo500Kbps[pAd->PortCfg.TxRate] * 500000;
			wrq->u.bitrate.disabled = 0;
			break;
		case SIOCSIWRATE:  //set default bit rate (bps)
			RTMPSetDesiredRates(pAd, wrq->u.bitrate.value);
			break;
		case SIOCGIWRTS:  // get RTS/CTS threshold (bytes)
			wrq->u.rts.value = (INT) pAd->PortCfg.RtsThreshold;
			wrq->u.rts.disabled = (wrq->u.rts.value == MAX_RTS_THRESHOLD);
			wrq->u.rts.fixed = 1;
			break;
		case SIOCSIWRTS:  //set RTS/CTS threshold (bytes)
			RtsThresh = wrq->u.rts.value;
			if (wrq->u.rts.disabled)
				RtsThresh = MAX_RTS_THRESHOLD;

			if((RtsThresh > 0) && (RtsThresh <= MAX_RTS_THRESHOLD))
				pAd->PortCfg.RtsThreshold = (USHORT)RtsThresh;
			else if (RtsThresh == 0)
				pAd->PortCfg.RtsThreshold = MAX_RTS_THRESHOLD;

			DBGPRINT(RT_DEBUG_TRACE, "IOCTL::SIOCSIWRTS (=%d)\n", pAd->PortCfg.RtsThreshold);
			break;
		case SIOCGIWFRAG:  //get fragmentation thr (bytes)
			wrq->u.frag.value = (INT) pAd->PortCfg.FragmentThreshold;
			wrq->u.frag.disabled = (wrq->u.frag.value >= MAX_FRAG_THRESHOLD);
			wrq->u.frag.fixed = 1;
			break;
		case SIOCSIWFRAG:  //set fragmentation thr (bytes)
			FragThresh = wrq->u.frag.value;
			if (wrq->u.rts.disabled)
				FragThresh = MAX_FRAG_THRESHOLD;

			if ( (FragThresh >= MIN_FRAG_THRESHOLD) && (FragThresh <= MAX_FRAG_THRESHOLD))
				pAd->PortCfg.FragmentThreshold = (USHORT)FragThresh;
			else if (FragThresh == 0)
				pAd->PortCfg.FragmentThreshold = MAX_FRAG_THRESHOLD;

			if (pAd->PortCfg.FragmentThreshold == MAX_FRAG_THRESHOLD)
				pAd->PortCfg.bFragmentZeroDisable = TRUE;
			else
				pAd->PortCfg.bFragmentZeroDisable = FALSE;

			DBGPRINT(RT_DEBUG_TRACE, "IOCTL::SIOCSIWFRAG (=%d)\n", pAd->PortCfg.FragmentThreshold);
			break;
		case SIOCGIWENCODE:  //get encoding token & mode
			index = (wrq->u.encoding.flags & IW_ENCODE_INDEX) - 1;
			if ((index < 0) || (index >= NR_WEP_KEYS))
				index = pAd->PortCfg.DefaultKeyId; // Default key for tx (shared key)

			if (pAd->PortCfg.AuthMode == Ndis802_11AuthModeOpen)
				wrq->u.encoding.flags = IW_ENCODE_OPEN;
			else if (pAd->PortCfg.AuthMode == Ndis802_11AuthModeShared)
				wrq->u.encoding.flags = IW_ENCODE_RESTRICTED;

			if (pAd->PortCfg.WepStatus == Ndis802_11WEPDisabled)
				wrq->u.encoding.flags |= IW_ENCODE_DISABLED;
			else
			{
				if(wrq->u.encoding.pointer)
				{
					wrq->u.encoding.length = pAd->SharedKey[index].KeyLen;
					Status = copy_to_user(wrq->u.encoding.pointer, 
								pAd->SharedKey[index].Key,
								pAd->SharedKey[index].KeyLen);
					wrq->u.encoding.flags |= (index + 1);
				}
			}
			break;
		case SIOCSIWENCODE:  //set encoding token & mode
			index = (wrq->u.encoding.flags & IW_ENCODE_INDEX) - 1;
			/* take the old default key if index is invalid */
			if((index < 0) || (index >= NR_WEP_KEYS))
				index = pAd->PortCfg.DefaultKeyId;	   // Default key for tx (shared key)

			if(wrq->u.encoding.pointer)
			{
				len = wrq->u.encoding.length;
				if(len > WEP_LARGE_KEY_LEN)
					len = WEP_LARGE_KEY_LEN;

				memset(pAd->SharedKey[index].Key, 0x00, MAX_LEN_OF_KEY);
				Status = copy_from_user(pAd->SharedKey[index].Key, 
								wrq->u.encoding.pointer, len);
				pAd->SharedKey[index].KeyLen = len <= WEP_SMALL_KEY_LEN ? WEP_SMALL_KEY_LEN : WEP_LARGE_KEY_LEN;
			}
			pAd->PortCfg.DefaultKeyId = (UCHAR) index;
			if (wrq->u.encoding.flags & IW_ENCODE_DISABLED)
				pAd->PortCfg.WepStatus = Ndis802_11WEPDisabled;
			else
				pAd->PortCfg.WepStatus = Ndis802_11WEPEnabled;

			if (wrq->u.encoding.flags & IW_ENCODE_RESTRICTED)
				pAd->PortCfg.AuthMode = Ndis802_11AuthModeShared;
			else 
				pAd->PortCfg.AuthMode = Ndis802_11AuthModeOpen;

			if(pAd->PortCfg.WepStatus == Ndis802_11WEPDisabled)
				pAd->PortCfg.AuthMode = Ndis802_11AuthModeOpen;

			DBGPRINT(RT_DEBUG_TRACE, "IOCTL::SIOCSIWENCODE Key[%x] => \n", index);
			for (i = 0; i < len; i++)
			{
				DBGPRINT(RT_DEBUG_TRACE, "%02x:", pAd->SharedKey[index].Key[i]);
				if (i%16 == 15)
					DBGPRINT(RT_DEBUG_TRACE, "\n");
			}
			DBGPRINT(RT_DEBUG_TRACE, "\n");
			break;
		case SIOCGIWAP: 	//get access point MAC addresses
			if (INFRA_ON(pAd) || ADHOC_ON(pAd))
			{
				wrq->u.ap_addr.sa_family = ARPHRD_ETHER;
				memcpy(wrq->u.ap_addr.sa_data, &pAd->PortCfg.Bssid, ETH_ALEN);
				DBGPRINT(RT_DEBUG_TRACE, "IOCTL::SIOCGIWAP(=%02x:%02x:%02x:%02x:%02x:%02x)\n",
					pAd->PortCfg.Bssid[0],pAd->PortCfg.Bssid[1],pAd->PortCfg.Bssid[2],
					pAd->PortCfg.Bssid[3],pAd->PortCfg.Bssid[4],pAd->PortCfg.Bssid[5]);
			}
			else
			{
				DBGPRINT(RT_DEBUG_TRACE, "IOCTL::SIOCGIWAP(=EMPTY)\n");
				Status = -ENOTCONN;
			}
			break;
		case SIOCSIWAP:  //set access point MAC addresses
			Status = copy_from_user(&Bssid, &wrq->u.ap_addr.sa_data, sizeof(NDIS_802_11_MAC_ADDRESS));
			if (pAd->Mlme.CntlMachine.CurrState != CNTL_IDLE)
			{
				MlmeEnqueue(pAd, 
				            MLME_CNTL_STATE_MACHINE,
				            RT_CMD_RESET_MLME,
				            0,
				            NULL);	
			}

			MlmeEnqueue(pAd, 
						MLME_CNTL_STATE_MACHINE, 
						OID_802_11_BSSID, 
						sizeof(NDIS_802_11_MAC_ADDRESS),
						(VOID *)&Bssid);
			Status = NDIS_STATUS_SUCCESS;
			StateMachineTouched = TRUE;
			DBGPRINT(RT_DEBUG_TRACE, "IOCTL::SIOCSIWAP %02x:%02x:%02x:%02x:%02x:%02x\n",
				Bssid[0], Bssid[1], Bssid[2], Bssid[3], Bssid[4], Bssid[5]);
			break;
		case SIOCGIWMODE:	//get operation mode
			if (ADHOC_ON(pAd))
			{
				BssType = Ndis802_11IBSS;
				wrq->u.mode = IW_MODE_ADHOC;
			}
			else if (INFRA_ON(pAd))
			{
				BssType = Ndis802_11Infrastructure;
				wrq->u.mode = IW_MODE_INFRA;
			}
			else
			{
				BssType = Ndis802_11AutoUnknown;
				wrq->u.mode = IW_MODE_AUTO;
			}
			DBGPRINT(RT_DEBUG_TRACE, "IOCTL::SIOCGIWMODE(=%d)\n", BssType);
			break;	
		case SIOCSIWMODE:	//set operation mode
			if(wrq->u.mode == IW_MODE_ADHOC)
			{
				if (pAd->PortCfg.BssType != BSS_ADHOC)
				{
					// Config has changed
					pAd->bConfigChanged = TRUE;
				}
				pAd->PortCfg.BssType = BSS_ADHOC;
				DBGPRINT(RT_DEBUG_TRACE, "IOCTL::SIOCSIWMODE (AD-HOC)\n");
			}
			else if (wrq->u.mode == IW_MODE_INFRA)
			{
				if (pAd->PortCfg.BssType != BSS_INFRA)
				{
					// Config has changed
					pAd->bConfigChanged = TRUE;
				}
				pAd->PortCfg.BssType = BSS_INFRA;
				DBGPRINT(RT_DEBUG_TRACE, "IOCTL::SIOCSIWMODE (INFRA)\n");
			}
			else
			{
				Status	= -EINVAL;
				DBGPRINT(RT_DEBUG_TRACE, "IOCTL::SIOCSIWMODE (unknown)\n");
			}
			// Reset Ralink supplicant to not use, it will be set to start when UI set PMK key
			pAd->PortCfg.WpaState = SS_NOTUSE;
			break;
		case SIOCGIWSENS:	//get sensitivity (dBm)
		case SIOCSIWSENS:	//set sensitivity (dBm)
		case SIOCGIWPOWER:	//get Power Management settings
		case SIOCSIWPOWER:	//set Power Management settings
		case SIOCGIWTXPOW:	//get transmit power (dBm)
		case SIOCSIWTXPOW:	//set transmit power (dBm)
		case SIOCGIWRANGE:	//Get range of parameters
		case SIOCGIWRETRY:	//get retry limits and lifetime
		case SIOCSIWRETRY:	//set retry limits and lifetime
			Status = -EOPNOTSUPP;
			break;			
		case RT_PRIV_IOCTL:
			subcmd = wrq->u.data.flags;
			if( subcmd & OID_GET_SET_TOGGLE)
				Status = RTMPSetInformation(pAd, rq, subcmd);
			else
				Status = RTMPQueryInformation(pAd, rq, subcmd);
			break;		
		case SIOCGIWPRIV:
			if (wrq->u.data.pointer) {
				if ( access_ok(VERIFY_WRITE, wrq->u.data.pointer, sizeof(privtab)) != TRUE)
					break;
				wrq->u.data.length = sizeof(privtab) / sizeof(privtab[0]);
				if (copy_to_user(wrq->u.data.pointer, privtab, sizeof(privtab)))
					Status = -EFAULT;
			}
			break;
		case RTPRIV_IOCTL_SET:
			{				
				CHAR *this_char;
				CHAR *value;
				
				if( access_ok(VERIFY_READ, wrq->u.data.pointer, wrq->u.data.length)!=TRUE )   
					break;

				this_char = kmalloc(wrq->u.data.length, MEM_ALLOC_FLAG);
				if(this_char == NULL)
				{
					Status = -ENOMEM;
					break;
				}
				
				Status = copy_from_user(this_char, wrq->u.data.pointer, wrq->u.data.length);
							
				if (!*this_char)
					break;

				if ((value = rtstrchr(this_char, '=')) != NULL)  
					*value++ = 0;

				if (!value)
					break;

				// reject setting nothing besides ANY ssid(ssidLen=0)
				if (!*value && (strcmp(this_char, "SSID") != 0))
					break;

				for (PRTMP_PRIVATE_SET_PROC = RTMP_PRIVATE_SUPPORT_PROC; PRTMP_PRIVATE_SET_PROC->name; PRTMP_PRIVATE_SET_PROC++)
				{
					if (!strcmp(this_char, PRTMP_PRIVATE_SET_PROC->name)) 
					{	
						if(!PRTMP_PRIVATE_SET_PROC->set_proc(pAd, value))
						{	//FALSE:Set private failed then return Invalid argument 								
							Status = -EINVAL;
						}
						break;	//Exit for loop.
					}
				}								
					
				if(PRTMP_PRIVATE_SET_PROC->name == NULL)
				{  //Not found argument
					Status = -EINVAL;
					DBGPRINT(RT_DEBUG_TRACE, "IOCTL::(iwpriv) Command not Support [%s=%s]\n", this_char, value);
					break;
				}				
				kfree(this_char);		
			}
			break;	
		case RTPRIV_IOCTL_GSITESURVEY:
			RTMPIoctlGetSiteSurvey(pAd, wrq);
			break;
		
		case RTPRIV_IOCTL_STATISTICS:
			RTMPIoctlStatistics(pAd, wrq);
			break;
#ifdef DBG
		//case RTPRIV_IOCTL_BBP:
		//	RTMPIoctlBBP(pAd, wrq);
		//	break;
			
		//case RTPRIV_IOCTL_MAC:
		//	RTMPIoctlMAC(pAd, wrq);
		//	break;		
#ifdef RALINK_ATE
		case RTPRIV_IOCTL_E2P:
			RTMPIoctlE2PROM(pAd, wrq);
			break;
#endif // RALINK_ATE			

#endif

#if 1
		case RTPRIV_IOCTL_AUTH:
			Status = RTMPIoctlSetAuth(pAd, wrq);
			break;
		case RTPRIV_IOCTL_WEPSTATUS:
			Status = RTMPIoctlSetEncryp(pAd, wrq);
			break;
		case RTPRIV_IOCTL_WPAPSK:
			Status = RTMPIoctlSetWpapsk(pAd, wrq);
			break;
		case RTPRIV_IOCTL_PSM:
			Status = RTMPIoctlSetPsm(pAd, wrq);
			break;
#endif	  
        	case RTPRIV_IOCTL_GETRAAPCFG:            
			RTMPIoctlGetRaAPCfg(pAd, wrq);            
			break;
		default:
			DBGPRINT(RT_DEBUG_TRACE, "IOCTL::unknown IOCTL's cmd = 0x%08x\n", cmd);
			if (cmd == 0x89F1)
				Status = NDIS_STATUS_SUCCESS; // ignore default config by some of platforms, e.g. SuSE
			else 
				Status = -EOPNOTSUPP;
				
			break;
	}

	if(StateMachineTouched) // Upper layer sent a MLME-related operations
		RTUSBMlmeUp(pAd);


	return Status;
}

/*
	========================================================================
	
	Routine Description:
		Add WPA key process

	Arguments:
		pAd						Pointer to our adapter
		pBuf					Pointer to the where the key stored

	Return Value:
		NDIS_SUCCESS			Add key successfully
		
	========================================================================
*/
NDIS_STATUS	RTMPWPAAddKeyProc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PVOID			pBuf)
{
	PNDIS_802_11_KEY	pKey;
	ULONG				KeyIdx;
//	NDIS_STATUS			Status;
	
	PUCHAR		pTxMic, pRxMic;
	BOOLEAN 	bTxKey; 		// Set the key as transmit key
	BOOLEAN 	bPairwise;		// Indicate the key is pairwise key
	BOOLEAN 	bKeyRSC;		// indicate the receive  SC set by KeyRSC value.
								// Otherwise, it will set by the NIC.
	BOOLEAN 	bAuthenticator; // indicate key is set by authenticator.
	
	pKey = (PNDIS_802_11_KEY) pBuf;
	KeyIdx = pKey->KeyIndex & 0xff;
	// Bit 31 of Add-key, Tx Key
	bTxKey		   = (pKey->KeyIndex & 0x80000000) ? TRUE : FALSE;
	// Bit 30 of Add-key PairwiseKey
	bPairwise	   = (pKey->KeyIndex & 0x40000000) ? TRUE : FALSE;
	// Bit 29 of Add-key KeyRSC
	bKeyRSC 	   = (pKey->KeyIndex & 0x20000000) ? TRUE : FALSE;
	// Bit 28 of Add-key Authenticator
	bAuthenticator = (pKey->KeyIndex & 0x10000000) ? TRUE : FALSE;

	//
	// Check Group / Pairwise Key
	//
	if (bPairwise)	// Pairwise Key
	{
		// 1. KeyIdx must be 0, otherwise, return NDIS_STATUS_FAILURE
		if (KeyIdx != 0)
			return(NDIS_STATUS_FAILURE);
		
		// 2. Check bTx, it must be true, otherwise, return NDIS_STATUS_FAILURE
		if (bTxKey == FALSE)
			return(NDIS_STATUS_FAILURE);

		// 3. If BSSID is all 0xff, return NDIS_STATUS_FAILURE
		if (MAC_ADDR_EQUAL(pKey->BSSID, BROADCAST_ADDR))
			return(NDIS_STATUS_FAILURE);

		// 3.1 Check Pairwise key length for TKIP key. For AES, it's always 128 bits
		//if ((pAd->PortCfg.WepStatus == Ndis802_11Encryption2Enabled) && (pKey->KeyLength != LEN_TKIP_KEY))
		if ((pAd->PortCfg.PairCipher == Ndis802_11Encryption2Enabled) && (pKey->KeyLength != LEN_TKIP_KEY))
			return(NDIS_STATUS_FAILURE);

		pAd->SharedKey[KeyIdx].Type = PAIRWISE_KEY;
		
		if (pAd->PortCfg.AuthMode == Ndis802_11AuthModeWPA2)
		{
			// Send media specific event to start PMKID caching
			RTMPIndicateWPA2Status(pAd);
		}	
	}
	else			// Group Key
	{
		// 1. Check BSSID, if not current BSSID or Bcast, return NDIS_STATUS_FAILURE
		if ((! MAC_ADDR_EQUAL(pKey->BSSID, BROADCAST_ADDR)) &&
			(! MAC_ADDR_EQUAL(pKey->BSSID, pAd->PortCfg.Bssid)))
			return(NDIS_STATUS_FAILURE);

		// 2. Check Key index for supported Group Key
		if (KeyIdx >= GROUP_KEY_NO) 
			return(NDIS_STATUS_FAILURE);
		
		pAd->SharedKey[KeyIdx].Type = GROUP_KEY;
	}

			
	// 4. Select RxMic / TxMic based on Supp / Authenticator
	if (pAd->PortCfg.AuthMode == Ndis802_11AuthModeWPANone)
	{
		// for WPA-None Tx, Rx MIC is the same
		pTxMic = (PUCHAR) (&pKey->KeyMaterial) + 16;
		pRxMic = pTxMic;
	}
	else if (bAuthenticator == TRUE)
	{
		pTxMic = (PUCHAR) (&pKey->KeyMaterial) + 16;
		pRxMic = (PUCHAR) (&pKey->KeyMaterial) + 24;
	}
	else
	{
		pRxMic = (PUCHAR) (&pKey->KeyMaterial) + 16;
		pTxMic = (PUCHAR) (&pKey->KeyMaterial) + 24;
	}
		
	// 5. Check RxTsc
	if (bKeyRSC == TRUE)
		NdisMoveMemory(pAd->SharedKey[KeyIdx].RxTsc, &pKey->KeyRSC, 6);			
	else
		NdisZeroMemory(pAd->SharedKey[KeyIdx].RxTsc, 6);
		
	// 6. Copy information into Pairwise Key structure.
	// pKey->KeyLength will include TxMic and RxMic, therefore, we use 16 bytes hardcoded.
	pAd->SharedKey[KeyIdx].KeyLen = (UCHAR) pKey->KeyLength;		
	NdisMoveMemory(pAd->SharedKey[KeyIdx].Key, &pKey->KeyMaterial, 16);
	if (pKey->KeyLength == LEN_TKIP_KEY)
	{
		// Only Key lenth equal to TKIP key have these
		NdisMoveMemory(pAd->SharedKey[KeyIdx].RxMic, pRxMic, 8);
		NdisMoveMemory(pAd->SharedKey[KeyIdx].TxMic, pTxMic, 8);
		//
		// pKey->KeyLength = 32.
		// 16: TkipKey, 8: RxMIC Key, 8: TxMIC Key
		// Update Key len here.
		//
		pAd->SharedKey[KeyIdx].KeyLen = 16;
	}
	COPY_MAC_ADDR(pAd->SharedKey[KeyIdx].BssId, pKey->BSSID);
	
	// Init TxTsc to one based on WiFi WPA specs
	pAd->SharedKey[KeyIdx].TxTsc[0] = 1;
	pAd->SharedKey[KeyIdx].TxTsc[1] = 0;
	pAd->SharedKey[KeyIdx].TxTsc[2] = 0;
	pAd->SharedKey[KeyIdx].TxTsc[3] = 0;
	pAd->SharedKey[KeyIdx].TxTsc[4] = 0;
	pAd->SharedKey[KeyIdx].TxTsc[5] = 0;

	// 7. Set as default Tx Key if bTxKey is TRUE
	if (bTxKey == TRUE)
		pAd->PortCfg.DefaultKeyId = (UCHAR) KeyIdx;

	if (pAd->PortCfg.WepStatus == Ndis802_11Encryption3Enabled)
		pAd->SharedKey[KeyIdx].CipherAlg = CIPHER_AES;
	else if (pAd->PortCfg.WepStatus == Ndis802_11Encryption2Enabled)
		pAd->SharedKey[KeyIdx].CipherAlg = CIPHER_TKIP;
	else if (pAd->PortCfg.WepStatus == Ndis802_11Encryption1Enabled)
	{
		if (pAd->SharedKey[KeyIdx].KeyLen == 5)
			pAd->SharedKey[KeyIdx].CipherAlg = CIPHER_WEP64;
		else if (pAd->SharedKey[KeyIdx].KeyLen == 13)
			pAd->SharedKey[KeyIdx].CipherAlg = CIPHER_WEP128;
		else
			pAd->SharedKey[KeyIdx].CipherAlg = CIPHER_NONE;
	}
	else
		pAd->SharedKey[KeyIdx].CipherAlg = CIPHER_NONE;
	
	if ((pAd->PortCfg.AuthMode >= Ndis802_11AuthModeWPA) && (pAd->PortCfg.AuthMode != Ndis802_11AuthModeWPANone))
	{
		//
		// Support WPA-Both , Update Group Key Cipher.
		//
		if (!bPairwise)
		{
			if (pAd->PortCfg.GroupCipher == Ndis802_11Encryption3Enabled)
				pAd->SharedKey[KeyIdx].CipherAlg = CIPHER_AES;
			else if (pAd->PortCfg.GroupCipher == Ndis802_11Encryption2Enabled)
				pAd->SharedKey[KeyIdx].CipherAlg = CIPHER_TKIP;
		}
	}

	DBGPRINT(RT_DEBUG_TRACE, "pAd->SharedKey[%d].CipherAlg = %d\n", KeyIdx, pAd->SharedKey[KeyIdx].CipherAlg);

#if 0
	DBGPRINT_RAW(RT_DEBUG_TRACE, "%s Key #%d", CipherName[pAd->SharedKey[KeyIdx].CipherAlg],KeyIdx);
	for (i = 0; i < 16; i++)
	{
		DBGPRINT_RAW(RT_DEBUG_TRACE, "%02x:", pAd->SharedKey[KeyIdx].Key[i]);
	}
	DBGPRINT_RAW(RT_DEBUG_TRACE, "\n	 Rx MIC Key = ");
	for (i = 0; i < 8; i++)
	{
		DBGPRINT_RAW(RT_DEBUG_TRACE, "%02x:", pAd->SharedKey[KeyIdx].RxMic[i]);
	}
	DBGPRINT_RAW(RT_DEBUG_TRACE, "\n	 Tx MIC Key = ");
	for (i = 0; i < 8; i++)
	{
		DBGPRINT_RAW(RT_DEBUG_TRACE, "%02x:", pAd->SharedKey[KeyIdx].TxMic[i]);
	}
	DBGPRINT_RAW(RT_DEBUG_TRACE, "\n	 RxTSC = ");
	for (i = 0; i < 6; i++)
	{
		DBGPRINT_RAW(RT_DEBUG_TRACE, "%02x:", pAd->SharedKey[KeyIdx].RxTsc[i]);
	}
	DBGPRINT_RAW(RT_DEBUG_TRACE, "\n	 BSSID:%02x:%02x:%02x:%02x:%02x:%02x \n",
		pKey->BSSID[0],pKey->BSSID[1],pKey->BSSID[2],pKey->BSSID[3],pKey->BSSID[4],pKey->BSSID[5]);
#endif	  
	AsicAddSharedKeyEntry(pAd, 
						  0, 
						  (UCHAR)KeyIdx, 
						  pAd->SharedKey[KeyIdx].CipherAlg,
						  pAd->SharedKey[KeyIdx].Key, 
						  pAd->SharedKey[KeyIdx].TxMic,
						  pAd->SharedKey[KeyIdx].RxMic);

	if (pAd->SharedKey[KeyIdx].Type == GROUP_KEY)
	{
		// 802.1x port control
		pAd->PortCfg.PortSecured = WPA_802_1X_PORT_SECURED;
	}
	else
		pAd->PortCfg.PortSecured = WPA_802_1X_PORT_PASS_4_WAY;

	return (NDIS_STATUS_SUCCESS);
}

/*
	========================================================================
	
	Routine Description:
		Remove WPA Key process

	Arguments:
		pAd						Pointer to our adapter
		pBuf					Pointer to the where the key stored

	Return Value:
		NDIS_SUCCESS			Add key successfully
		
	========================================================================
*/
NDIS_STATUS	RTMPWPARemoveKeyProc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PVOID			pBuf)
{
	PNDIS_802_11_REMOVE_KEY	pKey;
	ULONG					KeyIdx;
	NDIS_STATUS				Status = NDIS_STATUS_FAILURE;
	BOOLEAN 	bTxKey; 		// Set the key as transmit key
	BOOLEAN 	bPairwise;		// Indicate the key is pairwise key
	BOOLEAN 	bKeyRSC;		// indicate the receive  SC set by KeyRSC value.
								// Otherwise, it will set by the NIC.
	BOOLEAN 	bAuthenticator; // indicate key is set by authenticator.
	INT 		i;

	DBGPRINT(RT_DEBUG_TRACE,"---> RTMPWPARemoveKeyProc\n");
	
	pKey = (PNDIS_802_11_REMOVE_KEY) pBuf;
	KeyIdx = pKey->KeyIndex & 0xff;
	// Bit 31 of Add-key, Tx Key
	bTxKey		   = (pKey->KeyIndex & 0x80000000) ? TRUE : FALSE;
	// Bit 30 of Add-key PairwiseKey
	bPairwise	   = (pKey->KeyIndex & 0x40000000) ? TRUE : FALSE;
	// Bit 29 of Add-key KeyRSC
	bKeyRSC 	   = (pKey->KeyIndex & 0x20000000) ? TRUE : FALSE;
	// Bit 28 of Add-key Authenticator
	bAuthenticator = (pKey->KeyIndex & 0x10000000) ? TRUE : FALSE;

	// 1. If bTx is TRUE, return failure information
	if (bTxKey == TRUE)
		return(NDIS_STATUS_FAILURE);

	// 2. Check Pairwise Key
	if (bPairwise)
	{
		// a. If BSSID is broadcast, remove all pairwise keys.
		// b. If not broadcast, remove the pairwise specified by BSSID
		for (i = 0; i < SHARE_KEY_NUM; i++)
		{
			if (MAC_ADDR_EQUAL(pAd->SharedKey[i].BssId, pKey->BSSID))
			{
				DBGPRINT(RT_DEBUG_TRACE,"RTMPWPARemoveKeyProc(KeyIdx=%d)\n", i);
				pAd->SharedKey[i].KeyLen = 0;
				pAd->SharedKey[i].CipherAlg = CIPHER_NONE;
				AsicRemoveSharedKeyEntry(pAd, 0, (UCHAR)i);
				Status = NDIS_STATUS_SUCCESS;
				break;
			}
		}
	}
	// 3. Group Key
	else
	{
		// a. If BSSID is broadcast, remove all group keys indexed
		// b. If BSSID matched, delete the group key indexed.
		DBGPRINT(RT_DEBUG_TRACE,"RTMPWPARemoveKeyProc(KeyIdx=%d)\n", KeyIdx);
		pAd->SharedKey[KeyIdx].KeyLen = 0;
		pAd->SharedKey[KeyIdx].CipherAlg = CIPHER_NONE;
		AsicRemoveSharedKeyEntry(pAd, 0, (UCHAR)KeyIdx);
		Status = NDIS_STATUS_SUCCESS;
	}

	return (Status);
}

/*
	========================================================================
	
	Routine Description:
		Construct and indicate WPA2 Media Specific Status

	Arguments:
		pAd	Pointer to our adapter

	Return Value:
		None

	Note:
		
	========================================================================
*/
VOID	RTMPIndicateWPA2Status(
	IN	PRTMP_ADAPTER	pAd)
{
	struct
	{
		NDIS_802_11_STATUS_TYPE				Status;
		NDIS_802_11_PMKID_CANDIDATE_LIST	List;
	}	Candidate;


	Candidate.Status = Ndis802_11StatusType_PMKID_CandidateList;
	Candidate.List.Version = 1;
	// This might need to be fixed to accomadate with current saved PKMIDs
	Candidate.List.NumCandidates = 1;
	NdisMoveMemory(&Candidate.List.CandidateList[0].BSSID, pAd->PortCfg.Bssid, 6);
	Candidate.List.CandidateList[0].Flags = 0;
//	NdisMIndicateStatus(pAd->AdapterHandle, NDIS_STATUS_MEDIA_SPECIFIC_INDICATION, &Candidate, sizeof(Candidate));	


	DBGPRINT(RT_DEBUG_TRACE, "RTMPIndicateWPA2Status\n");
}

/*
	========================================================================
	
	Routine Description:
		Remove All WPA Keys

	Arguments:
		pAd						Pointer to our adapter

	Return Value:
		None
	
	========================================================================
*/
VOID	RTMPWPARemoveAllKeys(
	IN	PRTMP_ADAPTER	pAd)
{
	INT i;

	DBGPRINT(RT_DEBUG_TRACE,"RTMPWPARemoveAllKeys(AuthMode=%d, WepStatus=%d)\n", pAd->PortCfg.AuthMode, pAd->PortCfg.WepStatus);
	
	// For WPA-None, there is no need to remove it, since WinXP won't set it again after
	// Link up. And it will be replaced if user changed it.
	if (pAd->PortCfg.AuthMode == Ndis802_11AuthModeWPANone)
		return;

	for (i = 0; i < SHARE_KEY_NUM; i++)
	{
//		if ((pAd->SharedKey[i].CipherAlg == CIPHER_WEP64)  ||
//			(pAd->SharedKey[i].CipherAlg == CIPHER_WEP128) ||
//			(pAd->SharedKey[i].CipherAlg == CIPHER_CKIP64) ||
//			(pAd->SharedKey[i].CipherAlg == CIPHER_CKIP128))
//			continue;
		
		DBGPRINT(RT_DEBUG_TRACE,"remove %s key #%d\n", CipherName[pAd->SharedKey[i].CipherAlg], i);
		pAd->SharedKey[i].CipherAlg = CIPHER_NONE;
		pAd->SharedKey[i].KeyLen = 0;
		AsicRemoveSharedKeyEntry(pAd, 0, (UCHAR)i);
	}
	
}

/*
	========================================================================
	Routine Description:
		Change NIC PHY mode. Re-association may be necessary. possible settings
		include - PHY_11B, PHY_11BG_MIXED, PHY_11A, and PHY_11ABG_MIXED 

	Arguments:
		pAd - Pointer to our adapter
		phymode  - 

	========================================================================
*/
VOID	RTMPSetPhyMode(
	IN	PRTMP_ADAPTER	pAd,
	IN	ULONG			phymode)
{
	INT	i;

	DBGPRINT(RT_DEBUG_TRACE,"RTMPSetPhyMode(=%d)\n", phymode);
	
// pAd->RfIcType is not ready on RTMPReadParametersFromFile() init
#if 0
	// the selected phymode must be supported by the RF IC encoded in E2PROM
	if ((pAd->RfIcType == RFIC_5226) || (pAd->RfIcType == RFIC_5225))
	{
		DBGPRINT(RT_DEBUG_TRACE,"RFIC[%d], Phymode(=%d)\n", pAd->RfIcType, phymode);
	}
	else if ((pAd->RfIcType == RFIC_2528) || (pAd->RfIcType == RFIC_2527) || (pAd->RfIcType == RFIC_3020))
	{
		if ((phymode == PHY_11A) || (phymode == PHY_11ABG_MIXED))
			phymode = PHY_11BG_MIXED;
		DBGPRINT(RT_DEBUG_TRACE,"RFIC[%d], Phymode(=%d)\n", pAd->RfIcType, phymode);
	}
#endif		  
	// if no change, do nothing
	if (pAd->PortCfg.PhyMode == phymode)
		return;

	pAd->PortCfg.PhyMode = (UCHAR)phymode;
	BuildChannelList(pAd);

	// sanity check user setting in Registry
	for (i = 0; i < pAd->ChannelListNum; i++)
	{
		if (pAd->PortCfg.Channel == pAd->ChannelList[i].Channel)
			break;
	}
	if (i == pAd->ChannelListNum)
		pAd->PortCfg.Channel = FirstChannel(pAd);
	
	NdisZeroMemory(pAd->PortCfg.SupRate, MAX_LEN_OF_SUPPORTED_RATES);
	NdisZeroMemory(pAd->PortCfg.ExtRate, MAX_LEN_OF_SUPPORTED_RATES);
	NdisZeroMemory(pAd->PortCfg.DesireRate, MAX_LEN_OF_SUPPORTED_RATES);
	switch (phymode) {
		case PHY_11B:
			pAd->PortCfg.SupRate[0]  = 0x82;	// 1 mbps, in units of 0.5 Mbps, basic rate
			pAd->PortCfg.SupRate[1]  = 0x84;	// 2 mbps, in units of 0.5 Mbps, basic rate
			pAd->PortCfg.SupRate[2]  = 0x8B;	// 5.5 mbps, in units of 0.5 Mbps, basic rate
			pAd->PortCfg.SupRate[3]  = 0x96;	// 11 mbps, in units of 0.5 Mbps, basic rate
			pAd->PortCfg.SupRateLen  = 4;
			pAd->PortCfg.ExtRateLen  = 0;
			pAd->PortCfg.DesireRate[0]	= 2;	 // 1 mbps, in units of 0.5 Mbps
			pAd->PortCfg.DesireRate[1]	= 4;	 // 2 mbps, in units of 0.5 Mbps
			pAd->PortCfg.DesireRate[2]	= 11;	 // 5.5 mbps, in units of 0.5 Mbps
			pAd->PortCfg.DesireRate[3]	= 22;	 // 11 mbps, in units of 0.5 Mbps
			break;
			
		case PHY_11G:
		case PHY_11BG_MIXED:
		case PHY_11ABG_MIXED:
			pAd->PortCfg.SupRate[0]  = 0x82;	// 1 mbps, in units of 0.5 Mbps, basic rate
			pAd->PortCfg.SupRate[1]  = 0x84;	// 2 mbps, in units of 0.5 Mbps, basic rate
			pAd->PortCfg.SupRate[2]  = 0x8B;	// 5.5 mbps, in units of 0.5 Mbps, basic rate
			pAd->PortCfg.SupRate[3]  = 0x96;	// 11 mbps, in units of 0.5 Mbps, basic rate
			pAd->PortCfg.SupRate[4]  = 0x12;	// 9 mbps, in units of 0.5 Mbps
			pAd->PortCfg.SupRate[5]  = 0x24;	// 18 mbps, in units of 0.5 Mbps
			pAd->PortCfg.SupRate[6]  = 0x48;	// 36 mbps, in units of 0.5 Mbps
			pAd->PortCfg.SupRate[7]  = 0x6c;	// 54 mbps, in units of 0.5 Mbps
			pAd->PortCfg.SupRateLen  = 8;
			pAd->PortCfg.ExtRate[0]  = 0x8C;	// 6 mbps, in units of 0.5 Mbps, basic rate
			pAd->PortCfg.ExtRate[1]  = 0x98;	// 12 mbps, in units of 0.5 Mbps, basic rate
			pAd->PortCfg.ExtRate[2]  = 0xb0;	// 24 mbps, in units of 0.5 Mbps, basic rate
			pAd->PortCfg.ExtRate[3]  = 0x60;	// 48 mbps, in units of 0.5 Mbps
			pAd->PortCfg.ExtRateLen  = 4;
			pAd->PortCfg.DesireRate[0]	= 2;	 // 1 mbps, in units of 0.5 Mbps
			pAd->PortCfg.DesireRate[1]	= 4;	 // 2 mbps, in units of 0.5 Mbps
			pAd->PortCfg.DesireRate[2]	= 11;	 // 5.5 mbps, in units of 0.5 Mbps
			pAd->PortCfg.DesireRate[3]	= 22;	 // 11 mbps, in units of 0.5 Mbps
			pAd->PortCfg.DesireRate[4]	= 12;	 // 6 mbps, in units of 0.5 Mbps
			pAd->PortCfg.DesireRate[5]	= 18;	 // 9 mbps, in units of 0.5 Mbps
			pAd->PortCfg.DesireRate[6]	= 24;	 // 12 mbps, in units of 0.5 Mbps
			pAd->PortCfg.DesireRate[7]	= 36;	 // 18 mbps, in units of 0.5 Mbps
			pAd->PortCfg.DesireRate[8]	= 48;	 // 24 mbps, in units of 0.5 Mbps
			pAd->PortCfg.DesireRate[9]	= 72;	 // 36 mbps, in units of 0.5 Mbps
			pAd->PortCfg.DesireRate[10] = 96;	 // 48 mbps, in units of 0.5 Mbps
			pAd->PortCfg.DesireRate[11] = 108;	 // 54 mbps, in units of 0.5 Mbps
			break;
			
		case PHY_11A:
			pAd->PortCfg.SupRate[0]  = 0x8C;	// 6 mbps, in units of 0.5 Mbps, basic rate
			pAd->PortCfg.SupRate[1]  = 0x12;	// 9 mbps, in units of 0.5 Mbps
			pAd->PortCfg.SupRate[2]  = 0x98;	// 12 mbps, in units of 0.5 Mbps, basic rate
			pAd->PortCfg.SupRate[3]  = 0x24;	// 18 mbps, in units of 0.5 Mbps
			pAd->PortCfg.SupRate[4]  = 0xb0;	// 24 mbps, in units of 0.5 Mbps, basic rate
			pAd->PortCfg.SupRate[5]  = 0x48;	// 36 mbps, in units of 0.5 Mbps
			pAd->PortCfg.SupRate[6]  = 0x60;	// 48 mbps, in units of 0.5 Mbps
			pAd->PortCfg.SupRate[7]  = 0x6c;	// 54 mbps, in units of 0.5 Mbps
			pAd->PortCfg.SupRateLen  = 8;
			pAd->PortCfg.ExtRateLen  = 0;
			pAd->PortCfg.DesireRate[0]	= 12;	 // 6 mbps, in units of 0.5 Mbps
			pAd->PortCfg.DesireRate[1]	= 18;	 // 9 mbps, in units of 0.5 Mbps
			pAd->PortCfg.DesireRate[2]	= 24;	 // 12 mbps, in units of 0.5 Mbps
			pAd->PortCfg.DesireRate[3]	= 36;	 // 18 mbps, in units of 0.5 Mbps
			pAd->PortCfg.DesireRate[4]	= 48;	 // 24 mbps, in units of 0.5 Mbps
			pAd->PortCfg.DesireRate[5]	= 72;	 // 36 mbps, in units of 0.5 Mbps
			pAd->PortCfg.DesireRate[6]	= 96;	 // 48 mbps, in units of 0.5 Mbps
			pAd->PortCfg.DesireRate[7]	= 108;	 // 54 mbps, in units of 0.5 Mbps
			break;
			
		default:
			break;
	}
	
	MlmeUpdateTxRates(pAd, FALSE);
	AsicSetSlotTime(pAd, (BOOLEAN)pAd->PortCfg.UseShortSlotTime);

	pAd->PortCfg.BandState = UNKNOWN_BAND;	
//  MakeIbssBeacon(pAd);	  // supported rates may change
}

VOID	RTMPSetDesiredRates(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	LONG			Rates)
{
	NDIS_802_11_RATES aryRates;

	memset(&aryRates, 0x00, sizeof(NDIS_802_11_RATES));
	switch (pAdapter->PortCfg.PhyMode)
	{
		case PHY_11A: // A only
			switch (Rates)
			{
				case 6000000: //6M
					aryRates[0] = 0x0c; // 6M
					break;
				case 9000000: //9M
					aryRates[0] = 0x12; // 9M
					break;
				case 12000000: //12M
					aryRates[0] = 0x18; // 12M
					break;
				case 18000000: //18M
					aryRates[0] = 0x24; // 18M
					break;
				case 24000000: //24M
					aryRates[0] = 0x30; // 24M
					break;
				case 36000000: //36M
					aryRates[0] = 0x48; // 36M
					break;
				case 48000000: //48M
					aryRates[0] = 0x60; // 48M
					break;
				case 54000000: //54M
					aryRates[0] = 0x6c; // 54M
					break;
				case -1: //Auto
				default:
					aryRates[0] = 0x6c; // 54Mbps
					aryRates[1] = 0x60; // 48Mbps
					aryRates[2] = 0x48; // 36Mbps
					aryRates[3] = 0x30; // 24Mbps
					aryRates[4] = 0x24; // 18M
					aryRates[5] = 0x18; // 12M
					aryRates[6] = 0x12; // 9M
					aryRates[7] = 0x0c; // 6M
					break;
			}
			break;
		case PHY_11BG_MIXED: // B/G Mixed
		case PHY_11B: // B only
		case PHY_11ABG_MIXED: // A/B/G Mixed
		default:
			switch (Rates)
			{
				case 1000000: //1M
					aryRates[0] = 0x02;
					break;
				case 2000000: //2M
					aryRates[0] = 0x04;
					break;
				case 5000000: //5.5M
					aryRates[0] = 0x0b; // 5.5M
					break;
				case 11000000: //11M
					aryRates[0] = 0x16; // 11M
					break;
				case 6000000: //6M
					aryRates[0] = 0x0c; // 6M
					break;
				case 9000000: //9M
					aryRates[0] = 0x12; // 9M
					break;
				case 12000000: //12M
					aryRates[0] = 0x18; // 12M
					break;
				case 18000000: //18M
					aryRates[0] = 0x24; // 18M
					break;
				case 24000000: //24M
					aryRates[0] = 0x30; // 24M
					break;
				case 36000000: //36M
					aryRates[0] = 0x48; // 36M
					break;
				case 48000000: //48M
					aryRates[0] = 0x60; // 48M
					break;
				case 54000000: //54M
					aryRates[0] = 0x6c; // 54M
					break;
				case -1: //Auto
				default:
					if (pAdapter->PortCfg.PhyMode == PHY_11B)
					{ //B Only
						aryRates[0] = 0x16; // 11Mbps
						aryRates[1] = 0x0b; // 5.5Mbps
						aryRates[2] = 0x04; // 2Mbps
						aryRates[3] = 0x02; // 1Mbps
					}
					else
					{ //(B/G) Mixed or (A/B/G) Mixed
						aryRates[0] = 0x6c; // 54Mbps
						aryRates[1] = 0x60; // 48Mbps
						aryRates[2] = 0x48; // 36Mbps
						aryRates[3] = 0x30; // 24Mbps
						aryRates[4] = 0x16; // 11Mbps
						aryRates[5] = 0x0b; // 5.5Mbps
						aryRates[6] = 0x04; // 2Mbps
						aryRates[7] = 0x02; // 1Mbps
					}
					break;
			}
			break;
	}

	NdisZeroMemory(pAdapter->PortCfg.DesireRate, MAX_LEN_OF_SUPPORTED_RATES);
	NdisMoveMemory(pAdapter->PortCfg.DesireRate, &aryRates, sizeof(NDIS_802_11_RATES));
	DBGPRINT(RT_DEBUG_TRACE, " RTMPSetDesiredRates (%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x)\n",
		pAdapter->PortCfg.DesireRate[0],pAdapter->PortCfg.DesireRate[1],
		pAdapter->PortCfg.DesireRate[2],pAdapter->PortCfg.DesireRate[3],
		pAdapter->PortCfg.DesireRate[4],pAdapter->PortCfg.DesireRate[5],
		pAdapter->PortCfg.DesireRate[6],pAdapter->PortCfg.DesireRate[7] );
	// Changing DesiredRate may affect the MAX TX rate we used to TX frames out
	MlmeUpdateTxRates(pAdapter, FALSE);
}

INT Set_DriverVersion_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg)
{
	DBGPRINT(RT_DEBUG_TRACE, "Driver version-%s\n", DRIVER_VERSION);
	
	return TRUE;
}

/* 
	==========================================================================
	Description:
		Set Country Region.
		This command will not work, if the field of CountryRegion in eeprom is programmed.
	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT Set_CountryRegion_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg)
{
	ULONG region;
		
	region = simple_strtol(arg, 0, 10);

	// Country can be set only when EEPROM not programmed
	if (pAd->PortCfg.CountryRegion & 0x80)
	{
		DBGPRINT(RT_DEBUG_ERROR, "Set_CountryRegion_Proc::parameter of CountryRegion in eeprom is programmed \n");
		return FALSE;
	}
	
	if(region <= REGION_MAXIMUM_BG_BAND)
	{
		pAd->PortCfg.CountryRegion = (UCHAR) region;
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, "Set_CountryRegion_Proc::parameters out of range\n");
		return FALSE;
	}

	// if set country region, driver needs to be reset
	BuildChannelList(pAd);

	DBGPRINT(RT_DEBUG_TRACE, "Set_CountryRegion_Proc::(CountryRegion=%d)\n", pAd->PortCfg.CountryRegion);

	return TRUE;
}

/* 
	==========================================================================
	Description:
		Set Country Region for A band.
		This command will not work, if the field of CountryRegion in eeprom is programmed.
	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT Set_CountryRegionABand_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg)
{
	ULONG region;
		
	region = simple_strtol(arg, 0, 10);

	// Country can be set only when EEPROM not programmed
	if (pAd->PortCfg.CountryRegionForABand & 0x80)
	{
		DBGPRINT(RT_DEBUG_ERROR, "Set_CountryRegionABand_Proc::parameter of CountryRegion in eeprom is programmed \n");
		return FALSE;
	}
	
	if(region <= REGION_MAXIMUM_A_BAND)
	{
		pAd->PortCfg.CountryRegionForABand = (UCHAR) region;
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, "Set_CountryRegionABand_Proc::parameters out of range\n");
		return FALSE;
	}

	// if set country region, driver needs to be reset
	BuildChannelList(pAd);

	DBGPRINT(RT_DEBUG_TRACE, "Set_CountryRegionABand_Proc::(CountryRegion=%d)\n", pAd->PortCfg.CountryRegionForABand);

	return TRUE;
}

/* 
	==========================================================================
	Description:
		Set SSID
	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT Set_SSID_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)
{
	NDIS_802_11_SSID					Ssid, *pSsid=NULL;
	BOOLEAN 							StateMachineTouched = FALSE;
	int 								success = TRUE;

	if( strlen(arg) <= MAX_LEN_OF_SSID)
	{

		NdisZeroMemory(&Ssid, MAX_LEN_OF_SSID);
		if (strlen(arg) != 0)
		{
			NdisMoveMemory(Ssid.Ssid, arg, strlen(arg));
			Ssid.SsidLength = strlen(arg);
		}
		else
		{
			Ssid.SsidLength = 0; //ANY ssid
            memcpy(Ssid.Ssid, "", 0);		    
		}
		
		pSsid = &Ssid;

		if (pAdapter->Mlme.CntlMachine.CurrState != CNTL_IDLE)
		{
			MlmeEnqueue(pAdapter, 
                        MLME_CNTL_STATE_MACHINE,
                        RT_CMD_RESET_MLME,
                        0,
                        NULL);	
		}

		// To prevent some kernel thread is very low priority ...so essid copy immediately for later wpapsk counting.
		if (pAdapter->PortCfg.AuthMode >= Ndis802_11AuthModeWPA)
		{
			NdisMoveMemory(pAdapter->MlmeAux.Ssid, Ssid.Ssid, Ssid.SsidLength);
			pAdapter->MlmeAux.SsidLen = Ssid.SsidLength;
		}
		
		MlmeEnqueue(pAdapter, 
					MLME_CNTL_STATE_MACHINE, 
					OID_802_11_SSID,
					sizeof(NDIS_802_11_SSID),
					(VOID *)pSsid);

		StateMachineTouched = TRUE;
		DBGPRINT(RT_DEBUG_TRACE, "Set_SSID_Proc::(Len=%d,Ssid=%s)\n", Ssid.SsidLength, Ssid.Ssid);
	}
	else
		success = FALSE;

	if (StateMachineTouched) // Upper layer sent a MLME-related operations
		RTUSBMlmeUp(pAdapter);

	return success;
}

/* 
	==========================================================================
	Description:
		Set Wireless Mode
	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT Set_WirelessMode_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)
{
	ULONG								WirelessMode;
	int 								success = TRUE;

	WirelessMode = simple_strtol(arg, 0, 10);

	if ((WirelessMode == PHY_11BG_MIXED) || (WirelessMode == PHY_11B) ||
		(WirelessMode == PHY_11A) || (WirelessMode == PHY_11ABG_MIXED) ||
		(WirelessMode == PHY_11G))
	{
		// protect no A-band support
		if ((pAdapter->RfIcType != RFIC_5226) && (pAdapter->RfIcType != RFIC_5225))
		{
			if ((WirelessMode == PHY_11A) || (WirelessMode == PHY_11ABG_MIXED))
			{
				DBGPRINT_ERR("!!!!! Not support A band in RfIcType= %d\n", pAdapter->RfIcType);
				return FALSE;
			}	 
		}
	
		RTMPSetPhyMode(pAdapter, WirelessMode);
	  
		DBGPRINT(RT_DEBUG_TRACE, "Set_WirelessMode_Proc::(=%d)\n", WirelessMode);
	
		return success;
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, "Set_WirelessMode_Proc::parameters out of range\n");
		return FALSE;
	}
}

/* 
	==========================================================================
	Description:
		Set TxRate
	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT Set_TxRate_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)
{
	ULONG								TxRate;
	int 								success = TRUE;
   
	TxRate = simple_strtol(arg, 0, 10);

	if ((pAdapter->PortCfg.PhyMode == PHY_11B && TxRate <= 4) ||\
		((pAdapter->PortCfg.PhyMode == PHY_11BG_MIXED || pAdapter->PortCfg.PhyMode == PHY_11ABG_MIXED) && TxRate <= 12) ||\
		((pAdapter->PortCfg.PhyMode == PHY_11A || pAdapter->PortCfg.PhyMode == PHY_11G) && (TxRate == 0 ||(TxRate > 4 && TxRate <= 12))))
	{	 
		if (TxRate == 0)
			RTMPSetDesiredRates(pAdapter, -1);
		else
			RTMPSetDesiredRates(pAdapter, (LONG) (RateIdToMbps[TxRate-1] * 1000000));

		DBGPRINT(RT_DEBUG_TRACE, "Set_TxRate_Proc::(TxRate=%d)\n", TxRate);

		return success;
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, "Set_TxRate_Proc::parameters out of range\n");
		return FALSE;	//Invalid argument 
	} 
}

/* 
	==========================================================================
	Description:
		Set Channel
	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT Set_Channel_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)
{
	int 								success = TRUE;
	UCHAR								Channel;

	Channel = (UCHAR) simple_strtol(arg, 0, 10);

	if (ChannelSanity(pAdapter, Channel) == TRUE)
	{
		pAdapter->PortCfg.Channel = Channel;

        if (pAdapter->PortCfg.BssType == BSS_ADHOC)
			pAdapter->PortCfg.AdhocChannel = pAdapter->PortCfg.Channel; // sync. to the value of PortCfg.Channel

		DBGPRINT(RT_DEBUG_TRACE, "Set_Channel_Proc::(Channel=%d)\n", Channel);
	}
	else
		success = FALSE;

	return success;
}

#ifdef DBG
/* 
	==========================================================================
	Description:
		For Debug information
	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT	Set_Debug_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg)
{
	DBGPRINT(RT_DEBUG_TRACE, "==> Set_Debug_Proc *******************\n");

	if(simple_strtol(arg, 0, 10) <= RT_DEBUG_LOUD)
		RTDebugLevel = simple_strtol(arg, 0, 10);	

	DBGPRINT(RT_DEBUG_TRACE, "<== Set_Debug_Proc(RTDebugLevel = %d)\n", RTDebugLevel);

	return TRUE;
}
#endif

/* 
	==========================================================================
	Description:
		Set 11B/11G Protection
	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT Set_BGProtection_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)

{
	switch (simple_strtol(arg, 0, 10))
	{
		case 0: //AUTO
			pAdapter->PortCfg.UseBGProtection = 0;
			break;
		case 1: //Always On
			pAdapter->PortCfg.UseBGProtection = 1;
			break;
		case 2: //Always OFF
			pAdapter->PortCfg.UseBGProtection = 2;
			break;		
		default:  //Invalid argument 
			return FALSE;
	}
	DBGPRINT(RT_DEBUG_TRACE, "Set_BGProtection_Proc::(BGProtection=%d)\n", pAdapter->PortCfg.UseBGProtection);

	return TRUE;
}

/* 
	==========================================================================
	Description:
		Set TxPreamble
	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT Set_TxPreamble_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)
{
	RT_802_11_PREAMBLE					Preamble;

	Preamble = simple_strtol(arg, 0, 10);
	switch (Preamble)
	{
		case Rt802_11PreambleShort:
			pAdapter->PortCfg.TxPreamble = Preamble;
			MlmeSetTxPreamble(pAdapter, Rt802_11PreambleShort);
			break;
		case Rt802_11PreambleLong:
		case Rt802_11PreambleAuto:
			// if user wants AUTO, initialize to LONG here, then change according to AP's
			// capability upon association.
			pAdapter->PortCfg.TxPreamble = Preamble;
			MlmeSetTxPreamble(pAdapter, Rt802_11PreambleLong);
			break;
		default: //Invalid argument 
			return FALSE;
	}

	DBGPRINT(RT_DEBUG_TRACE, "Set_TxPreamble_Proc::(TxPreamble=%d)\n", Preamble);

	return TRUE;
}

/* 
	==========================================================================
	Description:
		Set RTS Threshold
	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT Set_RTSThreshold_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)
{
	NDIS_802_11_RTS_THRESHOLD			RtsThresh;

	RtsThresh = simple_strtol(arg, 0, 10);

	if((RtsThresh > 0) && (RtsThresh <= MAX_RTS_THRESHOLD))
		pAdapter->PortCfg.RtsThreshold = (USHORT)RtsThresh;
	else if (RtsThresh == 0)
		pAdapter->PortCfg.RtsThreshold = MAX_RTS_THRESHOLD;
	else
		return FALSE;

	DBGPRINT(RT_DEBUG_TRACE, "Set_RTSThreshold_Proc::(RTSThreshold=%d)\n", pAdapter->PortCfg.RtsThreshold);
	return TRUE;
}

/* 
	==========================================================================
	Description:
		Set Fragment Threshold
	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT Set_FragThreshold_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)
{
	NDIS_802_11_FRAGMENTATION_THRESHOLD 	FragThresh;

	FragThresh = simple_strtol(arg, 0, 10);

	if ( (FragThresh >= MIN_FRAG_THRESHOLD) && (FragThresh <= MAX_FRAG_THRESHOLD))
		pAdapter->PortCfg.FragmentThreshold = (USHORT)FragThresh;
	else if (FragThresh == 0)
		pAdapter->PortCfg.FragmentThreshold = MAX_FRAG_THRESHOLD;
	else
		return FALSE; //Invalid argument 

	if (pAdapter->PortCfg.FragmentThreshold == MAX_FRAG_THRESHOLD)
		pAdapter->PortCfg.bFragmentZeroDisable = TRUE;
	else
		pAdapter->PortCfg.bFragmentZeroDisable = FALSE;

	DBGPRINT(RT_DEBUG_TRACE, "Set_FragThreshold_Proc::(FragThreshold=%d)\n", FragThresh);

	return TRUE;
}

/* 
	==========================================================================
	Description:
		Set TxBurst
	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT Set_TxBurst_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)
{
	ULONG								TxBurst;

	TxBurst = simple_strtol(arg, 0, 10);

	if (TxBurst == 1)
		pAdapter->PortCfg.bEnableTxBurst = TRUE;
	else if (TxBurst == 0)
		pAdapter->PortCfg.bEnableTxBurst = FALSE;
	else
		return FALSE;  //Invalid argument 
	
	DBGPRINT(RT_DEBUG_TRACE, "Set_TxBurst_Proc::(TxBurst=%d)\n", pAdapter->PortCfg.bEnableTxBurst);

	return TRUE;
}

/* 
	==========================================================================
	Description:
		Set AdhocMode support Rate can or can not exceed 11Mbps against WiFi spec.
	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT Set_AdhocModeRate_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)
{
	ULONG		AdhocMode;

	AdhocMode = (ULONG)simple_strtol(arg, 0, 10);

	if (AdhocMode > 4)
		return FALSE;  //Invalid argument 
		
	if (pAdapter->PortCfg.AdhocMode != AdhocMode)
	{
		pAdapter->PortCfg.AdhocMode = AdhocMode;
		MlmeUpdateTxRates(pAdapter, FALSE);
		MakeIbssBeacon(pAdapter);			// re-build BEACON frame
		AsicEnableIbssSync(pAdapter);		// copy to on-chip memory
	}
	
	DBGPRINT(RT_DEBUG_TRACE, "Set_AdhocModeRate_Proc::(AdhocMode=%d)\n", pAdapter->PortCfg.AdhocMode);
	
	return TRUE;
}
#ifdef AGGREGATION_SUPPORT
/* 
	==========================================================================
	Description:
		Set TxBurst
	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT	Set_PktAggregate_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg)
{
	ULONG aggre;

	aggre = simple_strtol(arg, 0, 10);

	if (aggre == 1)
		pAd->PortCfg.bAggregationCapable = TRUE;
	else if (aggre == 0)
		pAd->PortCfg.bAggregationCapable = FALSE;
	else
		return FALSE;  //Invalid argument 
	
	DBGPRINT(RT_DEBUG_TRACE, "Set_PktAggregate_Proc::(AGGRE=%d)\n", pAd->PortCfg.bAggregationCapable);

	return TRUE;
}
#endif

/* 
	==========================================================================
	Description:
		Set TurboRate Enable or Disable
	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT Set_TurboRate_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)
{
	ULONG								TurboRate;

	TurboRate = simple_strtol(arg, 0, 10);

	if (TurboRate == 1)
		pAdapter->PortCfg.EnableTurboRate = TRUE;
	else if (TurboRate == 0)
		pAdapter->PortCfg.EnableTurboRate = FALSE;
	else
		return FALSE;  //Invalid argument 
	
	DBGPRINT(RT_DEBUG_TRACE, "Set_TurboRate_Proc::(TurboRate=%d)\n", pAdapter->PortCfg.EnableTurboRate);

	return TRUE;
}
#if 0
/* 
	==========================================================================
	Description:
		Set WmmCapable Enable or Disable
	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT	Set_WmmCapable_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg)
{
	BOOLEAN	bWmmCapable;

	bWmmCapable = simple_strtol(arg, 0, 10);

	if (bWmmCapable == 1)
		pAd->PortCfg.bWmmCapable = TRUE;
	else if (bWmmCapable == 0)
		pAd->PortCfg.bWmmCapable = FALSE;
	else
		return FALSE;  //Invalid argument 
	
	DBGPRINT(RT_DEBUG_TRACE, "Set_WmmCapable_Proc::(bWmmCapable=%d)\n", pAd->PortCfg.bWmmCapable);

	return TRUE;
}
#endif
/* 
	==========================================================================
	Description:
		Set IEEE80211H.
		This parameter is 1 when needs radar detection, otherwise 0
	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT	Set_IEEE80211H_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg)
{
	ULONG ieee80211h;

	ieee80211h = simple_strtol(arg, 0, 10);

	if (ieee80211h == 1)
		pAd->PortCfg.bIEEE80211H = TRUE;
	else if (ieee80211h == 0)
		pAd->PortCfg.bIEEE80211H = FALSE;
	else
		return FALSE;  //Invalid argument 
	
	DBGPRINT(RT_DEBUG_TRACE, "Set_IEEE80211H_Proc::(IEEE80211H=%d)\n", pAd->PortCfg.bIEEE80211H);

	return TRUE;
}

/* 
	==========================================================================
	Description:
		Set Network Type(Infrastructure/Adhoc mode)
	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT Set_NetworkType_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)
{
	if (strcmp(arg, "Adhoc") == 0)
	{
		if (pAdapter->PortCfg.BssType != BSS_ADHOC)	
		{
			// Config has changed
			if (INFRA_ON(pAdapter))
				RTUSBEnqueueInternalCmd(pAdapter, RT_OID_LINK_DOWN);
		}		
		pAdapter->PortCfg.BssType = BSS_ADHOC;	  
	}
	else
	{
		if (pAdapter->PortCfg.BssType != BSS_INFRA)	
		{
			// Config has changed
			if (ADHOC_ON(pAdapter)) 						  
				RTUSBEnqueueInternalCmd(pAdapter, RT_OID_LINK_DOWN);
		}
		pAdapter->PortCfg.BssType = BSS_INFRA;
	}

	// Reset Ralink supplicant to not use, it will be set to start when UI set PMK key
	pAdapter->PortCfg.WpaState = SS_NOTUSE;

	DBGPRINT(RT_DEBUG_TRACE, "Set_NetworkType_Proc::(NetworkType=%d)\n", pAdapter->PortCfg.BssType);

	return TRUE;
}

/* 
	==========================================================================
	Description:
		Set Authentication mode
	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT Set_AuthMode_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)
{
	if ((strcmp(arg, "WEPAUTO") == 0) || (strcmp(arg, "wepauto") == 0))
		pAdapter->PortCfg.AuthMode = Ndis802_11AuthModeAutoSwitch;
	else if ((strcmp(arg, "OPEN") == 0) || (strcmp(arg, "open") == 0))
		pAdapter->PortCfg.AuthMode = Ndis802_11AuthModeOpen;
	else if ((strcmp(arg, "SHARED") == 0) || (strcmp(arg, "shared") == 0))
		pAdapter->PortCfg.AuthMode = Ndis802_11AuthModeShared;
	else if ((strcmp(arg, "WPAPSK") == 0) || (strcmp(arg, "wpapsk") == 0))
		pAdapter->PortCfg.AuthMode = Ndis802_11AuthModeWPAPSK;
	else if ((strcmp(arg, "WPANONE") == 0) || (strcmp(arg, "wpanone") == 0))
		pAdapter->PortCfg.AuthMode = Ndis802_11AuthModeWPANone;
	else if ((strcmp(arg, "WPA2PSK") == 0) || (strcmp(arg, "wpa2psk") == 0))
		pAdapter->PortCfg.AuthMode = Ndis802_11AuthModeWPA2PSK;    
    else if ((strcmp(arg, "WPA") == 0) || (strcmp(arg, "wpa") == 0))
        pAdapter->PortCfg.AuthMode = Ndis802_11AuthModeWPA;    
	else
		return FALSE;  

	pAdapter->PortCfg.PortSecured = WPA_802_1X_PORT_NOT_SECURED;

	DBGPRINT(RT_DEBUG_TRACE, "Set_AuthMode_Proc::(AuthMode=%d)\n", pAdapter->PortCfg.AuthMode);

	return TRUE;
}

/* 
	==========================================================================
	Description:
		Set Encryption Type
	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT Set_EncrypType_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)
{
	if ((strcmp(arg, "NONE") == 0) || (strcmp(arg, "none") == 0))
	{			 
		pAdapter->PortCfg.WepStatus   = Ndis802_11WEPDisabled;
		pAdapter->PortCfg.PairCipher  = Ndis802_11WEPDisabled;
		pAdapter->PortCfg.GroupCipher = Ndis802_11WEPDisabled;
	}
	else if ((strcmp(arg, "WEP") == 0) || (strcmp(arg, "wep") == 0))
	{			 
		pAdapter->PortCfg.WepStatus   = Ndis802_11WEPEnabled;
		pAdapter->PortCfg.PairCipher  = Ndis802_11WEPEnabled;
		pAdapter->PortCfg.GroupCipher = Ndis802_11WEPEnabled;
	}
	else if ((strcmp(arg, "TKIP") == 0) || (strcmp(arg, "tkip") == 0))
	{			 
		pAdapter->PortCfg.WepStatus   = Ndis802_11Encryption2Enabled;
		pAdapter->PortCfg.PairCipher  = Ndis802_11Encryption2Enabled;
		pAdapter->PortCfg.GroupCipher = Ndis802_11Encryption2Enabled;
	}
	else if ((strcmp(arg, "AES") == 0) || (strcmp(arg, "aes") == 0))
	{			 
		pAdapter->PortCfg.WepStatus   = Ndis802_11Encryption3Enabled;
		pAdapter->PortCfg.PairCipher  = Ndis802_11Encryption3Enabled;
		pAdapter->PortCfg.GroupCipher = Ndis802_11Encryption3Enabled;
	}
	else
		return FALSE;

	RTMPMakeRSNIE(pAdapter, pAdapter->PortCfg.GroupCipher);
//	RTUSBEnqueueCmdFromNdis(pAdapter, OID_802_11_WEP_STATUS, TRUE, &(pAdapter->PortCfg.WepStatus), 
//							sizeof(pAdapter->PortCfg.WepStatus));

	DBGPRINT(RT_DEBUG_TRACE, "Set_EncrypType_Proc::(EncrypType=%d)\n", pAdapter->PortCfg.WepStatus);

	return TRUE;
}

/* 
	==========================================================================
	Description:
		Set Default Key ID
	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT Set_DefaultKeyID_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)
{
	ULONG								KeyIdx;

	KeyIdx = simple_strtol(arg, 0, 10);
	if((KeyIdx >= 1 ) && (KeyIdx <= 4))
		pAdapter->PortCfg.DefaultKeyId = (UCHAR) (KeyIdx - 1 );
	else
		return FALSE;  //Invalid argument 

	DBGPRINT(RT_DEBUG_TRACE, "Set_DefaultKeyID_Proc::(DefaultKeyID=%d)\n", pAdapter->PortCfg.DefaultKeyId);

	return TRUE;
}

/* 
	==========================================================================
	Description:
		Set WEP KEY1
	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT Set_Key1_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)
{
	int 								KeyLen;
	int 								i;
	UCHAR								CipherAlg=CIPHER_WEP64;
	

	if (pAdapter->PortCfg.WepStatus != Ndis802_11WEPEnabled ||\
		pAdapter->PortCfg.DefaultKeyId != 0)
		return TRUE;	// do nothing
	
	KeyLen = strlen(arg);

	switch (KeyLen)
	{
		case 5: //wep 40 Ascii type
			pAdapter->SharedKey[0].KeyLen = KeyLen;
			memcpy(pAdapter->SharedKey[0].Key, arg, KeyLen);
			CipherAlg = CIPHER_WEP64;
			DBGPRINT(RT_DEBUG_TRACE, "Set_Key1_Proc::(Key1=%s and type=%s)\n", arg, "Ascii");		
			break;
		case 10: //wep 40 Hex type
			for(i=0; i < KeyLen; i++)
			{
				if( !isxdigit(*(arg+i)) )
					return FALSE;  //Not Hex value;
			}
			pAdapter->SharedKey[0].KeyLen = KeyLen / 2 ;
			AtoH(arg, pAdapter->SharedKey[0].Key, KeyLen / 2);
			CipherAlg = CIPHER_WEP64;
			DBGPRINT(RT_DEBUG_TRACE, "Set_Key1_Proc::(Key1=%s and type=%s)\n", arg, "Hex"); 	
			break;
		case 13: //wep 104 Ascii type
			pAdapter->SharedKey[0].KeyLen = KeyLen;
			memcpy(pAdapter->SharedKey[0].Key, arg, KeyLen);
			CipherAlg = CIPHER_WEP128;
			DBGPRINT(RT_DEBUG_TRACE, "Set_Key1_Proc::(Key1=%s and type=%s)\n", arg, "Ascii");		
			break;
		case 26: //wep 104 Hex type
			for(i=0; i < KeyLen; i++)
			{
				if( !isxdigit(*(arg+i)) )
					return FALSE;  //Not Hex value;
			}
			pAdapter->SharedKey[0].KeyLen = KeyLen / 2 ;
			AtoH(arg, pAdapter->SharedKey[0].Key, KeyLen / 2);
			CipherAlg = CIPHER_WEP128;
			DBGPRINT(RT_DEBUG_TRACE, "Set_Key1_Proc::(Key1=%s and type=%s)\n", arg, "Hex"); 	
			break;
		default: //Invalid argument 
			DBGPRINT(RT_DEBUG_TRACE, "Set_Key1_Proc::Invalid argument (=%s)\n", arg);		
			return FALSE;
	}
	
	pAdapter->SharedKey[0].CipherAlg = CipherAlg;

	// Set keys (into ASIC)
	if (pAdapter->PortCfg.AuthMode >= Ndis802_11AuthModeWPA)
		;	// not support
	else	// Old WEP stuff
	{
		AsicAddSharedKeyEntry(pAdapter, 
							  0, 
							  0, 
							  pAdapter->SharedKey[0].CipherAlg, 
							  pAdapter->SharedKey[0].Key, 
							  NULL,
							  NULL);
	}	 
	
	return TRUE;
}

/* 
	==========================================================================
	Description:
		Set WEP KEY2
	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT Set_Key2_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)
{
	int 								KeyLen;
	int 								i;
	UCHAR								CipherAlg=CIPHER_WEP64;
	
	if (pAdapter->PortCfg.WepStatus != Ndis802_11WEPEnabled ||\
		pAdapter->PortCfg.DefaultKeyId != 1)
		return TRUE;	// do nothing
	
	KeyLen = strlen(arg);

	switch (KeyLen)
	{
		case 5: //wep 40 Ascii type
			pAdapter->SharedKey[1].KeyLen = KeyLen;
			memcpy(pAdapter->SharedKey[1].Key, arg, KeyLen);
			CipherAlg = CIPHER_WEP64;
			DBGPRINT(RT_DEBUG_TRACE, "Set_Key2_Proc::(Key2=%s and type=%s)\n", arg, "Ascii");
			break;
		case 10: //wep 40 Hex type
			for(i=0; i < KeyLen; i++)
			{
				if( !isxdigit(*(arg+i)) )
					return FALSE;  //Not Hex value;
			}
			pAdapter->SharedKey[1].KeyLen = KeyLen / 2 ;
			AtoH(arg, pAdapter->SharedKey[1].Key, KeyLen / 2);
			CipherAlg = CIPHER_WEP64;
			DBGPRINT(RT_DEBUG_TRACE, "Set_Key2_Proc::(Key2=%s and type=%s)\n", arg, "Hex");
			break;
		case 13: //wep 104 Ascii type
			pAdapter->SharedKey[1].KeyLen = KeyLen;
			memcpy(pAdapter->SharedKey[1].Key, arg, KeyLen);
			CipherAlg = CIPHER_WEP128;
			DBGPRINT(RT_DEBUG_TRACE, "Set_Key2_Proc::(Key2=%s and type=%s)\n", arg, "Ascii");
			break;
		case 26: //wep 104 Hex type
			for(i=0; i < KeyLen; i++)
			{
				if( !isxdigit(*(arg+i)) )
					return FALSE;  //Not Hex value;
			}
			pAdapter->SharedKey[1].KeyLen = KeyLen / 2 ;
			AtoH(arg, pAdapter->SharedKey[1].Key, KeyLen / 2);
			CipherAlg = CIPHER_WEP128;
			DBGPRINT(RT_DEBUG_TRACE, "Set_Key2_Proc::(Key2=%s and type=%s)\n", arg, "Hex");
			break;
		default: //Invalid argument 
			DBGPRINT(RT_DEBUG_TRACE, "Set_Key2_Proc::Invalid argument (=%s)\n", arg);
			return FALSE;
	}
	pAdapter->SharedKey[1].CipherAlg = CipherAlg;

	// Set keys (into ASIC)
	if (pAdapter->PortCfg.AuthMode >= Ndis802_11AuthModeWPA)
		;	// not support
	else	// Old WEP stuff
	{
		AsicAddSharedKeyEntry(pAdapter, 
							  0, 
							  1, 
							  pAdapter->SharedKey[1].CipherAlg, 
							  pAdapter->SharedKey[1].Key, 
							  NULL,
							  NULL);
	}		 
	
	return TRUE;
}

/* 
	==========================================================================
	Description:
		Set WEP KEY3
	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT Set_Key3_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)
{
	int 								KeyLen;
	int 								i;
	UCHAR								CipherAlg=CIPHER_WEP64;
	
	if (pAdapter->PortCfg.WepStatus != Ndis802_11WEPEnabled ||\
		pAdapter->PortCfg.DefaultKeyId != 2)
		return TRUE;	// do nothing
	
	KeyLen = strlen(arg);

	switch (KeyLen)
	{
		case 5: //wep 40 Ascii type
			pAdapter->SharedKey[2].KeyLen = KeyLen;
			memcpy(pAdapter->SharedKey[2].Key, arg, KeyLen);
			CipherAlg = CIPHER_WEP64;
			DBGPRINT(RT_DEBUG_TRACE, "Set_Key3_Proc::(Key3=%s and type=%s)\n", arg, "Ascii");
			break;
		case 10: //wep 40 Hex type
			for(i=0; i < KeyLen; i++)
			{
				if( !isxdigit(*(arg+i)) )
					return FALSE;  //Not Hex value;
			}
			pAdapter->SharedKey[2].KeyLen = KeyLen / 2 ;
			AtoH(arg, pAdapter->SharedKey[2].Key, KeyLen / 2);
			CipherAlg = CIPHER_WEP64;
			DBGPRINT(RT_DEBUG_TRACE, "Set_Key3_Proc::(Key3=%s and type=%s)\n", arg, "Hex");
			break;
		case 13: //wep 104 Ascii type
			pAdapter->SharedKey[2].KeyLen = KeyLen;
			memcpy(pAdapter->SharedKey[2].Key, arg, KeyLen);
			CipherAlg = CIPHER_WEP128;
			DBGPRINT(RT_DEBUG_TRACE, "Set_Key3_Proc::(Key3=%s and type=%s)\n", arg, "Ascii");
			break;
		case 26: //wep 104 Hex type
			for(i=0; i < KeyLen; i++)
			{
				if( !isxdigit(*(arg+i)) )
					return FALSE;  //Not Hex value;
			}
			pAdapter->SharedKey[2].KeyLen = KeyLen / 2 ;
			AtoH(arg, pAdapter->SharedKey[2].Key, KeyLen / 2);
			CipherAlg = CIPHER_WEP128;
			DBGPRINT(RT_DEBUG_TRACE, "Set_Key3_Proc::(Key3=%s and type=%s)\n", arg, "Hex");
			break;
		default: //Invalid argument 
			DBGPRINT(RT_DEBUG_TRACE, "Set_Key3_Proc::Invalid argument (=%s)\n", arg);
			return FALSE;
	}
	pAdapter->SharedKey[2].CipherAlg = CipherAlg;
	
	// Set keys (into ASIC)
	if (pAdapter->PortCfg.AuthMode >= Ndis802_11AuthModeWPA)
		;	// not support
	else	// Old WEP stuff
	{
		AsicAddSharedKeyEntry(pAdapter, 
							  0, 
							  2, 
							  pAdapter->SharedKey[2].CipherAlg, 
							  pAdapter->SharedKey[2].Key, 
							  NULL,
							  NULL);
   }
	
	return TRUE;
}

/* 
	==========================================================================
	Description:
		Set WEP KEY4
	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT Set_Key4_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)
{
	int 								KeyLen;
	int 								i;
	UCHAR								CipherAlg=CIPHER_WEP64;
	
	if (pAdapter->PortCfg.WepStatus != Ndis802_11WEPEnabled ||\
		pAdapter->PortCfg.DefaultKeyId != 3)
		return TRUE;	// do nothing
	
	KeyLen = strlen(arg);

	switch (KeyLen)
	{
		case 5: //wep 40 Ascii type
			pAdapter->SharedKey[3].KeyLen = KeyLen;
			memcpy(pAdapter->SharedKey[3].Key, arg, KeyLen);
			CipherAlg = CIPHER_WEP64;
			DBGPRINT(RT_DEBUG_TRACE, "Set_Key4_Proc::(Key4=%s and type=%s)\n", arg, "Ascii");
			break;
		case 10: //wep 40 Hex type
			for(i=0; i < KeyLen; i++)
			{
				if( !isxdigit(*(arg+i)) )
					return FALSE;  //Not Hex value;
			}
			pAdapter->SharedKey[3].KeyLen = KeyLen / 2 ;
			AtoH(arg, pAdapter->SharedKey[3].Key, KeyLen / 2);
			CipherAlg = CIPHER_WEP64;
			DBGPRINT(RT_DEBUG_TRACE, "Set_Key4_Proc::(Key4=%s and type=%s)\n", arg, "Hex");
			break;
		case 13: //wep 104 Ascii type
			pAdapter->SharedKey[3].KeyLen = KeyLen;
			memcpy(pAdapter->SharedKey[3].Key, arg, KeyLen);
			CipherAlg = CIPHER_WEP128;
			DBGPRINT(RT_DEBUG_TRACE, "Set_Key4_Proc::(Key4=%s and type=%s)\n", arg, "Ascii");
			break;
		case 26: //wep 104 Hex type
			for(i=0; i < KeyLen; i++)
			{
				if( !isxdigit(*(arg+i)) )
					return FALSE;  //Not Hex value;
			}
			pAdapter->SharedKey[3].KeyLen = KeyLen / 2 ;
			AtoH(arg, pAdapter->SharedKey[3].Key, KeyLen / 2);
			CipherAlg = CIPHER_WEP128;
			DBGPRINT(RT_DEBUG_TRACE, "Set_Key4_Proc::(Key4=%s and type=%s)\n", arg, "Hex");
			break;
		default: //Invalid argument 
			DBGPRINT(RT_DEBUG_TRACE, "Set_Key4_Proc::Invalid argument (=%s)\n", arg);
			return FALSE;
	} 
	pAdapter->SharedKey[3].CipherAlg = CipherAlg;
	
	// Set keys (into ASIC)
	if (pAdapter->PortCfg.AuthMode >= Ndis802_11AuthModeWPA)
		;	// not support
	else	// Old WEP stuff
	{
		AsicAddSharedKeyEntry(pAdapter, 
							  0, 
							  3, 
							  pAdapter->SharedKey[3].CipherAlg, 
							  pAdapter->SharedKey[3].Key, 
							  NULL,
							  NULL);
	} 
	
	return TRUE;
}

/* 
	==========================================================================
	Description:
		Set WPA PSK key
	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT Set_WPAPSK_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)
{
	UCHAR					keyMaterial[40];
	INT 					Status;
	
	if ((pAdapter->PortCfg.AuthMode != Ndis802_11AuthModeWPAPSK) &&\
		(pAdapter->PortCfg.AuthMode != Ndis802_11AuthModeWPA2PSK) &&\
		(pAdapter->PortCfg.AuthMode != Ndis802_11AuthModeWPANone) )
		return TRUE;	// do nothing
	
	DBGPRINT(RT_DEBUG_TRACE, "Set_WPAPSK_Proc::(WPAPSK=%s)\n", arg);

	NdisZeroMemory(keyMaterial, 40);

    // the end of this string
    arg[strlen(arg)]='\0';

	if ((strlen(arg) < 8) || (strlen(arg) > 64))
	{
		DBGPRINT(RT_DEBUG_TRACE, "Set failed!!(WPAPSK=%s), WPAPSK key-string required 8 ~ 64 characters \n", arg);
		return FALSE;
	}

	if (strlen(arg) == 64)
	{
		AtoH(arg, keyMaterial, 32);
		NdisMoveMemory(pAdapter->PortCfg.PskKey.Key, keyMaterial, 32);
	}
	else
	{		
		PasswordHash((char *)arg, pAdapter->MlmeAux.Ssid, pAdapter->MlmeAux.SsidLen, keyMaterial);
		NdisMoveMemory(pAdapter->PortCfg.PskKey.Key, keyMaterial, 32);
	}

	RTMPMakeRSNIE(pAdapter, pAdapter->PortCfg.GroupCipher);
	
	if(pAdapter->PortCfg.BssType == BSS_ADHOC &&\
	   pAdapter->PortCfg.AuthMode == Ndis802_11AuthModeWPANone)
	{
		//RTUSBEnqueueCmdFromNdis(pAdapter, OID_802_11_ADD_KEY, TRUE, &Key, sizeof(Key));
		Status = RTMPWPANoneAddKeyProc(pAdapter, &keyMaterial[0]);

		if (Status != NDIS_STATUS_SUCCESS)
			return FALSE;
			
		pAdapter->PortCfg.WpaState = SS_NOTUSE; 
	}
	else	// Use RaConfig as PSK agent.
	{
		// Start STA supplicant state machine
		pAdapter->PortCfg.WpaState = SS_START;
	}
	
	return TRUE;
}

/* 
	==========================================================================
	Description:
		Reset statistics counter

	Arguments:
		pAdapter			Pointer to our adapter
		arg 				

	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT	Set_ResetStatCounter_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg)
{
	DBGPRINT(RT_DEBUG_TRACE, "==>Set_ResetStatCounter_Proc\n");

	// add the most up-to-date h/w raw counters into software counters
	NICUpdateRawCounters(pAd);
	
	NdisZeroMemory(&pAd->WlanCounters, sizeof(COUNTER_802_11));
	NdisZeroMemory(&pAd->Counters8023, sizeof(COUNTER_802_3));
	NdisZeroMemory(&pAd->RalinkCounters, sizeof(COUNTER_RALINK));

	return TRUE;
}

/* 
	==========================================================================
	Description:
		Set Power Saving mode
	Return:
		TRUE if all parameters are OK, FALSE otherwise
	==========================================================================
*/
INT Set_PSMode_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)
{
	if (pAdapter->PortCfg.BssType == BSS_INFRA)
	{
		if ((strcmp(arg, "MAX_PSP") == 0) || (strcmp(arg, "max_psp") == 0))
		{
			// do NOT turn on PSM bit here, wait until MlmeCheckForPsmChange()
			// to exclude certain situations.
			//	   MlmeSetPsmBit(pAdapter, PWR_SAVE);
			if (pAdapter->PortCfg.bWindowsACCAMEnable == FALSE)
				pAdapter->PortCfg.WindowsPowerMode = Ndis802_11PowerModeMAX_PSP;
			pAdapter->PortCfg.WindowsBatteryPowerMode = Ndis802_11PowerModeMAX_PSP;
			OPSTATUS_SET_FLAG(pAdapter, fOP_STATUS_RECEIVE_DTIM);
			pAdapter->PortCfg.DefaultListenCount = 5;
		
		}
		else if ((strcmp(arg, "Fast_PSP") == 0) || (strcmp(arg, "fast_psp") == 0) ||
				 (strcmp(arg, "FAST_PSP") == 0))
		{
			// do NOT turn on PSM bit here, wait until MlmeCheckForPsmChange()
			// to exclude certain situations.
			//	   MlmeSetPsmBit(pAdapter, PWR_SAVE);
			OPSTATUS_SET_FLAG(pAdapter, fOP_STATUS_RECEIVE_DTIM);
			if (pAdapter->PortCfg.bWindowsACCAMEnable == FALSE)
				pAdapter->PortCfg.WindowsPowerMode = Ndis802_11PowerModeFast_PSP;
			pAdapter->PortCfg.WindowsBatteryPowerMode = Ndis802_11PowerModeFast_PSP;
			pAdapter->PortCfg.DefaultListenCount = 3;
		}
		else
		{
			//Default Ndis802_11PowerModeCAM
			// clear PSM bit immediately
			MlmeSetPsmBit(pAdapter, PWR_ACTIVE);
			OPSTATUS_SET_FLAG(pAdapter, fOP_STATUS_RECEIVE_DTIM);
			if (pAdapter->PortCfg.bWindowsACCAMEnable == FALSE)
				pAdapter->PortCfg.WindowsPowerMode = Ndis802_11PowerModeCAM;
			pAdapter->PortCfg.WindowsBatteryPowerMode = Ndis802_11PowerModeCAM;
		}

		DBGPRINT(RT_DEBUG_TRACE, "Set_PSMode_Proc::(PSMode=%d)\n", pAdapter->PortCfg.WindowsPowerMode);
	}
	else
		return FALSE;

		
	return TRUE;
}

#ifdef DBG
/* 
	==========================================================================
	Description:
		Read / Write BBP
	Arguments:
		pAdapter					Pointer to our adapter
		wrq 						Pointer to the ioctl argument

	Return Value:
		None

	Note:
		Usage: 
			   1.) iwpriv rausb0 bbp			   ==> read all BBP
			   2.) iwpriv rausb0 bbp 1			   ==> read BBP where RegID=1
			   3.) iwpriv rausb0 bbp 1=10		   ==> write BBP R1=0x10
	==========================================================================
*/
VOID RTMPIoctlBBP(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq)
{
	CHAR				*this_char;
	CHAR				*value;
	UCHAR				regBBP;
	CHAR				msg[2048];
	CHAR				arg[255];
	LONG				bbpId;
	LONG				bbpValue;
	BOOLEAN				bIsPrintAllBBP = FALSE;
	INT 				Status = 0;
	
	DBGPRINT(RT_DEBUG_INFO, "==>RTMPIoctlBBP\n");
	
	memset(msg, 0x00, 2048);
	if (wrq->u.data.length > 1) //No parameters.
	{			
		Status = copy_from_user(arg, wrq->u.data.pointer, (wrq->u.data.length > 255) ? 255 : wrq->u.data.length);
		sprintf(msg, "\n");
		
		//Parsing Read or Write
		this_char = arg;
		DBGPRINT(RT_DEBUG_INFO, "this_char=%s\n", this_char);										
		if (!*this_char)																			
			goto next;
			
		if ((value = rtstrchr(this_char, '=')) != NULL) 												
			*value++ = 0;		
		
		if (!value || !*value)																		
		{ //Read																					
			DBGPRINT(RT_DEBUG_INFO, "this_char=%s, value=%s\n", this_char, value);
			if (sscanf(this_char, "%d", &(bbpId)) == 1) 											
			{  
				if (bbpId <= 108)																	
				{																					
					RTUSBReadBBPRegister(pAdapter, bbpId, &regBBP); 						 
					sprintf(msg+strlen(msg), "R%02d[0x%02X]:%02X  ", bbpId, bbpId*2, regBBP);		
					DBGPRINT(RT_DEBUG_INFO, "msg=%s\n", msg);
				}																					
				else																				
				{//Invalid parametes, so default printk all bbp 									
					bIsPrintAllBBP = TRUE;
					goto next;																			
				}																					
			}																						
			else																					
			{ //Invalid parametes, so default printk all bbp										
				bIsPrintAllBBP = TRUE;
				goto next;																				
			}																						
		}																							
		else																						
		{ //Write																					
			DBGPRINT(RT_DEBUG_INFO, "this_char=%s, value=%s\n", this_char, value);
			if ((sscanf(this_char, "%d", &(bbpId)) == 1) && (sscanf(value, "%x", &(bbpValue)) == 1))
			{																						
				DBGPRINT(RT_DEBUG_INFO, "bbpID=%02d, value=0x%x\n", bbpId, bbpValue);				
				if (bbpId <= 108)																	
				{																					
					RTUSBWriteBBPRegister(pAdapter, (UCHAR)bbpId,(UCHAR) bbpValue); 		 
					//Read it back for showing														
					RTUSBReadBBPRegister(pAdapter, bbpId, &regBBP); 						 
					sprintf(msg+strlen(msg), "R%02d[0x%02X]:%02X\n", bbpId, bbpId*2, regBBP);		
					DBGPRINT(RT_DEBUG_INFO, "msg=%s\n", msg);										
				}																					
				else																				
				{//Invalid parametes, so default printk all bbp 									
					bIsPrintAllBBP = TRUE;															
					goto next;																			
				}																					
			}																						
			else																					
			{ //Invalid parametes, so default printk all bbp										
				bIsPrintAllBBP = TRUE;																
				goto next;																				
			}																						
		}							  
	}
	else
		bIsPrintAllBBP = TRUE;

next:	 
	if (bIsPrintAllBBP)
	{	
		memset(msg, 0x00, 2048);
		sprintf(msg, "\n");
		for (bbpId = 0; bbpId <= 108; bbpId++)
		{
			RTUSBReadBBPRegister(pAdapter, bbpId, &regBBP);
			sprintf(msg+strlen(msg), "R%02d[0x%02X]:%02X	", bbpId, bbpId*2, regBBP);
			if (bbpId%5 == 4)
				sprintf(msg+strlen(msg), "\n");
		}
		// Copy the information into the user buffer
		DBGPRINT(RT_DEBUG_TRACE, "strlen(msg)=%d\n", strlen(msg));
		wrq->u.data.length = strlen(msg);
		Status = copy_to_user(wrq->u.data.pointer, msg, wrq->u.data.length);
	}
	else
	{
		DBGPRINT(RT_DEBUG_INFO, "copy to user [msg=%s]\n", msg);
		// Copy the information into the user buffer
		DBGPRINT(RT_DEBUG_INFO, "strlen(msg) =%d\n", strlen(msg));
		wrq->u.data.length = strlen(msg);
		Status =  copy_to_user(wrq->u.data.pointer, msg, wrq->u.data.length);
	}
	DBGPRINT(RT_DEBUG_TRACE, "<==RTMPIoctlBBP\n\n");	
}

/* 
	==========================================================================
	Description:
		Read / Write MAC
	Arguments:
		pAdapter					Pointer to our adapter
		wrq 						Pointer to the ioctl argument

	Return Value:
		None

	Note:
		Usage: 
			   1.) iwpriv rausb0 mac 0		  ==> read MAC where Addr=0x0
			   2.) iwpriv rausb0 mac 0=12	  ==> write MAC where Addr=0x0, value=12
	==========================================================================
*/
VOID RTMPIoctlMAC(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	struct iwreq	*wrq)
{
	CHAR				*this_char;
	CHAR				*value;
	INT					j = 0, k = 0;
	CHAR				msg[1024];
	CHAR				arg[255];
	ULONG				macAddr = 0;
	UCHAR				temp[16], temp2[16];
	ULONG				macValue;
	INT 				Status = 0;

	DBGPRINT(RT_DEBUG_INFO, "==>RTMPIoctlMAC\n");
	memset(msg, 0x00, 1024);
	if (wrq->u.data.length > 1) //No parameters.
	{	
		Status = copy_from_user(arg, wrq->u.data.pointer, (wrq->u.data.length > 255) ? 255 : wrq->u.data.length);
		sprintf(msg, "\n");
		
		//Parsing Read or Write
		this_char = arg;
		DBGPRINT(RT_DEBUG_INFO, "this_char=%s\n", this_char);
		if (!*this_char)
			goto next;

		if ((value = rtstrchr(this_char, '=')) != NULL)
			*value++ = 0;

		if (!value || !*value)
		{ //Read
			DBGPRINT(RT_DEBUG_INFO, "Read: this_char=%s, strlen=%d\n", this_char, strlen(this_char));

			// Sanity check
			if(strlen(this_char) > 4)
				goto next;

			j = strlen(this_char);
			while(j-- > 0)
			{
				if(this_char[j] > 'f' || this_char[j] < '0')
					return;
			}

			// Mac Addr
			k = j = strlen(this_char);
			while(j-- > 0)
			{
				this_char[4-k+j] = this_char[j];
			}
			
			while(k < 4)
				this_char[3-k++]='0';
			this_char[4]='\0';

			if(strlen(this_char) == 4)
			{
				AtoH(this_char, temp, 4);
				macAddr = *temp*256 + temp[1];					
				if (macAddr < 0xFFFF)
				{
					RTUSBReadMACRegister(pAdapter, macAddr, &macValue);
					DBGPRINT(RT_DEBUG_TRACE, "MacAddr=%x, MacValue=%x\n", macAddr, macValue);
					sprintf(msg+strlen(msg), "[0x%08X]:%08X  ", macAddr , macValue);
					DBGPRINT(RT_DEBUG_INFO, "msg=%s\n", msg);
				}
				else
				{//Invalid parametes, so default printk all bbp
					goto next;
				}
			}
		}
		else
		{ //Write
			DBGPRINT(RT_DEBUG_INFO, "Write: this_char=%s, strlen(value)=%d, value=%s\n", this_char, strlen(value), value);
			memcpy(&temp2, value, strlen(value));
			temp2[strlen(value)] = '\0';

			// Sanity check
			if((strlen(this_char) > 4) || strlen(temp2) > 8)
				goto next;

			j = strlen(this_char);
			while(j-- > 0)
			{
				if(this_char[j] > 'f' || this_char[j] < '0')
					return;
			}

			j = strlen(temp2);
			while(j-- > 0)
			{
				if(temp2[j] > 'f' || temp2[j] < '0')
					return;
			}

			//MAC Addr
			k = j = strlen(this_char);
			while(j-- > 0)
			{
				this_char[4-k+j] = this_char[j];
			}

			while(k < 4)
				this_char[3-k++]='0';
			this_char[4]='\0';

			//MAC value
			k = j = strlen(temp2);
			while(j-- > 0)
			{
				temp2[8-k+j] = temp2[j];
			}
			
			while(k < 8)
				temp2[7-k++]='0';
			temp2[8]='\0';

			{
				AtoH(this_char, temp, 4);
				macAddr = *temp*256 + temp[1];

				AtoH(temp2, temp, 8);
				macValue = *temp*256*256*256 + temp[1]*256*256 + temp[2]*256 + temp[3];

				// debug mode
				if (macAddr == (HW_DEBUG_SETTING_BASE + 4))
				{
					// 0x2bf4: byte0 non-zero: enable R17 tuning, 0: disable R17 tuning
					if (macValue & 0x000000ff) 
					{
						pAdapter->BbpTuning.bEnable = TRUE;
						DBGPRINT(RT_DEBUG_TRACE,"turn on R17 tuning\n");
					}
					else
					{
						UCHAR R17;
						pAdapter->BbpTuning.bEnable = FALSE;
						if (pAdapter->PortCfg.Channel > 14)
							R17 = pAdapter->BbpTuning.R17LowerBoundA;
						else
							R17 = pAdapter->BbpTuning.R17LowerBoundG;
						RTUSBWriteBBPRegister(pAdapter, 17, R17);
						DBGPRINT(RT_DEBUG_TRACE,"turn off R17 tuning, restore to 0x%02x\n", R17);
					}
					return;
				}

				DBGPRINT(RT_DEBUG_TRACE, "MacAddr=%02x, MacValue=0x%x\n", macAddr, macValue);
				
				RTUSBWriteMACRegister(pAdapter, macAddr, macValue);
				sprintf(msg+strlen(msg), "[0x%08X]:%08X  ", macAddr, macValue);
				DBGPRINT(RT_DEBUG_INFO, "msg=%s\n", msg);
			}
		}
	}
next:
	if(strlen(msg) == 1)
		sprintf(msg+strlen(msg), "===>Error command format!");
	DBGPRINT(RT_DEBUG_INFO, "copy to user [msg=%s]\n", msg);
	// Copy the information into the user buffer
	DBGPRINT(RT_DEBUG_INFO, "strlen(msg) =%d\n", strlen(msg));
	wrq->u.data.length = strlen(msg);
	Status = copy_to_user(wrq->u.data.pointer, msg, wrq->u.data.length);
	
	DBGPRINT(RT_DEBUG_TRACE, "<==RTMPIoctlMAC\n\n");
}

#ifdef RALINK_ATE
/* 
    ==========================================================================
    Description:
        Read / Write E2PROM
    Arguments:
        pAdapter                    Pointer to our adapter
        wrq                         Pointer to the ioctl argument

    Return Value:
        None

    Note:
        Usage: 
               1.) iwpriv rausb0 e2p 0     		==> read E2PROM where Addr=0x0
               2.) iwpriv rausb0 e2p 0=1234	==> write E2PROM where Addr=0x0, value=1234
    ==========================================================================
*/
VOID RTMPIoctlE2PROM(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	struct iwreq	*wrq)
{
	CHAR				*this_char;
	CHAR				*value;
	INT					j = 0, k = 0;
	CHAR				msg[1024];
	CHAR				arg[255];
	USHORT				eepAddr = 0;
	UCHAR				temp[16], temp2[16];
	USHORT				eepValue;
	int					Status;

	DBGPRINT(RT_DEBUG_TRACE, "==>RTMPIoctlE2PROM\n");
	memset(msg, 0x00, 1024);
	if (wrq->u.data.length > 1) //No parameters.
	{   
	    Status = copy_from_user(arg, wrq->u.data.pointer, (wrq->u.data.length > 255) ? 255 : wrq->u.data.length);
		sprintf(msg, "\n");

	    //Parsing Read or Write
		this_char = arg;
		DBGPRINT(RT_DEBUG_INFO, "this_char=%s\n", this_char);

		//Check parameters
		if (!*this_char)
			goto next;

		if ((value = rtstrchr(this_char, '=')) != NULL)
			*value++ = 0;

		if (!value || !*value)
		{ //Read
			DBGPRINT(RT_DEBUG_INFO, "Read: this_char=%s, strlen=%d\n", this_char, strlen(this_char));

			// Sanity check
			if(strlen(this_char) > 4)
				goto next;

			j = strlen(this_char);
			while(j-- > 0)
			{
				if(this_char[j] > 'f' || this_char[j] < '0')
					return;
			}

			// E2PROM addr
			k = j = strlen(this_char);
			while(j-- > 0)
			{
				this_char[4-k+j] = this_char[j];
			}
			
			while(k < 4)
				this_char[3-k++]='0';
			this_char[4]='\0';

			if(strlen(this_char) == 4)
			{
				AtoH(this_char, temp, 4);			
				eepAddr = *temp*256 + temp[1];					
				if (eepAddr < 0xFFFF)
				{
					RTUSBReadEEPROM(pAdapter, eepAddr, (PUCHAR)&eepValue, 2);
					DBGPRINT(RT_DEBUG_INFO, "eepAddr=%x, eepValue=0x%x\n", eepAddr, eepValue);
					sprintf(msg+strlen(msg), "[0x%04X]:0x%02X  ", eepAddr , eepValue);	
					DBGPRINT(RT_DEBUG_INFO, "msg=%s\n", msg);
				}
				else
				{//Invalid parametes, so default printk all bbp
					goto next;
				}
			}
		}				
		else
		{ //Write
			DBGPRINT(RT_DEBUG_TRACE, "Write: this_char=%s, strlen(value)=%d, value=%s\n", this_char, strlen(value), value);
			memcpy(&temp2, value, strlen(value));
			temp2[strlen(value)] = '\0';

			// Sanity check
			if((strlen(this_char) > 4) || strlen(temp2) > 8)
				goto next;

			j = strlen(this_char);
			while(j-- > 0)
			{
				if(this_char[j] > 'f' || this_char[j] < '0')
					return;
			}
			j = strlen(temp2);
			while(j-- > 0)
			{
				if(temp2[j] > 'f' || temp2[j] < '0')
					return;
			}

			//MAC Addr
			k = j = strlen(this_char);
			while(j-- > 0)
			{
				this_char[4-k+j] = this_char[j];
			}

			while(k < 4)
				this_char[3-k++]='0';
			this_char[4]='\0';

			//MAC value
			k = j = strlen(temp2);
			while(j-- > 0)
			{
				temp2[4-k+j] = temp2[j];
			}
			
			while(k < 4)
				temp2[3-k++]='0';
			temp2[4]='\0';

			AtoH(this_char, temp, 4);
			eepAddr = *temp*256 + temp[1];

			AtoH(temp2, temp, 4);
			eepValue = *temp*256 + temp[1];

			DBGPRINT(RT_DEBUG_TRACE, "eepAddr=%02x, eepValue=0x%x\n", eepAddr, eepValue);
			
			RTUSBWriteEEPROM(pAdapter, eepAddr, (PUCHAR)&eepValue, sizeof(eepValue));
			sprintf(msg+strlen(msg), "[0x%02X]:%02X  ", eepAddr, eepValue);
			DBGPRINT(RT_DEBUG_TRACE, "msg=%s\n", msg);
		}
	}
next:
	if(strlen(msg) == 1)
		sprintf(msg+strlen(msg), "===>Error command format!");

	// Copy the information into the user buffer
	DBGPRINT(RT_DEBUG_TRACE, "copy to user [msg=%s], msg_length=%d\n", msg, strlen(msg));
	wrq->u.data.length = strlen(msg);		
	Status =  copy_to_user(wrq->u.data.pointer, msg, wrq->u.data.length);
	
	DBGPRINT(RT_DEBUG_TRACE, "<==RTMPIoctlE2PROM\n");		
}
#endif //RALINK_ATE

#endif //#ifdef DBG

/* 
	==========================================================================
	Description:
		Read statistics counter
Arguments:
	pAdapter					Pointer to our adapter
	wrq 						Pointer to the ioctl argument

	Return Value:
		None

	Note:
		Usage: 
			   1.) iwpriv rausb0 stat 0 		==> Read statistics counter
	==========================================================================
*/
VOID RTMPIoctlStatistics(
	IN	PRTMP_ADAPTER	pAd, 
	IN	struct iwreq	*wrq)
{
	char				msg[1600];
	INT 				Status=0;
	
	memset(msg, 0x00, 1600);
	sprintf(msg, "\n");

	sprintf(msg+strlen(msg), "Tx success					  = %d\n", (ULONG)pAd->WlanCounters.TransmittedFragmentCount.QuadPart);
	sprintf(msg+strlen(msg), "Tx success without retry		  = %d\n", (ULONG)pAd->WlanCounters.TransmittedFragmentCount.QuadPart - (ULONG)pAd->WlanCounters.RetryCount.QuadPart);
	sprintf(msg+strlen(msg), "Tx success after retry		  = %d\n", (ULONG)pAd->WlanCounters.RetryCount.QuadPart);
	sprintf(msg+strlen(msg), "Tx fail to Rcv ACK after retry  = %d\n", (ULONG)pAd->WlanCounters.FailedCount.QuadPart);
	sprintf(msg+strlen(msg), "RTS Success Rcv CTS			  = %d\n", (ULONG)pAd->WlanCounters.RTSSuccessCount.QuadPart);
	sprintf(msg+strlen(msg), "RTS Fail Rcv CTS				  = %d\n", (ULONG)pAd->WlanCounters.RTSFailureCount.QuadPart);

	sprintf(msg+strlen(msg), "Rx success					  = %d\n", (ULONG)pAd->WlanCounters.ReceivedFragmentCount.QuadPart);
	sprintf(msg+strlen(msg), "Rx with CRC					  = %d\n", (ULONG)pAd->WlanCounters.FCSErrorCount.QuadPart);
	sprintf(msg+strlen(msg), "Rx drop due to out of resource  = %d\n", (ULONG)pAd->Counters8023.RxNoBuffer);
	sprintf(msg+strlen(msg), "Rx duplicate frame			  = %d\n", (ULONG)pAd->WlanCounters.FrameDuplicateCount.QuadPart);

	sprintf(msg+strlen(msg), "False CCA (one second)		  = %d\n", (ULONG)pAd->RalinkCounters.OneSecFalseCCACnt);
	sprintf(msg+strlen(msg), "RSSI-A						  = %d\n", (LONG)(pAd->PortCfg.LastRssi - pAd->BbpRssiToDbmDelta));
	sprintf(msg+strlen(msg), "RSSI-B (if available) 		  = %d\n\n", (LONG)(pAd->PortCfg.LastRssi2 - pAd->BbpRssiToDbmDelta));
	
	// Copy the information into the user buffer
	DBGPRINT(RT_DEBUG_INFO, "copy to user [msg=%s]\n", msg);
	wrq->u.data.length = strlen(msg);
	Status = copy_to_user(wrq->u.data.pointer, msg, wrq->u.data.length);

	DBGPRINT(RT_DEBUG_TRACE, "<==RTMPIoctlStatistics\n");
}

/* 
	==========================================================================
	Description:
		Parse encryption type
    Arguments:
	pAdapter					Pointer to our adapter
	wrq 						Pointer to the ioctl argument

	Return Value:
		None

	Note:
	==========================================================================
*/
static CHAR *GetEncryptType(CHAR enc, CHAR encBitMode)
{
    /*** parse TKIP/AES  mixed mode ***/
    if (encBitMode == (TKIPBIT | CCMPBIT))
        return "TKIP, AES";
        
	if(enc == Ndis802_11WEPDisabled)
		return "NONE";
	if(enc == Ndis802_11WEPEnabled)
		return "WEP";
	if(enc == Ndis802_11Encryption2Enabled)
		return "TKIP";
	if(enc == Ndis802_11Encryption3Enabled)
		return "AES";
	else
		return "UNKNOW";
}

static CHAR *GetAuthMode(CHAR auth, CHAR authBitMode)
{
    /*** parse WPA1/WPA2 mixed mode ***/
    if (authBitMode == (WPA1PSKAKMBIT | WPA2PSKAKMBIT))
        return "WPA(2)-PSK";
    if (authBitMode == (WPA1AKMBIT | WPA2AKMBIT))
        return "WPA(2)";
        
    /*** parse SSN/SSNPSK mixed mode ***/
    if (authBitMode == (WPA1AKMBIT | WPA1PSKAKMBIT))
        return "WPA/WPA-PSK";
    if (authBitMode == (WPA2AKMBIT | WPA2PSKAKMBIT))
        return "WPA2/WPA2-PSK";     
    if (authBitMode == (WPA1AKMBIT | WPA1PSKAKMBIT | WPA2AKMBIT | WPA2PSKAKMBIT))
        return "WPA(2)/WPA(2)-PSK";


	if(auth == Ndis802_11AuthModeOpen)
		return "OPEN";
	if(auth == Ndis802_11AuthModeShared)
		return "SHARED";
	if(auth == Ndis802_11AuthModeWPA)
		return "WPA";
	if(auth == Ndis802_11AuthModeWPAPSK)
		return "WPA-PSK";
	if(auth == Ndis802_11AuthModeWPANone)
		return "WPANONE";
	if(auth == Ndis802_11AuthModeWPA2)
		return "WPA2";
	if(auth == Ndis802_11AuthModeWPA2PSK)
		return "WPA2-PSK";
	else
		return "UNKNOW";
}		

/* 
	==========================================================================
	Description:
		Get site survey results
	Arguments:
		pAdapter					Pointer to our adapter
		wrq 						Pointer to the ioctl argument

	Return Value:
		None

	Note:
		Usage:
				1.) UI needs to wait 4 seconds after issue a site survey command
				2.) iwpriv ra0 get_site_survey
				3.) UI needs to prepare at least 4096bytes to get the results
	==========================================================================
*/
#define	LINE_LEN	(8+8+36+20+12+20+12)	// Channel+RSSI+SSID+Bssid+WepStatus+AuthMode+NetworkType
VOID RTMPIoctlGetSiteSurvey(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	struct iwreq	*wrq)
{
	CHAR		*msg;
	INT 		i=0;	 
	INT 		Status=0;
	CHAR		Ssid[MAX_LEN_OF_SSID +1];

	msg = (CHAR *) kmalloc(sizeof(CHAR)*((MAX_LEN_OF_BSS_TABLE)*LINE_LEN), MEM_ALLOC_FLAG);

    if (msg == NULL)
    {
        return;
    }
    
	memset(msg, 0 ,(MAX_LEN_OF_BSS_TABLE)*LINE_LEN );
	memset(Ssid, 0 ,(MAX_LEN_OF_SSID+1) );

	sprintf(msg,"%s","\n");
	sprintf(msg+strlen(msg),"%-8s%-8s%-36s%-20s%-12s%-20s%-12s\n", 
	    "Channel", "RSSI", "SSID", "BSSID", "Enc", "Auth", "NetworkType");
	for(i=0;i<MAX_LEN_OF_BSS_TABLE;i++)
	{
		if( pAdapter->ScanTab.BssEntry[i].Channel==0)
			break;

		if((strlen(msg)+LINE_LEN ) >= ((MAX_LEN_OF_BSS_TABLE)*LINE_LEN) )
			break;	
		
		if((strlen(msg)+LINE_LEN ) >= IW_SCAN_MAX_DATA)
			break;
	
		sprintf(msg+strlen(msg),"%-8d", pAdapter->ScanTab.BssEntry[i].Channel);
		sprintf(msg+strlen(msg),"%-8d", pAdapter->ScanTab.BssEntry[i].Rssi - pAdapter->BbpRssiToDbmDelta);
		
		memcpy(Ssid, pAdapter->ScanTab.BssEntry[i].Ssid, pAdapter->ScanTab.BssEntry[i].SsidLen);
		Ssid[pAdapter->ScanTab.BssEntry[i].SsidLen] = '\0';
        sprintf(msg+strlen(msg),"%-36s", Ssid);
         
		sprintf(msg+strlen(msg),"%02x:%02x:%02x:%02x:%02x:%02x	", 
			pAdapter->ScanTab.BssEntry[i].Bssid[0], 
			pAdapter->ScanTab.BssEntry[i].Bssid[1],
			pAdapter->ScanTab.BssEntry[i].Bssid[2], 
			pAdapter->ScanTab.BssEntry[i].Bssid[3], 
			pAdapter->ScanTab.BssEntry[i].Bssid[4], 
			pAdapter->ScanTab.BssEntry[i].Bssid[5]);
		sprintf(msg+strlen(msg),"%-12s",GetEncryptType(pAdapter->ScanTab.BssEntry[i].WepStatus,
		                                               pAdapter->ScanTab.BssEntry[i].PairCipherBitMode));
		sprintf(msg+strlen(msg),"%-20s",GetAuthMode(pAdapter->ScanTab.BssEntry[i].AuthMode,
		                                            pAdapter->ScanTab.BssEntry[i].AuthBitMode));
        if (pAdapter->ScanTab.BssEntry[i].BssType == BSS_ADHOC)
            sprintf(msg+strlen(msg),"%-12s\n", "Adhoc");
        else
		    sprintf(msg+strlen(msg),"%-12s\n", "Infra");
	}

	wrq->u.data.length = strlen(msg);
	Status = copy_to_user(wrq->u.data.pointer, msg, wrq->u.data.length);
	kfree(msg);
}

VOID RTMPMakeRSNIE(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	UCHAR			GroupCipher)
{

	if (pAdapter->PortCfg.AuthMode == Ndis802_11AuthModeWPA2PSK)
	{
		NdisMoveMemory(pAdapter->PortCfg.RSN_IE, CipherWpa2Template, CipherWpa2TemplateLen);
		// Modify Group cipher
		pAdapter->PortCfg.RSN_IE[7] = ((GroupCipher == Ndis802_11Encryption2Enabled) ? 0x2 : 0x4);			
		// Modify Pairwise cipher
		pAdapter->PortCfg.RSN_IE[13] = ((pAdapter->PortCfg.PairCipher == Ndis802_11Encryption2Enabled) ? 0x2 : 0x4);
		// Modify AKM
		pAdapter->PortCfg.RSN_IE[19] = ((pAdapter->PortCfg.AuthMode == Ndis802_11AuthModeWPA2) ? 0x1 : 0x2);

		DBGPRINT_RAW(RT_DEBUG_TRACE,"WPA2PSK GroupC=%d, PairC=%d\n",pAdapter->PortCfg.GroupCipher ,
			pAdapter->PortCfg.PairCipher);
		
		pAdapter->PortCfg.RSN_IELen = CipherWpa2TemplateLen;
	}
	else if (pAdapter->PortCfg.AuthMode == Ndis802_11AuthModeWPAPSK)
	{
		NdisMoveMemory(pAdapter->PortCfg.RSN_IE, CipherWpaPskTkip, CipherWpaPskTkipLen);
		// Modify Group cipher
		pAdapter->PortCfg.RSN_IE[11] = ((GroupCipher == Ndis802_11Encryption2Enabled) ? 0x2 : 0x4);			
		// Modify Pairwise cipher
		pAdapter->PortCfg.RSN_IE[17] = ((pAdapter->PortCfg.PairCipher == Ndis802_11Encryption2Enabled) ? 0x2 : 0x4);
		// Modify AKM
		pAdapter->PortCfg.RSN_IE[23] = ((pAdapter->PortCfg.AuthMode == Ndis802_11AuthModeWPA) ? 0x1 : 0x2);

		DBGPRINT_RAW(RT_DEBUG_TRACE,"WPAPSK GroupC = %d, PairC=%d ,  \n ",pAdapter->PortCfg.GroupCipher ,
			pAdapter->PortCfg.PairCipher);
		
		pAdapter->PortCfg.RSN_IELen = CipherWpaPskTkipLen;


	}
	else
		pAdapter->PortCfg.RSN_IELen = 0;
		
}

NDIS_STATUS RTMPWPANoneAddKeyProc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PVOID			pBuf)
{	 
	PNDIS_802_11_KEY		pKey;
	ULONG					KeyLen;
	ULONG					BufLen;
	NDIS_STATUS 			Status = NDIS_STATUS_SUCCESS;
	int 					i;
			
	KeyLen = 32;
	BufLen = sizeof(NDIS_802_11_KEY) + KeyLen - 1;

	pKey = kmalloc(BufLen, MEM_ALLOC_FLAG);
	if (pKey == NULL)
	{
		DBGPRINT(RT_DEBUG_TRACE, "RTMPWPANoneAddKeyProc() allocate memory failed \n");
		Status = NDIS_STATUS_FAILURE;
		return	Status;
	}
	
	NdisZeroMemory(pKey, BufLen);
	pKey->Length = BufLen;
	pKey->KeyIndex	= 0x80000000;
	pKey->KeyLength = KeyLen;
	for (i=0; i<6; i++)
		pKey->BSSID[i] = 0xff;
		
	NdisMoveMemory(pKey->KeyMaterial, pBuf, 32);
	RTMPWPAAddKeyProc(pAd, pKey);
	kfree(pKey);

	return Status;
}

#ifdef RALINK_ATE
UCHAR TemplateFrame[24] = {0x08,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xAA,0xBB,0x12,0x34,0x56,0x00,0x11,0x22,0xAA,0xBB,0xCC,0x00,0x00};	// 802.11 MAC Header, Type:Data, Length:24bytes 
/*
    ==========================================================================
    Description:
        Set ATE operation mode to
        0. APSTOP  = Stop AP Mode
        1. APSTART = Start AP Mode
        2. TXCONT  = Continuous Transmit
        3. TXCARR  = Transmit Carrier
        4. TXFRAME = Transmit Frames
        5. RXFRAME = Receive Frames
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/

#ifdef BIG_ENDIAN
static inline INT	set_ate_proc_inline(
#else
INT	Set_ATE_Proc(
#endif
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg)
{
#if 1
	UCHAR			BbpData;
	ULONG			MacData;
	UINT			i=0, atemode;
	ULONG 			temp;
	unsigned long	IrqFlags;

	RTUSBReadBBPRegister(pAd, 63, &BbpData);
	RTUSBReadMACRegister(pAd, PHY_CSR1, &MacData);

	BbpData = 0;
	MacData &= 0xFFFFFFFE;
	
	if (!strcmp(arg, "STASTOP")) 		//Enter ATE mode and set Tx/Rx Idle
	{						
		DBGPRINT(RT_DEBUG_TRACE, "ATE: STASTOP\n");
		
		atemode = pAd->ate.Mode;
		pAd->ate.Mode = ATE_STASTOP;
		pAd->ate.TxDoneCount = pAd->ate.TxCount;
		
		RTUSBRejectPendingPackets(pAd);
		RTUSBCleanUpDataBulkOutQueue(pAd);

		MlmeSuspend(pAd, TRUE);
		RTMPCancelTimer(&pAd->Mlme.PeriodicTimer);

		while (((atomic_read(&pAd->PendingRx) > 0) || 
				(pAd->BulkOutPending[0] == TRUE) ||
				(pAd->BulkOutPending[1] == TRUE) || 
				(pAd->BulkOutPending[2] == TRUE) ||
				(pAd->BulkOutPending[3] == TRUE)) && (pAd->BulkFlags != 0))	//pAd->BulkFlags != 0 wait bulk out finish
		{
			if (atomic_read(&pAd->PendingRx) > 0)
			{
				DBGPRINT_RAW(RT_DEBUG_TRACE, "BulkIn IRP Pending!!!\n");
				RTUSB_VendorRequest(pAd,
										0,
										DEVICE_VENDOR_REQUEST_OUT,
										0x0C,
										0x0,
										0x0,
										NULL,
										0);
			}

			if ((pAd->BulkOutPending[0] == TRUE) ||
				(pAd->BulkOutPending[1] == TRUE) || 
				(pAd->BulkOutPending[2] == TRUE) ||
				(pAd->BulkOutPending[3] == TRUE))
			{
				DBGPRINT_RAW(RT_DEBUG_TRACE, "BulkOut IRP Pending!!!\n");
				if (i == 0)
				{
					RTUSBCancelPendingBulkOutIRP(pAd);
					i++;
				}
			}

			RTMPusecDelay(500000);
		}
		
		LinkDown(pAd, FALSE);	
		AsicEnableBssSync(pAd);
		
		netif_stop_queue(pAd->net_dev);
		RTMPStationStop(pAd);
		RTUSBWriteMACRegister(pAd, TXRX_CSR0, 0x0257b032);        // Disable Rx
		
		//disable auto responder
		RTUSBReadMACRegister(pAd, TXRX_CSR4, &temp);
		temp = temp & 0xFFFDFFFF;
		RTUSBWriteMACRegister(pAd, TXRX_CSR4, temp);
		
	   	RTUSBWriteBBPRegister(pAd, 63, BbpData);
    		RTUSBWriteMACRegister(pAd, PHY_CSR1, MacData);		

		//Clean ATE Bulk in/out conter and continue setup
		atomic_set(&pAd->BulkOutRemained, 0);			
		NdisAcquireSpinLock(&pAd->GenericLock, IrqFlags);	
		pAd->ContinBulkOut = FALSE;		
		pAd->ContinBulkIn = FALSE;
		NdisReleaseSpinLock(&pAd->GenericLock, IrqFlags);
		RTUSB_CLEAR_BULK_FLAG(pAd, fRTUSB_BULK_OUT_DATA_ATE);					

	}
	
	else if (!strcmp(arg, "STASTART")) 
	{						
		DBGPRINT(RT_DEBUG_TRACE, "ATE: STASTART\n");
		pAd->ate.Mode = ATE_STASTART;	

		//make sure there are no pending bulk in/out IRPs before we go on.
		while ((atomic_read(&pAd->PendingRx) > 0) || 
                        (pAd->BulkOutPending[0] == TRUE) ||
			   (pAd->BulkOutPending[1] == TRUE) || 
			   (pAd->BulkOutPending[2] == TRUE) ||
			   (pAd->BulkOutPending[3] == TRUE))
		{
			if (atomic_read(&pAd->PendingRx) > 0)
			{
				DBGPRINT_RAW(RT_DEBUG_TRACE, "BulkIn IRP Pending!!!\n");
				RTUSB_VendorRequest(pAd,
										0,
										DEVICE_VENDOR_REQUEST_OUT,
										0x0C,
										0x0,
										0x0,
										NULL,
										0);
			}

			if ((pAd->BulkOutPending[0] == TRUE) ||
				(pAd->BulkOutPending[1] == TRUE) || 
				(pAd->BulkOutPending[2] == TRUE) ||
				(pAd->BulkOutPending[3] == TRUE))
			{
				DBGPRINT_RAW(RT_DEBUG_TRACE, "BulkOut IRP Pending!!!\n");
				if (i == 0)
				{
					RTUSBCancelPendingBulkOutIRP(pAd);
					i++;
				}
			}

			RTMPusecDelay(500000);
		}

		//enable auto responder
		RTUSBReadMACRegister(pAd, TXRX_CSR4, &temp);
		temp = temp | 0x20000;
		RTUSBWriteMACRegister(pAd, TXRX_CSR4, temp);

		{
			MlmeResume(pAd);
			RTMPSetTimer(pAd, &pAd->Mlme.PeriodicTimer, MLME_TASK_EXEC_INTV);
			RTUSBBulkReceive(pAd);

			//Enable Rx						
			RTUSBWriteMACRegister(pAd, TXRX_CSR0, 0x0256b032);							
		}	

	    	RTUSBWriteBBPRegister(pAd, 63, BbpData);
	   	RTUSBWriteMACRegister(pAd, PHY_CSR1, MacData);
		
		//Clean ATE Bulk in/out conter and continue setup
		atomic_set(&pAd->BulkOutRemained, 0);				
		NdisAcquireSpinLock(&pAd->GenericLock, IrqFlags);
		pAd->ContinBulkOut = FALSE;		
		pAd->ContinBulkIn = FALSE;
		NdisReleaseSpinLock(&pAd->GenericLock, IrqFlags);		
	}	
	
	else if (!strcmp(arg, "TXCONT")) 		// Continuous Tx
	{						
		DBGPRINT(RT_DEBUG_TRACE, "ATE: TXCONT\n");
		
		pAd->ate.Mode = ATE_TXCONT;

		BbpData |= 0x80;
		MacData |= 0x00000001;

		RTUSBWriteBBPRegister(pAd, 63, BbpData);
		RTUSBWriteMACRegister(pAd, PHY_CSR1, MacData);

		RTUSBWriteMACRegister(pAd, TXRX_CSR0, 0x0257b032);        // Disable Rx


		ATEAsicSwitchChannel(pAd, pAd->ate.Channel);
		AsicLockChannel(pAd, pAd->ate.Channel);
		RTMPusecDelay(5000);		
		
		//set SIFS to 194						
		RTUSBWriteMACRegister(pAd, MAC_CSR8, 0x120);						
		
		RTUSBWriteMACRegister(pAd, TXRX_CSR0, 0x0257b032);        // Disable Rx

		//2006/06/11 Check count is continue or no
		atomic_set(&pAd->BulkOutRemained, pAd->ate.TxCount);
		DBGPRINT_RAW(RT_DEBUG_TRACE, "bulk out count = %d\n", atomic_read(&pAd->BulkOutRemained));
		if (atomic_read(&pAd->BulkOutRemained) == 0)
		{
			NdisAcquireSpinLock(&pAd->GenericLock, IrqFlags);
			pAd->ContinBulkOut = TRUE;		
			NdisReleaseSpinLock(&pAd->GenericLock, IrqFlags);
		}
		else
		{
			NdisAcquireSpinLock(&pAd->GenericLock, IrqFlags);
			pAd->ContinBulkOut = FALSE;		
			NdisReleaseSpinLock(&pAd->GenericLock, IrqFlags);
		}				
		
		ATE_RTMPSendNullFrame(pAd);	
		RTUSB_SET_BULK_FLAG(pAd, fRTUSB_BULK_OUT_DATA_ATE);

		// Kick bulk out 
		RTUSBKickBulkOut(pAd);		
		
	}

	else if (!strcmp(arg, "TXCARR"))			// Tx Carrier
	{
		DBGPRINT(RT_DEBUG_TRACE, "ATE: TXCARR\n");
		pAd->ate.Mode = ATE_TXCARR;

		BbpData |= 0x40;
		MacData |= 0x00000001;

		RTUSBWriteBBPRegister(pAd, 63, BbpData);
		RTUSBWriteMACRegister(pAd, PHY_CSR1, MacData);

		RTUSBWriteMACRegister(pAd, TXRX_CSR0, 0x0257b032);        // Disable Rx

		ATEAsicSwitchChannel(pAd, pAd->ate.Channel);
		AsicLockChannel(pAd, pAd->ate.Channel);
		RTMPusecDelay(5000);		
		
		//set SIFS to 194						
		RTUSBWriteMACRegister(pAd, MAC_CSR8, 0x120);						
		
		RTUSBWriteMACRegister(pAd, TXRX_CSR0, 0x0257b032);        // Disable Rx

		//2006/06/11 Check count is continue or no
		atomic_set(&pAd->BulkOutRemained, pAd->ate.TxCount);
		DBGPRINT_RAW(RT_DEBUG_TRACE, "bulk out count = %d\n", atomic_read(&pAd->BulkOutRemained));
		if (atomic_read(&pAd->BulkOutRemained) == 0)
		{
			NdisAcquireSpinLock(&pAd->GenericLock, IrqFlags);
			pAd->ContinBulkOut = TRUE;		
			NdisReleaseSpinLock(&pAd->GenericLock, IrqFlags);
		}
		else
		{
			NdisAcquireSpinLock(&pAd->GenericLock, IrqFlags);
			pAd->ContinBulkOut = FALSE;		
			NdisReleaseSpinLock(&pAd->GenericLock, IrqFlags);
		}				
		
		ATE_RTMPSendNullFrame(pAd);	
		RTUSB_SET_BULK_FLAG(pAd, fRTUSB_BULK_OUT_DATA_ATE);

		// Kick bulk out 
		RTUSBKickBulkOut(pAd);	
		
	}
	
	else if (!strcmp(arg, "TXFRAME"))			// Tx Frames
	{						
		DBGPRINT(RT_DEBUG_TRACE, "ATE: TXFRAME(Count=%d)\n", pAd->ate.TxCount);
		pAd->ate.Mode = ATE_TXFRAME;

		RTUSBWriteBBPRegister(pAd, 63, BbpData);
		RTUSBWriteMACRegister(pAd, PHY_CSR1, MacData);

		ATEAsicSwitchChannel(pAd, pAd->ate.Channel);
		AsicLockChannel(pAd, pAd->ate.Channel);
		RTMPusecDelay(5000);		
		
		//set SIFS to 194						
		RTUSBWriteMACRegister(pAd, MAC_CSR8, 0x120);						
		
		RTUSBWriteMACRegister(pAd, TXRX_CSR0, 0x0257b032);        // Disable Rx
				
		//2006/06/11 Check count is continue or no
		atomic_set(&pAd->BulkOutRemained, pAd->ate.TxCount);
		DBGPRINT_RAW(RT_DEBUG_TRACE, "bulk out count = %d\n", atomic_read(&pAd->BulkOutRemained));
		if (atomic_read(&pAd->BulkOutRemained) == 0)
		{
			DBGPRINT_RAW(RT_DEBUG_TRACE, "Send packet countinue\n");
			NdisAcquireSpinLock(&pAd->GenericLock, IrqFlags);
			pAd->ContinBulkOut = TRUE;		
			pAd->BulkOutPending[0] = FALSE;
			NdisReleaseSpinLock(&pAd->GenericLock, IrqFlags);
		}
		else
		{
			DBGPRINT_RAW(RT_DEBUG_TRACE, "Send packet depand on counter\n");
			NdisAcquireSpinLock(&pAd->GenericLock, IrqFlags);
			pAd->ContinBulkOut = FALSE;		
			pAd->BulkOutPending[0] = FALSE;			
			NdisReleaseSpinLock(&pAd->GenericLock, IrqFlags);
		}				

		ATE_RTMPSendNullFrame(pAd);		
		RTUSB_SET_BULK_FLAG(pAd, fRTUSB_BULK_OUT_DATA_ATE);

		// Kick bulk out 
		RTUSBKickBulkOut(pAd);
	}
	
#endif
#if 1
	else if (!strcmp(arg, "RXFRAME")) 			// Rx Frames
	{
		ULONG	temp;	
		DBGPRINT(RT_DEBUG_TRACE, "ATE: RXFRAME\n");

		pAd->ate.Mode = ATE_RXFRAME;
		
    		RTUSBWriteBBPRegister(pAd, 63, BbpData);
	    	RTUSBWriteMACRegister(pAd, PHY_CSR1, MacData);
    	
		//Set RF Registers
		ATEAsicSwitchChannel(pAd, pAd->ate.Channel);
		AsicLockChannel(pAd, pAd->ate.Channel);
		RTMPusecDelay(5000);	

		// read to clear counters
		RTUSBReadMACRegister(pAd, STA_CSR0, &temp); //FCS & PLCP error count
		RTUSBReadMACRegister(pAd, STA_CSR1, &temp); //Long error & CCA false
		RTUSBReadMACRegister(pAd, STA_CSR2, &temp); // RX FIFO overflow

		NdisAcquireSpinLock(&pAd->GenericLock, IrqFlags);
		pAd->ContinBulkIn = TRUE;
		NdisReleaseSpinLock(&pAd->GenericLock, IrqFlags);

		//Enable Rx
		RTUSBWriteMACRegister(pAd, TXRX_CSR0, 0x0256b032);       // enable RX of MAC block

		//Kick bulk in
		RTUSBBulkReceive(pAd);
	}
#endif	
	else
	{	
		DBGPRINT(RT_DEBUG_TRACE, "ATE: Invalid arg!\n");
		return FALSE;
	}
	RTMPusecDelay(5000);
	
	return TRUE;
}

#ifdef BIG_ENDIAN
INT	Set_ATE_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg)
{

	return set_ate_proc_inline(pAd, arg);
}
#endif

/* 
    ==========================================================================
    Description:
        Set ATE ADDR1=DA for TxFrames    
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_ATE_DA_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)
{
	CHAR                *value;
	INT					i;
	
	if(strlen(arg) != 17)  //Mac address acceptable format 01:02:03:04:05:06 length 17
		return FALSE;

    for (i=0, value = rstrtok(arg,":"); value; value = rstrtok(NULL,":"))               
	{     
		if((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))) ) 
			return FALSE;  //Invalid

		AtoH(value, &pAdapter->ate.Addr1[i++], 2);
	}
    
	if(i != 6)
		return FALSE;  //Invalid
		
	DBGPRINT(RT_DEBUG_TRACE, "Set_ATE_DA_Proc (DA = %2X:%2X:%2X:%2X:%2X:%2X)\n", 
	    pAdapter->ate.Addr1[0], pAdapter->ate.Addr1[1], pAdapter->ate.Addr1[2], 
	    pAdapter->ate.Addr1[3], pAdapter->ate.Addr1[4], pAdapter->ate.Addr1[5]);
	
	return TRUE;
}

/* 
    ==========================================================================
    Description:
        Set ATE ADDR2=SA for TxFrames    
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_ATE_SA_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)
{
	CHAR				*value;
	INT					i;
	
	if(strlen(arg) != 17)  //Mac address acceptable format 01:02:03:04:05:06 length 17
		return FALSE;

    for (i=0, value = rstrtok(arg,":"); value; value = rstrtok(NULL,":"))                
	{          
		if((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))) ) 
			return FALSE;  //Invalid

		AtoH(value, &pAdapter->ate.Addr2[i++], 2);
	}
    
    
	if(i != 6)
		return FALSE;  //Invalid

	DBGPRINT(RT_DEBUG_TRACE, "Set_ATE_SA_Proc (DA = %2X:%2X:%2X:%2X:%2X:%2X)\n", 
	    pAdapter->ate.Addr2[0], pAdapter->ate.Addr2[1], pAdapter->ate.Addr2[2], 
	    pAdapter->ate.Addr2[3], pAdapter->ate.Addr2[4], pAdapter->ate.Addr2[5]);
	
	return TRUE;
}

/* 
    ==========================================================================
    Description:
        Set ATE ADDR3=BSSID for TxFrames    
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_ATE_BSSID_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)
{
	CHAR				*value;
	INT					i;
	
	if(strlen(arg) != 17)  //Mac address acceptable format 01:02:03:04:05:06 length 17
		return FALSE;

    for (i=0, value = rstrtok(arg,":"); value; value = rstrtok(NULL,":"))
	{
		if((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))) ) 
			return FALSE;  //Invalid

		AtoH(value, &pAdapter->ate.Addr3[i++], 2);
	}

	if(i != 6)
		return FALSE;  //Invalid

	DBGPRINT(RT_DEBUG_TRACE, "Set_ATE_BSSID_Proc (DA = %2X:%2X:%2X:%2X:%2X:%2X)\n",	
	    pAdapter->ate.Addr3[0], pAdapter->ate.Addr3[1], pAdapter->ate.Addr3[2], 
	    pAdapter->ate.Addr3[3], pAdapter->ate.Addr3[4], pAdapter->ate.Addr3[5]);
	
	return TRUE;
}

/* 
    ==========================================================================
    Description:
        Set ATE Channel    
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_ATE_CHANNEL_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)
{
	pAdapter->ate.Channel = simple_strtol(arg, 0, 10);

	if((pAdapter->ate.Channel < 1) || (pAdapter->ate.Channel > 200))// to allow A band channel
	{
		pAdapter->ate.Channel = 1;
		DBGPRINT(RT_DEBUG_ERROR, "Set_ATE_CHANNEL_Proc::Out of range\n");
		return FALSE;
	}

	DBGPRINT(RT_DEBUG_TRACE, "Set_ATE_CHANNEL_Proc (ATE Channel = %d)\n", pAdapter->ate.Channel);
	
	return TRUE;
}

/* 
    ==========================================================================
    Description:
        Set ATE Tx Power    
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_ATE_TX_POWER_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg)
{
	ULONG R3;
	UCHAR Bbp94 = 0;
	
	pAd->ate.TxPower = simple_strtol(arg, 0, 10);

	if ((pAd->ate.TxPower > 36) || (pAd->ate.TxPower < -6))
	{
		DBGPRINT(RT_DEBUG_ERROR, "Set_ATE_TX_POWER_Proc::Out of range (Value=%d)\n", pAd->ate.TxPower);
		return FALSE;
	}

	if (pAd->ate.TxPower > 31)  
	{
		//
		// R3 can't large than 36 (0x24), 31 ~ 36 used by BBP 94
		//
		R3 = 31;
		if (pAd->ate.TxPower <= 36)
			Bbp94 = BBPR94_DEFAULT + (UCHAR) (pAd->ate.TxPower - 31);		
	}
	else if (pAd->ate.TxPower < 0)
	{
		//
		// R3 can't less than 0, -1 ~ -6 used by BBP 94
		//	
		R3 = 0;
		if (pAd->ate.TxPower >= -6)
			Bbp94 = BBPR94_DEFAULT + pAd->ate.TxPower;
	}
	else
	{  
		// 0 ~ 31
		R3 = (ULONG) pAd->ate.TxPower;
		Bbp94 = BBPR94_DEFAULT;
	}

	DBGPRINT(RT_DEBUG_TRACE, "Set_ATE_TX_POWER_Proc (TxPower=%d, R3=%d, BBP_R94=%d)\n", pAd->ate.TxPower, R3, Bbp94);

	R3 = R3 << 9;		// shift TX power control to correct RF register bit position
	R3 |= (pAd->LatchRfRegs.R3 & 0xffffc1ff);
	pAd->LatchRfRegs.R3 = R3;

	RTUSBWriteRFRegister(pAd, pAd->LatchRfRegs.R1);
	RTUSBWriteRFRegister(pAd, pAd->LatchRfRegs.R2);
	RTUSBWriteRFRegister(pAd, pAd->LatchRfRegs.R3);
	RTUSBWriteRFRegister(pAd, pAd->LatchRfRegs.R4);

	if (Bbp94 > 0)
		RTUSBWriteBBPRegister(pAd, BBP_R94, Bbp94);
	
	return TRUE;
}

/* 
    ==========================================================================
    Description:
        Set ATE RF frequence offset
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_ATE_TX_FREQOFFSET_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg)
{
	ULONG R4;
	
	pAd->ate.RFFreqOffset = simple_strtol(arg, 0, 10);

	if(pAd->ate.RFFreqOffset >= 64)
	{
		DBGPRINT(RT_DEBUG_ERROR, "Set_ATE_TX_FREQOFFSET_Proc::Out of range\n");
		return FALSE;
	}

	R4 = pAd->ate.RFFreqOffset << 12;		// shift TX power control to correct RF register bit position
	R4 |= (pAd->LatchRfRegs.R4 & (~0x0003f000));
	pAd->LatchRfRegs.R4 = R4;
	
	RTUSBWriteRFRegister(pAd, pAd->LatchRfRegs.R1);
	RTUSBWriteRFRegister(pAd, pAd->LatchRfRegs.R2);
	RTUSBWriteRFRegister(pAd, pAd->LatchRfRegs.R4);
	RTUSBWriteRFRegister(pAd, pAd->LatchRfRegs.R4);

	DBGPRINT(RT_DEBUG_TRACE, "Set_ATE_TX_FREQOFFSET_Proc (RFFreqOffset = %d)\n", pAd->ate.RFFreqOffset);
	
	return TRUE;
}

/* 
    ==========================================================================
    Description:
        Set ATE Tx Length    
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_ATE_TX_LENGTH_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)
{
	pAdapter->ate.TxLength = simple_strtol(arg, 0, 10);

	if((pAdapter->ate.TxLength < 24) || (pAdapter->ate.TxLength > 1500))
	{
		pAdapter->ate.TxLength = 1500;
		DBGPRINT(RT_DEBUG_ERROR, "Set_ATE_TX_LENGTH_Proc::Out of range\n");
		return FALSE;
	}

	DBGPRINT(RT_DEBUG_TRACE, "Set_ATE_TX_LENGTH_Proc (TxLength = %d)\n", pAdapter->ate.TxLength);
	
	return TRUE;
}

/* 
    ==========================================================================
    Description:
        Set ATE Tx Count    
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_ATE_TX_COUNT_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)
{
	pAdapter->ate.TxCount = simple_strtol(arg, 0, 10);

	DBGPRINT(RT_DEBUG_TRACE, "Set_ATE_TX_COUNT_Proc (TxCount = %d)\n", pAdapter->ate.TxCount);
	
	return TRUE;
}

/* 
    ==========================================================================
    Description:
        Set ATE Tx Rate
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_ATE_TX_RATE_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PUCHAR			arg)
{
	pAdapter->ate.TxRate = simple_strtol(arg, 0, 10);

	if(pAdapter->ate.TxRate > RATE_54)
	{
		pAdapter->ate.TxRate = RATE_11;
		DBGPRINT(RT_DEBUG_ERROR, "Set_ATE_TX_RATE_Proc::Out of range\n");
		return FALSE;
	}

	DBGPRINT(RT_DEBUG_TRACE, "Set_ATE_TX_RATE_Proc (TxRate = %d)\n", pAdapter->ate.TxRate);
	
	return TRUE;
}

VOID RTMPStationStop(
    IN  PRTMP_ADAPTER   pAd)
{

	DBGPRINT(RT_DEBUG_TRACE, "==> RTMPStationStop\n");
	
	RTMPCancelTimer(&pAd->MlmeAux.AssocTimer); 
	RTMPCancelTimer(&pAd->MlmeAux.ReassocTimer); 
	RTMPCancelTimer(&pAd->MlmeAux.DisassocTimer); 
	RTMPCancelTimer(&pAd->MlmeAux.AuthTimer); 
	RTMPCancelTimer(&pAd->MlmeAux.BeaconTimer); 
	RTMPCancelTimer(&pAd->MlmeAux.ScanTimer); 
	RTMPCancelTimer(&pAd->Mlme.PeriodicTimer);


	DBGPRINT(RT_DEBUG_TRACE, "<== RTMPStationStop\n");
	
}

VOID RTMPStationStart(
    IN  PRTMP_ADAPTER   pAd)
{

	DBGPRINT(RT_DEBUG_TRACE, "==> RTMPStationStart\n");
	
	pAd->Mlme.PeriodicTimer.Timer.expires = jiffies + MLME_TASK_EXEC_INTV;
	add_timer(&pAd->Mlme.PeriodicTimer.Timer);

	DBGPRINT(RT_DEBUG_TRACE, "<== RTMPStationStart\n");

}

/*
    ==========================================================================
    Description:
    ==========================================================================
 */
VOID ATEAsicSwitchChannel(
    IN PRTMP_ADAPTER pAd, 
    IN UCHAR Channel) 
{
	ULONG R3, R4;
	UCHAR index, BbpReg;
	//RTMP_RF_REGS *RFRegTable;

	if(pAd->ate.RFFreqOffset != 0)
		pAd->RfFreqOffset = pAd->ate.RFFreqOffset;

	// Select antenna
	AsicAntennaSelect(pAd, Channel);

	R3 = pAd->ate.TxPower << 9; // shift TX power control to correct RF R3 bit position     

	switch (pAd->RfIcType)
	{
		case RFIC_2528:
			for (index = 0; index < NUM_OF_2528_CHNL; index++)
			{
				if (Channel == RF2528RegTable[index].Channel)
				{
					R3 = R3 | (RF2528RegTable[index].R3 & 0xffffc1ff); // set TX power
					R4 = (RF2528RegTable[index].R4 & (~0x0003f000)) | (pAd->RfFreqOffset << 12);
					
					// Update variables
					pAd->LatchRfRegs.Channel = Channel;
					pAd->LatchRfRegs.R1 = RF2528RegTable[index].R1;
					pAd->LatchRfRegs.R2 = RF2528RegTable[index].R2;
					pAd->LatchRfRegs.R3 = R3;
					pAd->LatchRfRegs.R4 = R4;
					
					break;
				}
			}
			break;

		case RFIC_5226:
			for (index = 0; index < NUM_OF_5226_CHNL; index++)
			{
				if (Channel == RF5226RegTable[index].Channel)
				{
					R3 = R3 | (RF5226RegTable[index].R3 & 0xffffc1ff); // set TX power
					R4 = (RF5226RegTable[index].R4 & (~0x0003f000)) | (pAd->RfFreqOffset << 12);
					
					// Update variables
					pAd->LatchRfRegs.Channel = Channel;
					pAd->LatchRfRegs.R1 = RF5226RegTable[index].R1;
					pAd->LatchRfRegs.R2 = RF5226RegTable[index].R2;
					pAd->LatchRfRegs.R3 = R3;
					pAd->LatchRfRegs.R4 = R4;
					
					break;
				}
			}
			break;
			
		case RFIC_5225:
		case RFIC_2527:			
			for (index = 0; index < NUM_OF_5225_CHNL; index++)
			{
				if (Channel == RF5225RegTable[index].Channel)
				{
					R3 = R3 | (RF5225RegTable[index].R3 & 0xffffc1ff); // set TX power
					R4 = (RF5225RegTable[index].R4 & (~0x0003f000)) | (pAd->RfFreqOffset << 12);

					// Update variables
					pAd->LatchRfRegs.Channel = Channel;
					pAd->LatchRfRegs.R1 = RF5225RegTable[index].R1;
					pAd->LatchRfRegs.R2 = RF5225RegTable[index].R2;
					pAd->LatchRfRegs.R3 = R3;
					pAd->LatchRfRegs.R4 = R4;
					
					break;
				}
			}

			RTUSBReadBBPRegister(pAd, BBP_R3, &BbpReg);
			if ((pAd->RfIcType == RFIC_5225) || (pAd->RfIcType == RFIC_2527))
				BbpReg &= 0xFE;    // b0=0 for none Smart mode
			else
				BbpReg |= 0x01;    // b0=1 for Smart mode
			RTUSBWriteBBPRegister(pAd, BBP_R3, BbpReg);
			break;
			
		default:
			break;
	}

	//
	// On 11A, We should delay and wait RF/BBP to be stable
	// and the appropriate time should be 1000 micro seconds 
	//
    if (Channel > 14)
    	RTMPusecDelay(1000);
    
    DBGPRINT(RT_DEBUG_TRACE, "ATEAsicSwitchChannel(RF=%d, Pwr=%d) to #%d, R1=0x%08x, R2=0x%08x, R3=0x%08x, R4=0x%08x\n",
        pAd->RfIcType, 
        (R3 & 0x00003e00) >> 9,
        Channel, 
        pAd->LatchRfRegs.R1, 
        pAd->LatchRfRegs.R2, 
        pAd->LatchRfRegs.R3, 
        pAd->LatchRfRegs.R4);	
}
#endif  // RALINK_ATE

#if 1
INT RTMPIoctlSetAuth(	
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	struct iwreq	*wrq)
{
	CHAR	 arg[1];
	INT 	 Status = 0;
	
	if (wrq->u.data.length != 1)
	{
		Status = -EINVAL;
		return (Status);
		
	}
	else
	{
		Status = copy_from_user(arg, wrq->u.data.pointer, wrq->u.data.length);
		
		if (arg[0] == 1)
			pAdapter->PortCfg.AuthMode = Ndis802_11AuthModeOpen;
		else if (arg[0] == 2)
			pAdapter->PortCfg.AuthMode = Ndis802_11AuthModeShared;
		else if (arg[0] == 3)
			pAdapter->PortCfg.AuthMode = Ndis802_11AuthModeWPAPSK;
		else if (arg[0] == 4)
			pAdapter->PortCfg.AuthMode = Ndis802_11AuthModeWPA2PSK;
		else if (arg[0] == 5)
			pAdapter->PortCfg.AuthMode = Ndis802_11AuthModeWPANone;
		else
		{
			Status = -EINVAL;
			return (Status);
		}
	}		   

		
	pAdapter->PortCfg.PortSecured = WPA_802_1X_PORT_NOT_SECURED;
	DBGPRINT(RT_DEBUG_TRACE, "RTMPIoctlSetAuth::(AuthMode=%d)\n", pAdapter->PortCfg.AuthMode); 

	return (Status);
}

INT RTMPIoctlSetEncryp(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	struct iwreq	*wrq)
{
	CHAR	 arg[1];
	INT 	 Status = 0;
	
	if (wrq->u.data.length != 1)
	{
		Status = -EINVAL;  
		return (Status);
	}	 
	else
	{
		Status = copy_from_user(arg, wrq->u.data.pointer, wrq->u.data.length);

		if (arg[0] == 1)	// NONE
		{
			pAdapter->PortCfg.WepStatus   = Ndis802_11WEPDisabled;
			pAdapter->PortCfg.PairCipher  = Ndis802_11WEPDisabled;
			pAdapter->PortCfg.GroupCipher = Ndis802_11WEPDisabled;
		}
		else if (arg[0] == 2)	// WEP
		{
			pAdapter->PortCfg.WepStatus   = Ndis802_11WEPEnabled;
			pAdapter->PortCfg.PairCipher  = Ndis802_11WEPEnabled;
			pAdapter->PortCfg.GroupCipher = Ndis802_11WEPEnabled;

		}
		else if (arg[0] == 3)	// TKIP
		{
			pAdapter->PortCfg.WepStatus   = Ndis802_11Encryption2Enabled;
			pAdapter->PortCfg.PairCipher  = Ndis802_11Encryption2Enabled;
			pAdapter->PortCfg.GroupCipher = Ndis802_11Encryption2Enabled;

		}
		else if (arg[0] == 4)	// AES
		{
			pAdapter->PortCfg.WepStatus   = Ndis802_11Encryption3Enabled;
			pAdapter->PortCfg.PairCipher  = Ndis802_11Encryption3Enabled;
			pAdapter->PortCfg.GroupCipher = Ndis802_11Encryption3Enabled;

		}
		else
		{
			Status = -EINVAL;
			return (Status);
		}	 
	}


	RTMPMakeRSNIE(pAdapter, pAdapter->PortCfg.GroupCipher);
//	RTUSBEnqueueCmdFromNdis(pAdapter, OID_802_11_WEP_STATUS, TRUE, &(pAdapter->PortCfg.WepStatus), 
//							sizeof(pAdapter->PortCfg.WepStatus));
	DBGPRINT(RT_DEBUG_TRACE, "RTMPIoctlSetEncryp::(EncrypType=%d)\n", pAdapter->PortCfg.WepStatus);

	return (Status);
}

INT RTMPIoctlSetWpapsk(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	struct iwreq	*wrq)
{
	CHAR			arg[128];
	UCHAR			keyMaterial[40];
	INT 			Status = 0;
	
	if ((pAdapter->PortCfg.AuthMode != Ndis802_11AuthModeWPAPSK) &&\
		(pAdapter->PortCfg.AuthMode != Ndis802_11AuthModeWPA2PSK) &&\
		(pAdapter->PortCfg.AuthMode != Ndis802_11AuthModeWPANone) )
		return TRUE;	// do nothing

	Status = copy_from_user(arg, wrq->u.data.pointer, wrq->u.data.length);

	NdisZeroMemory(keyMaterial, 40);

    // the end of this string
    arg[strlen(arg)]='\0';

	if ((wrq->u.data.length < 8) || (wrq->u.data.length > 64))
	{
		DBGPRINT(RT_DEBUG_TRACE, "Set failed!!(WPAPSK=%s), WPAPSK key-string required 8 ~ 64 characters \n", arg);
		Status = -EINVAL;
		return (Status);
	}
	else if (wrq->u.data.length == 64)
	{
		AtoH(arg, keyMaterial, 32);
		NdisMoveMemory(pAdapter->PortCfg.PskKey.Key, keyMaterial, 32);
	}
	else
	{
		PasswordHash((char *)arg, pAdapter->MlmeAux.Ssid, pAdapter->MlmeAux.SsidLen, keyMaterial);
		NdisMoveMemory(pAdapter->PortCfg.PskKey.Key, keyMaterial, 32);
	}
  
	RTMPMakeRSNIE(pAdapter, pAdapter->PortCfg.GroupCipher);
	
	if(pAdapter->PortCfg.BssType == BSS_ADHOC &&\
	   pAdapter->PortCfg.AuthMode == Ndis802_11AuthModeWPANone)
	{
		//RTUSBEnqueueCmdFromNdis(pAdapter, OID_802_11_ADD_KEY, TRUE, &Key, sizeof(Key));
		Status = RTMPWPANoneAddKeyProc(pAdapter, &keyMaterial[0]);

		if (Status != NDIS_STATUS_SUCCESS)
			return -ENOMEM;

		pAdapter->PortCfg.WpaState = SS_NOTUSE; 
	}
	else	 // Use RaConfig as PSK agent.
	{
		// Start STA supplicant state machine
		pAdapter->PortCfg.WpaState = SS_START;
	}
  
	DBGPRINT(RT_DEBUG_TRACE, "RTMPIoctlSetWpapsk::(WPAPSK=%s)\n", arg);

	return (Status);
}	 

INT RTMPIoctlSetPsm(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	struct iwreq	*wrq)
{
	CHAR	 arg[1];
	INT 	 Status = 0;
	
	if (wrq->u.data.length != 1)
	{
		Status = -EINVAL;  
		return (Status);
	}	 
	else
	{
		Status = copy_from_user(arg, wrq->u.data.pointer, wrq->u.data.length);

		if (arg[0] == 0)
		{
			//Default Ndis802_11PowerModeCAM
			// clear PSM bit immediately
			MlmeSetPsmBit(pAdapter, PWR_ACTIVE);
			OPSTATUS_SET_FLAG(pAdapter, fOP_STATUS_RECEIVE_DTIM);
			if (pAdapter->PortCfg.bWindowsACCAMEnable == FALSE)
				pAdapter->PortCfg.WindowsPowerMode = Ndis802_11PowerModeCAM;
			pAdapter->PortCfg.WindowsBatteryPowerMode = Ndis802_11PowerModeCAM;
		}
		else if (arg[0] == 1)
		{
			// do NOT turn on PSM bit here, wait until MlmeCheckForPsmChange()
			// to exclude certain situations.
			//	   MlmeSetPsmBit(pAdapter, PWR_SAVE);
			if (pAdapter->PortCfg.bWindowsACCAMEnable == FALSE)
				pAdapter->PortCfg.WindowsPowerMode = Ndis802_11PowerModeMAX_PSP;
			pAdapter->PortCfg.WindowsBatteryPowerMode = Ndis802_11PowerModeMAX_PSP;
			OPSTATUS_SET_FLAG(pAdapter, fOP_STATUS_RECEIVE_DTIM);
			pAdapter->PortCfg.DefaultListenCount = 5;
		
		}
		else if (arg[0] == 2)
		{
			// do NOT turn on PSM bit here, wait until MlmeCheckForPsmChange()
			// to exclude certain situations.
			//	   MlmeSetPsmBit(pAdapter, PWR_SAVE);
			OPSTATUS_SET_FLAG(pAdapter, fOP_STATUS_RECEIVE_DTIM);
			if (pAdapter->PortCfg.bWindowsACCAMEnable == FALSE)
				pAdapter->PortCfg.WindowsPowerMode = Ndis802_11PowerModeFast_PSP;
			pAdapter->PortCfg.WindowsBatteryPowerMode = Ndis802_11PowerModeFast_PSP;
			pAdapter->PortCfg.DefaultListenCount = 3;
		}
		else
		{
			Status = -EINVAL;
			return (Status);
		}	 
	}

		
	DBGPRINT(RT_DEBUG_TRACE, "RTMPIoctlSetPsm::(PSMode=%d)\n", pAdapter->PortCfg.WindowsPowerMode);
	return (Status);
	
}

VOID RTMPIoctlGetRaAPCfg(	
	IN	PRTMP_ADAPTER	pAdapter, 	
	IN	struct iwreq	*wrq)
{	
	pAdapter->PortCfg.bGetAPConfig = TRUE;    
	EnqueueProbeRequest(pAdapter);    
	DBGPRINT(RT_DEBUG_TRACE, "RTMPIoctlGetRaAPCfg::(bGetAPConfig=%d)\n", pAdapter->PortCfg.bGetAPConfig);	
	return;
}

#endif

