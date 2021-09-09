#include "stdafx.h"
#include "uDDE_API_C.h"



uDDE_API_API NI_RESULT NI_Startup()
{
	return NI_OK;
}

uDDE_API_API NI_RESULT NI_AttachNewDevice(int CONNECTION_MODE, char *IPAddress_or_SN, int TCPPort, int UDPPort, int *handle)
{
	int newdevid;
	int i;
	newdevid =-1;

		for (i=0;i<MAX_NUMBER_OF_DEVICE;i++)
		{
			if (Devices[i].valid == 0)
			{
				newdevid = i;
				break;
			}
		}
		if (newdevid == -1)
		{
			return NI_TOO_MANY_DEVICES_CONNECTED;
		}
		else
		{
			*handle = newdevid;
			Devices[*handle].niuDEE = new uDDE_API () ;
			Devices[*handle].niuDEE->Allocate ();
			if (CONNECTION_MODE==NI_CONNECT_USB)
			{
				if (Devices[*handle].niuDEE->USBConnect (IPAddress_or_SN) == NI_OK )
				{
					Devices[*handle].valid = 1;
					return NI_OK;
				}
			}
			else
			{
				if (Devices[*handle].niuDEE->EthernetConnect (TCPPort,IPAddress_or_SN) ==NI_OK)
				{
					Devices[*handle].valid = 1;
					return NI_OK;
				}

			}

			//Connection failed
			Devices[*handle].niuDEE->Destroy ();
			delete Devices[*handle].niuDEE;
			return NI_ERROR;
		}

	
}

