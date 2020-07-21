// Central Auto Record

#include <filesystem> // requires C++17
#include <iostream>
#include <thread>
#include <chrono>
#include "cbsdk.h"
#include "cbhwlib.h"

const char* help_text =
"Central Auto Record\n"
"This program takes the following command line arguments:\n"
"- cbSDK Instrument number (default: 0)\n"
"- subject ID (default: \"Test\")\n"
"- duration in seconds. 0 to stop current recording, -1 to start without timing, -2 to fill in subject and path info without starting or stopping recording. (default: 5)\n"
"- path to folder (default: \"C:\\CentralRecordings\")\n"
"- subject first name (default: \"None\")\n"
"- subject last name (default: \"None\")\n"
"- subject DOB month (default: Today's month)\n"
"- subject DOB day (default: Today's date)\n"
"- subject DOB year (default: Today's year)\n";

int main(int argc, char* argv[])
{
	printf("\n"); // print newline after Qt warning.

	try {
		// Process command line arguments

		UINT32 cbInst = 0;
		if (argc > 1) { // cbInst or help request
			if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "help") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--h") == 0 || strcmp(argv[1], "?") == 0 || strcmp(argv[1], "/?") == 0) {
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

		char default_firstname[] = "none";
		char default_lastname[] = "none";
		char* firstname;
		char* lastname;
		if (argc > 5)
			firstname = argv[5];
		else
			firstname = default_firstname;
		if (argc > 6)
			lastname = argv[6];
		else
			lastname = default_lastname;

		UINT32 DOBMonth;
		UINT32 DOBDay;
		UINT32 DOBYear;
		time_t t = time(0);
		tm* now = localtime(&t);
		if (argc > 7)
			DOBMonth = atoi(argv[7]);
		else
			DOBMonth = (UINT32)now->tm_mon + 1;
		if (argc > 8)
			DOBDay = atoi(argv[8]);
		else
			DOBDay = (UINT32)now->tm_mday;
		if (argc > 9)
			DOBYear = atoi(argv[9]);
		else
			DOBYear = (UINT32)now->tm_year + 1900;


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

		double time = 0; // time elapsed in s
		bool bRecording = false;

		if (duration == 0) //stop recording in progress
			bRecording = true;
		else {
			
			// open window
			cbRes = cbSdkSetFileConfig(cbInst, cpath, "", 0, cbFILECFG_OPT_OPEN);
			if (cbRes != CBSDKRESULT_SUCCESS)
				printf("cbSdkSetFileConfig Error: %d\n", cbRes);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			
			// set path
			cbRes = cbSdkSetFileConfig(cbInst, cpath, "", 0, cbFILECFG_OPT_NONE);
			if (cbRes != CBSDKRESULT_SUCCESS)
				printf("cbSdkSetFileConfig Error: %d\n", cbRes);
			
			//set subjID/date
			cbRes = cbSdkSetPatientInfo(cbInst, subjectID, firstname, lastname, DOBMonth, DOBDay, DOBYear); 
			if (cbRes != CBSDKRESULT_SUCCESS)
				printf("cbSdkSetPatientInfo Error: %d\n", cbRes);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));

			//set subjID/date if ignored first time
			cbRes = cbSdkSetPatientInfo(cbInst, subjectID, firstname, lastname, DOBMonth, DOBDay, DOBYear);
			if (cbRes != CBSDKRESULT_SUCCESS)
				printf("cbSdkSetPatientInfo Error: %d\n", cbRes);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));

			if (duration > -2) // duration == -2 means fill in info and exit
			{
				// start recording
				printf("Starting recording, %d seconds\n", duration);
				cbRes = cbSdkSetFileConfig(cbInst, cpath, "", 1, cbFILECFG_OPT_NONE);
				if (cbRes != CBSDKRESULT_SUCCESS)
					printf("cbSdkSetFileConfig Error: %d\n", cbRes);
				else {
					UINT32 cbtime = 0; // time elapsed in 30kHz samples
					double freq = cbSdk_TICKS_PER_SECOND; // sample rate (30K)
					UINT32 s_time = 5; // sleep time in ms
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

						std::this_thread::sleep_for(std::chrono::milliseconds(s_time));
					} while (!bRecording);

					if (bRecording)
						printf("Recording started at t0 = %d\n", t_start);

					if (duration < 0) // start without stopping
						bRecording = false;

					// record timer
					while (bRecording && time < duration) {
						std::this_thread::sleep_for(std::chrono::milliseconds(s_time));
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

				} // end successful file config (start recording)
			} // end if duration > -2
		} // end duration != 0

		if (bRecording) {
			cbRes = cbSdkSetFileConfig(cbInst, cpath, "", 0, cbFILECFG_OPT_NONE); // stop recording
			if (cbRes != CBSDKRESULT_SUCCESS)
				printf("Error: Failed to stop recording, cbRes = %d\n", cbRes);
			printf("Finished recording, duration = %f\n", time);
			std::this_thread::sleep_for(std::chrono::seconds(3));
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
