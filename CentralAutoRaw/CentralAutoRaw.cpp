// Central Auto Raw

#include <iostream>
#include "cbsdk.h"
#include "cbhwlib.h"

const char* help_text =
"Central Auto Raw\n"
"This program takes the following command line arguments:\n"
"- cbSDK Instrument number (default: 0)\n"
"- Enable/Disable Flag (default: 1)\n";

int main(int argc, char* argv[])
{
	printf("\n"); // print newline after Qt warning.

	try {
		// Process command line arguments

		UINT32 cbInst = 0;
		if (argc > 1) { // cbInst or help request
			if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "help") == 0 || strcmp(argv[1], "?") == 0 || strcmp(argv[1], "/?") == 0) {
				printf(help_text);
				return 0;
			}
			cbInst = atoi(argv[1]);
		}

		UINT8 rawFlag = 1;
		if (argc > 2)
			rawFlag = atoi(argv[2]);
		

		cbSdkResult cbRes;

		cbRes = cbSdkOpen(cbInst);
		if (cbRes == CBSDKRESULT_SUCCESS)
			printf("Connected to Central\n");
		else {
			printf("Error: Failed to open cbSdk, cbRes = %d\n", cbRes);
			return cbRes;
		}

		cbSdkCCF* ccf = new cbSdkCCF();
		cbRes = cbSdkReadCCF(cbInst, ccf,NULL,false);
		if (cbRes == CBSDKRESULT_SUCCESS)
			printf("Read CCF struct\n");
		else {
			printf("Error: Failed to read CCF, cbRes = %d\n", cbRes);
			delete ccf;
			return cbRes;
		}
		
		//printf("CCF, chan[0].ainpopts = 0x%08x\n", ccf->data.isChan[0].ainpopts);
		//printf("CCF, chan[0].chancaps = 0x%08x\n", ccf->data.isChan[0].chancaps);

		//printf("Modifying CCF\n");
		int count = 0;
		for (int chan = 0; chan < cbMAXCHANS; chan++) { // TODO: Option to pick channels or at least ensure these are valid front end channels
			UINT32 chancaps = ccf->data.isChan[chan].chancaps;
			if (chancaps & cbCHAN_EXISTS && chancaps & cbCHAN_CONNECTED && chancaps & cbCHAN_ISOLATED && chancaps & cbCHAN_AINP ) { // need to verify all these flags are correct. I think cbCHAN_ISOLATED distinguished front end channels from non-front end channels
				count++;
				if (rawFlag)
					ccf->data.isChan[chan].ainpopts |= cbAINP_RAWSTREAM_ENABLED;
				else
					ccf->data.isChan[chan].ainpopts &= ~cbAINP_RAWSTREAM_ENABLED;
			} // if chancaps
		} // end for chans
		
		const char *s = (rawFlag ? "Enabled" : "Disabled");
		printf("%s raw data on %d chans\n", (rawFlag ? "Enabled" : "Disabled"), count);

		cbRes = cbSdkWriteCCF(cbInst, ccf, NULL);
		delete ccf;
		if (cbRes == CBSDKRESULT_SUCCESS)
			printf("Wrote CCF struct\n");
		else {
			printf("Error: Failed to write CCF, cbRes = %d\n", cbRes);
			return cbRes;
		}

		cbRes = cbSdkClose(cbInst);
		if (cbRes != CBSDKRESULT_SUCCESS)
			printf("Error: Failed to close cbSDK, cbRes = %d\n", cbRes);
		
	}
	catch (std::exception& e) {
		std::cout << "Exception occurred: " << e.what() << std::endl;
	}
	catch (...) {
		std::cout << "Unknown exception occurred!" << std::endl;
	}

	return 0;
}
