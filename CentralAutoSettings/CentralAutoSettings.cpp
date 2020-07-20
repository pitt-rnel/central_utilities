// Central Auto Settings

#include <iostream>

//#ifndef _WIN32_WINNT    // Allow use of features specific to Windows XP or later.
//#define _WIN32_WINNT 0x0501   // Change this to the appropriate value to target other versions of Windows.
//#endif
#define WIN32_LEAN_AND_MEAN      // Exclude rarely-used stuff from Windows headers
#include <Windows.h>

#include "cbsdk.h"
#include "cbhwlib.h"

const char* help_text =
"Central Auto Settings\n"
"This program takes the following command line arguments:\n"
"- .ccf Filename to read or write (required, no default)\n"
"- cbSDK Instrument number(default: 0)\n"
"- Read/Write Flag: 0 to read .ccf file and apply to NSP, 1 to read settings from NSP and write to .ccf file (default: 0)\n";


int main(int argc, char* argv[])
{
	printf("\n"); // print newline after Qt warning.

	try {
		// Process command line arguments

		char* filepath;
		if (argc > 1) {
			if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "help") == 0 || strcmp(argv[1], "?") == 0 || strcmp(argv[1], "/?") == 0) {
				printf(help_text);
				return 0;
			}
			filepath = argv[1];
		}
		else {
			printf("Error: filename required\n");
			return CBSDKRESULT_INVALIDFILENAME;
		}

		UINT32 cbInst = 0;
		if (argc > 2) { // cbInst
			cbInst = atoi(argv[2]);
		}

		UINT8 writeFlag = 0;
		if (argc > 3)
			writeFlag = atoi(argv[3]);

		cbSdkResult cbRes;
		cbSdkCCF ccf;

		// open SDK
		cbRes = cbSdkOpen(cbInst);
		if (cbRes == CBSDKRESULT_SUCCESS)
			printf("Connected to Central\n");
		else {
			printf("Error: Failed to open cbSdk, cbRes = %d\n", cbRes);
			return cbRes;
		}

		// load or write CCF

		if (writeFlag == 0) { // load mode

			// read CCF from filename
			printf("Reading CCF from file\n");
			cbRes = cbSdkReadCCF(cbInst, &ccf, filepath, true, true);
			if (cbRes == CBSDKRESULT_SUCCESS)
				printf("Successfully loaded CCF struct with version %d \n", ccf.ccfver);
			else {
				printf("Error: Failed to read CCF, cbRes = %d\n", cbRes);
				return cbRes;
			}

		}
		else if (writeFlag == 1) { // write mode

			// read CCF from NSP
			printf("Reading CCF from NSP\n");
			cbRes = cbSdkReadCCF(cbInst, &ccf, NULL, true);
			if (cbRes == CBSDKRESULT_SUCCESS)
				printf("Successfully read CCF struct from NSP with version %d \n", ccf.ccfver);
			else {
				printf("Error: Failed to read CCF, cbRes = %d\n", cbRes);
				return cbRes;
			}

			// write CCF to filename
			printf("Writing CCF to file\n");
			cbRes = cbSdkWriteCCF(cbInst, &ccf, filepath); 
			if (cbRes == CBSDKRESULT_SUCCESS)
				printf("Successfully saved CCF struct\n");
			else {
				printf("Error: Failed to write CCF, cbRes = %d\n", cbRes);
				return cbRes;
			}
		}

		// close SDK
		cbRes = cbSdkClose(cbInst);
		if (cbRes != CBSDKRESULT_SUCCESS) {
			printf("Error: Failed to close cbSDK, cbRes = %d\n", cbRes);
			return cbRes;
		}
		else
			printf("Success.\n");
		
		
	}
	catch (std::exception& e) {
		std::cout << "Exception occurred: " << e.what() << std::endl;
	}
	catch (...) {
		std::cout << "Unknown exception occurred!" << std::endl;
	}

	return 0;
}
