#include "stdafx.h"
#include "uDDE_API.h"
#include <math.h>
#include <time.h>
#include <stdlib.h> 
#include <signal.h>
#include <fstream>
#include "mkfilter.h"

#define __CHOFFSET						0x10000000
//TIMEBASE
#define RFA_TIMEBASE_SEED1				0x00000200
#define RFA_TIMEBASE_SEED2				0x00000201
#define RFA_TIMEBASE_SEEDPROG			0x00000202
#define RFA_TIMEBASE_STATSOURCEMUX		0x00000203
#define RFA_TIMEBASE_POISSONP			0x00000204
#define RFA_TIMEBASE_PARFLAG			0x00000205
#define RFA_TIMEBASE_DEADTIME			0x00000206
#define RFA_TIMEBASE_RATE				0x00000207
#define RFA_TIMEBASE_MUXSOURCE			0x00000208
#define RFA_TIMEBASE_TRIGGEROUTLEN		0x00000209
#define RFA_TIMEBASE_TRIGGEROUTEN		0x0000020A

//DAC CONTROLLER
#define RFA_DACCNRL_REG					0x00000300
#define RFA_DACCNRL_RESET				0x00000301
#define RFA_DACCNRL_START				0x00000302

//DIGITAL RC
#define RFA_DRC_BETA11A					0x00000400
#define RFA_DRC_BETA11B					0x00000401
#define RFA_DRC_BETA12A					0x00000402
#define RFA_DRC_BETA12B					0x00000403
#define RFA_DRC_ALFA11A					0x00000404
#define RFA_DRC_ALFA11B					0x00000405
#define RFA_DRC_BETA21A					0x00000406
#define RFA_DRC_BETA21B					0x00000407
#define RFA_DRC_BETA22A					0x00000408
#define RFA_DRC_BETA22B					0x00000409
#define RFA_DRC_ALFA21A					0x0000040A
#define RFA_DRC_ALFA21B					0x0000040B
#define RFA_DRC_GAIN21A					0x0000040C
#define RFA_DRC_GAIN21B					0x0000040D
#define RFA_DRC_RESET					0x0000040E


//NOISE
#define RFA_NOISE_GAUSSM				0x00000500
#define RFA_NOISE_RWM					0x00000501
#define RFA_NOISE_SM					0x00000502
#define RFA_NOISE_SAMPL					0x00000503
#define RFA_NOISE_TEMPSEED1				0x00000504
#define RFA_NOISE_TEMPSEED2				0x00000505
#define RFA_NOISE_STARTSEEDGEN			0x00000506
#define RFA_NOISE_RESETLFSR				0x00000507
#define RFA_NOISE_RESETLFSR				0x00000507

//OFFSET GAIN
#define RFA_OFFGAIN_GAIN				0x00000600
#define RFA_OFFGAIN_OFFSET				0x00000601
#define RFA_OFFGAIN_INVERT				0x00000602
#define RFA_OFFGAIN_ENABLE				0x00000603
#define RFA_OFFGAIN_OUTFILTER			0x00000604


//OFFSET ICAPDNA
#define RFA_ICAPDNA_REBOOTADDR			0x00000700
#define RFA_ICAPDNA_UNLOCK				0x00000701
#define RFA_ICAPDNA_nREBOOT				0x00000702
#define RFA_ICAPDNA_ENABLE				0x00000703
#define RFA_ICAPDNA_UID1				0x00000704
#define RFA_ICAPDNA_UID2				0x00000705
#define RFA_ICAPDNA_CHECK1				0x00000706
#define RFA_ICAPDNA_CHECK2				0x00000707
#define RFA_ICAPDNA_CHECK3				0x00000708
#define RFA_ICAPDNA_CHECK4				0x00000709
#define RFA_ICAPDNA_CHECK5				0x0000070A
#define RFA_ICAPDNA_CHECK6				0x0000070B
#define RFA_ICAPDNA_CHECK7				0x0000070C
#define RFA_ICAPDNA_CHECK8				0x0000070D
#define RFA_ICAPDNA_WF					0x0000070E
#define RFA_ICAPDNA_EEPROM				0x0000070F



//ENERGY
#define RFA_ENERGY_MEMBA				0x00010000
#define RFA_ENERGY_SEED1				0x0001F000
#define RFA_ENERGY_SEED2				0x0001F001
#define RFA_ENERGY_SEEDPROG				0x0001F002
#define RFA_ENERGY_PROGMODE				0x0001F003
#define RFA_ENERGY_FIXSPE				0x0001F004
#define RFA_ENERGY_FIXENERGY			0x0001F005
#define RFA_ENERGY_EMPTYMEM				0x0001F006

//SPECTRUM LOOPBACK
#define RFA_SPELOOP_MEMBA				0x00020000
#define RFA_SPELOOP_RESETSM				0x0002F000
#define RFA_SPELOOP_MODE				0x0002F001
#define RFA_SPELOOP_SCALE				0x0002F002

