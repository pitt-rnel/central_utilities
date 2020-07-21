// Central Auto Record

//#define _HAS_STD_BYTE 0 // Fixes Windows.h issue with C++17 standard
#include <filesystem> // requires C++17

#include <iostream>

//#ifndef _WIN32_WINNT    // Allow use of features specific to Windows XP or later.
//#define _WIN32_WINNT 0x0501   // Change this to the appropriate value to target other versions of Windows.
//#endif
#define WIN32_LEAN_AND_MEAN      // Exclude rarely-used stuff from Windows headers
#include <Windows.h>

#include "cbsdk.h"
#include "cbhwlib.h"

const char* help_text =
"Central Auto Record\n"
"This program takes the following command line arguments:\n"
"- cbSDK Instrument number (default: 0)\n"
"- subject ID (default: \"Test\")\n"
"- duration (seconds, default: 5, 0 to stop current recording, -1 to start without timing)\n"
"- path to folder (default: \"C:\\CentralRecordings\")\n";

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

		char* subjectID;
		char default_subject[] = "Test";
		if (argc > 2)
			subjectID = argv[2];
		else
			subjectID = default_subject;

		int duration = 5;
		if (argc > 3)
			duration = atoi(argv[3]);

		char *filepath;
		char default_filepath[] = "C:\\CentralRecordings";
		if (argc > 4)
			filepath = argv[4];
		else
			filepath = default_filepath;

		cbSdkResult cbRes;

		cbRes = cbSdkOpen(cbInst);
		if (cbRes == CBSDKRESULT_SUCCESS)
			printf("Connected to Central\n");
		else {
			printf("Error: Failed to open cbSdk, cbRes = %d\n", cbRes);
			return cbRes;
		}


		std::filesystem::path spath(filepath);
		std::string fullpath;
		if (spath.has_filename()) {
			fullpath = spath.parent_path().string();
			// uncomment below to add cbInst to pathname
			//fullpath.append("\\nsp");
			//fullpath.append(std::to_string(cbInst));
			fullpath.append("\\");
			fullpath.append(spath.filename().string());
		}
		else{ 
			fullpath = spath.string();
			// uncomment below to add cbInst to pathname
			//fullpath.append("\\nsp");
			//fullpath.append(std::to_string(cbInst));
			//fullpath.append("\\");
		}

		
		const char* cpath = fullpath.c_str();

		printf("path: %s\n", cpath);

		const char* name = "none";
		time_t t = time(0);
		tm* now = localtime(&t);
		double time = 0; // time elapsed in s

		bool bRecording = false;

		if (duration == 0) //stop recording in progress
			bRecording = true;
		else {

			cbRes = cbSdkSetFileConfig(cbInst, cpath, "", 0, cbFILECFG_OPT_OPEN); // open window
			if (cbRes != CBSDKRESULT_SUCCESS)
				printf("cbSdkSetFileConfig Error: %d\n", cbRes);
			Sleep(100);
			cbRes = cbSdkSetFileConfig(cbInst, cpath, "", 0, cbFILECFG_OPT_NONE); // set path
			if (cbRes != CBSDKRESULT_SUCCESS)
				printf("cbSdkSetFileConfig Error: %d\n", cbRes);
			cbRes = cbSdkSetPatientInfo(cbInst, subjectID, name, name, (UINT32)now->tm_mon + 1, (UINT32)now->tm_mday, (UINT32)now->tm_year + 1900); //set subjID/date
			if (cbRes != CBSDKRESULT_SUCCESS)
				printf("cbSdkSetPatientInfo Error: %d\n", cbRes);
			Sleep(100);
			cbRes = cbSdkSetPatientInfo(cbInst, subjectID, name, name, (UINT32)now->tm_mon + 1, (UINT32)now->tm_mday, (UINT32)now->tm_year + 1900); //set subjID/date if ignored first time
			if (cbRes != CBSDKRESULT_SUCCESS)
				printf("cbSdkSetPatientInfo Error: %d\n", cbRes);
			Sleep(100);

			printf("Starting recording, %d seconds\n", duration);
			cbRes = cbSdkSetFileConfig(cbInst, cpath, "", 1, cbFILECFG_OPT_NONE); // start recording
			if (cbRes != CBSDKRESULT_SUCCESS)
				printf("cbSdkSetFileConfig Error: %d\n", cbRes);
			else {
				UINT32 cbtime = 0; // time elapsed in 30kHz samples
				double freq = 30000; // sample rate (30K)
				UINT32 s_time = 5; // sleep time in ms
				//bool bRecording = false;
				UINT32 t_start = 0; // start time in 30K samples

				// wait for recording to start
				int count = 0;
				do {
					count++;
					cbRes = cbSdkGetFileConfig(cbInst, NULL, NULL, &bRecording); // determine if recording started
					if (cbRes != CBSDKRESULT_SUCCESS) {
						printf("cbSdkGetFileConfig Error: %d\n", cbRes);
						time = duration; // abort recording
						break;
					}

					cbRes = cbSdkGetTime(cbInst, &t_start); // get start time (valid if recording started)
					if (cbRes != CBSDKRESULT_SUCCESS) {
						printf("cbSdkGetTime Error: %d\n", cbRes);
						time = duration; // abort recording
						break;
					}

					if (count > 500) {
						printf("Recording failed to start\n");
						time = duration; // abort recording
						break;
					}

					Sleep(s_time);
				} while (!bRecording);

				if (bRecording)
					printf("Recording started at t0 = %d\n", t_start);

				if (duration < 0) // start without stopping
					bRecording = false;

				// record timer
				while (bRecording && time < duration) {
					Sleep(s_time);
					cbRes = cbSdkGetTime(cbInst, &cbtime);
					if (cbRes != CBSDKRESULT_SUCCESS) {
						printf("cbSdkGetTime Error: %d\n", cbRes);
						break;
					}
					else {
						time = double(cbtime - t_start) / freq;
						if ((duration - time) * 1000 < s_time) // decrease sleep rate at end of recording
							s_time = 1;
					}
				}

			}
		}

		if (bRecording) {
			cbRes = cbSdkSetFileConfig(cbInst, cpath, "", 0, cbFILECFG_OPT_NONE); // stop recording
			if (cbRes != CBSDKRESULT_SUCCESS)
				printf("Error: Failed to stop recording, cbRes = %d\n", cbRes);
			printf("Finished recording, duration = %f\n", time);
			Sleep(3000);
		}

		if (duration >= 0) { // close window unless we're only starting a recording
			cbRes = cbSdkSetFileConfig(cbInst, cpath, "", 0, cbFILECFG_OPT_CLOSE); // close window
			if (cbRes != CBSDKRESULT_SUCCESS)
				printf("Error: Failed to close file window, cbRes = %d\n", cbRes);
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
