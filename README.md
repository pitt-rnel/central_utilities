# central_utilities
A collection of command-line utilities for use with Blackrock Neurotech Central software suite, which are intended to be called from scripts (e.g. cmd batch files, or matlab or python scripts running an experiment).

This repo currently includes the following utilities:
1. `CentralAutoRaw`: A utility to enable or disable the raw data stream (30k sample/sec unfiltered continuous data) on all front-end analog inputs
2. `CentralAutoRecord`: A utility to automate functions of the Central FileStorage recording interface (clinical "TOC" interface), including entering patient info, recording location, and starting, stopping, or timing a recording.
3. `CentralAutoSettings`: A utility to automate loading or saving a .ccf Central settings file.
4. `CentralAutoThreshold`: A utility to automate thresholding neural channels in Central. This utility replicates the functionality of the "Thresholding" window in Central.
5. `CentralAutoContinuous`: A utility to automate setting the continuous acquisition sample rate and filter settings for front-end channels.

All utilities can be built from the CentralUtilities.sln Visual Studio solution. Each utility depends on the cbSDK, which is provided with Central (in the Program Files folder), available from [Blackrock Microsystems](https://www.blackrockmicro.com/technical-support/software-downloads/). The cbsdkx64.lib file should be copied to central_utilities/lib and the cbsdkx64.dll, QtCorex644.dll, and QtXmlx644.dll files should be placed in the central_utiilties/bin folder. You may also need to replace the cbhwlib.h and cbsdk.h files in central_utilities/include if you are using a different version of Central. Executable files will be placed in the bin directory.

Example .bat scripts to call each utility are provided in central_utilities/scripts. Each source directory contains an additional README file explaining how to use each utility.

The authors of this package are unaffiliated with Blackrock Neurotech and provide this open-source software as is without warranty.

-------------
TESTING CODEOWNERS, IGNORE THIS
