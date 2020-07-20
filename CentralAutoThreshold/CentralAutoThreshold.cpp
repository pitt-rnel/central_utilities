// Central Auto Threshold

#include <iostream>

//#ifndef _WIN32_WINNT    // Allow use of features specific to Windows XP or later.
//#define _WIN32_WINNT 0x0501   // Change this to the appropriate value to target other versions of Windows.
//#endif
#define WIN32_LEAN_AND_MEAN      // Exclude rarely-used stuff from Windows headers
#include <Windows.h>

#include "cbsdk.h"
#include "cbhwlib.h"
#include <cmath>
#include <algorithm>

const char* help_text =
"Central Auto Threshold\n"
"This program takes the following command line arguments:\n"
"- cbSDK Instrument number (default: 0)\n"
"- Threshold Value (RMS multiplier or absolute uV) (default `-4.5`)\n"
"- Absolute Flag (0: RMS, 1 : Absolute uV) (default `0`)\n";

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

		float threshold = -4.5;
		if (argc > 2) { // cbInst
			threshold = atof(argv[2]);
		}

		char absFlag = 0; // 0 for RMS, 1 for absolute uV
		if (argc > 3)
			absFlag = atoi(argv[3]);


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

		// read CCF from NSP
		printf("Reading CCF from NSP\n");
		cbRes = cbSdkReadCCF(cbInst, &ccf, NULL, true);
		if (cbRes == CBSDKRESULT_SUCCESS)
			printf("Successfully read CCF struct with version %d \n", ccf.ccfver);
		else {
			printf("Error: Failed to read CCF, cbRes = %d\n", cbRes);
			return cbRes;
		}

		if (absFlag == 1) {// apply absolute threshold

			int count = 0;
			for (int chan = 0; chan < cbMAXCHANS; chan++) { 
				cbPKT_CHANINFO* chaninfo = &ccf.data.isChan[chan];
				UINT32 chancaps = chaninfo->chancaps;
				if (chancaps & cbCHAN_EXISTS && chancaps & cbCHAN_CONNECTED && chancaps & cbCHAN_ISOLATED && chancaps & cbCHAN_AINP) { // need to verify all these flags are correct. I think cbCHAN_ISOLATED distinguished front end channels from non-front end channels
					count++;
					// convert threshold to INT32 digital value
					INT32 digThresh = (INT32) round( ( (threshold - chaninfo->physcalin.anamin) / 
						(chaninfo->physcalin.anamax - chaninfo->physcalin.anamin) ) 
						* (chaninfo->physcalin.digmax - chaninfo->physcalin.digmin) + chaninfo->physcalin.digmin);
					chaninfo->spkthrlevel = digThresh;
				} // end if chancaps
			} // end for chans

			printf("Applied threshold: %.2f uV\n", threshold);

		} // end absolute threshold
		else if (absFlag == 0) { // apply RMS threshold

			// enable raw data
			cbSdkCCF ccf2;
			memcpy(&ccf2, &ccf, sizeof(ccf)); // partial deep copy of ccf
			memcpy(&ccf2.data, &ccf.data, sizeof(ccf.data));
			// enable raw data using ccf2
			int count = 0;
			for (int chan = 0; chan < cbMAXCHANS; chan++) { // TODO: Option to pick channels or at least ensure these are valid front end channels
				memcpy(&ccf2.data.isChan[chan], &ccf.data.isChan[chan], sizeof(cbPKT_CHANINFO));
				UINT32 chancaps = ccf2.data.isChan[chan].chancaps;
				if (chancaps & cbCHAN_EXISTS && chancaps & cbCHAN_CONNECTED && chancaps & cbCHAN_ISOLATED && chancaps & cbCHAN_AINP) { // need to verify all these flags are correct. I think cbCHAN_ISOLATED distinguished front end channels from non-front end channels
					count++;
					ccf2.data.isChan[chan].smpgroup = 5;
					ccf2.data.isChan[chan].smpfilter = ccf2.data.isChan[chan].spkfilter;
				} // if chancaps
			} // end for chans

			// get filter label
			cbFILTDESC filtdesc;
			cbSdkGetFilterDesc(cbInst, 1, ccf2.data.isChan[0].smpfilter, &filtdesc);

			printf("Enabled 30kHz %s data on %d chans\n", filtdesc.label, count);

			cbRes = cbSdkWriteCCF(cbInst, &ccf2, NULL);
			if (cbRes == CBSDKRESULT_SUCCESS)
				printf("Wrote CCF struct\n\n");
			else {
				printf("Error: Failed to write CCF, cbRes = %d\n", cbRes);
				return cbRes;
			}

			// collect 2 seconds of data
			UINT32 bActive = 1;
			UINT16 begchan = 0; UINT16 endchan = 0;
			UINT16 begmask = 0; UINT16 begval = 0; UINT16 endmask = 0; UINT16 endval = 0;
			bool bDouble = true; // I believe this casts to double but does not convert from digital int16 vals
			UINT32 uWaveforms = 0;
			UINT32 uConts = cbSdk_CONTINUOUS_DATA_SAMPLES;//1024;//2*cbSdk_TICKS_PER_SECOND;//cbSdk_CONTINUOUS_DATA_SAMPLES;
			UINT32 uEvents = 0;
			UINT32 uComments = 0;
			UINT32 uTrackings = 0;
			bool bAbsolute = false;
			UINT32 total_samples = 2 * cbSdk_TICKS_PER_SECOND;

			printf("Setting trial config\n");
			cbRes = cbSdkSetTrialConfig(cbInst, bActive, begchan, begmask, begval, endchan, endmask, endval, bDouble, uWaveforms, uConts, uEvents, uComments, uTrackings, bAbsolute);
			if (cbRes != CBSDKRESULT_SUCCESS)
			{
				printf("Error: Failed to set trial config, cbRes = %d\n", cbRes);
				return cbRes;
			}

			//create structure to hold the data
			cbSdkTrialCont trial;
			//alloc memory for samples
			for (int i = 0; i < cbNUM_ANALOG_CHANS; i++)
			{
				trial.samples[i] = malloc(uConts * sizeof(double));
			}


			bActive = 0;
			UINT32 sleep_dur = 200;
			bool enough_samples = false;
			UINT32 samples_collected = 0;
			while (!enough_samples) { 
				Sleep(sleep_dur);

				// determine if buffer has enough samples
				cbSdkResult cbRes = cbSdkInitTrialData(cbInst, bActive, NULL, &trial, NULL, NULL);
				if (cbRes != CBSDKRESULT_SUCCESS)
				{
					printf("Error: Failed to init trial data, cbRes = %d\n", cbRes);
					return cbRes;
				}

				int chans_with_enough_samples = 0;
				samples_collected = trial.num_samples[0];
				for (int i = 0; i < trial.count; i++) {
					samples_collected = min(samples_collected, trial.num_samples[i]);
					if (trial.num_samples[i] > total_samples)
						chans_with_enough_samples++;
				}
				enough_samples = (samples_collected > total_samples);

				if (!enough_samples)
					printf("Collecting trial data: %d chans, %d samples, %d rate\n", trial.count, samples_collected, trial.sample_rates[0]);

				if (trial.num_samples[0] > 0.95 * total_samples)
					sleep_dur = 10; // reduce sleep duration to 10 ms when we only have 10% of samples left
			}

			cbRes = cbSdkGetTrialData(0, bActive, NULL, &trial, NULL, NULL);
			if (cbRes == CBSDKRESULT_SUCCESS)
				printf("Collected trial data: %d chans, %d samples, %d rate\n", trial.count, samples_collected, trial.sample_rates[0]);
			else
			{
				printf("Error: Failed to get trial data, cbRes = %d\n", cbRes);
				return cbRes;
			}
			

			// THRESHOLD
			for (int iChan = 0; iChan < trial.count; iChan++) {
				cbSCALING* scale = &ccf.data.isChan[trial.chan[iChan]-1].physcalin;

				double* dSamps = (double*)trial.samples[iChan];
				double msq[100];
				for (int j = 0; j < 100; j++) {
					int start_idx = j * 600;
					int end_idx = (j + 1) * 600;
					double ss = 0; // sum of squares
					for (int i = start_idx; i < end_idx; i++) {
						double anaval = ((dSamps[i] - (double)scale->digmin) / (double)(scale->digmax - (double)scale->digmin))
							* ((double)scale->anamax - (double)scale->anamin) + (double)scale->anamin; // analog value
						ss += pow(anaval, 2);
					}
					msq[j] = ss / 600; // mean square
				}
				// sort
				int n = sizeof(msq) / sizeof(double);
				//printf("presort msq[0]=%.1f\n", msq[0]);
				std::sort(msq, msq + n);
				//printf("sort msq[0]=%.1f\n", msq[0]);
				double rms = 0;
				for (int i = 0; i < 20; i++) {
					rms += msq[i+5]; // add 6th to 25th vals
				}
				rms = sqrt(rms / 20);
				double thresh = threshold * rms;
				printf("%03d RMS = %5.2f uV, thresh = % 6.2f uV\n", trial.chan[iChan], rms, thresh);

				// convert RMS thresh to INT32 digital value
				double digThresh = ((thresh - (double)scale->anamin) / ((double)scale->anamax - (double)scale->anamin))
					* ((double)scale->digmax - (double)scale->digmin) + (double)scale->digmin;
				
				// set threshold in CCF struct
				ccf.data.isChan[trial.chan[iChan]-1].spkthrlevel = (INT32) round(digThresh);

		} // end channel threshold loop


			//free trial data
			for (int i = 0; i < cbNUM_ANALOG_CHANS; i++)
			{
				free(trial.samples[i]);
			}
			printf("\n");

		} // end RMS threshold


		// write CCF to NSP
		printf("Writing CCF to NSP\n");
		cbRes = cbSdkWriteCCF(cbInst, &ccf, NULL);
		if (cbRes == CBSDKRESULT_SUCCESS)
			printf("Successfully loaded CCF struct\n");
		else {
			printf("Error: Failed to write CCF, cbRes = %d\n", cbRes);
			return cbRes;
		}

		// close SDK
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