//MONITOR
#define RFA_MON_MEMBA					0x00030000
#define RFA_MON_RESTOREACQ				0x0003F000
#define RFA_MON_CPSR_DATAREADY			0x0003F001
#define RFA_MON_CPSR					0x0003F002
#define RFA_MON_CPSLIVER				0x0003F003
#define RFA_MON_CPSR_CCOUNTER1			0x0003F004
#define RFA_MON_CPSR_CCOUNTER2			0x0003F005
#define RFA_MON_CPSR_OVERLOAD_OVERFLOW	0x0003F006
#define RFA_MON_CPSR_RESET				0x0003F007


//INFOS
#define RFA_INFO_CLKFREQ				0x00000001
#define RFA_INFO_CHANNELCOUNT			0x00000002
#define RFA_INFO_CHANNELOFFSET			0x00000003
#define RFA_INFO_HWREV					0x00000004
#define RFA_INFO_SWREV					0x00000005
#define RFA_INFO_HWOPT					0x00000006
#define RFA_INFO_WRITETOFLASH			0x00000009
#define RFA_INFO_gOFFSET				0x0000000A
#define RFA_INFO_gGAIN					0x0000000B
#define RFA_INFO_gCTV					0x0000000C


#define REG_COUNT				26					
#define REG_ADDRESS				0

//top_module
#define RFA_GLOBAL_LFSR_SOURCE_CONTROL	0
#define RFA_GLOBAL_LFSR_RESET_ETH	1
#define RFA_GLOBAL_LFSR_ENABLE_ETH	2
#define RFA_GLOBAL_LFSR_ONESHOT	3
#define RFA_GLOBAL_PLAYPAUSE_ENABLE	4
#define RFA_GLOBAL_PLAYPAUSA_MORESHOT_COUNTER	5
#define RFA_GLOBAL_PLAYPAUSA_MORESHOT_ENABLE	6
//fire_pulse_source	
#define RFA_TIMEBASE_SOURCE_REG	7
//timebase_by_rate	
#define RFA_CONSTANTRATE_REG	8
//timebase_statistic	
#define RFA_STAT_POISSON_P	9
#define RFA_LFSR_SOURCE_CONTROL_TBS	10
#define RFA_LFSR_PC_ENABLE_ETH_TBS	11
#define RFA_RESET_LFSR_ETH_TBS	12
#define RFA_PAR_FLAG_REG	13
#define RFA_DEADTIME_REG	14
#define RFA_SEME1_TBS	15
#define RFA_SEME2_TBS	16
//spectrum_modulator	
#define RFA_ENERGY_SOURCE_REG	17
#define RFA_CONSTANT_ENERGY_REG	18
#define RFA_PROG_MODE	19
#define RFA_SCALE_FACTOR	20
#define RFA_RESET_LFSR_ETH_SP_MOD	21
#define RFA_LFSR_PC_ENABLE_ETH_SP_MOD	22
#define RFA_LFSR_SOURCE_CONTROL_SP_MOD	23
#define RFA_SEME1_SP_MOD	24
#define RFA_SEME2_SP_MOD	25

// Default values
//top
#define	RFD_TIMEBASE_SOURCE_REG					00
#define	RFD_CONSTANTRATE_REG					0x00F00000
#define	RFD_GLOBAL_LFSR_RESET_ETH				0
#define	RFD_GLOBAL_LFSR_ENABLE_ETH				1
#define	RFD_GLOBAL_LFSR_SOURCE_CONTROL			00
#define	RFD_GLOBAL_LFSR_ONESHOT					0
#define	RFD_GLOBAL_PLAYPAUSE_ENABLE				1
#define	RFD_GLOBAL_PLAYPAUSA_MORESHOT_COUNTER	0x00000000
#define	RFD_GLOBAL_PLAYPAUSA_MORESHOT_ENABLE	0
#define	RFD_ENERGY_SOURCE_REG					01
#define	RFD_CONSTANT_ENERGY_REG					0x7FFF
//timebase_statistic
#define	RFD_STAT_POISSON_P						0x00400000
#define	RFD_LFSR_SOURCE_CONTROL_TBS				00
#define	RFD_LFSR_PC_ENABLE_ETH_TBS				1
#define	RFD_RESET_LFSR_ETH_TBS					0
#define	RFD_PAR_FLAG_REG						0
#define	RFD_DEADTIME_REG						0x0000
#define	RFD_SEME1_TBS							0x166C3967
#define	RFD_SEME2_TBS							0x1AF0F0E9
//spectrum modulator		
#define	RFD_PROG_MODE							0
#define	RFD_SCALE_FACTOR						00
#define	RFD_RESET_LFSR_ETH_SP_MOD				0
#define	RFD_LFSR_PC_ENABLE_ETH_SP_MOD			1
#define	RFD_LFSR_SOURCE_CONTROL_SP_MOD			00
#define	RFD_SEME1_SP_MOD						0x2CD872CE
#define	RFD_SEME2_SP_MOD						0x35E1E1D2

unsigned int REGISTER_MEMMAP[REG_COUNT];
unsigned int _REGISTER_MEMMAP[REG_COUNT];

