# CentralAutoRecord

Utility to automate recordings in Central.

This program takes the following command line arguments:
- cbSDK Instrument number (default: `0`)
- subject ID (default: `Test`)
- duration (seconds,  default: `5`, `0` to stop current recording, `-1` to start without timing, `-2` to fill in patient/path info without starting a recording)
- path to folder (default: `C:\CentralRecordings`)