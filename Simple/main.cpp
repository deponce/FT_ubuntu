/*
	Simple example to open a maximum of 4 devices - write some data then read it back.
	Shows one method of using list devices also.
	Assumes the devices have a loopback connector on them and they also have a serial number

	To build use the following gcc statement 
	(assuming you have the d2xx library in the /usr/local/lib directory).
	gcc -o simple main.c -L. -lftd2xx -Wl,-rpath /usr/local/lib
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "ftd2xx.h"
#include <iostream>
#include <chrono>
#define BUF_SIZE 0x10

#define MAX_DEVICES	1000
#define OneSector 8
int main(int argc, char *argv[]){
	system("sudo rmmod ftdi_sio");
	system("sudo rmmod usbserial");
	FT_STATUS ftStatus;
	FT_HANDLE ftHandle0;
	FT_HANDLE ftHandle1;
	UCHAR Mask = 0xff;
	UCHAR Mode;
	UCHAR LatencyTimer = 2; //our default setting is 16
	// size_t i;
	int retCode = EXIT_FAILURE;
	DWORD libVersion = 0;
	// DWORD Flags;
	// DWORD Type;
	// DWORD ID;
	// DWORD LocID;
	// char SerialNumber[16];
	// char Description[64];
	// DWORD standardDevices = 0;
	// DWORD totalDevice = 0;
	static FT_PROGRAM_DATA Data;
	char *BufPtrs[3]; // pointer to array of 3 pointers
	char Buffer1[64]; // buffer for description of first device
	char Buffer2[64]; // buffer for description of second device
	
	BufPtrs[0] = Buffer1;
	BufPtrs[1] = Buffer2;
	BufPtrs[2] = NULL; // last entry should be NULL

	UCHAR rxBuffer[65536] = {0};
	// memset(rxBuffer,0,1028);
	DWORD byteCount;
	auto start = std::chrono::high_resolution_clock::now();
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	double mbps;
	double secondsElapsed;
	// DWORD devIndex = 0; // first device
	// char Buffer[64]; // more than enough room!

	DWORD RxBytes = 8;
	DWORD BytesReceived;
	DWORD numDevs;
	// char RxBuffer[OneSector];
	ftStatus = FT_GetLibraryVersion(&libVersion);
	if (ftStatus == FT_OK)
	{
		printf("D2xx Library version = 0x%x\n", (unsigned int)libVersion);
	}
	else
	{
		printf("Error reading library version.\n");
		return 1;
	}

	ftStatus = FT_ListDevices(BufPtrs,&numDevs,FT_LIST_ALL|FT_OPEN_BY_DESCRIPTION);
	if (ftStatus == FT_OK) {
	// FT_ListDevices OK, product descriptions are in Buffer1 and Buffer2, and
	// numDevs contains the number of devices connected
	if(numDevs<=0){
		goto exit;
	}
	printf("%s\n", Buffer1);
	printf("%s\n", Buffer2);
	printf("Number of ports: %d\n", numDevs);
	}
	else {
	// FT_ListDevices failed
	}
	ftStatus = FT_Open(0, &ftHandle0);
	if(ftStatus != FT_OK) {
		printf("FT_Open(%d) failed\n", 0);}
	else{
		printf("Port 0 FT_Open succeeded.  Handle is %p\n", ftHandle0);}
	ftStatus = FT_SetTimeouts(ftHandle0, 500, 500);
	ftStatus = FT_Open(1, &ftHandle1);
	if(ftStatus != FT_OK) {
		printf("FT_Open(%d) failed\n", 1);}
	else{
		printf("Port 1 FT_Open succeeded.  Handle is %p\n", ftHandle1);}

	Data.Signature1 = 0x00000000;
	Data.Signature2 = 0xffffffff;
	Data.Manufacturer = (char *)malloc(256); /* E.g "deponce" */
	Data.ManufacturerId = (char *)malloc(256); /* E.g. "FT" */
	Data.Description = (char *)malloc(256); /* E.g. "USB HS Serial Converter" */
	Data.SerialNumber = (char *)malloc(256); /* E.g. "FT000001" if fixed, or NULL */
	if (Data.Manufacturer == NULL ||
	    Data.ManufacturerId == NULL ||
	    Data.Description == NULL ||
	    Data.SerialNumber == NULL)
	{
		printf("Failed to allocate memory.\n");
		retCode = 1;
		goto exit;
	}
	ftStatus = FT_EE_Read(ftHandle0, &Data);
	if(ftStatus != FT_OK) {
		printf("FT_EE_Read failed\n");
		retCode = 1;
		goto exit;
	}
	printf("FT_EE_Read succeeded.\n\n");
	printf("Signature1 = %d\n", (int)Data.Signature1);			
	printf("Signature2 = %d\n", (int)Data.Signature2);
	printf("Version = %d\n", (int)Data.Version);
	printf("VendorId = 0x%04X\n", Data.VendorId);				
	printf("ProductId = 0x%04X\n", Data.ProductId);
	printf("Manufacturer = %s\n", Data.Manufacturer);			
	printf("ManufacturerId = %s\n", Data.ManufacturerId);
	Mode = 0x00; //reset mode
	if(FT_SetBitMode(ftHandle0, Mask, Mode) == FT_OK){
		printf("Reset mode succeeded\n");}
	else{
		printf("Reset mode failed\n"); retCode=1; goto exit;}
	usleep(10);
	Mask = 0x00;
	Mode = 0x40;
	if(FT_SetBitMode(ftHandle0, Mask, Mode) == FT_OK){
		printf("set bit mode succeeded\n");}
	else{
		printf("set bit mode failed\n"); retCode=1; goto exit;}
	
	ftStatus = FT_SetLatencyTimer(ftHandle0, LatencyTimer);
	FT_SetUSBParameters(ftHandle0, 65536, 65536);
	FT_SetFlowControl(ftHandle0, FT_FLOW_RTS_CTS, 0, 0);
	// FT_Purge(ftHandle0, FT_PURGE_RX | FT_PURGE_TX);
	
	
	if(ftStatus == FT_OK){std::cout<<"set FT_SetTimeouts success."<<std::endl;}
	start = std::chrono::high_resolution_clock::now();
	for(int i = 0; i < 1; i++){
		// std::cout<<i<<std::endl<<std::flush;
		if(FT_Read(ftHandle0, rxBuffer, 65536, &byteCount) != FT_OK) {
						printf("Error while reading from the device. Exiting.\r\n");
                        goto exit;
                    }
		// printf(byteCount);
		printf("bytes read %d\n", byteCount);
		for(int j=0; j<65536; j++){
			printf("%x ", rxBuffer[j]);
			// std::cout<<"i: "<<rxBuffer[j]<<std::endl;
		}
	}
	std::cout<<std::endl;
	stop = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	std::cout << "Read 1GB from the FTDI in : "
	<< duration.count()/1000000. << " seconds" << std::endl;
	mbps = 8589.934592 / (duration.count()/1000000.);
	printf("Average read speed: %0.1f Mbps.\r\n", mbps);
	exit:
    	return retCode;
}
