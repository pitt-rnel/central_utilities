# CentralAutoContinuous

Utility to automate configuring continuous aquisition settings in Central.

This program takes the following command line arguments:
- cbSDK Instrument number (default: `0`)
- Sample Rate (default: `-1` for no change)
- Filter Setting (default: `-1` for no change)

## Sample Rates:
0. None
1.  500 S/s
2. 1 kS/s
3. 2 kS/s
4. 10 kS/s
5. 30 kS/s

## Filters:
0. None
1. HP 750Hz
2. HP 250Hz
3. HP 100Hz
4. LP 50Hz
5. LP 125Hz
6. LP 250Hz
7. LP 500Hz
8. LP 150Hz
9. BP 10Hz-250Hz
10. LP 2.5kHz
11. LP 2kHz
12. BP 250Hz-5kHz
13. User Filter