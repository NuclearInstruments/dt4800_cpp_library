// ETH_CPP_DLL.cpp: definisce le funzioni esportate per l'applicazione DLL.
//
#define _WINSOCKAPI_  
#include "stdafx.h"

#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "ETH_CPP_DLL.h"
#pragma comment(lib, "Ws2_32.lib")


#include <stdexcept>

using namespace std;

namespace NIETHPHY
{

	NI_ETHPHY::NI_ETHPHY()
	{
		return;
	}

	NI_ETHPHY::~NI_ETHPHY()
	{
		return;
	}

	//CONNECTTOHOST – Connects to a remote host	
	NI_RESULT NI_ETHPHY::ConnectToHost(int PortNo, char* IPAddress)
	{
		//Start up Winsock…
		WSADATA wsadata;

		ConnectionStatus = NI_NOTCONNECTED;

		int error = WSAStartup(0x0202, &wsadata);

		//Did something happen?
		if (error)
			return NI_ERROR_INTERFACE;

		//Did we get the right Winsock version?
		if (wsadata.wVersion != 0x0202)
		{
			WSACleanup(); //Clean up Winsock
			return NI_ERROR_INTERFACE;
		}

		//Fill out the information needed to initialize a socket…
		SOCKADDR_IN target; //Socket address information

		target.sin_family = AF_INET; // address family Internet
		target.sin_port = htons (PortNo); //Port to connect on
		target.sin_addr.s_addr = inet_addr (IPAddress); //Target IP

		s = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP); //Create socket
		if (s == INVALID_SOCKET)
		{
		   return NI_ERROR_INTERFACE; //Couldn't create the socket
		}  

		//Try connecting...

