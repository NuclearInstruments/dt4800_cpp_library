#define MAX_NUMBER_OF_DEVICE 100

#include "NIErrorCode.h"
#include "uDDE_API.h"
#include <stdint.h>

#ifdef uDDE_API_EXPORTS
#define uDDE_API_API extern "C" __declspec(dllexport)
#else
#define uDDE_API_API extern "C" __declspec(dllimport)
#endif

uDDE_API_API NI_RESULT NI_Startup();
uDDE_API_API NI_RESULT NI_AttachNewDevice(int CONNECTION_MODE, 
									  char *IPAddress_or_SN, 
									  int TCPPort, 
									  int UDPPort, 
									  int *handle);

uDDE_API_API NI_RESULT NI_DeleteDevice(int handle);


uDDE_API_API int NI_ConnectionStatus( int *status,
								  int handle, 
								  int channel	);

uDDE_API_API NI_RESULT NI_USBEnumerate( tUSBDevice *pvArg1, unsigned int *numDevs	);

uDDE_API_API NI_RESULT NI_ETHEnumerate(tETHDevices *pvArg1);

uDDE_API_API NI_RESULT ETHGetSerialNumber(short int port, char *IPAddress, char *SN);

uDDE_API_API NI_RESULT NI_DHA_WriteReg(UINT32 value, 
									  UINT32 address,
									  int handle, 
									  int channel	);

uDDE_API_API NI_RESULT NI_DHA_ReadReg(UINT32 *value, 
									  UINT32 address,
									  int handle, 
									  int channel	);

uDDE_API_API NI_RESULT NI_DHA_WriteArray(UINT32 *value, 
									  UINT32 address,
									  UINT32 length,
									  int handle, 
									  int channel	);

uDDE_API_API NI_RESULT NI_DHA_ReadArray(UINT32 *value, 
									  UINT32 address,
									  UINT32 length,
									  int handle, 
									  int channel	);



uDDE_API_API NI_RESULT EnableChanel(UINT32 ENABLE,
									  int handle, 
									  int channel	);

uDDE_API_API NI_RESULT ConfigureEnergy(	
										UINT32 MODE,
										UINT32 ENERGY,
										int handle, 
										int channel	);


uDDE_API_API NI_RESULT ProgramSpectrum(	
										UINT32 *spectrum,
										UINT32 spectrumLen,
										int handle, 
										int channel	);


uDDE_API_API NI_RESULT ConfigureTimebase(	
										UINT32 MODE,
										UINT32 RATE,
										int handle, 
										int channel	);

uDDE_API_API NI_RESULT ConfigureDIO(	
										UINT32 OUTTRIGGEREN,
										UINT32 OUTTRIGGERLEN,
										int handle, 
										int channel	);

uDDE_API_API NI_RESULT ConfigureDRC(	
										UINT32 RISETIME,
										UINT32 FALLTIME,
										int handle, 
										int channel	);

uDDE_API_API NI_RESULT ConfigureNOISE(	
										UINT32 GAUSSM,
										UINT32 DRIFTM,
										int handle, 
										int channel	);

uDDE_API_API NI_RESULT ConfigureGeneral(	
										double GAIN,
										INT32 OFFSET,
										UINT32 INVERT,
										UINT32 OUTFILTER,
										int handle, 
										int channel	);

uDDE_API_API double ChannelsToVoltage(	
										INT32 CHANNELS,
										int handle, 
										int channel	);

uDDE_API_API double VoltageToChannels(	
										double VOLTAGE,
										int handle, 
										int channel	);


uDDE_API_API NI_RESULT GetStat(	
										UINT32 *CPS,
										UINT32 *LIVE,
										UINT64 *CCOUNTER,
										UINT32 *OVERFLOW,
										int handle, 
										int channel	);

uDDE_API_API NI_RESULT ResetCCounter(	
										int handle, 
										int channel	);

uDDE_API_API NI_RESULT GetSignalLoopback(	
										INT32 *DATA,
										UINT32 *LEN,
										int handle, 
										int channel	);

uDDE_API_API NI_RESULT GetSpectrumLoopback(	
										UINT32  *DATA,
										UINT32 *LEN,
										int handle, 
										int channel	);

uDDE_API_API NI_RESULT ResetSpectrum(	
										int handle, 
										int channel	);

uDDE_API_API NI_RESULT ActiveteGetUID(	
										unsigned int  *key1,
										unsigned int  *key2,
										int handle);


uDDE_API_API NI_RESULT ActiveteRegister(	
										unsigned char  *DATA,
										int handle);

uDDE_API_API NI_RESULT IsActivated(	
										unsigned int  *DATA,
										unsigned int *expire_time,
										int handle);

uDDE_API_API NI_RESULT HardwareInfo(	
										UINT32  *HWREV,
										UINT32  *FWREV,
										UINT32  *HWOPTIONS,
										int handle);

uDDE_API_API NI_RESULT Reboot(			UINT32 MODE,
										int handle);

uDDE_API_API NI_RESULT CheckRunningMode(
										UINT32 *MODE,
										int handle);

uDDE_API_API NI_RESULT FlashLock(			
										UINT32 MODE,
										int handle);

uDDE_API_API NI_RESULT FLASHWritePage(			
										UINT32 PAGEN,
										UCHAR *DATA,
										UINT32 LEN,
										int handle);

uDDE_API_API NI_RESULT FLASHErasePage(			
										UINT32 PAGEN,
										int handle);


uDDE_API_API NI_RESULT SECWriteWord(
										UINT32 address,
										UINT32 word,
										int handle, 
										int channel	);

uDDE_API_API NI_RESULT SECReadWord(
										UINT32 address,
										UINT32 *word,
										int handle, 
										int channel	);

uDDE_API_API NI_RESULT WriteCalibrarionFlash(
												double offset, 
												double gain, 
												double chctv,
												int handle,
												unsigned int channel);

struct 
{
	uDDE_API *niuDEE;
	int valid;
} Devices[MAX_NUMBER_OF_DEVICE];
