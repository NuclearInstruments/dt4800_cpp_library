#define TIMER_DELAY 1

#include "ETHUSB_HAL_LIB.h"
#include <stdint.h>


class uDDE_API {
public:
	uDDE_API::uDDE_API();
	uDDE_API::~uDDE_API();
	NI_RESULT uDDE_API::EthernetConnect(short int port, char *IPAddress);
	NI_RESULT uDDE_API::USBConnect(char *SN);
	NI_RESULT uDDE_API::CloseConnection();
	NI_RESULT uDDE_API::ConnectionStatus();

	NI_RESULT uDDE_API::USBEnumerate(tUSBDevice *pvArg1, unsigned int *numDevs);
	NI_RESULT uDDE_API::ETHEnumerate(tETHDevices *pvArg1);
	NI_RESULT uDDE_API::ETHGetSerialNumber(short int port, char *IPAddress, char *SN);


	NI_RESULT uDDE_API::DHA_WriteReg(UINT32 value, UINT32 address);
	NI_RESULT uDDE_API::DHA_ReadReg(UINT32 *value, UINT32 address);
	NI_RESULT uDDE_API::DHA_WriteArray(UINT32 *value, UINT32 address, UINT32 length);
	NI_RESULT uDDE_API::DHA_ReadArray(UINT32 *value, UINT32 address, UINT32 length);
	NI_RESULT uDDE_API::SetuDDERegister(UINT32 value, UINT32 address);
	NI_RESULT uDDE_API::GetuDDERegister(UINT32 *value, UINT32 address);
	
	NI_RESULT uDDE_API::ReadHardwareProperties();
	NI_RESULT uDDE_API::EnableChanel(unsigned int channel, unsigned int ENABLE);
	NI_RESULT uDDE_API::ConfigureEnergy(unsigned int channel, unsigned int MODE, unsigned int ENERGY);
	NI_RESULT uDDE_API::ProgramSpectrum(unsigned int channel, unsigned int *spectrum, unsigned int spectrumLen);
	NI_RESULT uDDE_API::ConfigureTimebase(unsigned int channel, unsigned int MODE, unsigned int RATE);
	NI_RESULT uDDE_API::ConfigureDIO(unsigned int channel, unsigned int INTRIGGER, unsigned int OUTTRIGGER);
	NI_RESULT uDDE_API::ConfigureDRC(unsigned int channel, unsigned int RISETIME, unsigned int FALLTIME);
	NI_RESULT uDDE_API::ConfigureNOISE(unsigned int channel, unsigned int GAUSSM, unsigned int DRIFTM);
	NI_RESULT uDDE_API::ConfigureGeneral(unsigned int channel, double gain, int offset, unsigned int invert, unsigned int OUTFILTER);
	double uDDE_API::ChannelsToVoltage(int channel, int channels);
	unsigned int uDDE_API::VoltageToChannels(unsigned int channel, double voltage);
	NI_RESULT uDDE_API::GetStat(unsigned int channel, unsigned int *cps,  unsigned int *live, UINT64 *ccounter, unsigned int *overflow);
	NI_RESULT uDDE_API::ResetCCounter(unsigned int channel);
	NI_RESULT uDDE_API::GetSignalLoopback(unsigned int channel, INT32 *data, unsigned int *len);
	NI_RESULT uDDE_API::ResetSpectrum(unsigned int channel);
	NI_RESULT uDDE_API::GetSpectrumLoopback(unsigned int channel, UINT32 *data, unsigned int *len);
	NI_RESULT uDDE_API::ActiveteGetUID(unsigned int *key1, unsigned int *key2);
	NI_RESULT uDDE_API::ActiveteRegister(unsigned char *data);
	NI_RESULT uDDE_API::HardwareInfo(unsigned int *HWREV, unsigned int *FWREV, unsigned int *HWOPTIONS);
	NI_RESULT uDDE_API::Reboot(unsigned int mode);
	NI_RESULT uDDE_API::CheckRunningMode(unsigned int *mode);
	NI_RESULT uDDE_API::FlashLock(unsigned int mode);
	NI_RESULT uDDE_API::FLASHWritePage(unsigned int pagen, unsigned char *data, unsigned int len);
	NI_RESULT uDDE_API::FLASHErasePage(unsigned int pagen);
	NI_RESULT uDDE_API::SECWriteWord(unsigned int channel, unsigned int address, unsigned int word);
	NI_RESULT uDDE_API::SECReadWord(unsigned int channel, unsigned int address, unsigned int *word);
	NI_RESULT uDDE_API::IsActivated(unsigned int *data, unsigned int *expire_time);
	NI_RESULT uDDE_API::WriteCalibrarionFlash(unsigned int channel, float offset, float gain, float chctv);
	NI_RESULT uDDE_API::initializeDAC(unsigned int channel);
	
	NI_RESULT uDDE_API::SyncToDevice();
	NI_RESULT uDDE_API::SyncFromDevice();

	NI_RESULT uDDE_API::Load_image(char *fpath);
	NI_RESULT uDDE_API::Load_psf(char *fpath);

	void uDDE_API::Allocate();
	void uDDE_API::Destroy();

	

private:

	void uDDE_API::programReg(unsigned int channel, unsigned int addr, unsigned int val);

	NIHAL::NI_HAL *niHAL;

	double _clockFREQUENCY;
	double _gOFFSET[4];
	double _gGAIN[4];
	double _VoltageChannelFactor[4];
	int _ChannelsCount;
	protected:
};

	

