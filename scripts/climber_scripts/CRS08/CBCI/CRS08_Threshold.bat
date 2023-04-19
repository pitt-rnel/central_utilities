@echo off

:: Set RMS thresh and cbInst ID params for 2 pedestals
set threshold=-4.5
set CB1=0

:: Threshold
call ..\..\base_scripts\CRS_AutoThreshold.bat %CB1% %threshold%