uDDE_API::uDDE_API()
{
	return;
}

uDDE_API::~uDDE_API()
{
		return;
}


	void uDDE_API::Allocate()
	{
		niHAL = new NIHAL::NI_HAL();
		return;
	}

	void uDDE_API::Destroy()
	{
		niHAL->CloseConnection();
		delete niHAL;
	}


NI_RESULT uDDE_API::USBEnumerate(tUSBDevice *pvArg1, unsigned int *numDevs)
{
	return niHAL->USBEnumerate(pvArg1, numDevs);
}

NI_RESULT uDDE_API::ETHEnumerate(tETHDevices *pvArg1)
{
	return niHAL->ETHEnumerate(pvArg1);
}

NI_RESULT uDDE_API::ETHGetSerialNumber(short int port, char *IPAddress, char *SN)
{
	return niHAL->ETHGetSerialNumber(port,IPAddress,SN);
}

NI_RESULT uDDE_API::ReadHardwareProperties()
{
	unsigned int TEMP;
	float t;
	unsigned int OFS;
	int i;

	if (niHAL->ReadReg (&TEMP, RFA_INFO_HWREV) != NI_OK)
		return NI_ERROR ;
	if (TEMP>>24 & 0xF != 0xA)
		return NI_INVALID_HARDWARE;


	if (niHAL->ReadReg (&TEMP, RFA_INFO_CLKFREQ)!= NI_OK)
		return NI_ERROR ;
	_clockFREQUENCY = TEMP;
	_clockFREQUENCY=100000000;


	if (niHAL->ReadReg (&TEMP, RFA_INFO_CHANNELCOUNT)!= NI_OK)
		return NI_ERROR ;
	_ChannelsCount = TEMP;

	if (niHAL->ReadReg (&TEMP, RFA_INFO_CHANNELOFFSET)!= NI_OK)
		return NI_ERROR ;
	OFS = TEMP;
	for (i=0;i<_ChannelsCount;i++)
	{
		if (niHAL->ReadReg (&TEMP, RFA_INFO_gOFFSET+i*OFS)!= NI_OK)
			return NI_ERROR ;
		memcpy(&t, &TEMP,4);
		if (TEMP!=0xFFFFFFFF)
			_gOFFSET[i] = (double)t;
		else
			_gOFFSET[i] = 0;

		if (niHAL->ReadReg (&TEMP, RFA_INFO_gGAIN+i*OFS)!= NI_OK)
			return NI_ERROR ;
		memcpy(&t, &TEMP,4);
		if (TEMP!=0xFFFFFFFF)
			_gGAIN[i]= (double)t;
		else
			_gGAIN[i]= 1;
		
		if (niHAL->ReadReg (&TEMP, RFA_INFO_gCTV+i*OFS)!= NI_OK)
			return NI_ERROR ;
		memcpy(&t, &TEMP,4);
		if (TEMP!=0xFFFFFFFF)
			_VoltageChannelFactor[i]= (double)t;
		else
		{
			TEMP= 0x3A83126F;
			memcpy(&t, &TEMP,4);
			_VoltageChannelFactor[i]= (double) t;
		}
	}

	

	return NI_OK;
}
NI_RESULT uDDE_API::EthernetConnect(short int port, char *IPAddress)
{
	NI_RESULT Status = niHAL->EthernetConnect(port, IPAddress);
	if (Status == NI_CONNECTED)
		{
			niHAL->WriteReg(0, 0);
			ReadHardwareProperties();

			initializeDAC(0);
			return NI_OK;
		}
		else
			return NI_ERROR;
}

NI_RESULT uDDE_API::USBConnect(char *SN)
{	int i;
	
	NI_RESULT Status = niHAL->USBConnect(SN);

	


	if (Status == NI_CONNECTED)
	{
		niHAL->WriteReg(0, 0);
		ReadHardwareProperties	();

		initializeDAC(0);
		return NI_OK;
	}
		else
			return NI_ERROR;
}

NI_RESULT uDDE_API::CloseConnection()
{
	return niHAL->CloseConnection();
}

NI_RESULT uDDE_API::ConnectionStatus()
{
	return niHAL->ConnectionStatus();
}

NI_RESULT uDDE_API::DHA_WriteReg(UINT32 value, UINT32 address)
{
	NI_STATUS Status;
		Status = niHAL->WriteReg(value,address);
	return Status;
}

NI_RESULT uDDE_API::DHA_ReadReg(UINT32 *value, UINT32 address)
{
	NI_STATUS Status;
	Status = niHAL->ReadReg(value,address);
	return Status;
}

NI_RESULT uDDE_API::DHA_WriteArray(UINT32 *value, UINT32 address, UINT32 length)
{
	NI_STATUS Status;
	Status = niHAL->WriteArray(value,address,length);
	return Status;

}

NI_RESULT uDDE_API::DHA_ReadArray(UINT32 *value, UINT32 address, UINT32 length)
{
	NI_STATUS Status;
	Status = niHAL->ReadArray(value,address,length);
	return Status;

}