uDDE_API_API NI_RESULT NI_DeleteDevice(int handle)
{
	if (Devices[handle].valid == 1)
	{
		Devices[handle].valid = 0;
		Devices[handle].niuDEE->Destroy ();
		delete Devices[handle].niuDEE;
		return NI_OK;
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}



uDDE_API_API int NI_ConnectionStatus( int *status,
								  int handle, 
								  int channel	)
{
	if (Devices[handle].valid == 1)
	{
		*status = Devices[handle].niuDEE->ConnectionStatus( );
		return NI_OK;
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}



uDDE_API_API NI_RESULT NI_USBEnumerate( tUSBDevice *pvArg1, unsigned int *numDevs	)
{
	NI_RESULT Status;
	uDDE_API enumClass;
	return enumClass.USBEnumerate(pvArg1,numDevs);
}

uDDE_API_API NI_RESULT NI_ETHEnumerate(tETHDevices *pvArg1)
{
	NI_RESULT Status;
	uDDE_API enumClass;
	return enumClass.ETHEnumerate(pvArg1);
}

uDDE_API_API NI_RESULT ETHGetSerialNumber(short int port, char *IPAddress, char *SN)
{
	NI_RESULT Status;
	uDDE_API enumClass;
	return enumClass.ETHGetSerialNumber(port,IPAddress, SN);
}


uDDE_API_API NI_RESULT NI_DHA_WriteReg(UINT32 value, 
									  UINT32 address,
									  int handle, 
									  int channel	)
{
	NI_RESULT Status;
	if (Devices[handle].valid == 1)
	{
		Status = Devices[handle].niuDEE->DHA_WriteReg(value, address);
		return Status;
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}

uDDE_API_API NI_RESULT NI_DHA_ReadReg(UINT32 *value, 
									  UINT32 address,
									  int handle, 
									  int channel	)
{
	NI_RESULT Status;
	if (Devices[handle].valid == 1)
	{
		Status = Devices[handle].niuDEE->DHA_ReadReg(value, address);
		return Status;
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}

uDDE_API_API NI_RESULT NI_DHA_WriteArray(UINT32 *value, 
									  UINT32 address,
									  UINT32 length,
									  int handle, 
									  int channel	)
{
	NI_RESULT Status;
	if (Devices[handle].valid == 1)
	{
		Status = Devices[handle].niuDEE->DHA_WriteArray(value, address, length);
		return Status;
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}

uDDE_API_API NI_RESULT NI_DHA_ReadArray(UINT32 *value, 
									  UINT32 address,
									  UINT32 length,
									  int handle, 
									  int channel	)
{
	NI_RESULT Status;
	if (Devices[handle].valid == 1)
	{
		Status = Devices[handle].niuDEE->DHA_ReadArray(value, address, length);
		return Status;
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}


uDDE_API_API NI_RESULT EnableChanel(UINT32 ENABLE,
									  int handle, 
									  int channel	)
{
	NI_RESULT Status;
	if (Devices[handle].valid == 1)
	{
		Status = Devices[handle].niuDEE->EnableChanel(channel, ENABLE);
		return Status;
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}

uDDE_API_API NI_RESULT ConfigureEnergy(	
										UINT32 MODE,
										UINT32 ENERGY,
										int handle, 
										int channel	)
{
	NI_RESULT Status;
	if (Devices[handle].valid == 1)
	{
		Status = Devices[handle].niuDEE->ConfigureEnergy(channel, MODE, ENERGY);
		return Status;
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}

uDDE_API_API NI_RESULT ProgramSpectrum(	
										UINT32 *spectrum,
										UINT32 spectrumLen,
										int handle, 
										int channel	)
{
	NI_RESULT Status;
	if (Devices[handle].valid == 1)
	{
		Status = Devices[handle].niuDEE->ProgramSpectrum(channel, spectrum, spectrumLen);
		return Status;
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}


uDDE_API_API NI_RESULT ConfigureTimebase(	
										UINT32 MODE,
										UINT32 RATE,
										int handle, 
										int channel	)
{
	NI_RESULT Status;
	if (Devices[handle].valid == 1)
	{
		Status = Devices[handle].niuDEE->ConfigureTimebase(channel, MODE, RATE);
		return Status;
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}


uDDE_API_API NI_RESULT ConfigureDIO(	
										UINT32 OUTTRIGGEREN,
										UINT32 OUTTRIGGERLEN,
										int handle, 
										int channel	)
{
	NI_RESULT Status;
	if (Devices[handle].valid == 1)
	{
		Status = Devices[handle].niuDEE->ConfigureDIO(channel, OUTTRIGGEREN, OUTTRIGGERLEN);
		return Status;
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}

uDDE_API_API NI_RESULT ConfigureDRC(	
										UINT32 RISETIME,
										UINT32 FALLTIME,
										int handle, 
										int channel	)
{
	NI_RESULT Status;
	if (Devices[handle].valid == 1)
	{
		Status = Devices[handle].niuDEE->ConfigureDRC(channel, RISETIME, FALLTIME);
		return Status;
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}

uDDE_API_API NI_RESULT ConfigureNOISE(	
										UINT32 GAUSSM,
										UINT32 DRIFTM,
										int handle, 
										int channel	)
{
	NI_RESULT Status;
	if (Devices[handle].valid == 1)
	{
		Status = Devices[handle].niuDEE->ConfigureNOISE(channel, GAUSSM, DRIFTM);
		return Status;
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}

uDDE_API_API NI_RESULT ConfigureGeneral(	
										double GAIN,
										INT32 OFFSET,
										UINT32 INVERT,
										UINT32 OUTFILTER,
										int handle, 
										int channel	)
{
	NI_RESULT Status;
	if (Devices[handle].valid == 1)
	{
		Status = Devices[handle].niuDEE->ConfigureGeneral(channel, GAIN, OFFSET, INVERT, OUTFILTER);
		return Status;
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}


uDDE_API_API double ChannelsToVoltage(	
										INT32 CHANNELS,
										int handle, 
										int channel	)
{
	
	if (Devices[handle].valid == 1)
	{
		return Devices[handle].niuDEE->ChannelsToVoltage(channel, CHANNELS);
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}

uDDE_API_API double VoltageToChannels(	
										double VOLTAGE,
										int handle, 
										int channel	)
{
	
	if (Devices[handle].valid == 1)
	{
		return Devices[handle].niuDEE->VoltageToChannels(channel, VOLTAGE);
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}


uDDE_API_API NI_RESULT GetStat(	
										UINT32 *CPS,
										UINT32 *LIVE,
										UINT64 *CCOUNTER,
										UINT32 *OVERFLOW,
										int handle, 
										int channel	)
{
	
	if (Devices[handle].valid == 1)
	{
		return Devices[handle].niuDEE->GetStat(channel, CPS, LIVE, CCOUNTER, OVERFLOW);
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}

uDDE_API_API NI_RESULT ResetCCounter(	
										int handle, 
										int channel	)
{
	
	if (Devices[handle].valid == 1)
	{
		return Devices[handle].niuDEE->ResetCCounter(channel);
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}

uDDE_API_API NI_RESULT GetSignalLoopback(	
										INT32 *DATA,
										UINT32 *LEN,
										int handle, 
										int channel	)
{
	
	if (Devices[handle].valid == 1)
	{
		return Devices[handle].niuDEE->GetSignalLoopback(channel, DATA, LEN);
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}


uDDE_API_API NI_RESULT GetSpectrumLoopback(	
									UINT32  *DATA,
										UINT32 *LEN,
										int handle, 
										int channel	)
{
	
	if (Devices[handle].valid == 1)
	{
		return Devices[handle].niuDEE->GetSpectrumLoopback(channel, DATA, LEN);
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}

uDDE_API_API NI_RESULT ResetSpectrum(	
										int handle, 
										int channel	)
{
	
	if (Devices[handle].valid == 1)
	{
		return Devices[handle].niuDEE->ResetSpectrum (channel);
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}


uDDE_API_API NI_RESULT ActiveteGetUID(	
										unsigned int  *key1,
										unsigned int  *key2,
										int handle)
{
	
	if (Devices[handle].valid == 1)
	{
		return Devices[handle].niuDEE->ActiveteGetUID(key1,key2);
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}

uDDE_API_API NI_RESULT ActiveteRegister(	
										unsigned char  *DATA,
										int handle)
{
	
	if (Devices[handle].valid == 1)
	{
		return Devices[handle].niuDEE->ActiveteRegister(DATA);
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}


uDDE_API_API NI_RESULT HardwareInfo(	
										UINT32  *HWREV,
										UINT32  *FWREV,
										UINT32  *HWOPTIONS,
										int handle)
{
	
	if (Devices[handle].valid == 1)
	{
		return Devices[handle].niuDEE->HardwareInfo(HWREV, FWREV, HWOPTIONS);
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}


uDDE_API_API NI_RESULT Reboot(			UINT32 MODE,
										int handle)
{
	
	if (Devices[handle].valid == 1)
	{
		return Devices[handle].niuDEE->Reboot(MODE);
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}

uDDE_API_API NI_RESULT CheckRunningMode(
										UINT32 *MODE,
										int handle)
{
	
	if (Devices[handle].valid == 1)
	{
		return Devices[handle].niuDEE->CheckRunningMode(MODE);
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}

uDDE_API_API NI_RESULT FlashLock(			
										UINT32 MODE,
										int handle)
{
	
	if (Devices[handle].valid == 1)
	{
		return Devices[handle].niuDEE->FlashLock(MODE);
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}

uDDE_API_API NI_RESULT FLASHWritePage(			
										UINT32 PAGEN,
										UCHAR *DATA,
										UINT32 LEN,
										int handle)
{
	
	if (Devices[handle].valid == 1)
	{
		return Devices[handle].niuDEE->FLASHWritePage(PAGEN, DATA, LEN);
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}

uDDE_API_API NI_RESULT FLASHErasePage(			
										UINT32 PAGEN,
										int handle)
{
	
	if (Devices[handle].valid == 1)
	{
		return Devices[handle].niuDEE->FLASHErasePage(PAGEN);
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}


uDDE_API_API NI_RESULT SECWriteWord(
										UINT32 address,
										UINT32 word,
										int handle, 
										int channel	)
{
	
	if (Devices[handle].valid == 1)
	{
		return Devices[handle].niuDEE->SECWriteWord (channel, address, word);
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}


uDDE_API_API NI_RESULT SECReadWord(
										UINT32 address,
										UINT32 *word,
										int handle, 
										int channel	)
{
	
	if (Devices[handle].valid == 1)
	{
		return Devices[handle].niuDEE->SECReadWord (channel, address, word);
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}


uDDE_API_API NI_RESULT IsActivated(
										UINT32 *word,
										unsigned int *expire_time,
										int handle	)
{
	
	if (Devices[handle].valid == 1)
	{
		return Devices[handle].niuDEE->IsActivated (word, expire_time);
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}


uDDE_API_API NI_RESULT WriteCalibrarionFlash(
												double offset, 
												double gain, 
												double chctv,
												int handle,
												unsigned int channel)
{
	
	if (Devices[handle].valid == 1)
	{
		return Devices[handle].niuDEE->WriteCalibrarionFlash(channel, offset, gain, chctv);
	}
	else
	{
		return NI_INVALID_HANDLE;
	}
}

