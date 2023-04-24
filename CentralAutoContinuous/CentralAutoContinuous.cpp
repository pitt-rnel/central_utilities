// Central Auto Continuous

#include <iostream>
#include "cbsdk.h"
#include "cbhwlib.h"

const char* help_text =
"Central Auto Continuous\n"
"This program takes the following command line arguments:\n"
"- cbSDK Instrument number (default: 0)\n"
"- Sample Rate Index (default: -1 for no change)\n"
"- Filter Index (default: -1 for no change)\n"
"\n"
"Sample Rates:\n"
"[0]: None\n"
"[1]: 500 S/s\n"
"[2]: 1 kS/s\n"
"[3]: 2 kS/s\n"
"[4]: 10 kS/s\n"
"[5]: 30 kS/s\n"
"\n"
"Filters:\n"
"[0]: None\n"
"[1]: HP 750Hz\n"
"[2]: HP 250Hz\n"
"[3]: HP 100Hz\n"
"[4]: LP 50Hz\n"
"[5]: LP 125Hz\n"
"[6]: LP 250Hz\n"
"[7]: LP 500Hz\n"
"[8]: LP 150Hz\n"
"[9]: BP 10Hz-250Hz\n"
"[10]: LP 2.5kHz\n"
"[11]: LP 2kHz\n"
"[12]: BP 250Hz-5kHz\n"
"[13]: User Filter\n"
;

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

		INT32 rate_idx = -1; // -1 means do not change setting
		if (argc > 2)
			rate_idx = atoi(argv[2]);

		INT32 filt_idx = -1; // -1 means do not change setting
		if (argc > 3)
			filt_idx = atoi(argv[3]);
		

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
				if (rate_idx > -1)
					ccf->data.isChan[chan].smpgroup = (UINT32)rate_idx;
				if (filt_idx > -1)
					ccf->data.isChan[chan].smpfilter = (UINT32) filt_idx;
			} // if chancaps
		} // end for chans
		
		//const char *s = (rawFlag ? "Enabled" : "Disabled");
		//printf("%s raw data on %d chans\n", (rawFlag ? "Enabled" : "Disabled"), count);
		if (rate_idx > -1 || filt_idx > -1)
			printf("Modified continuous settings on %d chans\n", count);

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