NI_RESULT uDDE_API::SetuDDERegister(UINT32 value, UINT32 address)
{
	_REGISTER_MEMMAP[address] = value;
	return NI_OK;
}

NI_RESULT uDDE_API::GetuDDERegister(UINT32 *value, UINT32 address)
{
	*value=_REGISTER_MEMMAP[address];
	return NI_OK;
}



/*
	CONFIGURE CHANNEL
	enable			0					disabled
					1					enabled
*/


void uDDE_API::programReg(unsigned int channel, unsigned int addr, unsigned int val)
{

		//Switch on DAC
	if(niHAL->WriteReg((addr << 16) + val, RFA_DACCNRL_REG + __CHOFFSET * channel) != NI_OK)
		return ;
	if(niHAL->WriteReg(0x1, RFA_DACCNRL_START + __CHOFFSET * channel) != NI_OK)
		return ;
	if(niHAL->WriteReg(0x6, RFA_DACCNRL_START + __CHOFFSET * channel) != NI_OK)
		return ;

}

NI_RESULT uDDE_API::initializeDAC(unsigned int channel)
{
	//Reset DAC
	if(niHAL->WriteReg(0x6, RFA_DACCNRL_START + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;
	if(niHAL->WriteReg(0x00, RFA_DACCNRL_RESET + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;
	if(niHAL->WriteReg(0x01, RFA_DACCNRL_RESET + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;


	programReg (channel,0x0, 0x4664);
	programReg (channel,0x1, 0x600E);
	programReg (channel,0x2, 0x3FFF);
	programReg (channel,0x3, 0x1C70);
	programReg (channel,0x8, 0x6000);
	programReg (channel,0x9, 0x8000);
	programReg (channel,0xA, 0xF080);
	programReg (channel,0x14, 0x0000);


	
	//Disable the sync channels
	if(niHAL->WriteReg(0xC, 0xFF0100 + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;
	if(niHAL->WriteReg(0x4, 0xFF0100 + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;


}

NI_RESULT uDDE_API::EnableChanel(unsigned int channel, unsigned int ENABLE)
{
	
	if (ENABLE == 1) 
	{


	
		if(niHAL->WriteReg(0x1, RFA_OFFGAIN_ENABLE + __CHOFFSET * channel) != NI_OK)
			return NI_ERROR;
	}
	else
	{
		if(niHAL->WriteReg(0x0, RFA_OFFGAIN_ENABLE + __CHOFFSET * channel) != NI_OK)
			return NI_ERROR;
	}





	return NI_OK;
}



/*
	SETUP ENERGY EMULATION MODE
	mode			0					fixed energy
					1					spectrum
	energy			0 .. 2^15 e			fixed energy amplitude
*/
NI_RESULT uDDE_API::ConfigureEnergy(unsigned int channel, unsigned int MODE, unsigned int ENERGY)
{
	NI_STATUS Status;

	if(niHAL->WriteReg(MODE, RFA_ENERGY_FIXSPE + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;
	
	if(niHAL->WriteReg(ENERGY, RFA_ENERGY_FIXENERGY + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;

	return NI_OK;
}


/*
	SETUP ENERGY EMULATION MODE
	spectrum		*					spectrum
	spectrumLen							length of the spectrum vector
*/

NI_RESULT uDDE_API::ProgramSpectrum(unsigned int channel, unsigned int *spectrum, unsigned int spectrumLen)
{
	int i;
	unsigned int q;
	double max;
	double * cumulativo  = new double [32000];
	unsigned int * Icumulativo  = new unsigned int [32000];
	q=spectrum[0];
	cumulativo[0] = 0;
	for (i=1;i<16384;i++)
	{
		if (spectrumLen > i)
		{
			q=spectrum[i];
		}
		else
		{
			q=0;
		}

		cumulativo[i] = cumulativo[i-1] + q;
	}

	max = cumulativo[16383];

	for (i=1;i<16384;i++)
	{
		cumulativo[i] =  (( cumulativo[i] /  max) * (pow(2.0,32)-1));
		Icumulativo[i] = (unsigned int) cumulativo[i];
	}

	
	if(niHAL->WriteReg(1, RFA_ENERGY_PROGMODE + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;
	
	if(niHAL->WriteArray(Icumulativo, RFA_ENERGY_MEMBA + __CHOFFSET * channel, 16384) != NI_OK)
		return NI_ERROR;

	if(niHAL->WriteReg(0, RFA_ENERGY_PROGMODE + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;
	


	return NI_OK;
}


/*
	SETUP TIMEBASE
	mode			0					fixed rate
					1					poisson
					2					external trigger
	rate			0 .. 1 MHz 			rate of the pulses
*/

NI_RESULT uDDE_API::ConfigureTimebase(unsigned int channel, unsigned int MODE, unsigned int RATE)
{
	NI_STATUS Status;

	double fRATE;
	double pRATE;

	fRATE = _clockFREQUENCY/RATE; 
	pRATE = (RATE * (1.0 / _clockFREQUENCY)) * pow(2.0, 32);

	if(niHAL->WriteReg(MODE, RFA_TIMEBASE_MUXSOURCE + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;

	if(niHAL->WriteReg(fRATE, RFA_TIMEBASE_RATE + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;

	if(niHAL->WriteReg(pRATE, RFA_TIMEBASE_POISSONP + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;

	return NI_OK;
}

/*
	SETUP DIGITAL CONNECTOR
	outtriggered	0					disabled
					1					enabled
	outtriggerlen	0... 10us			ns pulse len
					

*/

NI_RESULT uDDE_API::ConfigureDIO(unsigned int channel, unsigned int OUTTRIGGEREN, unsigned int OUTTRIGGERLEN)
{
	if(niHAL->WriteReg(OUTTRIGGEREN, RFA_TIMEBASE_TRIGGEROUTEN + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;

	int length;

	length = _clockFREQUENCY * ( ((double)OUTTRIGGERLEN) * 10e-9);
	if (length < 1)
		length = 1;

	if(niHAL->WriteReg(length, RFA_TIMEBASE_TRIGGEROUTLEN + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;

	return NI_OK;
}


/*
	SETUP DIGITAL RC
	risetime		0 .. 10us			risetime in ns
	falltime		0 .. 10ms			falltime in ns
*/

NI_RESULT uDDE_API::ConfigureDRC(unsigned int channel, unsigned int RISETIME, unsigned int FALLTIME)
{
	double AGain, BGain; 
	double AZerosI[4], BZerosI[4];
	double AZerosR[4], BZerosR[4];
	int AnZeros, BnZeros;
	double APolesI[4], BPolesI[4];
	double APolesR[4], BPolesR[4];
	double gain1;
	double gain2;
	double a1, a2;
	double scale;
	int AnPoles, BnPoles;
	UINT64 beta11, beta12, alfa11, beta21, beta22, alfa21, gain21;
	unsigned int msb, lsb;

	CalculateFilter((double)FALLTIME/1000.0, _clockFREQUENCY, &AGain, AZerosI, AZerosR, &AnZeros, APolesI, APolesR, &AnPoles);
	a1 = APolesR[0];
	gain1 = 2.0 / AGain;
	if (RISETIME > 0.008)
	{
		CalculateFilter((double)RISETIME/1000.0, _clockFREQUENCY, &BGain, BZerosI, BZerosR, &BnZeros, BPolesI, BPolesR, &BnPoles);
		a2 = BPolesR[0];
		gain2 = 2.46/BGain;
	}
	else
	{
		a2=0;
		gain2 = 1;
	}
	scale = (double)pow(2.0,48.0);
	beta11 =(UINT64) ((double)pow(a1,1) * scale);
	beta12 =(UINT64) ((double)pow(a1,2) * scale);
	alfa11 =(UINT64) ((double)pow(a1,3) * scale);

	scale = (double)pow(2.0,47.0);
	beta21 =(UINT64) ((double)pow(a2,1) * scale);
	beta22 =(UINT64) ((double)pow(a2,2) * scale);
	alfa21 =(UINT64) ((double)pow(a2,3) * scale);
	gain21 =(UINT64) ((double)gain2 * scale);

	msb = (beta11>>32) & 0xFFFFFFFF;
	lsb = (beta11) & 0xFFFFFFFF;
	if(niHAL->WriteReg(msb, RFA_DRC_BETA11A + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;
	if(niHAL->WriteReg(lsb, RFA_DRC_BETA11B + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;

	msb = (beta12>>32) & 0xFFFFFFFF;
	lsb = (beta12) & 0xFFFFFFFF;
	if(niHAL->WriteReg(msb, RFA_DRC_BETA12A + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;
	if(niHAL->WriteReg(lsb, RFA_DRC_BETA12B + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;

	msb = (alfa11>>32) & 0xFFFFFFFF;
	lsb = (alfa11) & 0xFFFFFFFF;
	if(niHAL->WriteReg(msb, RFA_DRC_ALFA11A + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;
	if(niHAL->WriteReg(lsb, RFA_DRC_ALFA11B + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;

	msb = (beta21>>32) & 0xFFFFFFFF;
	lsb = (beta21) & 0xFFFFFFFF;
	if(niHAL->WriteReg(msb, RFA_DRC_BETA21A + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;
	if(niHAL->WriteReg(lsb, RFA_DRC_BETA21B + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;

	msb = (beta22>>32) & 0xFFFFFFFF;
	lsb = (beta22) & 0xFFFFFFFF;
	if(niHAL->WriteReg(msb, RFA_DRC_BETA22A + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;
	if(niHAL->WriteReg(lsb, RFA_DRC_BETA22B + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;

	msb = (alfa21>>32) & 0xFFFFFFFF;
	lsb = (alfa21) & 0xFFFFFFFF;
	if(niHAL->WriteReg(msb, RFA_DRC_ALFA21A + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;
	if(niHAL->WriteReg(lsb, RFA_DRC_ALFA21B + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;

	msb = (gain21>>32) & 0xFFFFFFFF;
	lsb = (gain21) & 0xFFFFFFFF;
	if(niHAL->WriteReg(msb, RFA_DRC_GAIN21A + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;
	if(niHAL->WriteReg(lsb, RFA_DRC_GAIN21B + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;

	if(niHAL->WriteReg(1, RFA_DRC_RESET + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;
	if(niHAL->WriteReg(0, RFA_DRC_RESET + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;
	return NI_OK;
}

/*
	SETUP DIGITAL RC
	gauss		0 ... 100				magnitude
	drift		0 ... 100				magnitude
*/

NI_RESULT uDDE_API::ConfigureNOISE(unsigned int channel, unsigned int GAUSSM, unsigned int DRIFTM)
{
	NI_STATUS Status;

	if(niHAL->WriteReg(GAUSSM, RFA_NOISE_GAUSSM + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;

	if(niHAL->WriteReg(DRIFTM, RFA_NOISE_RWM + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;

	return NI_OK;
}


/*
	SETUP GENERAL
	gain			0..4				
	offset			-2^15 .. 2^15
	invert			0 / 1
					

*/

NI_RESULT uDDE_API::ConfigureGeneral(unsigned int channel, double gain, int offset, unsigned int invert, unsigned int OUTFILTER)
{
	NI_STATUS Status;
	unsigned int gainn;
	gainn = (unsigned int)(gain * 8192);

	if(niHAL->WriteReg(gainn, RFA_OFFGAIN_GAIN + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;

	if(niHAL->WriteReg(offset, RFA_OFFGAIN_OFFSET + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;

	if(niHAL->WriteReg(invert, RFA_OFFGAIN_INVERT + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;

	if(niHAL->WriteReg(OUTFILTER, RFA_OFFGAIN_OUTFILTER + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;
	return NI_OK;
}


/*
	CH TO VOLTAGE
	channel		0 ... 2^15					
*/

double uDDE_API::ChannelsToVoltage(int channel, int channels)
{
	return _VoltageChannelFactor[channel] * channels;
}



/*
	VOLTAGE TO CHANNEL
	channel		0 ... 2^15					
*/

unsigned int uDDE_API::VoltageToChannels(unsigned int channel, double voltage)
{
	return voltage / _VoltageChannelFactor[channel] ;
}


/*
	GET STATS
	cps				counts per second
	ccounter		general event counter
	sat				saturation counter
	overflow		non poisson event generated
*/

NI_RESULT uDDE_API::GetStat(unsigned int channel, unsigned int *cps,  unsigned int *live, UINT64 *ccounter, unsigned int *overflow)
{
	unsigned int temp, temp2;
	if(niHAL->ReadReg(&temp, RFA_MON_CPSR + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;
	*cps = temp;

	if(niHAL->ReadReg(&temp, RFA_MON_CPSLIVER + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;
	*live = temp;

	if(niHAL->ReadReg(&temp, RFA_MON_CPSR_CCOUNTER1 + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;
	if(niHAL->ReadReg(&temp2, RFA_MON_CPSR_CCOUNTER2 + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;
	*ccounter = temp + temp2 << 32;

	if(niHAL->ReadReg(&temp, RFA_MON_CPSR_CCOUNTER1 + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;
	*overflow = temp;

	return NI_OK;
}


/*
	RESETS THE USER COUNTER

	*/
NI_RESULT uDDE_API::ResetCCounter(unsigned int channel)
{
	if(niHAL->WriteReg(1, RFA_MON_CPSR_RESET + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;
	if(niHAL->WriteReg(0, RFA_MON_CPSR_RESET + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;
	
	return NI_OK;
}


/*
	GET AN OUTPUT SHAPE
	data			vector with downloaded shape
	len				length of the data downloaded
	*/
NI_RESULT uDDE_API::GetSignalLoopback(unsigned int channel, INT32 *data, unsigned int *len)
{
	int i;
	unsigned int temp;
	int st;
	unsigned int * buffer  = new unsigned int [20000];
	if(niHAL->ReadReg(&temp, RFA_MON_CPSR_DATAREADY + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;
	
	if (temp & 0x01 == 1)
	{
		if(niHAL->ReadArray(buffer, RFA_MON_MEMBA + __CHOFFSET * channel, 8192) != NI_OK)
			return NI_ERROR;
		if(niHAL->WriteReg(1, RFA_MON_RESTOREACQ + __CHOFFSET * channel) != NI_OK)
			return NI_ERROR;
		if(niHAL->WriteReg(0, RFA_MON_RESTOREACQ + __CHOFFSET * channel) != NI_OK)
			return NI_ERROR;

		for (i=0;i<8192;i++)
		{
			st = (INT32) (buffer[i] & 0xFFF);
			if (st>0x7FF)
				data[i] = -st+0xFFF;
			else
				data[i] = -st;
		}
		*len = 8192;
		delete [] buffer;
		return NI_OK;
	}
	else
		return NI_NO_DATA_AVAILABLE;
	
}


NI_RESULT uDDE_API::ResetSpectrum(unsigned int channel)
{
	if(niHAL->WriteReg(1, RFA_SPELOOP_RESETSM + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;
	if(niHAL->WriteReg(0, RFA_SPELOOP_RESETSM + __CHOFFSET * channel) != NI_OK)
		return NI_ERROR;
	
	return NI_OK;
}




/*
	GET SPECTRUM GENERATED
	data			vector with downloaded shape
	len				length of the data downloaded
	*/
NI_RESULT uDDE_API::GetSpectrumLoopback(unsigned int channel, UINT32*data, unsigned int *len)
{
	int i;
	unsigned int * buffer  = new unsigned int [5000];

	if(niHAL->ReadArray(buffer, RFA_SPELOOP_MEMBA + __CHOFFSET * channel, 1024) != NI_OK)
			return NI_ERROR;

	for (i=0;i<1024;i++)
		data[i] = (UINT32)buffer[i];

	*len = 1024;
	delete [] buffer;
	return NI_OK;

}


/*
	ACTIVATE GET UID
	*data			vector with UID
	*/
NI_RESULT uDDE_API::ActiveteGetUID(unsigned int *key1, unsigned int *key2)
{
	NI_STATUS Status;
	unsigned int v;
	if (niHAL->ReadReg (&v, RFA_ICAPDNA_UID1)!= NI_OK)
		return NI_ERROR ;
	*key1 = v;
	if (niHAL->ReadReg (&v, RFA_ICAPDNA_UID2)!= NI_OK)
		return NI_ERROR ;
	*key2 = v;
	return NI_OK;

}

/*
	ACTIVATE PROGRAM KEY
	*data			vector with the key
	*/
NI_RESULT uDDE_API::ActiveteRegister(unsigned char *data)
{
	int i;


	NI_STATUS Status;

	return NI_OK;

}

NI_RESULT uDDE_API::IsActivated(unsigned int *data, unsigned int *expire_time)
{
	unsigned int v;
	if (niHAL->ReadReg (&v, RFA_ICAPDNA_ENABLE)!= NI_OK)
		return NI_ERROR ;

	if ( (v & 0x01) == 1)
		*data = 1;
	else
		*data = 0;
	NI_STATUS Status;

	if (niHAL->ReadReg (&v, RFA_ICAPDNA_ENABLE)!= NI_OK)
		return NI_ERROR ;

	*expire_time = (v >> 2) & 0xFFFF;

	return NI_OK;

}

/*
	GET HARDWARE INFORMATION

	*/

NI_RESULT uDDE_API::HardwareInfo(unsigned int *HWREV, unsigned int *FWREV, unsigned int *HWOPTIONS)
{

	if (niHAL->ReadReg (HWREV, RFA_INFO_HWREV)!= NI_OK)
		return NI_ERROR ;

	if (niHAL->ReadReg (FWREV, RFA_INFO_SWREV)!= NI_OK)
		return NI_ERROR ;

	if (niHAL->ReadReg (HWOPTIONS, RFA_INFO_HWOPT)!= NI_OK)
		return NI_ERROR ;

	NI_STATUS Status;

	return NI_OK;
}



/*
	Reboot
		mode	0		standard mode
				1		bootloader
*/
#define RFA_ICAPDNA_REBOOTADDR			0x00000700
#define RFA_ICAPDNA_UNLOCK				0x00000701
#define RFA_ICAPDNA_nREBOOT				0x00000702
NI_RESULT uDDE_API::Reboot(unsigned int mode)
{
	NI_STATUS Status;
	UINT32 address;
	if (mode == 1)
	{
		if(niHAL->WriteReg(0x10000, RFA_ICAPDNA_REBOOTADDR ) != NI_OK)
			return NI_ERROR;
	}
	else
	{
		if(niHAL->WriteReg(0x400000, RFA_ICAPDNA_REBOOTADDR ) != NI_OK)
			return NI_ERROR;

	}

	if(niHAL->ReadReg(&address, RFA_ICAPDNA_REBOOTADDR ) != NI_OK)
		return NI_ERROR;


	if(niHAL->WriteReg(0xABBA5511, RFA_ICAPDNA_UNLOCK ) != NI_OK)
		return NI_ERROR;

	if(niHAL->WriteReg(0xABBA5511, RFA_ICAPDNA_nREBOOT ) != NI_OK)
		return NI_ERROR;

	return NI_OK;
}

/*
	CheckRunningMode
		mode	0		standard mode
				1		bootloader
*/

NI_RESULT uDDE_API::CheckRunningMode(unsigned int *mode)
{
	NI_STATUS Status;
	unsigned int HWOPTIONS;

	if (niHAL->ReadReg (&HWOPTIONS, 0xFFFE0006)!= NI_OK)
		return NI_ERROR ;

	if (HWOPTIONS >> 24 == 0x5B)
		*mode = 0;
	else
		if (HWOPTIONS >> 24 == 0x5A)
		*mode = 1;
		else
			*mode = 2;
	

	return NI_OK;
}

/*
	Lock/Unlock Flash
		mode	0		locked
				1		unlocked
*/

NI_RESULT uDDE_API::FlashLock(unsigned int mode)
{
	NI_STATUS Status;

	return NI_OK;
}

/*
	Flash write page
		pagen			page number
		data			page content
		datalen			page len
*/

NI_RESULT uDDE_API::FLASHWritePage(unsigned int pagen, unsigned char *data, unsigned int len)
{
	NI_STATUS Status;

	return NI_OK;
}


/*
	Flash erase page
		pagen			page number
*/

NI_RESULT uDDE_API::FLASHErasePage(unsigned int pagen)
{
	NI_STATUS Status;

	return NI_OK;
}



/*
	Write Security EEPROM
*/

NI_RESULT uDDE_API::SECWriteWord(unsigned int channel, unsigned int address, unsigned int word)
{
	if(niHAL->WriteReg(0, RFA_ICAPDNA_WF) != NI_OK)
		return NI_ERROR;
	Sleep(10);
	if(niHAL->WriteReg(1 << 24, RFA_ICAPDNA_EEPROM ) != NI_OK)
		return NI_ERROR;
	Sleep(10);
	if(niHAL->WriteReg(1, RFA_ICAPDNA_WF) != NI_OK)
		return NI_ERROR;
	if(niHAL->WriteReg(0, RFA_ICAPDNA_WF) != NI_OK)
		return NI_ERROR;
	Sleep (50);
	if(niHAL->WriteReg((address << 16) + (2 << 24), RFA_ICAPDNA_EEPROM ) != NI_OK)
		return NI_ERROR;
	Sleep(50);
	if(niHAL->WriteReg(1, RFA_ICAPDNA_WF ) != NI_OK)
		return NI_ERROR;
	if(niHAL->WriteReg(0, RFA_ICAPDNA_WF ) != NI_OK)
		return NI_ERROR;
	Sleep (50);

	if(niHAL->WriteReg(word + (address << 16) + (4 << 24), RFA_ICAPDNA_EEPROM ) != NI_OK)
		return NI_ERROR;
	Sleep(10);
	if(niHAL->WriteReg(1, RFA_ICAPDNA_WF ) != NI_OK)
		return NI_ERROR;
	if(niHAL->WriteReg(0, RFA_ICAPDNA_WF ) != NI_OK)
		return NI_ERROR;
	Sleep (50);
	
	return NI_OK;
}



/*
	Read Security EEPROM
*/

NI_RESULT uDDE_API::SECReadWord(unsigned int channel, unsigned int address, unsigned int *word)
{
	unsigned int w;
	if(niHAL->WriteReg(0, RFA_ICAPDNA_WF) != NI_OK)
		return NI_ERROR;

	if(niHAL->WriteReg((address << 16) + (8 << 24), RFA_ICAPDNA_EEPROM ) != NI_OK)
		return NI_ERROR;

	if(niHAL->WriteReg(1, RFA_ICAPDNA_WF ) != NI_OK)
		return NI_ERROR;
	if(niHAL->WriteReg(0, RFA_ICAPDNA_WF ) != NI_OK)
		return NI_ERROR;

	Sleep (10);

	if(niHAL->ReadReg(&w, RFA_ICAPDNA_EEPROM ) != NI_OK)
		return NI_ERROR;
	if(niHAL->WriteReg(0, RFA_ICAPDNA_WF) != NI_OK)
		return NI_ERROR;

	if(niHAL->WriteReg(1, RFA_ICAPDNA_WF) != NI_OK)
		return NI_ERROR;
	if(niHAL->WriteReg(0, RFA_ICAPDNA_WF) != NI_OK)
		return NI_ERROR;

	*word = w & 0xFFFF;
	
	return NI_OK;
}


NI_RESULT uDDE_API::WriteCalibrarionFlash(unsigned int channel, float offset, float gain, float chctv)
{
	unsigned int TEMP;

	memcpy(&TEMP, &gain, 4);
	if(SECWriteWord(0, 16 + 0 + 6*channel, (TEMP>>0) & 0xFFFF))
		return NI_ERROR;
	if(SECWriteWord(0, 16 + 1 + 6*channel, (TEMP>>16) & 0xFFFF))
		return NI_ERROR;

	memcpy(&TEMP, &offset, 4);
	if(SECWriteWord(0, 16 + 2 + 6*channel, (TEMP>>0) & 0xFFFF))
		return NI_ERROR;
	if (SECWriteWord(0, 16 + 3 + 6*channel, (TEMP>>16) & 0xFFFF))
		return NI_ERROR;

	memcpy(&TEMP, &chctv, 4);
	if (SECWriteWord(0, 16 + 4 + 6*channel, (TEMP>>0) & 0xFFFF))
		return NI_ERROR;
	if (SECWriteWord(0, 16 + 5 + 6*channel, (TEMP>>16) & 0xFFFF))
		return NI_ERROR;

	return NI_OK;
}