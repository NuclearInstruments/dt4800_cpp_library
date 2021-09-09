#include "NIErrorCode.h"
#include "ETH_CPP_DLL.h"
#include "FTDI_CPP_DLL.h"

namespace NIHAL
{
    class NI_HAL
    {
    public:
		NI_HAL();
		NI_HAL::~NI_HAL();
		NI_RESULT EthernetConnect(short int port, char *IPAddress);
		NI_RESULT USBConnect(char *SN);
		NI_RESULT USBEnumerate(tUSBDevice *pvArg1, unsigned int *numDevs);
		NI_RESULT ETHEnumerate(tETHDevices *pvArg1);
		NI_RESULT ETHGetSerialNumber(short int port, char *IPAddress, char *SN);
		NI_RESULT CloseConnection();
		NI_RESULT ConnectionStatus();
		NI_RESULT WriteReg(UINT32 value, UINT32 address);
		NI_RESULT ReadReg(UINT32 *value, UINT32 address);
		NI_RESULT WriteArray(UINT32 *value, UINT32 address, UINT32 length);
		NI_RESULT ReadArray(UINT32 *value, UINT32 address, UINT32 length);
	private:
		NIETHPHY::NI_ETHPHY *niETH;
		NIUSBPHY::NI_USBPHY *niUSB;
		int Connected;
	};
}