:: Script to enable 1 kS/s stream with LP 250 Hz filter in Central
@echo off

:: Set Input Values
SET cbInst=0
SET rateIndex=2
SET filtIndex=6

:: bin dir
SET bin=..\bin

@echo on
::Run Central Auto Raw
%bin%\CentralAutoContinuous.exe %cbInst% %rateIndex% %filtIndex%