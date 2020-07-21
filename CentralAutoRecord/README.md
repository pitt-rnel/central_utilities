# CentralAutoRecord

Utility to automate recordings in Central.

This program takes the following command line arguments:
- cbSDK Instrument number (default: `0`)
- subject ID (default: `Test`)
- - duration in seconds. `0` to stop current recording, `-1` to start without timing, `-2` to fill in subject and path info without starting or stopping recording. (default: `5`)
- path to folder (default: `C:\CentralRecordings`)
- subject first name (default: `None`)
- subject last name (default: `None`)
- subject DOB month (default: `Today's month`)
- subject DOB day (default: `Today's date`)
- subject DOB year (default: `Today's year`)