		if (connect(s, (SOCKADDR *)&target, sizeof(target)) == SOCKET_ERROR)
		{
			return NI_ERROR_INTERFACE; //Couldn't connect
		}
		else
		{
			ConnectionStatus = NI_CONNECTED;
			return NI_OK; //Success
		}
	}

	//CLOSECONNECTION – shuts down the socket and closes any connection on it
	NI_RESULT NI_ETHPHY::CloseConnection ()
	{
		//Close the socket if it exists
		if (s)
		{
			shutdown(s, SD_BOTH);
			closesocket(s);
		}
		WSACleanup(); //Clean up Winsock
		return NI_OK; //Success
	}


	NI_RESULT NI_ETHPHY::WriteToFPGA (unsigned int *d, unsigned int addr, unsigned int length)
	{
			char h[200];
			unsigned int ll;
			unsigned int aa;
			unsigned int wb;
			unsigned int wbtot;
			DWORD translen;
			DWORD bytewritten;
			int currentpointer=0;
			char *d_byte;
			int i;

			if (ConnectionStatus != NI_CONNECTED)
				return NI_ERROR_NOTCONNECTED;

			if (length > 16777216)
				return NI_ERROR_TRANSFER_MAX_LENGTH;


			d_byte = (char*) d;



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
			h[8] = 0;
			h[9] = (ll >> 16) & 0xFF;
			h[10] = (ll >> 8) & 0xFF;
			h[11] = (ll >> 0) & 0xFF;

			wbtot = 0;
			translen = 4* length;
			if(send(s,h,12, 0)==SOCKET_ERROR)
				return NI_ERROR_INTERFACE;

			do
			{
				bytewritten = send(s,&d_byte[currentpointer],translen, 0);
				if(bytewritten==SOCKET_ERROR)
					return NI_ERROR_INTERFACE;
				translen = translen - bytewritten;
				currentpointer = currentpointer + bytewritten;
				wbtot++;
			}while(translen > 0);


			return NI_OK;
	
	}



	NI_RESULT NI_ETHPHY::ReadFromFPGA (unsigned int *d, unsigned int addr, unsigned int length)
	{
			char h[200];
			unsigned int ll;
			unsigned int aa;
			unsigned int wb;
			unsigned int wbtot;
			DWORD translen;
			DWORD bytewritten;
			DWORD byteread;
			int currentpointer=0;
			char *d_byte;
			int i;

			if (ConnectionStatus != NI_CONNECTED)
				return NI_ERROR_NOTCONNECTED;

			if (length > 16777216)
				return NI_ERROR_TRANSFER_MAX_LENGTH;


			d_byte = (char*) d;



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
			h[8] = 0;
			h[9] = (ll >> 16) & 0xFF;
			h[10] = (ll >> 8) & 0xFF;
			h[11] = (ll >> 0) & 0xFF;

			wbtot = 0;
			translen = 4* length;
			if(send(s,h,12, 0)==SOCKET_ERROR)
				return NI_ERROR_INTERFACE;

		
			do
			{
				byteread = recv(s,&d_byte[currentpointer],translen, 0);
				if(byteread==SOCKET_ERROR)
				{
					 //printf("recv failed: %d\n", WSAGetLastError());
					 //getchar();
					return NI_ERROR_INTERFACE;
				}
				translen = translen - byteread;
				currentpointer = currentpointer + byteread;
				wbtot++;
			}while(translen > 0);


			return NI_OK;
	
	}

	NI_RESULT NI_ETHPHY::WriteReg (unsigned int regVal, unsigned int addr)
	{

		return WriteToFPGA(&regVal, addr,1);

	}

	NI_RESULT NI_ETHPHY::ReadReg (unsigned int *regVal, unsigned int addr)
	{

		return ReadFromFPGA(regVal, addr,1);

	}


	NI_RESULT NI_ETHPHY::DiscoverProducts (tETHDevices *devices)
	{
		WSADATA wsaData;
		int iResult = 0;
		char sendBytes[]= "Nuclear Instruments Discovery Tool. Who is there?\n";
		char recvBuffer[1536];
		u_long  byteavailable;

		WSAStartup(MAKEWORD(2, 2), &wsaData);

		SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (sock == INVALID_SOCKET)
		{
			WSACleanup();
			return NI_ERROR_INTERFACE;
		}

		BOOL enabled = TRUE;
		if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&enabled, sizeof(BOOL)) < 0)
		{
			closesocket(sock);
			WSACleanup();
			return NI_ERROR_INTERFACE;
		}

		struct sockaddr_in Sender_addr;
		Sender_addr.sin_family = AF_INET;
		Sender_addr.sin_port = htons(30303);
		Sender_addr.sin_addr.s_addr = inet_addr("255.255.255.255");

		struct sockaddr_in Recv_addr;
		Recv_addr.sin_family = AF_INET;
		Recv_addr.sin_port = htons(30303);
		Recv_addr.sin_addr.s_addr = INADDR_ANY;

		if (bind(sock, (sockaddr*)&Recv_addr, sizeof(Recv_addr)) < 0)
		{
			closesocket(sock);
			WSACleanup();
			return 1;
		}

  	
		if (sendto(sock, sendBytes, strlen(sendBytes), 0, (sockaddr *)&Sender_addr, sizeof(Sender_addr)) < 0)
		{
        
			closesocket(sock);
			WSACleanup();
			return 0;
		}
		Sleep(1000);
		int SenderAddrSize = sizeof (Sender_addr);
		devices->Count = 0;
		do
		{
			//unsigned int FIONREAD = 0x4004667F;
		
			iResult = ioctlsocket(sock, FIONREAD, &byteavailable);
			if (byteavailable>0)
			{
				iResult = recvfrom(sock, recvBuffer, 1024, 0, (SOCKADDR *) &Sender_addr, &SenderAddrSize);
				if (iResult == SOCKET_ERROR)
				{
					closesocket(sock);
					WSACleanup();
					return NI_ERROR_INTERFACE;
				}

				//Check for valid answer
				if (recvBuffer[0] != 0x4E)
				{

					memcpy_s(devices->Devices[devices->Count].mac_address,6,&recvBuffer[1],6);
					memcpy_s(devices->Devices[devices->Count].ip_address,4,&recvBuffer[10],4);
					memcpy_s(devices->Devices[devices->Count].HW_CODE,4,&recvBuffer[17],4);
					devices->Devices[devices->Count].pic_version_high = recvBuffer[21];
					devices->Devices[devices->Count].pic_version_low = recvBuffer[22];
					devices->Devices[devices->Count].UDP_port = recvBuffer[26]<<8+recvBuffer[27];
					devices->Devices[devices->Count].TCP_port = recvBuffer[31]<<8+recvBuffer[32];
					memcpy_s(devices->Devices[devices->Count].serial_number,8,&recvBuffer[36],8);
					sprintf_s(devices->Devices[devices->Count].SIPAddress,"%d.%d.%d.%d",devices->Devices[devices->Count].ip_address[0],
																							devices->Devices[devices->Count].ip_address[1],
																							devices->Devices[devices->Count].ip_address[2],
																							devices->Devices[devices->Count].ip_address[3]);
					devices->Count ++;

				}
				else
				{
					Sleep(1000);
				}
			}
		}
		while (byteavailable > 0);
	



	closesocket(sock);
	WSACleanup();
		return NI_OK;

	}

	NI_RESULT NI_ETHPHY::ResetConfiguration (char *SerialNumber)
	{

		return NI_OK;
	}

	unsigned short int NI_ETHPHY::calc_crc(unsigned char *buffer, int len)
	{
		unsigned short int crc_table[]  = {0x0, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7, 0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF};

		unsigned int i;
		unsigned int iterazione=0;
		unsigned short int crc=0;
		crc=0;
		while(len)
		{
				len = len -1;
				i = (crc >> 12) ^ (buffer[iterazione] >> 4);
				crc = crc_table[i & 0xF] ^ (crc << 4);
				i = (crc >> 12) ^ (buffer[iterazione] >> 0);
				crc = crc_table[i & 0xF] ^ (crc << 4);
				iterazione ++;
		}

		return crc;
	}


	NI_RESULT NI_ETHPHY::STATUS_Create_TX_buf (unsigned char command, unsigned char *payload, unsigned char paylen, unsigned char *outBuffer, int *outBufferLength)
	{
		unsigned char txbuf[1000];
		unsigned char tmpbuf[1000];
		int i;
		int len;
		short unsigned int tmp;
		unsigned char tmp8;
		txbuf[0] = 0x01;
		len = 1;
		if ((command ==0x01) || (command==0x10) || (command==0x04))
		{
			txbuf[len] = 0x10;
			txbuf[len+1] = command;
			len +=2;
		}
		else
		{
			txbuf[len] = command;
			len +=1;
		}
		if (paylen > 0)
		{
			for (i=0;i<paylen;i++)
			{
				if ((payload[i] == 0x01) || (payload[i] == 0x10) || (payload[i] == 0x04))
				{
					txbuf[len] = 0x10;
					txbuf[len+1] = payload[i];
					len+=2;
				}
				else
				{
					txbuf[len] = payload[i];
					len+=1;
				}
			}
		}
		tmpbuf[0]= command;
	
		if (paylen > 0)
		{
			memcpy_s(&tmpbuf[1],1000,payload, paylen);
		}
	
		tmp = calc_crc(tmpbuf, paylen+1);
		tmp8 = (tmp & 0xFF);

		if ((tmp8 == 0x01) || (tmp8 == 0x10) || (tmp8 == 0x4))
		{
			txbuf[len] = 0x10;
			txbuf[len+1] = tmp8;
			len +=2;
		}
		else
		{
			txbuf[len] = tmp8;
			len +=1;
		}

		tmp8 = ((tmp>>8) & 0xFF);

		if ((tmp8 == 0x01) || (tmp8 == 0x10) || (tmp8 == 0x4))
		{
			txbuf[len] = 0x10;
			txbuf[len+1] = tmp8;
			len +=2;
		}
		else
		{
			txbuf[len] = tmp8;
			len +=1;
		}
		txbuf[len]=0x04;
		len+=1;
		memcpy_s(outBuffer,65535,txbuf,len);
		*outBufferLength=len;
		return NI_OK;
	}


	bool NI_ETHPHY::STATUS_Parse_RX_buf (unsigned char *inBuffer, int inBufferLength, unsigned char *command, unsigned char *payload, unsigned short int *paylen)
	{
		unsigned char tmpbuf[1000];
		unsigned short int start_payload, i, tmp;
		unsigned char rec_crc_l, rec_crc_h, loc_crc_l, loc_crc_h;
		*paylen = 0;
		if (inBuffer[0] == 1)
		{
				if (inBuffer[1] != 0x10)
				{
					*command = inBuffer[1];
					start_payload = 2;
				}
				else
				{
					*command = inBuffer[2];
					start_payload = 3;
				}

				i = start_payload;
				while (i < inBufferLength)
				{
					if (inBuffer[i] == 0x10)
					{
						payload[*paylen ] = inBuffer[i + 1];
						i += 1;
						*paylen  += 1;
					}
					else
					{
						if (inBuffer[i] != 0x4) 
						{
							payload[*paylen ] = inBuffer[i];
							*paylen += 1;
						}
						else                            
						{
							rec_crc_l = inBuffer[i - 2];
							rec_crc_h = inBuffer[i - 1];
							payload[*paylen  - 2] = 0;
							payload[*paylen  - 1] = 0;
							*paylen -= 2;
							break;
						}
					}
					i += 1;
				}
				tmpbuf[0] = *command;
				memcpy_s(&tmpbuf[1], 1000, payload, *paylen);
				tmp = calc_crc(tmpbuf, *paylen + 1);
				loc_crc_l = (tmp & 0xFF);
				loc_crc_h = ((tmp >> 8) & 0xFF);
            
				if ((loc_crc_h == rec_crc_h) && (loc_crc_l == rec_crc_l))
					return true;
				else
					return false;
            
		}
		else
		{
				return false;
		}
	}

	NI_RESULT NI_ETHPHY::STATUS_Open_Connection(char *ipadd, unsigned int port)
	{
	
		//---------------------------------------------
		// Initialize Winsock
	
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2,2), &wsaData);

		//---------------------------------------------
		// Create a socket for sending data
		STATUS_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if(STATUS_socket == INVALID_SOCKET)
		{
		
			return NI_ERROR_INTERFACE;
		}

		int	recvTmo=10;	// receive tmo, ms
		if (setsockopt(STATUS_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&recvTmo, sizeof(int)) == SOCKET_ERROR)
		{
	
			return FALSE;
		}
		
		//---------------------------------------------
		// Set up the RecvAddr structure with the IP address of
		// the receiver and the specified port number.
		UDPRemoteAddress.sin_family = AF_INET;
		UDPRemoteAddress.sin_addr.s_addr = inet_addr(ipadd);//inet_addr(ip);
		UDPRemoteAddress.sin_port = htons(port);

	
		STATUS_connesso = true;

		return NI_OK;
	}

	bool NI_ETHPHY::STATUS_send_frame(unsigned char *data, int len)
	{
		int server_length;
		if (STATUS_connesso == true)
		{
			server_length = sizeof(struct sockaddr_in);
			if(sendto(STATUS_socket, (char*) data , len, 0, (SOCKADDR*) &UDPRemoteAddress, server_length)<0)
				return false;
		}
		else
		{
			return false;
		}
	}

	NI_RESULT NI_ETHPHY::STATUS_Get_Version(char *fw_string, char *version)
	{
			unsigned char ret[30];
			unsigned char datain[500];
			unsigned char payload[500];
			int len;
			unsigned char command;
			unsigned short int len_out;
			unsigned short int i;
			int server_length;
			STATUS_Create_TX_buf(GET_VERSION, payload, 0, ret, &len);
			STATUS_send_frame(ret,len);
			server_length = sizeof(UDPRemoteAddress);
			len = recvfrom(STATUS_socket, (char*)&datain[0], 255, 0, (struct sockaddr *)&UDPRemoteAddress, &server_length);
			i = WSAGetLastError();
			if (STATUS_Parse_RX_buf(datain, len, &command, payload, &len_out))
			{
				if ((command==GET_VERSION) && ( len_out>0))
				{
					if ((payload[0] == 0xB0) && (payload[1] == 0xFC))
					{
						*fw_string = NUCLEAR_INSTRUMENTS_PIC32_BOOTLOADER;
						memcpy_s(version,255,&payload[4],len_out-4-1);
					}

					if ((payload[0] == 0xF1) && (payload[1] == 0xCA) && (payload[2] == 0x60) && (payload[3] == 0xD))
					{
						*fw_string = NUCLEAR_INSTRUMENTS_DPP_ETHERNET;
						memcpy_s(version,255,&payload[4],len_out-4);
					}

					if ((payload[0] == 0xF1) && (payload[1] == 0xCA) && (payload[2] == 0xB) && (payload[3] == 0xAD))
					{
						*fw_string = NUCLEAR_INSTRUMENTS_EMULATOR_ETHERNET;
						memcpy_s(version,255,&payload[4],len_out-4);
					}
				}
			}

			return NI_OK;
	}


	NI_RESULT NI_ETHPHY::STATUS_Read_Conf(tBOARDConfig *config)
	{
		unsigned char ret[30];
		int len;
		unsigned char payload[512];
		unsigned char datain[512];
		unsigned char tmpstring[512];
		unsigned short int len_out;
		unsigned int j,i;
		unsigned int indirizzo;
		unsigned char comando;
		unsigned int FLASH_content[4096];
		int server_length;

		if (STATUS_connesso)
		{
			STATUS_Create_TX_buf(READ_CONFIG, payload, 0, ret, &len);
			if (STATUS_send_frame(ret,len))
			{
				for (i=0;i<32;i++)
				{
					server_length = sizeof(UDPRemoteAddress);
					len = recvfrom(STATUS_socket, (char*)&datain[0], 255, 0, (struct sockaddr *)&UDPRemoteAddress, &server_length);

					if (STATUS_Parse_RX_buf(datain, len, &comando, payload, &len_out))
					{
						if (comando==READ_CONFIG)
						{
							indirizzo = (payload[1]<<8) + payload[2];
							for (j=0;j<payload[0]-1;j++)
							{
								FLASH_content[indirizzo+j] = payload[3+j];
						
							}
						}
					}
				}


									// IP ADDRESS
						config->IPAddress[0] = FLASH_content[0];
						config->IPAddress[1] = FLASH_content[1];
						config->IPAddress[2] = FLASH_content[2];
						config->IPAddress[3] = FLASH_content[3];

						// NETMASK
						config->NETMask[0] = FLASH_content[4];
						config->NETMask[1] = FLASH_content[5];
						config->NETMask[2] = FLASH_content[6];
						config->NETMask[3] = FLASH_content[7];

						//GATEWAY
						config->IPGateway[0] = FLASH_content[8];
						config->IPGateway[1] = FLASH_content[9];
						config->IPGateway[2] = FLASH_content[10];
						config->IPGateway[3] = FLASH_content[11];

						//PRI DNS
						config->PriDNS[0] = FLASH_content[12];
						config->PriDNS[1] = FLASH_content[13];
						config->PriDNS[2] = FLASH_content[14];
						config->PriDNS[3] = FLASH_content[15];

						//MAC ADDRESS
						config->MAC[0] = FLASH_content[16];
						config->MAC[1] = FLASH_content[17];
						config->MAC[2] = FLASH_content[18];
						config->MAC[3] = FLASH_content[19];
						config->MAC[4] = FLASH_content[20];
						config->MAC[5] = FLASH_content[21];
						config->MAC[6] = FLASH_content[22];
						config->MAC[7] = FLASH_content[23];

						//UDP PORT
						config->UDPPort = (FLASH_content[0x1A] << 8) + FLASH_content[0x1B];

						//HW CODE NUMBER
						config->HW_Code_Number =  (FLASH_content[0x1C] << 24) +  (FLASH_content[0x1D] << 16) +  (FLASH_content[0x1E] << 8)  +  FLASH_content[0x1F];

						//COMPANY INFO
						for (i=0;i<256;i++)
							config->CompanyInfo[i] = FLASH_content[0x20+i];

						//FRIENDLY NAME
						for (i=0;i<128;i++)
							config->FriendlyName[i] = FLASH_content[0x120+i];

						//HARDWARE REVISION
						config->HW_Revision = (FLASH_content[0x1A0] << 24) +  (FLASH_content[0x1A1] << 16) +  (FLASH_content[0x1A2] << 8)  +  FLASH_content[0x1A3]; 

						//BOOTLOADER DELAY
						config->HW_Bootloader_Delay = (FLASH_content[0x1A4] << 24) +  (FLASH_content[0x1A5] << 16) +  (FLASH_content[0x1A6] << 8)  +  FLASH_content[0x1A7];

						//HW SERIAL NUMBER
						for (i=0;i<8;i++)
							config->HW_SerialNumber[i] = FLASH_content[0x1A8+i];

						//BOARD TCP PORT
						config->TCPPort = (FLASH_content[0x400] << 24) +  (FLASH_content[0x401] << 16) +  (FLASH_content[0x402] << 8)  +  FLASH_content[0x403];

			}
			return NI_OK;
		}
		else
			return NI_NOTCONNECTED;
	
	}

	NI_RESULT NI_ETHPHY::STATUS_Erase_Conf()
	{
		unsigned char ret[512];
		int len;

		unsigned char payload[512];
		unsigned char datain[32];
		int i;
		unsigned int indirizzo;
		unsigned char FLASH_content[4096];
		unsigned short int len_out;
		unsigned char command;
		int server_length;
	

		if (STATUS_connesso)
		{
			int	recvTmo=3000;
			if (setsockopt(STATUS_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&recvTmo, sizeof(int)) == SOCKET_ERROR)
				return NI_ERROR_INTERFACE;
			STATUS_Create_TX_buf(ERASE_CONFIG, payload, 0, ret, &len);
			if (!STATUS_send_frame(ret,len))
				return NI_ERROR_INTERFACE;
	
			server_length = sizeof(UDPRemoteAddress);
			len = recvfrom(STATUS_socket, (char*)&datain[0], 255, 0, (struct sockaddr *)&UDPRemoteAddress, &server_length);		
			if (STATUS_Parse_RX_buf (datain,len, &command, payload, &len_out))
				return NI_OK;
			else
				;

			recvTmo=10;
			if (setsockopt(STATUS_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&recvTmo, sizeof(int)) == SOCKET_ERROR)
				return NI_ERROR_INTERFACE;

			//	return NI_ERROR_INTERFACE;
		}
		else
			return NI_NOTCONNECTED;

		return NI_OK;
	}

	NI_RESULT NI_ETHPHY::STATUS_Write_Conf(tBOARDConfig config)
	{
		unsigned char ret[512];
		int len;

		unsigned char payload[512];
		unsigned char datain[32];
		int i;
		unsigned int indirizzo;
		unsigned char FLASH_content[4096];
		unsigned short int len_out;
		unsigned char command;
		int server_length;

		for (i=0;i<4096;i++)
			FLASH_content[i] = 0;

		//IP
		FLASH_content[0] = config.IPAddress[0];
		FLASH_content[1] = config.IPAddress[1];
		FLASH_content[2] = config.IPAddress[2];
		FLASH_content[3] = config.IPAddress[3];

		//NETMASK
		FLASH_content[4] = config.NETMask[0];
		FLASH_content[5] = config.NETMask[1];
		FLASH_content[6] = config.NETMask[2];
		FLASH_content[7] = config.NETMask[3];

		//GETEWAY
		FLASH_content[8] = config.IPGateway[0];
		FLASH_content[9] = config.IPGateway[1];
		FLASH_content[10] = config.IPGateway[2];
		FLASH_content[11] = config.IPGateway[3];

		//PRI DNS
		FLASH_content[12] = config.PriDNS[0];
		FLASH_content[13] = config.PriDNS[1];
		FLASH_content[14] = config.PriDNS[2];
		FLASH_content[15] = config.PriDNS[3];

		//PRI DNS
		FLASH_content[16] = config.MAC[0];
		FLASH_content[17] = config.MAC[1];
		FLASH_content[18] = config.MAC[2];
		FLASH_content[19] = config.MAC[3];
		FLASH_content[20] = config.MAC[4];
		FLASH_content[21] = config.MAC[5];

		//UDP Port
		FLASH_content[0x1A] = (config.UDPPort >> 8) & 0xFF;
		FLASH_content[0x1B] = (config.UDPPort) & 0xFF;

		//HW Code Number
		FLASH_content[0x1C] = (config.HW_Code_Number >> 24) & 0xFF;
		FLASH_content[0x1D] = (config.HW_Code_Number >> 16) & 0xFF;
		FLASH_content[0x1E] = (config.HW_Code_Number >> 8) & 0xFF;
		FLASH_content[0x1F] = (config.HW_Code_Number) & 0xFF;

		//Company Info
		for (i=0;i<256;i++)
			FLASH_content[0x20+i] = config.CompanyInfo[i];

		//Friendly Name
		for (i=0;i<128;i++)
			FLASH_content[0x120+i] = config.FriendlyName[i];

		//HW Revision
		FLASH_content[0x1A0] = (config.HW_Revision >> 24) & 0xFF;
		FLASH_content[0x1A1] = (config.HW_Revision >> 16) & 0xFF;
		FLASH_content[0x1A2] = (config.HW_Revision >> 8) & 0xFF;
		FLASH_content[0x1A3] = (config.HW_Revision) & 0xFF;

		//Bootloader delay
		FLASH_content[0x1A4] = (config.HW_Bootloader_Delay >> 24) & 0xFF;
		FLASH_content[0x1A5] = (config.HW_Bootloader_Delay >> 16) & 0xFF;
		FLASH_content[0x1A6] = (config.HW_Bootloader_Delay >> 8) & 0xFF;
		FLASH_content[0x1A7] = (config.HW_Bootloader_Delay) & 0xFF;

		//HW serial number
		for (i=0;i<8;i++)
			FLASH_content[0x1A8+i] = config.HW_SerialNumber[i];

		//TCP Port
		FLASH_content[0x400] = (config.TCPPort >> 24) & 0xFF;
		FLASH_content[0x401] = (config.TCPPort >> 16) & 0xFF;
		FLASH_content[0x402] = (config.TCPPort >> 8) & 0xFF;
		FLASH_content[0x403] = (config.TCPPort) & 0xFF;
	
		NI_RESULT statusErase = STATUS_Erase_Conf();

		if (statusErase!= NI_OK)
			return statusErase;

		if (STATUS_connesso)
		{
			int	recvTmo=411;
			if (setsockopt(STATUS_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&recvTmo, sizeof(int)) == SOCKET_ERROR)
				return NI_ERROR_INTERFACE;

			for (i=0;i<32;i++)
			{
				payload[0] = 128;
				indirizzo = i * 128;
				payload[1] = (indirizzo >> 8) & 0xFF;
				payload[2] = (indirizzo) & 0xFF;
				memcpy_s(&payload[3],512,&FLASH_content[indirizzo], 128);
				STATUS_Create_TX_buf(WRITE_CONFIG, payload, 128+3, ret, &len);
				if (!STATUS_send_frame(ret,len))
					return NI_ERROR_INTERFACE;
				//Sleep(400);
			
				server_length = sizeof(UDPRemoteAddress);
				len = recvfrom(STATUS_socket, (char*)&datain[0], 255, 0, (struct sockaddr *)&UDPRemoteAddress, &server_length);			
				if (STATUS_Parse_RX_buf (datain,len, &command, payload, &len_out))
					;
				else
					;//return NI_ERROR;

			}

			recvTmo=10;
			if (setsockopt(STATUS_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&recvTmo, sizeof(int)) == SOCKET_ERROR)
				return NI_ERROR_INTERFACE;

		}
		else
			return NI_NOTCONNECTED;

		return NI_OK;
	}

	NI_RESULT NI_ETHPHY::STATUS_Reboot_Board()
	{
		unsigned char ret[512];
		int len;
		unsigned char payload[512];
	

		if (STATUS_connesso)
		{
			STATUS_Create_TX_buf(REBOOT_BOARD, payload, 0, ret, &len);
			if (!STATUS_send_frame(ret,len))
				return NI_ERROR_INTERFACE;
		}
		else
			return NI_NOTCONNECTED;

		return NI_OK;

	}
}