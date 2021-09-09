// ETHUSB_HAL_APP.cpp : definisce il punto di ingresso dell'applicazione console.
//

#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include "ETHUSB_HAL_LIB.h"


#define NIREG_INTERFACE_COMMUTATOR			0xFFFFFF

using namespace std;

namespace NIHAL
{
	NI_HAL::NI_HAL()
	{
		Connected=0;
		niUSB = new NIUSBPHY::NI_USBPHY;//= NULL;
		niETH  = new NIETHPHY::NI_ETHPHY;//= NULL;
	}

	NI_HAL::~NI_HAL()
	{
		delete niUSB;
		delete niETH;
		Connected=0;
	}

	NI_RESULT NI_HAL::EthernetConnect(short int port, char *IPAddress)
	{
		//if (niETH == NULL)
		//	niETH = new NIETHPHY::NI_ETHPHY;
		if (niETH->ConnectToHost(port,IPAddress) == NI_OK)
		{
			Connected = 1;
			return NI_CONNECTED;
		}
		else
		{
			Connected = 0;
			return NI_ERROR_INTERFACE;
		}
	}

	NI_RESULT NI_HAL::USBConnect(char *SN)
	{
		//if (niUSB == NULL)
		//	niUSB = new NIUSBPHY::NI_USBPHY;
		if (niUSB->OpenDeviceBySerialNumber(SN) == NI_OK)
		{
			niUSB->WriteReg(1,NIREG_INTERFACE_COMMUTATOR);
			niUSB->WriteReg(1,NIREG_INTERFACE_COMMUTATOR);
			niUSB->WriteReg(1,NIREG_INTERFACE_COMMUTATOR);
			Connected = 2;
			return NI_CONNECTED ;
		}
		else
		{
			Connected = 0;
			return NI_ERROR_INTERFACE;
		}
	}

	NI_RESULT NI_HAL::USBEnumerate(tUSBDevice *pvArg1, unsigned int *numDevs)
	{
		NI_STATUS Status;
		NIUSBPHY::NI_USBPHY *niUSBEnumerator = new NIUSBPHY::NI_USBPHY;
		if (niUSBEnumerator != NULL)
		{
			Status = niUSBEnumerator->EnumerateUsbDevice(pvArg1, numDevs);
			delete niUSBEnumerator;
			return Status;
		}
		else
		{
			return NI_ERROR;
		}
	}


	NI_RESULT NI_HAL::ETHEnumerate(tETHDevices *pvArg1)
	{
		NI_STATUS Status;
		NIETHPHY::NI_ETHPHY *niETHEnumerator = new NIETHPHY::NI_ETHPHY;
		if (niETHEnumerator != NULL)
		{
			Status = niETHEnumerator->DiscoverProducts(pvArg1);
			delete niETHEnumerator;
			return Status;
		}
		else
		{
			return NI_ERROR;
		}
	}

	NI_RESULT NI_HAL::ETHGetSerialNumber(short int port, char *IPAddress, char *SN)
	{
		NIETHPHY::NI_ETHPHY *niETHEnumerator = new NIETHPHY::NI_ETHPHY;
		tBOARDConfig boardData;
		if (niETHEnumerator != NULL)
		{
			if (niETHEnumerator->STATUS_Open_Connection(IPAddress, port) != NI_OK)
				return NI_ERROR_INTERFACE;
			if (niETHEnumerator->STATUS_Read_Conf(&boardData) != NI_OK)
				return NI_ERROR_INTERFACE;
			strcpy_s (SN,16, (char *)boardData.HW_SerialNumber);

			return NI_OK;
		}
	}


	NI_RESULT NI_HAL::CloseConnection()
	{
		if (Connected == 1)
		{
			Connected=0;
			return niETH->CloseConnection ();
		}
		else
		{
			if (Connected == 2)
			{
				Connected=0;
				return niUSB->CloseConnection ();
			}
			else
			{
				return NI_OK;
			}
		}
	}

	NI_RESULT NI_HAL::ConnectionStatus()
	{
		return Connected;
	}

	NI_RESULT NI_HAL::WriteReg(UINT32 value, UINT32 address)
	{
		if (Connected == 1)
		{
			return niETH->WriteReg(value, address);
		}
		else
		{
			if (Connected == 2)
			{
				return niUSB->WriteReg(value, address);
			}
			else
			{
				return NI_ERROR_INTERFACE;
			}
		}
	}

	NI_RESULT NI_HAL::ReadReg(UINT32 *value, UINT32 address)
	{
		if (Connected == 1)
		{
			return niETH->ReadReg(value, address);
		}
		else
		{
			if (Connected == 2)
			{
				return niUSB->ReadReg(value, address);
			}
			else
			{
				return NI_ERROR_INTERFACE;
			}
		}
	}


	NI_RESULT NI_HAL::WriteArray(UINT32 *value, UINT32 address, UINT32 length)
	{
		if (Connected == 1)
		{
			return niETH->WriteToFPGA(value, address, length);
		}
		else
		{
			if (Connected == 2)
			{
				return niUSB->WriteToFPGA(value, address, length);
			}
			else
			{
				return NI_ERROR_INTERFACE;
			}
		}
	}

	NI_RESULT NI_HAL::ReadArray(UINT32 *value, UINT32 address, UINT32 length)
	{
		if (Connected == 1)
		{
			return niETH->ReadFromFPGA(value, address, length);
		}
		else
		{
			if (Connected == 2)
			{
				return niUSB->ReadFromFPGA(value, address, length);
			}
			else
			{
				return NI_ERROR_INTERFACE;
			}
		}
	}
}