#include "stdafx.h"
#include <windows.h>
#include "ftd2xx.h"
#include<iostream>
#include <time.h>

#include "FTDI_CPP_DLL.h"



#include <stdexcept>

using namespace std;

namespace NIUSBPHY
{

	NI_USBPHY::NI_USBPHY()
	{
		int i;
		i=1;
		return;
	}
	NI_USBPHY::~NI_USBPHY()
	{
		int i;
		i=1;
		return;
	}
	NI_RESULT NI_USBPHY::EnumerateUsbDevice (tUSBDevice *pvArg1, unsigned int *numDevs)
{
	char *BufPtrs[255]; 
	char Buffer[255][64];
	unsigned int i;
	unsigned int nDevs;
	FT_STATUS ftStatus;

	for (i=0;i<255;i++)
	{
		BufPtrs[i] = Buffer[i];
	}
	ftStatus = FT_ListDevices(BufPtrs,numDevs,FT_LIST_ALL|FT_OPEN_BY_DESCRIPTION); 

	if (ftStatus == FT_OK)
	{

		for (i=0;i<*numDevs;i++)
		{
			strcpy_s(pvArg1[i].DESC,Buffer[i]);
		}

		ftStatus = FT_ListDevices(BufPtrs, &nDevs,FT_LIST_ALL|FT_OPEN_BY_SERIAL_NUMBER  ); 

		if (ftStatus == FT_OK)
		{
			for (i=0;i<nDevs;i++)
			{
				strcpy_s(pvArg1[i].SN,64, Buffer[i]);
				pvArg1[i].id = i;
			}
			*numDevs = nDevs;
			return NI_OK;
		}
		else
		{
			return NI_ERROR_INTERFACE;
		}
	}
	else
	{
		return NI_ERROR_INTERFACE;
	}

}


NI_RESULT NI_USBPHY::OpenDeviceBySerialNumber (char *SN)
{
	FT_STATUS ftStatus;
	ftStatus = FT_OpenEx(SN,FT_OPEN_BY_SERIAL_NUMBER,&ftHandle); 
	if (ftStatus == FT_OK) { 
		ConnectionStatus = NI_CONNECTED;
		if (FT_ResetDevice(ftHandle) != FT_OK)
			return NI_ERROR_INTERFACE;
		if (FT_SetBitMode(ftHandle,0xFF,FT_BITMODE_SYNC_FIFO) != FT_OK)
			return NI_ERROR_INTERFACE;
		if (FT_SetFlowControl(ftHandle, FT_FLOW_RTS_CTS, 0, 0) != FT_OK)
			return NI_ERROR_INTERFACE;
		if (FT_SetTimeouts(ftHandle, 4000, 4000) != FT_OK)
			return NI_ERROR_INTERFACE;
		if (FT_Purge(ftHandle, FT_PURGE_RX) != FT_OK)
			return NI_ERROR_INTERFACE;
		if (FT_Purge(ftHandle, FT_PURGE_TX) != FT_OK)
			return NI_ERROR_INTERFACE;

		return NI_OK;
	} 
	else { 
		ConnectionStatus = NI_NOTCONNECTED;
		return NI_ERROR_INTERFACE;
	} 

}

NI_RESULT NI_USBPHY::CloseConnection ()
{
	FT_STATUS ftStatus;
	ftStatus = FT_Close(ftHandle); 
	if (ftStatus == FT_OK) { 
		return NI_OK;
	}
	else
	{
		return NI_ERROR_INTERFACE;
	}
}
NI_RESULT NI_USBPHY::WriteToFPGA (unsigned int *d, unsigned int addr, unsigned int length)
{
		unsigned char h[200];
		unsigned int ll;
		unsigned int aa;
		unsigned int wb;
		unsigned int wbtot;
		DWORD translen;
		DWORD bytewritten;
		int currentpointer=0;
		unsigned char *d_byte;
		int i;

		if (ConnectionStatus != NI_CONNECTED)
			return NI_ERROR_NOTCONNECTED;

		if (length > 16777216)
			return NI_ERROR_TRANSFER_MAX_LENGTH;


		d_byte = (unsigned char*) d;

		FT_Purge(ftHandle, FT_PURGE_TX);
			

		ll = length -1;
		aa = addr;

		//HEADER PACCHETTO
		h[0] = 0xFF;
		h[1] = 0x00;
		h[2] = 0xAB;
		h[3] = 0xF0;
		h[4] = (aa >> 24) & 0xFF;
		h[5] = (aa >> 16) & 0xFF;
		h[6] = (aa >> 8) & 0xFF;
		h[7] = (aa >> 0) & 0xFF;
		h[8] = (ll >> 16) & 0xFF;
		h[9] = (ll >> 8) & 0xFF;
		h[10] = (ll >> 0) & 0xFF;

		wbtot = 0;
		translen = 4* length;
		if(FT_Write(ftHandle,h,11, &bytewritten))
			return NI_ERROR_INTERFACE;
		if (bytewritten==0)
			return NI_ERROR_INTERFACE;
		do
		{
			if(FT_Write(ftHandle,&d_byte[currentpointer],translen, &bytewritten))
				return NI_ERROR_INTERFACE;

			if ( (bytewritten==0) && (translen > 0))
				return NI_ERROR_INTERFACE;

			translen = translen - bytewritten;
			currentpointer = currentpointer + bytewritten;
			wbtot++;
		}while(translen > 0);


		return NI_OK;
	
}


NI_RESULT NI_USBPHY::ReadFromFPGA (unsigned int *d, unsigned int addr, unsigned int length)
{
		unsigned char h[200];
		unsigned int ll;
		unsigned int aa;
		unsigned int wb;
		unsigned int wbtot;
		DWORD translen;
		DWORD byteread;
		DWORD bytewritten;
		DWORD curtrans;
		int currentpointer=0;
		unsigned char *d_byte;
		int i;

		if (ConnectionStatus != NI_CONNECTED)
			return NI_ERROR_NOTCONNECTED;

		if (length > 16777216)
			return NI_ERROR_TRANSFER_MAX_LENGTH;


		d_byte = (unsigned char*) d;

		FT_Purge(ftHandle, FT_PURGE_TX);
			

		ll = length -1;
		aa = addr;

		//HEADER PACCHETTO
		h[0] = 0xFF;
		h[1] = 0x00;
		h[2] = 0xAB;
		h[3] = 0xF1;
		h[4] = (aa >> 24) & 0xFF;
		h[5] = (aa >> 16) & 0xFF;
		h[6] = (aa >> 8) & 0xFF;
		h[7] = (aa >> 0) & 0xFF;
		h[8] = (ll >> 16) & 0xFF;
		h[9] = (ll >> 8) & 0xFF;
		h[10] = (ll >> 0) & 0xFF;

		wbtot = 0;
		translen = 4* length;
		if(FT_Write(ftHandle,h,11, &bytewritten))
			return NI_ERROR_INTERFACE;

		if (bytewritten==0)
			return NI_ERROR_INTERFACE;
		do
		{
			if(FT_Read(ftHandle,&d_byte[currentpointer],translen, &byteread))
				return NI_ERROR_INTERFACE;
			if ( (byteread==0) && (translen > 0))
				return NI_ERROR_INTERFACE;
			translen = translen - byteread;
			currentpointer = currentpointer + byteread;
			wbtot++;
		}while(translen > 0);


		return NI_OK;
}
NI_RESULT NI_USBPHY::WriteReg (unsigned int regVal, unsigned int addr)
{

	return WriteToFPGA(&regVal, addr,1);

}

NI_RESULT NI_USBPHY::ReadReg (unsigned int *regVal, unsigned int addr)
{

	return ReadFromFPGA(regVal, addr,1);

}

}