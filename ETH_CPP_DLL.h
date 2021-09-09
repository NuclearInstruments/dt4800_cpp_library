#define _WINSOCKAPI_  
#include <winsock2.h>
#include <ws2tcpip.h>

#include "NIErrorCode.h"
#include <stdio.h>

namespace NIETHPHY
{
    class NI_ETHPHY
    {
    public:

	NI_ETHPHY::NI_ETHPHY();
	NI_ETHPHY::~NI_ETHPHY();

	NI_RESULT ConnectToHost(int PortNo, char* IPAddress);
	NI_RESULT CloseConnection ();
	NI_RESULT WriteToFPGA (unsigned int *d, unsigned int addr, unsigned int length);
	NI_RESULT ReadFromFPGA (unsigned int *d, unsigned int addr, unsigned int length);
	NI_RESULT WriteReg (unsigned int regVal, unsigned int addr);
	NI_RESULT ReadReg (unsigned int *regVal, unsigned int addr);
	NI_RESULT DiscoverProducts (tETHDevices *devices);
	NI_RESULT STATUS_Open_Connection(char *ipadd, unsigned int port);
	NI_RESULT STATUS_Get_Version(char *fw_string, char *version);
	NI_RESULT STATUS_Read_Conf(tBOARDConfig *config);
	NI_RESULT STATUS_Write_Conf(tBOARDConfig config);
	NI_RESULT STATUS_Reboot_Board();

	private:
	NI_RESULT ResetConfiguration (char *SerialNumber);
	unsigned short int calc_crc(unsigned char *buffer, int len);
	NI_RESULT STATUS_Create_TX_buf (unsigned char command, unsigned char *payload, unsigned char paylen, unsigned char *outBuffer, int *outBufferLength);
	bool STATUS_Parse_RX_buf (unsigned char *inBuffer, int inBufferLength, unsigned char *command, unsigned char *payload, unsigned short int *paylen);
	bool STATUS_send_frame(unsigned char *data, int len);
	NI_RESULT STATUS_Erase_Conf();

	SOCKET s; //Socket handle
	SOCKET STATUS_socket; //Socket handle
	sockaddr_in UDPmyAddress;
	sockaddr_in UDPRemoteAddress;

	int ConnectionStatus;
	int STATUS_connesso;
    };
}
