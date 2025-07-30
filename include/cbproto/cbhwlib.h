///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   cbhwlib.h
/// @attention  (c) Copyright 2002 - 2008 Cyberkinetics, Inc. All rights reserved.
/// @attention  (c) Copyright 2008 - 2024 Blackrock Microsystems LLC. All rights reserved.
///
/// @author     Kirk Korver
/// @date       2002
///
/// @brief      Definition of the Neuromatic library protocols.
///
/// This code libary defines an standardized control and data acess interface for microelectrode
/// neurophysiology equipment.  The interface allows several applications to simultaneously access
/// the control and data stream for the equipment through a central control application.  This
/// central application governs the flow of information to the user interface applications and
/// performs hardware specific data processing for the instruments.  This is diagrammed as follows:
///
///   Instruments <---> Central Control App <--+--> Cerebus Library <---> User Application
///                                            +--> Cerebus Library <---> User Application
///                                            +--> Cerebus Library <---> User Application
///
/// The Central Control Application can also exchange window/application configuration data so that
/// the Central Application can save and restore instrument and application window settings.
///
/// All hardware configuration, hardware acknowledgement, and data information are passed on the
/// system in packet form.  Cerebus user applications interact with the hardware in the system by
/// sending and receiving configuration and data packets through the Central Control Application.
/// In order to aid efficiency, the Central Control App caches information regarding hardware
/// configuration so that multiple applications do not need to request hardware configuration
/// packets from the system.  The Neuromatic Library provides high-level functions for retreiving
/// data from this cache and high-level functions for transmitting configuration packets to the
/// hardware.  Neuromatic applications must provide a callback function for receiving data and
/// configuration acknowledgement packets.
///
/// The data stream from the hardware is composed of "neural data" to be saved in experiment files
/// and "preview data" that provides information such as compressed real-time channel data for
/// scrolling displays and Line Noise Cancellation waveforms to update the user.
///
/// Central Startup Procedure:
/// On start, central sends cbPKT_SYSINFO with the runlevel set to cbRUNLEVEL_RUNNING
/// The NSP responds with its current runlevel (cbRUNLEVEL_STARTUP, STANDBY, or RUNNING)
/// At 0.5 seconds Central checks the returned runlevel and sets a flag if it is STARTUP for other
///     processing such as auto loading a CCF file.  Then it sends a cbRUNLEVEL_HARDRESET
/// At 1.0 seconds Central sends a generic packet with the type set to cbPKTTYPE_REQCONFIGALL
/// The NSP responds with a boatload of configuration packets
/// At 2.0 seconds, Central sets runlevel to cbRUNLEVEL_RUNNING
/// The NSP will then start sending data packets and respond to configuration packets.=
///
///////////////////////////////////////////////////////////////////////////////////////////////////

// Standard include guards
#ifndef CBHWLIB_H_INCLUDED
#define CBHWLIB_H_INCLUDED

#include "cbproto/cbproto.h"
#include "launching.h"
// Only standard headers might be included here
#if defined(WIN32)
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif
// It has to be in this order for right version of sockets
#ifdef NO_AFX
#include <winsock2.h>
#include <windows.h>
#endif
#endif // WIN32

#pragma pack(push, 1)

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Fixed storage size definitions for delcared variables
// (includes conditional testing so that there is no clash with win32 headers)
//
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef signed char     INT8;
typedef unsigned char   UINT8;

typedef signed short    INT16;
typedef unsigned short  UINT16;

typedef signed int      INT32;
typedef unsigned int    UINT32;

#ifndef WIN32
# if __WORDSIZE == 64
typedef long int                 INT64;
typedef unsigned long int       UINT64;
# else
typedef long long int            INT64;
typedef unsigned long long int  UINT64;
# endif

typedef UINT32 COLORREF;
typedef INT32 LONG;
typedef LONG * LPLONG;
typedef LONG * PLONG;
typedef UINT8 BYTE;
typedef UINT32 DWORD;
#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif
#ifndef BOOL
#define BOOL int
#endif
#endif


// maximum udp datagram size used to transport cerebus packets, taken from MTU size
#ifdef WIN32
#define cbCER_UDP_SIZE_MAX          58080             // Note that multiple packets may reside in one udp datagram as aggregate
#else
#define cbCER_UDP_SIZE_MAX          1452 // Note that multiple packets may reside in one udp datagram as aggregate
#endif

// The maximum number of packets to look at for each pass
enum { MAX_NUM_OF_PACKETS_TO_PROCESS_PER_PASS = 5000 };

//
#define cbMAXPROCS  3                               // Number of NSP's for Central
#define cbNUM_FE_CHANS        512                   // #Front end channels for Central
#define cbMAXGROUPS 8                               // number of sample rate groups
#define cbMAXFILTS  32
#define cbMAXVIDEOSOURCE 1                          // maximum number of video sources
#define cbMAXTRACKOBJ 20                            // maximum number of trackable objects
#define cbMAXHOOPS  4
#define cbMAX_AOUT_TRIGGER 5                        // maximum number of per-channel (analog output, or digital output) triggers

// Channel Definitions
#define cbNUM_ANAIN_CHANS     16 * cbMAXPROCS                                       // #Analog Input channels
#define cbNUM_ANALOG_CHANS    (cbNUM_FE_CHANS + cbNUM_ANAIN_CHANS)      // Total Analog Inputs
#define cbNUM_ANAOUT_CHANS    4 * cbMAXPROCS                                         // #Analog Output channels
#define cbNUM_AUDOUT_CHANS    2 * cbMAXPROCS                                         // #Audio Output channels
#define cbNUM_ANALOGOUT_CHANS (cbNUM_ANAOUT_CHANS + cbNUM_AUDOUT_CHANS) // Total Analog Output
#define cbNUM_DIGIN_CHANS     1 * cbMAXPROCS                                         // #Digital Input channels
#define cbNUM_SERIAL_CHANS    1 * cbMAXPROCS                                         // #Serial Input channels
#define cbNUM_DIGOUT_CHANS    4 * cbMAXPROCS                                         // #Digital Output channels

// Total of all channels = 156
#define cbMAXCHANS            (cbNUM_ANALOG_CHANS +  cbNUM_ANALOGOUT_CHANS + cbNUM_DIGIN_CHANS + cbNUM_SERIAL_CHANS + cbNUM_DIGOUT_CHANS)

#define cbFIRST_FE_CHAN       0                                          // 0   First Front end channel

// Bank definitions - NOTE: If any of the channel types have more than cbCHAN_PER_BANK channels, the banks must be increased accordingly
#define cbCHAN_PER_BANK       32                                         // number of 32 channel banks == 1024
#define cbNUM_FE_BANKS        (cbNUM_FE_CHANS / cbCHAN_PER_BANK)         // number of Front end banks
#define cbNUM_ANAIN_BANKS     1                                          // number of Analog Input banks
#define cbNUM_ANAOUT_BANKS    1                                          // number of Analog Output banks
#define cbNUM_AUDOUT_BANKS    1                                          // number of Audio Output banks
#define cbNUM_DIGIN_BANKS     1                                          // number of Digital Input banks
#define cbNUM_SERIAL_BANKS    1                                          // number of Serial Input banks
#define cbNUM_DIGOUT_BANKS    1                                          // number of Digital Output banks

// Custom digital filters
#define cbFIRST_DIGITAL_FILTER  13  // (0-based) filter number, must be less than cbMAXFILTS
#define cbNUM_DIGITAL_FILTERS   4

// This is the number of aout chans with gain. Conveniently, the
// 4 Analog Outputs and the 2 Audio Outputs are right next to each other
// in the channel numbering sequence.
#define AOUT_NUM_GAIN_CHANS             (cbNUM_ANAOUT_CHANS + cbNUM_AUDOUT_CHANS)

// Total number of banks
#define cbMAXBANKS            (cbNUM_FE_BANKS + cbNUM_ANAIN_BANKS + cbNUM_ANAOUT_BANKS + cbNUM_AUDOUT_BANKS + cbNUM_DIGIN_BANKS + cbNUM_SERIAL_BANKS + cbNUM_DIGOUT_BANKS)

#define cbMAXUNITS            5                                         // hard coded to 5 in some places
#define cbMAXNTRODES          (cbNUM_FE_CHANS / 2)                      // minimum is stereotrode so max n-trodes is max front-end chans / 2

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Systemwide Inquiry and Configuration Functions
//
///////////////////////////////////////////////////////////////////////////////////////////////////

// Open multiple instances of library as stand-alone or under Central application
cbRESULT cbOpen(BOOL bStandAlone = FALSE, UINT32 nInstance = 0);
// Initializes the Neuromatic library (and establishes a link to the Central Control Application if bStandAlone is FALSE).
// This function must be called before any other functions are called from this library.
// Returns OK, NOCENTRALAPP, LIBINITERROR, MEMORYUNVAIL, or HARDWAREOFFLINE

cbRESULT cbClose(BOOL bStandAlone = FALSE, UINT32 nInstance = 0);
// Close the library (must match how library is openned)

cbRESULT cbCheckApp(const char * lpName);
// Check if an application is running using its mutex

cbRESULT cbAquireSystemLock(const char * lpName, HANDLE & hLock);
cbRESULT cbReleaseSystemLock(const char * lpName, HANDLE & hLock);
// Aquire or release application system lock

UINT32 GetInstrumentLocalChan(UINT32 nChan, UINT32 nInstance = 0);
// Get the instrument local channel number

#define  cbINSTINFO_READY      0x0001     // Instrument is connected
#define  cbINSTINFO_LOCAL      0x0002     // Instrument runs on the localhost
#define  cbINSTINFO_NPLAY      0x0004     // Instrument is nPlay
#define  cbINSTINFO_CEREPLEX   0x0008     // Instrument is Cereplex
#define  cbINSTINFO_EMULATOR   0x0010     // Instrument is Emulator
#define  cbINSTINFO_NSP1       0x0020     // Instrument is NSP1
#define  cbINSTINFO_WNSP       0x0040     // Instrument is WNSP
#define  cbINSTINFO_GEMINI_NSP 0x0080     // Instrument is Gemini NSP
#define  cbINSTINFO_GEMINI_HUB 0x0100     // Instrument is Gemini Hub
cbRESULT cbGetInstInfo(uint8_t nInstrument, UINT32 *instInfo, UINT32 nInstance = 0);
// Purpose: get instrument information.

cbRESULT cbGetLatency(UINT32 *nLatency, UINT32 nInstance = 0);
// Purpose: get instrument latency.

// Returns instrument information
// Returns cbRESULT_OK if successful, cbRESULT_NOLIBRARY if library was never initialized.
cbRESULT cbGetSystemClockFreq(UINT32 *freq, UINT32 nInstance = 0);
// Retrieves the system timestamp/sample clock frequency (in Hz) from the Central App cache.
//
// Returns: cbRESULT_OK if data successfully retrieved.
//          cbRESULT_NOLIBRARY if the library was not properly initialized


cbRESULT cbGetSystemClockTime(PROCTIME *time, UINT32 nInstance = 0);
// Retrieves the last 32-bit timestamp from the Central App cache.
//
// Returns: cbRESULT_OK if data successfully retrieved.
//          cbRESULT_NOLIBRARY if the library was not properly initialized



// Shuts down the programming library and frees any resources linked in cbOpen()
// Returns cbRESULT_OK if successful, cbRESULT_NOLIBRARY if library was never initialized.
// Updates the read pointers in the memory area so that
// all of the un-read packets are ignored. In other words, it
// initializes all of the pointers so that the begging of read time is NOW.
cbRESULT cbMakePacketReadingBeginNow(UINT32 nInstance = 0);
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Data checking and processing functions
//
// To get data from the shared memory buffers used in the Central App, the user can:
// 1) periodically poll for new data using a multimedia or windows timer
// 2) create a thread that uses a Win32 Event synchronization object to que the data polling
//
///////////////////////////////////////////////////////////////////////////////////////////////////

enum cbLevelOfConcern
{
    LOC_LOW,                // Time for sippen lemonaide
    LOC_MEDIUM,             // Step up to the plate
    LOC_HIGH,               // Put yer glass down
    LOC_CRITICAL,           // Get yer but in gear
    LOC_COUNT               // How many level of concerns are there
};

cbRESULT cbCheckforData(cbLevelOfConcern & nLevelOfConcern, UINT32 *pktstogo = NULL, UINT32 nInstance = 0);
// The pktstogo and timetogo are optional fields (NULL if not used) that returns the number of new
// packets and timestamps that need to be read to catch up to the buffer.
//
// Returns: cbRESULT_OK    if there is new data in the buffer
//          cbRESULT_NONEWDATA if there is no new data available
//          cbRESULT_DATALOST if the Central App incoming data buffer has wrapped the read buffer


cbRESULT cbWaitforData(UINT32 nInstance = 0);
// Executes a WaitForSingleObject command to wait for the Central App event signal
//
// Returns: cbRESULT_OK    if there is new data in the buffer
//          cbRESULT_NONEWDATA if the function timed out after 250ms
//          cbRESULT_DATALOST if the Central App incoming data buffer has wrapped the read buffer
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// NEV file definitions
//
///////////////////////////////////////////////////////////////////////////////////////////////////

cbPKT_GENERIC *cbGetNextPacketPtr(UINT32 nInstance = 0);
// Returns pointer to next packet in the shared memory space.  If no packet available, returns NULL

// Cerebus Library function to send packets via the Central Application Queue
cbRESULT cbSendPacket(void * pPacket, UINT32 nInstance = 0);
cbRESULT cbSendPacketToAllInstruments(void* pPacket, UINT32 nInstance = 0);
cbRESULT cbSendLoopbackPacket(void * pPacket, UINT32 nInstance = 0);


///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Configuration/Report Packet Definitions (chid = 0x8000)
//
///////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Struct - NeuroMotive video source
cbRESULT cbGetVideoSource(char *name, float *fps, UINT32 id, UINT32 nInstance = 0);
cbRESULT cbSetVideoSource(const char *name, float fps, UINT32 id, UINT32 nInstance = 0);
// Get/Set the video source parameters.
//
// Returns: cbRESULT_OK if data successfully retrieved.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.

/// @brief Struct - Track object structure for NeuroMotive
cbRESULT cbGetTrackObj(char *name, UINT16 *type, UINT16 *pointCount, UINT32 id, UINT32 nInstance = 0);
cbRESULT cbSetTrackObj(const char *name, UINT16 type, UINT16 pointCount, UINT32 id, UINT32 nInstance = 0);
// Get/Set the trackable object parameters.
//
// Returns: cbRESULT_OK if data successfully retrieved.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.

/// @brief Get the channel capabilities
cbRESULT cbGetChanCaps(UINT32 chan, UINT32 *chancaps, UINT32 nInstance = 0);
// Retreives the channel capabilities from the Central App Cache.
//
// Returns: cbRESULT_OK if data successfully retreived or packet successfully queued to be sent.
//          cbRESULT_INVALIDCHANNEL if the specified channel is not mapped or does not exist.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Digital Input Inquiry and Configuration Functions
//
///////////////////////////////////////////////////////////////////////////////////////////////////


cbRESULT cbGetDinpCaps(UINT32 chan, UINT32 *dinpcaps, UINT32 nInstance = 0);
// Retreives the channel's digital input capabilities from the Central App Cache.
// Port Capabilities are reported as compbined cbDINPOPT_* flags.  Zero = no DINP capabilities.
//
// Returns: cbRESULT_OK if data successfully retreived.
//          cbRESULT_INVALIDCHANNEL if the specified channel is not mapped or does not exist.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.


cbRESULT cbGetDinpOptions(UINT32 chan, UINT32 *options, UINT32 *eopchar, UINT32 nInstance = 0);
cbRESULT cbSetDinpOptions(UINT32 chan, UINT32 options, UINT32 eopchar, UINT32 nInstance = 0);
// Get/Set the Digital Input Port options for the specified channel.
//
// Port options are expressed as a combined set of cbDINP_* option flags, for example:
// a) cbDINP_SERIAL + cbDINP_BAUDxx = capture single 8-bit RS232 serial values.
// b) cbDINP_SERIAL + cbDINP_BAUDxx + cbDINP_PKTCHAR = capture serial packets that are terminated
//      with an end of packet character (only the lower 8 bits are used).
// c) cbDINP_1BIT + cbDINP_ANYBIT = capture the changes of a single digital input line.
// d) cbDINP_xxBIT + cbDINP_ANYBIT = capture the xx-bit input word when any bit changes.
// e) cbDINP_xxBIT + cbDINP_WRDSTRB = capture the xx-bit input based on a word-strobe line.
// f) cbDINP_xxBIT + cbDINP_WRDSTRB + cbDINP_PKTCHAR = capture packets composed of xx-bit words
//      in which the packet is terminated with the specified end-of-packet character.
// g) cbDINP_xxBIT + cbDINP_WRDSTRB + cbDINP_PKTLINE = capture packets composed of xx-bit words
//      in which the last character of a packet is accompanyied with an end-of-pkt logic signal.
// h) cbDINP_xxBIT + cbDINP_REDGE = capture the xx-bit input word when any bit goes from low to hi.
// i) cbDINP_xxBIT + dbDINP_FEDGE = capture the xx-bit input word when any bit goes from hi to low.
//
// NOTE: If the end-of-packet character value (eopchar) is not used in the options, it is ignored.
//
// Add cbDINP_PREVIEW to the option set to get preview updates (cbPKT_PREVDINP) at each word,
// not only when complete packets are sent.
//
// The Get function returns values from the Central Control App cache.  The Set function validates
// that the specified options are available and then queues a cbPKT_SETDINPOPT packet.  The system
// acknowledges this change with a cbPKT_ACKDINPOPT packet.
//
// Returns: cbRESULT_OK if data successfully retreived or packet successfully queued to be sent.
//          cbRESULT_INVALIDFUNCTION a requested option is not available on that channel.
//          cbRESULT_INVALIDCHANNEL if the specified channel is not mapped or does not exist.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Digital Output Inquiry and Configuration Functions
//
///////////////////////////////////////////////////////////////////////////////////////////////////

cbRESULT cbGetDoutCaps(UINT32 chan, UINT32 *doutcaps, UINT32 nInstance = 0);
// Retreives the channel's digital output capabilities from the Central Control App Cache.
// Port Capabilities are reported as compbined cbDOUTOPT_* flags.  Zero = no DINP capabilities.
//
// Returns: cbRESULT_OK if data successfully retreived.
//          cbRESULT_INVALIDCHANNEL if the specified channel is not mapped or does not exist.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.


cbRESULT cbGetDoutOptions(UINT32 chan, UINT32 *options, UINT32 *monchan, UINT32 *doutval,
                          UINT8 *triggertype = NULL, UINT16 *trigchan = NULL, UINT16 *trigval = NULL, UINT32 nInstance = 0);
cbRESULT cbSetDoutOptions(UINT32 chan, UINT32 options, UINT32 monchan, UINT32 doutval,
                          UINT8 triggertype = cbDOUT_TRIGGER_NONE, UINT16 trigchan = 0, UINT16 trigval = 0, UINT32 nInstance = 0);
// Get/Set the Digital Output Port options for the specified channel.
//
// The only changable DOUT options in this version of the interface libraries are baud rates for
// serial output ports.  These are set with the cbDOUTOPT_BAUDxx options.
//
// The Get function returns values from the Central Control App cache.  The Set function validates
// that the specified options are available and then queues a cbPKT_SETDOUTOPT packet.  The system
// acknowledges this change with a cbPKT_REPDOUTOPT packet.
//
// Returns: cbRESULT_OK if data successfully retreived or packet successfully queued to be sent.
//          cbRESULT_INVALIDCHANNEL if the specified channel is not mapped or does not exist.
//          cbRESULT_NOINTERNALCHAN if there is no internal channel for mapping the in->out chan
//          cbRESULT_NOLIBRARY if the library was not properly initialized.

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Analog Input Inquiry and Configuration Functions
//

cbRESULT cbGetAinpCaps(UINT32 chan, UINT32 *ainpcaps, cbSCALING *physcalin, cbFILTDESC *phyfiltin, UINT32 nInstance = 0);
// Retreives the channel's analog input capabilities from the Central Control App Cache.
// Capabilities are reported as combined cbAINP_* flags.  Zero = no AINP capabilities.
//
// Returns: cbRESULT_OK if data successfully retreived.
//          cbRESULT_INVALIDCHANNEL if the specified channel is not mapped or does not exist.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.


cbRESULT cbGetAinpOpts(UINT32 chan, UINT32 *ainpopts, UINT32 *LNCrate, UINT32 *refElecChan, UINT32 nInstance = 0);
cbRESULT cbSetAinpOpts(UINT32 chan, const UINT32 ainpopts,  UINT32 LNCrate, const UINT32 refElecChan, UINT32 nInstance = 0);
// Get and Set the user-assigned amplitude reject values.
//
// Returns: cbRESULT_OK if data successfully retreived or packet successfully queued to be sent.
//          cbRESULT_INVALIDCHANNEL if the specified channel is not mapped or does not exist.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.
//
// The LNC configuration is composed of an adaptation rate and a mode variable.  The rate sets the
// first order decay of the filter according to:
//
//    newLNCvalue = (LNCrate/65536)*(oldLNCvalue) + ((65536-LNCrate)/65536)*LNCsample
//
// The relationships between the adaptation time constant in sec, line frequency in Hz and the
// the LNCrate variable are given below:
//
//         time_constant = 1 / ln[ (LNCrate/65536)^(-line_freq) ]
//
//         LNCrate = 65536 * e^[-1/(time_constant*line_freq)]
//
// The LNCmode sets whether the channel LNC block is disabled, running, or on hold.
//
// To set multiple channels on hold or run, pass channel=0.  In this case, the LNCrate is ignored
// and the run or hold value passed to the LNCmode variable is applied to all LNC enabled channels.


cbRESULT cbGetAinpScaling(UINT32 chan, cbSCALING *scaling, UINT32 nInstance = 0);
cbRESULT cbSetAinpScaling(UINT32 chan, cbSCALING *scaling, UINT32 nInstance = 0);
// Get/Set the user-specified scaling for the channel.  The digmin and digmax values of the user
// specified scaling must be within the digmin and digmax values for the physical channel mapping.
//
// Returns: cbRESULT_OK if data successfully retreived.
//          cbRESULT_INVALIDCHANNEL if the specified channel is not mapped or does not exist.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.

cbRESULT cbGetAinpDisplay(UINT32 chan, INT32 *smpdispmin, INT32 *smpdispmax, INT32 *spkdispmax, INT32 *lncdispmax, UINT32 nInstance = 0);
cbRESULT cbSetAinpDisplay(UINT32 chan, INT32  smpdispmin, INT32  smpdispmax, INT32  spkdispmax, INT32  lncdispmax, UINT32 nInstance = 0);
// Get and Set the display ranges used by User applications.  smpdispmin/max set the digital value
// range that should be displayed for the sampled analog stream.  Spike streams are assumed to be
// symmetric about zero so that spikes should be plotted from -spkdispmax to +spkdispmax.  Passing
// zero as a scale instructs the Central app to send the cached value.  Fields with NULL pointers
// are ignored by the library.
//
// Returns: cbRESULT_OK if data successfully retreived or packet successfully queued to be sent.
//          cbRESULT_INVALIDCHANNEL if the specified channel is not mapped or does not exist.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.


cbRESULT cbSetAinpPreview(UINT32 chan, UINT32 prevopts, UINT32 nInstance = 0);
// Requests preview packets for a specific channel.
// Setting the AINPPREV_LNC option gets a single LNC update waveform.
// Setting the AINPPREV_STREAMS enables compressed preview information.
//
// A channel ID of zero requests the specified preview packets from all active ainp channels.
//
// Returns: cbRESULT_OK if data successfully retreived.
//          cbRESULT_INVALIDCHANNEL if the specified channel is not mapped or does not exist.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.


//////////////////////////////////////////////////////////////
// AINP Continuous Stream Functions


cbRESULT cbGetAinpSampling(UINT32 chan, UINT32 *filter, UINT32 *group, UINT32 nInstance = 0);
cbRESULT cbSetAinpSampling(UINT32 chan, UINT32 filter,  UINT32 group, UINT32 nInstance = 0);
// Get/Set the periodic sample group for the channel.  Continuous sampling is performed in
// groups with each Neural Signal Processor.  There are up to 4 groups for each processor.
// A group number of zero signifies that the channel is not part of a continuous sample group.
// filter = 1 to cbNFILTS, 0 is reserved for the null filter case
//
// Returns: cbRESULT_OK if data successfully retreived.
//          cbRESULT_INVALIDCHANNEL if the specified channel is not mapped or does not exist.
//          cbRESULT_INVALIDFUNCTION if the group number is not valid.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.

//////////////////////////////////////////////////////////////
// AINP Spike Stream Functions

cbRESULT cbGetAinpSpikeCaps(UINT32 chan, UINT32 *flags, UINT32 nInstance = 0);
cbRESULT cbGetAinpSpikeOptions(UINT32 chan, UINT32 *flags, UINT32 *filter, UINT32 nInstance = 0);
cbRESULT cbSetAinpSpikeOptions(UINT32 chan, UINT32 flags,  UINT32 filter, UINT32 nInstance = 0);
// Get/Set spike capabilities and options.  The EXTRACT flag must be set for a channel to perform
// spike extraction and processing.  The HOOPS and TEMPLATE flags are exclusive, only one can be
// used at a time.
// the THRSHOVR flag will turn auto thresholding off for that channel
// filter = 1 to cbNFILTS, 0 is reserved for the null filter case
//
// Returns: cbRESULT_OK if data successfully retreived.
//          cbRESULT_INVALIDCHANNEL if the specified channel is not mapped or does not exist.
//          cbRESULT_INVALIDFUNCTION if invalid flag combinations are passed.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.
//


cbRESULT cbGetAinpSpikeThreshold(UINT32 chan, INT32 *level, UINT32 nInstance = 0);
cbRESULT cbSetAinpSpikeThreshold(UINT32 chan, INT32 level, UINT32 nInstance = 0);
// Get/Set the spike detection threshold and threshold detection mode.
//
// Returns: cbRESULT_OK if data successfully retreived.
//          cbRESULT_INVALIDCHANNEL if the specified channel is not mapped or does not exist.
//          cbRESULT_INVALIDFUNCTION if invalid flag combinations are passed.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.

cbRESULT cbGetAinpSpikeHoops(UINT32 chan, cbHOOP *hoops, UINT32 nInstance = 0);
cbRESULT cbSetAinpSpikeHoops(UINT32 chan, cbHOOP *hoops, UINT32 nInstance = 0);
// Get/Set the spike hoop set.  The hoops parameter points to an array of hoops declared as
// cbHOOP hoops[cbMAXUNITS][cbMAXHOOPS].
//
// Empty hoop definitions have zeros for the cbHOOP structure members.  Hoop definitions can be
// cleared by passing a NULL cbHOOP pointer to the Set function or by calling the Set function
// with an all-zero cbHOOP structure.
//
// Returns: cbRESULT_OK if data successfully retreived.
//          cbRESULT_INVALIDCHANNEL if the specified channel is not mapped or does not exist.
//          cbRESULT_INVALIDFUNCTION if an invalid unit or hoop number is passed.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Analog Output Inquiry and Configuration Functions
//
///////////////////////////////////////////////////////////////////////////////////////////////////

cbRESULT cbGetAoutCaps(UINT32 chan, UINT32 *aoutcaps, cbSCALING *physcalout, cbFILTDESC *phyfiltout, UINT32 nInstance = 0);
// Get/Set the spike template capabilities and options.  The nunits and nhoops values detail the
// number of units that the channel supports.
//
// Empty template definitions have zeros for the cbSPIKETEMPLATE structure members.  Spike
// Template definitions can be cleared by passing a NULL cbSPIKETEMPLATE pointer to the Set
// function or by calling the Set function with an all-zero cbSPIKETEMPLATE structure.
//
// Returns: cbRESULT_OK if data successfully retreived.
//          cbRESULT_INVALIDCHANNEL if the specified channel is not mapped or does not exist.
//          cbRESULT_INVALIDFUNCTION if an invalid unit number is passed.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.


cbRESULT cbGetAoutScaling(UINT32 chan, cbSCALING *scaling, UINT32 nInstance = 0);
cbRESULT cbSetAoutScaling(UINT32 chan, cbSCALING *scaling, UINT32 nInstance = 0);
// Get/Set the user-specified scaling for the channel.  The digmin and digmax values of the user
// specified scaling must be within the digmin and digmax values for the physical channel mapping.
//
// Returns: cbRESULT_OK if data successfully retreived.
//          cbRESULT_INVALIDCHANNEL if the specified channel is not mapped or does not exist.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.


cbRESULT cbGetAoutOptions(UINT32 chan, UINT32* options, UINT32* monchan, UINT32* trigchan, UINT32* value, UINT32 nInstance = 0);
cbRESULT cbSetAoutOptions(UINT32 chan, UINT32 options, UINT32 monchan, UINT32 trigchan, UINT32 value, UINT32 nInstance = 0);
// Get/Set the Monitored channel for a Analog Output Port.  Setting zero for the monitored channel
// stops the monitoring and frees any instrument monitor resources.  The factor ranges
//
// Returns: cbRESULT_OK if data successfully retreived or packet successfully queued to be sent.
//          cbRESULT_NOINTERNALCHAN if there is no internal channel for mapping the in->out chan
//          cbRESULT_INVALIDCHANNEL if the specified channel is not mapped or does not exist.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.


///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Spike Sorting Inquiry and Configuration Functions
//
///////////////////////////////////////////////////////////////////////////////////////////////////


// Request that the sorting model be updated
cbRESULT cbGetSortingModel(UINT32 nInstance = 0);
cbRESULT cbGetFeatureSpaceDomain(UINT32 nInstance = 0);


// Getting and setting the noise boundary
cbRESULT cbSSGetNoiseBoundary(UINT32 chanIdx, float afCentroid[3], float afMajor[3], float afMinor_1[3], float afMinor_2[3], UINT32 nInstance = 0);
cbRESULT cbSSSetNoiseBoundary(UINT32 chanIdx, float afCentroid[3], float afMajor[3], float afMinor_1[3], float afMinor_2[3], UINT32 nInstance = 0);

cbRESULT cbSSGetNoiseBoundaryByTheta(UINT32 chanIdx, float afCentroid[3], float afAxisLen[3], float afTheta[3], UINT32 nInstance = 0);
cbRESULT cbSSSetNoiseBoundaryByTheta(UINT32 chanIdx, const float afCentroid[3], const float afAxisLen[3], const float afTheta[3], UINT32 nInstance = 0);

// Getting and settings statistics
cbRESULT cbSSGetStatistics(UINT32 * pnUpdateSpikes, UINT32 * pnAutoalg, UINT32 * nMode,
                           float * pfMinClusterPairSpreadFactor,
                           float * pfMaxSubclusterSpreadFactor,
                           float * pfMinClusterHistCorrMajMeasure,
                           float * pfMaxClusterPairHistCorrMajMeasure,
                           float * pfClusterHistValleyPercentage,
                           float * pfClusterHistClosePeakPercentage,
                           float * pfClusterHistMinPeakPercentage,
                           UINT32 * pnWaveBasisSize,
                           UINT32 * pnWaveSampleSize,
                           UINT32 nInstance = 0);

cbRESULT cbSSSetStatistics(UINT32 nUpdateSpikes, UINT32 nAutoalg, UINT32 nMode,
                           float fMinClusterPairSpreadFactor,
                           float fMaxSubclusterSpreadFactor,
                           float fMinClusterHistCorrMajMeasure,
                           float fMaxClusterPairHistCorrMajMeasure,
                           float fClusterHistValleyPercentage,
                           float fClusterHistClosePeakPercentage,
                           float fClusterHistMinPeakPercentage,
                           UINT32 nWaveBasisSize,
                           UINT32 nWaveSampleSize,
                           UINT32 nInstance = 0);


// Spike sorting artifact rejecting
cbRESULT cbSSGetArtifactReject(UINT32 * pnMaxChans, UINT32 * pnRefractorySamples, UINT32 nInstance = 0);
cbRESULT cbSSSetArtifactReject(UINT32 nMaxChans, UINT32 nRefractorySamples, UINT32 nInstance = 0);

// Spike detection parameters
cbRESULT cbSSGetDetect(float * pfThreshold, float * pfScaling, UINT32 nInstance = 0);
cbRESULT cbSSSetDetect(float fThreshold, float fScaling, UINT32 nInstance = 0);

// Getting and setting spike sorting status parameters
cbRESULT cbSSGetStatus(cbAdaptControl * pcntlUnitStats, cbAdaptControl * pcntlNumUnits, UINT32 nInstance = 0);
cbRESULT cbSSSetStatus(cbAdaptControl cntlUnitStats, cbAdaptControl cntlNumUnits, UINT32 nInstance = 0);

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Data Packet Structures (chid<0x8000)
//
///////////////////////////////////////////////////////////////////////////////////////////////////

// nPlay configuration packet(sent on restart together with config packet)
cbRESULT cbGetNplay(char *fname, float *speed, UINT32 *flags, PROCTIME *ftime, PROCTIME *stime, PROCTIME *etime, PROCTIME * filever, UINT32 nInstance = 0);
cbRESULT cbSetNplay(const char *fname, float speed, UINT32 mode, PROCTIME val, PROCTIME stime, PROCTIME etime, UINT32 nInstance = 0);
// Get/Set the nPlay parameters.
//
// Returns: cbRESULT_OK if data successfully retrieved.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.

/// Contains system information including the runlevel
cbRESULT cbGetSpikeLength(UINT32 *length, UINT32 *pretrig, UINT32 * pSysfreq, UINT32 nInstance = 0);
cbRESULT cbSetSpikeLength(UINT32 length, UINT32 pretrig, UINT32 nInstance = 0);
// Get/Set the system-wide spike length.  Lengths should be specified in multiples of 2 and
// within the range of 16 to 128 samples long.
//
// Returns: cbRESULT_OK if data successfully retrieved.
//          cbRESULT_INVALIDCHANNEL if the specified channel is not mapped or does not exist.
//          cbRESULT_INVALIDFUNCTION if invalid flag combinations are passed.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.

cbRESULT cbGetSystemRunLevel(UINT32 *runlevel, UINT32 *runflags, UINT32 *resetque, UINT32 nInstance = 0);
cbRESULT cbSetSystemRunLevel(UINT32 runlevel, UINT32 runflags, UINT32 resetque, uint8_t nInstrument = cbNSP1, UINT32 nInstance = 0);
// Get Set the System Condition
// Returns: cbRESULT_OK if data successfully retrieved.
//          cbRESULT_NOLIBRARY if the library was not properly initialized

/// This packet injects a comment into the data stream which gets recorded in the file and displayed on Raster.
cbRESULT cbSetComment(UINT8 charset, UINT32 rgba, PROCTIME time, const char * comment, UINT32 nInstance = 0);
// Set one comment event.
//
// Returns: cbRESULT_OK if data successfully retrieved.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.

/// Includes information about the counts of various features of the processor
cbRESULT cbGetProcInfo(UINT32 proc, cbPROCINFO *procinfo, UINT32 nInstance = 0);
// Retreives information for a the Signal Processor module located at procid
// The function requires an allocated but uninitialized cbPROCINFO structure.
//
// Returns: cbRESULT_OK if data successfully retreived.
//          cbRESULT_INVALIDADDRESS if no hardware at the specified Proc and Bank address
//          cbRESULT_NOLIBRARY if the library was not properly initialized.

cbRESULT cbGetChanCount(UINT32 *count, UINT32 nInstance = 0);
// Retreives the total number of channels in the system
//
// Returns: cbRESULT_OK if data successfully retreived.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.

// Report Bank Information (duplicates the cbBANKINFO structure)
cbRESULT cbGetBankInfo(UINT32 proc, UINT32 bank, cbBANKINFO *bankinfo, UINT32 nInstance = 0);
// Retreives information for the Signal bank located at bankaddr on Proc procaddr.
// The function requires an allocated but uninitialized cbBANKINFO structure.
//
// Returns: cbRESULT_OK if data successfully retreived.
//          cbRESULT_INVALIDADDRESS if no hardware at the specified Proc and Bank address
//          cbRESULT_NOLIBRARY if the library was not properly initialized.

/// Describes the filters contained in the NSP including the filter coefficients
cbRESULT cbGetFilterDesc(UINT32 proc, UINT32 filt, cbFILTDESC *filtdesc, UINT32 nInstance = 0);
// Retreives the user filter definitions from a specific processor
// filter = 1 to cbNFILTS, 0 is reserved for the null filter case
// Returns: cbRESULT_OK if data successfully retreived.
//          cbRESULT_INVALIDADDRESS if no hardware at the specified Proc and Bank address
//          cbRESULT_NOLIBRARY if the library was not properly initialized.

///////////////////////////////////////////////////////////////////////////////////////////////////
// Adaptive filtering
/// This sets the parameters for the adaptive filtering.
// Tell me about the current adaptive filter settings
cbRESULT cbGetAdaptFilter(UINT32  proc,             // which NSP processor?
                          UINT32  * pnMode,         // 0=disabled, 1=filter continuous & spikes, 2=filter spikes
                          float   * pdLearningRate, // speed at which adaptation happens. Very small. e.g. 5e-12
                          UINT32  * pnRefChan1,     // The first reference channel (1 based).
                          UINT32  * pnRefChan2,     // The second reference channel (1 based).
                          UINT32 nInstance = 0);


// Update the adaptive filter settings
cbRESULT cbSetAdaptFilter(UINT32  proc,             // which NSP processor?
                          UINT32  * pnMode,         // 0=disabled, 1=filter continuous & spikes, 2=filter spikes
                          float   * pdLearningRate, // speed at which adaptation happens. Very small. e.g. 5e-12
                          UINT32  * pnRefChan1,     // The first reference channel (1 based).
                          UINT32  * pnRefChan2,     // The second reference channel (1 based).
                          UINT32 nInstance = 0);

// Useful for creating cbPKT_ADAPTFILTINFO packets
struct PktAdaptFiltInfo : public cbPKT_ADAPTFILTINFO
{
    PktAdaptFiltInfo(UINT32 nMode, float dLearningRate, UINT32 nRefChan1, UINT32 nRefChan2)
    {
        this->cbpkt_header.chid = 0x8000;
        this->cbpkt_header.type = cbPKTTYPE_ADAPTFILTSET;
        this->cbpkt_header.dlen = cbPKTDLEN_ADAPTFILTINFO;

        this->nMode = nMode;
        this->dLearningRate = dLearningRate;
        this->nRefChan1 = nRefChan1;
        this->nRefChan2 = nRefChan2;
    };
};

// Tell me about the current reference electrode filter settings
cbRESULT cbGetRefElecFilter(UINT32  proc,           // which NSP processor?
                            UINT32  * pnMode,       // 0=disabled, 1=filter continuous & spikes, 2=filter spikes
                            UINT32  * pnRefChan,    // The reference channel (1 based).
                            UINT32 nInstance = 0);


// Update the reference electrode filter settings
cbRESULT cbSetRefElecFilter(UINT32  proc,           // which NSP processor?
                            UINT32  * pnMode,       // 0=disabled, 1=filter continuous & spikes, 2=filter spikes
                            UINT32  * pnRefChan,    // The reference channel (1 based).
                            UINT32 nInstance = 0);

// N-Trode Information Packets
/// Sets information about an N-Trode.  The user change the name, number of sites, sites (channels),
cbRESULT cbGetNTrodeInfo( const UINT32 ntrode, char *label, cbMANUALUNITMAPPING ellipses[][cbMAXUNITS], UINT16 * nSite, UINT16 * chans, UINT16 * fs, UINT32 nInstance = 0);
cbRESULT cbSetNTrodeInfo( const UINT32 ntrode, const char *label, cbMANUALUNITMAPPING ellipses[][cbMAXUNITS], UINT16 fs, UINT32 nInstance = 0);
cbRESULT cbSetNTrodeLabel( const UINT32 ntrode, const char *label, UINT32 nInstance = 0);

// Sample Group (GROUP) Information Packets
cbRESULT cbGetSampleGroupInfo(UINT32 proc, UINT32 group, char *label, UINT32 *period, UINT32 *length, UINT32 nInstance = 0);
cbRESULT cbGetSampleGroupList(UINT32 proc, UINT32 group, UINT32 *length, UINT16 *list, UINT32 nInstance = 0);
cbRESULT cbSetSampleGroupOptions(UINT32 proc, UINT32 group, UINT32 period, char *label, UINT32 nInstance = 0);
// Retreives the Sample Group information in a processor and their definitions
// Labels are 16-characters maximum.
//
// Returns: cbRESULT_OK if data successfully retreived.
//          cbRESULT_INVALIDADDRESS if no hardware at the specified Proc and Bank address
//          cbRESULT_NOLIBRARY if the library was not properly initialized.

// Analog Input (AINP) Information Packets
/// This contains the details for each channel within the system.
cbRESULT cbGetChanInfo(UINT32 chan, cbPKT_CHANINFO *pChanInfo, UINT32 nInstance = 0);
// Get the full channel config.
//
// Returns: cbRESULT_OK if data successfully retreived or packet successfully queued to be sent.
//          cbRESULT_INVALIDCHANNEL if the specified channel is not mapped or does not exist.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.
cbRESULT cbGetChanAmplitudeReject(UINT32 chan, cbAMPLITUDEREJECT *AmplitudeReject, UINT32 nInstance = 0);
cbRESULT cbSetChanAmplitudeReject(UINT32 chan, const cbAMPLITUDEREJECT AmplitudeReject, UINT32 nInstance = 0);
// Get and Set the user-assigned amplitude reject values.
//
// Returns: cbRESULT_OK if data successfully retreived or packet successfully queued to be sent.
//          cbRESULT_INVALIDCHANNEL if the specified channel is not mapped or does not exist.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.

cbRESULT cbGetChanAutoThreshold(UINT32 chan, UINT32 *bEnabled, UINT32 nInstance = 0);
cbRESULT cbSetChanAutoThreshold(UINT32 chan, const UINT32 bEnabled, UINT32 nInstance = 0);
// Get and Set the user-assigned auto threshold option
//
// Returns: cbRESULT_OK if data successfully retreived or packet successfully queued to be sent.
//          cbRESULT_INVALIDCHANNEL if the specified channel is not mapped or does not exist.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.

cbRESULT cbGetChanUnitMapping( UINT32 chan, cbMANUALUNITMAPPING *unitmapping, UINT32 nInstance = 0);
cbRESULT cbSetChanUnitMapping( UINT32 chan, cbMANUALUNITMAPPING *unitmapping, UINT32 nInstance = 0);
// Get and Set the user-assigned unit override for the channel.
//
// Returns: cbRESULT_OK if data successfully retreived or packet successfully queued to be sent.
//          cbRESULT_INVALIDCHANNEL if the specified channel is not mapped or does not exist.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.

cbRESULT cbGetChanLoc(UINT32 chan, UINT32 *proc, UINT32 *bank, char *banklabel, UINT32 *term, UINT32 nInstance = 0);
// Gives the physical processor number, bank label, and terminal number of the specified channel
// by reading the configuration data in the Central App Cache.  Bank Labels are the name of the
// bank that is written on the instrument and they are null-terminated, up to 16 char long.
//
// Returns: cbRESULT_OK if all is ok
//          cbRESULT_INVALIDCHANNEL if the specified channel is not mapped or does not exist.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.


// flags for user flags...no effect on the cerebus
//#define cbUSER_DISABLED     0x00000001  // Channel should be electrically disabled
//#define cbUSER_EXPERIMENT   0x00000100  // Channel used for experiment environment information
//#define cbUSER_NEURAL       0x00000200  // Channel connected to neural electrode or signal

cbRESULT cbGetChanLabel(UINT32 chan, char *label, UINT32 *userflags, INT32 *position, UINT32 nInstance = 0);
cbRESULT cbSetChanLabel(UINT32 chan, const char *label, UINT32 userflags,  INT32 *position, UINT32 nInstance = 0);
// Get and Set the user-assigned label for the channel.  Channel Names may be up to 16 chars long
// and should be null terminated if shorter.
//
// Returns: cbRESULT_OK if data successfully retreived or packet successfully queued to be sent.
//          cbRESULT_INVALIDCHANNEL if the specified channel is not mapped or does not exist.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.

cbRESULT cbGetChanNTrodeGroup(UINT32 chan, UINT32 *NTrodeGroup, UINT32 nInstance = 0);
cbRESULT cbSetChanNTrodeGroup(UINT32 chan, const UINT32 NTrodeGroup, UINT32 nInstance = 0);
// Get and Set the user-assigned label for the N-Trode.  N-Trode Names may be up to 16 chars long
// and should be null terminated if shorter.
//
// Returns: cbRESULT_OK if data successfully retreived or packet successfully queued to be sent.
//          cbRESULT_INVALIDCHANNEL if the specified channel is not mapped or does not exist.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.

/////////////////////////////////////////////////////////////////////////////////
// These are part of the "reflected" mechanism. They go out as type 0xE? and come
// Back in as type 0x6?

/// Packet which says that these channels are now selected
cbRESULT cbGetChannelSelection(cbPKT_UNIT_SELECTION * pPktUnitSel, uint8_t nInstrument, UINT32 nInstance = 0);
cbRESULT cbInitPktChannelSelection(cbPKT_UNIT_SELECTION * pPktUnitSel, uint8_t nInstrument, UINT32 nLastChan = 1);
// Get and Set the channel unit selection status
// Returns: cbRESULT_OK if data successfully retreived.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.
int cbUnitToUnitmask(int nUnit);

// file config options
/// File recording can be started or stopped externally using this packet.  It also contains a timeout mechanism to notify
/// if file isn't still recording.
cbRESULT cbGetFileInfo(cbPKT_FILECFG * filecfg, UINT32 nInstance = 0);
// Retreives the file recordign status
// Returns: cbRESULT_OK if data successfully retreived.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.

/// Sets the mapfile for applications that use a mapfile so they all display similarly.
cbRESULT cbInitPktMapfile(cbPKT_MAPFILE* pPkt, const char* szMapFilename);

//-----------------------------------------------------
///// Packets to tell me about the spike sorting model
/// Sets the noise boundary parameters
// The information obtained by these functions is implicit within the structure data,
// but they are provided for convenience
void GetAxisLengths(const cbPKT_SS_NOISE_BOUNDARY *pPkt, float afAxisLen[3]);
void GetRotationAngles(const cbPKT_SS_NOISE_BOUNDARY *pPkt, float afTheta[3]);
void InitPktSSNoiseBoundary(cbPKT_SS_NOISE_BOUNDARY* pPkt, UINT32 chan, float cen1, float cen2, float cen3, float maj1, float maj2, float maj3,
    float min11, float min12, float min13, float min21, float min22, float min23);

/// Send this packet to the NSP to tell it to reset all spike sorting to default values
cbRESULT cbSetSSReset(UINT32 nInstance = 0);
// Restart spike sorting (applies to histogram peak count).
//
// Returns: cbRESULT_OK if data successfully retrieved.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.

// This packet contains the status of the automatic spike sorting.
//
/// Send this packet to the NSP to tell it to re calculate all PCA Basis Vectors and Values
cbRESULT cbSetSSRecalc(UINT8 proc, UINT32 chan, UINT32 mode, UINT32 nInstance = 0);
// Recalc spike sorting (applies to PCA based algorithms).
//
// Returns: cbRESULT_OK if data successfully retrieved.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.

// This packet holds the Line Noise Cancellation parameters
cbRESULT cbGetLncParameters(uint8_t nInstrument, UINT32 *nLncFreq, UINT32 *nLncRefChan, UINT32 *nLncGMode, UINT32 nInstance = 0);
cbRESULT cbSetLncParameters(uint8_t nInstrument, UINT32 nLncFreq, UINT32 nLncRefChan, UINT32 nLncGMode, UINT32 nInstance = 0);
// Get/Set the system-wide LNC parameters.
//
// Returns: cbRESULT_OK if data successfully retrieved.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.

// Send this packet to force the digital output to this value
cbRESULT cbSetDigitalOutput(UINT32 nChan, bool bSet, UINT32 nInstance = 0);
// Set digital output value.
//
// Returns: cbRESULT_OK if data successfully retrieved.
//          cbRESULT_NOLIBRARY if the library was not properly initialized.

/// Contains the parameters to define a waveform for Analog Output channels
cbRESULT cbGetAoutWaveform(UINT32 channel, UINT8  trigNum, UINT16  * mode, UINT32  * repeats, UINT8  * trig,
                           UINT16  * trigChan, UINT16  * trigValue, cbWaveformData * wave, UINT32 nInstance = 0);
cbRESULT cbGetAoutWaveformNumber(UINT32 channel, UINT32* wavenum, UINT32 nInstance = 0);
// Returns anallog output waveform information
// Returns cbRESULT_OK if successful, cbRESULT_NOLIBRARY if library was never initialized.

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Shared Memory Definitions used by Central App and Cerebus library functions
//
///////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: separate out these definitions so that there are no conditional compiles
#ifdef __cplusplus

typedef struct {
    COLORREF winrsvd[48];
    COLORREF dispback;
    COLORREF dispgridmaj;
    COLORREF dispgridmin;
    COLORREF disptext;
    COLORREF dispwave;
    COLORREF dispwavewarn;
    COLORREF dispwaveclip;
    COLORREF dispthresh;
    COLORREF dispmultunit;
    COLORREF dispunit[16];  // 0 = unclassified
    COLORREF dispnoise;
    COLORREF dispchansel[3];
    COLORREF disptemp[5];
    COLORREF disprsvd[14];
} cbCOLORTABLE;

cbRESULT cbGetColorTable(cbCOLORTABLE **colortable, UINT32 nInstance = 0);


typedef struct {
    float fRMSAutoThresholdDistance;    // multiplier to use for autothresholding when using
                                        // RMS to guess noise
    UINT32 reserved[31];
} cbOPTIONTABLE;


// Get/Set the multiplier to use for autothresholdine when using RMS to guess noise
// This will adjust fAutoThresholdDistance above, but use the API instead
float cbGetRMSAutoThresholdDistance(UINT32 nInstance = 0);
void cbSetRMSAutoThresholdDistance(float fRMSAutoThresholdDistance, UINT32 nInstance = 0);

//////////////////////////////////////////////////////////////////////////////////////////////////


#define cbPKT_SPKCACHEPKTCNT  400
#define cbPKT_SPKCACHELINECNT cbMAXCHANS

typedef struct {
    UINT32 chid;            // ID of the Channel
    UINT32 pktcnt;          // # of packets which can be saved
    UINT32 pktsize;         // Size of an individual packet
    UINT32 head;            // Where (0 based index) in the circular buffer to place the NEXT packet.
    UINT32 valid;           // How many packets have come in since the last configuration
    cbPKT_SPK spkpkt[cbPKT_SPKCACHEPKTCNT];     // Circular buffer of the cached spikes
} cbSPKCACHE;

typedef struct {
    UINT32 flags;
    UINT32 chidmax;
    UINT32 linesize;
    UINT32 spkcount;
    cbSPKCACHE cache[cbPKT_SPKCACHELINECNT];
} cbSPKBUFF;

cbRESULT cbGetSpkCache(UINT32 chid, cbSPKCACHE **cache, UINT32 nInstance = 0);

#ifdef WIN32
enum WM_USER_GLOBAL
{
    WM_USER_WAITEVENT = WM_USER,                // mmtimer says it is OK to continue
    WM_USER_CRITICAL_DATA_CATCHUP,              // We have reached a critical data point and we have skipped
};
#endif // WIN32

//#define cbRECBUFFLEN   0x00400000     //4194304
#define cbRECBUFFLEN   cbNUM_FE_CHANS * 65536 * 4   // max should be 4095 channels or 0x1FFFFFFF

typedef struct {
    UINT32 received;
    PROCTIME lasttime;
    UINT32 headwrap;
    UINT32 headindex;
    UINT32 buffer[cbRECBUFFLEN];
} cbRECBUFF;

#ifdef _MSC_VER
// The following structure is used to hold Cerebus packets queued for transmission to the NSP.
// The length of the structure is set during initialization of the buffer in the Central App.
// The pragmas allow a zero-length data field entry in the structure for referencing the data.
#pragma warning(push)
#pragma warning(disable:4200)
#endif // _MSC_VER

typedef struct {
    UINT32 transmitted;     // How many packets have we sent out?

    UINT32 headindex;       // 1st empty position
                            // (moves on filling)

    UINT32 tailindex;       // 1 past last emptied position (empty when head = tail)
                            // Moves on emptying

    UINT32 last_valid_index;// index number of greatest valid starting index for a head (or tail)
    UINT32 bufferlen;       // number of indexes in buffer (units of UINT32) <------+
    UINT32 buffer[0];       // big buffer of data...there are actually "bufferlen"--+ indices
} cbXMTBUFF;

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#define WM_USER_SET_THOLD_SIGMA     (WM_USER + 100)
#define WM_USER_SET_THOLD_TIME      (WM_USER + 101)

typedef struct {
    // ***** THESE MUST BE 1ST IN THE STRUCTURE WITH MODELSET LAST OF THESE ***
    // ***** SEE WriteCCFNoPrompt() ***
    cbPKT_FS_BASIS          asBasis[cbMAXCHANS];    // All of the PCA basis values
    cbPKT_SS_MODELSET       asSortModel[cbMAXCHANS][cbMAXUNITS + 2];    // All of the model (rules) for spike sorting

    //////// These are spike sorting options
    cbPKT_SS_DETECT         pktDetect;        // parameters dealing with actual detection
    cbPKT_SS_ARTIF_REJECT   pktArtifReject;   // artifact rejection
    cbPKT_SS_NOISE_BOUNDARY pktNoiseBoundary[cbMAXCHANS]; // where o'where are the noise boundaries
    cbPKT_SS_STATISTICS     pktStatistics;    // information about statistics
    cbPKT_SS_STATUS         pktStatus;        // Spike sorting status

} cbSPIKE_SORTING;

#define PCSTAT_TYPE_CERVELLO        0x00000001      // Cervello type system
#define PCSTAT_DISABLE_RAW          0x00000002      // Disable recording of raw data

/// @brief Workspace configurationinformation
///
/// Sets the workspace configuration as applications are opened/closed.
///
typedef struct {
    UINT32 m_nWorkspace;            //!< workspace - 1 based
    enLaunchView m_nApplication;    //!< index of application from Launching.h
    UINT32 m_nChannel;              //!< channel number displayed 1 based
    UINT32 m_nLeft;
    UINT32 m_nTop;
    UINT32 m_nRight;
    UINT32 m_nBottom;
} APP_WORKSPACE;
enum WORKSPACE_TYPE { WORKSPACE_ALWAYS, WORKSPACE_PROMPT, WORKSPACE_NEVER };
#define cbMAXAPPWORKSPACES 10

enum   NSP_STATUS  { NSP_INIT, NSP_NOIPADDR, NSP_NOREPLY, NSP_FOUND, NSP_INVALID };

class cbPcStatus
{
public:
    cbPKT_UNIT_SELECTION isSelection[cbMAXPROCS];

private:
    INT32 m_iBlockRecording;
    UINT32 m_nPCStatusFlags;

    UINT32 m_nNumFEChans;           // number of each type of channels received from the instrument
    UINT32 m_nNumAnainChans;
    UINT32 m_nNumAnalogChans;
    UINT32 m_nNumAoutChans;
    UINT32 m_nNumAudioChans;
    UINT32 m_nNumAnalogoutChans;
    UINT32 m_nNumDiginChans;
    UINT32 m_nNumSerialChans;
    UINT32 m_nNumDigoutChans;
    UINT32 m_nNumTotalChans;
    NSP_STATUS  m_nNspStatus[cbMAXPROCS];       // true if the nsp has received a sysinfo from each NSP
    UINT32 m_nNumNTrodesPerInstrument[cbMAXPROCS];
    UINT32 m_nGeminiSystem;         // used as boolean true if connected to a gemini system
    APP_WORKSPACE m_icAppWorkspace[cbMAXAPPWORKSPACES];

public:
    cbPcStatus() :
        m_iBlockRecording(0),
        m_nPCStatusFlags(0),
        m_nGeminiSystem(0)
        {
            for (uint8_t nInstrument = cbNSP1; nInstrument <= cbMAXPROCS; ++nInstrument)
            {
                uint8_t nInstrumentIdx = nInstrument - 1;
                isSelection[nInstrumentIdx].lastchan = 1;
                m_nNspStatus[nInstrumentIdx] = NSP_INIT;
            }
        }

    bool IsRecordingBlocked() { return m_iBlockRecording != 0; }
    void SetBlockRecording(bool bBlockRecording) { m_iBlockRecording += bBlockRecording ? 1 : -1; }
    UINT32 cbGetPCStatusFlags() { return m_nPCStatusFlags; }
    void cbSetPCStatusFlags(UINT32 nPCStatusFlags) { m_nPCStatusFlags = nPCStatusFlags; }

    UINT32 cbGetNumFEChans()        { return m_nNumFEChans; }
    UINT32 cbGetNumAnainChans()     { return m_nNumAnainChans; }
    UINT32 cbGetNumAnalogChans()    { return m_nNumAnalogChans; }
    UINT32 cbGetNumAoutChans()      { return m_nNumAoutChans; }
    UINT32 cbGetNumAudioChans()     { return m_nNumAudioChans; }
    UINT32 cbGetNumAnalogoutChans() { return m_nNumAnalogoutChans; }
    UINT32 cbGetNumDiginChans()     { return m_nNumDiginChans; }
    UINT32 cbGetNumSerialChans()    { return m_nNumSerialChans; }
    UINT32 cbGetNumDigoutChans()    { return m_nNumDigoutChans; }
    UINT32 cbGetNumTotalChans()     { return m_nNumTotalChans; }
    NSP_STATUS cbGetNspStatus(uint8_t nInstrument) { return m_nNspStatus[nInstrument - 1]; }
    UINT32 cbIsGeminiSystem()       { return m_nGeminiSystem; }
    UINT32 cbSetAppWorkspace(UINT32 nWorkspace, enLaunchView nApplication, UINT32 nChannel, UINT32 nLeft, UINT32 nTop, UINT32 nWidth, UINT32 nHeight);
    UINT32 cbClearAppWorkspace(UINT32 nWorkspace);

    void cbSetNumFEChans(UINT32 nNumFEChans)                { m_nNumFEChans = nNumFEChans; }
    void cbSetNumAnainChans(UINT32 nNumAnainChans)          { m_nNumAnainChans = nNumAnainChans; }
    void cbSetNumAnalogChans(UINT32 nNumAnalogChans)        { m_nNumAnalogChans = nNumAnalogChans; }
    void cbSetNumAoutChans(UINT32 nNumAoutChans)            { m_nNumAoutChans = nNumAoutChans; }
    void cbSetNumAudioChans(UINT32 nNumAudioChans)          { m_nNumAudioChans = nNumAudioChans; }
    void cbSetNumAnalogoutChans(UINT32 nNumAnalogoutChans)  { m_nNumAnalogoutChans = nNumAnalogoutChans; }
    void cbSetNumDiginChans(UINT32 nNumDiginChans)        { m_nNumDiginChans = nNumDiginChans; }
    void cbSetNumSerialChans(UINT32 nNumSerialChans)        { m_nNumSerialChans = nNumSerialChans; }
    void cbSetNumDigoutChans(UINT32 nNumDigoutChans)        { m_nNumDigoutChans = nNumDigoutChans; }
    void cbSetNumTotalChans(UINT32 nNumTotalChans)          { m_nNumTotalChans = nNumTotalChans; }
    void cbSetNspStatus(uint8_t nInstrument, NSP_STATUS nStatus)   { m_nNspStatus[nInstrument - 1] = nStatus; }
    void cbSetGeminiSystem(UINT32 nGeminiSystem)            { m_nGeminiSystem = nGeminiSystem; }
    UINT32 cbGetAppWorkspace(UINT32 nWorkspace, enLaunchView *nApplication, UINT32 *nChannel, UINT32 *nLeft, UINT32* nTop, UINT32 *nWidth, UINT32 *nHeight);
};

typedef struct {
    UINT32          version;
    UINT32          sysflags;
    cbOPTIONTABLE   optiontable;    // Should be 32 32-bit values
    cbCOLORTABLE    colortable;     // Should be 96 32-bit values
    cbPKT_SYSINFO   sysinfo;
    cbPKT_PROCINFO  procinfo[cbMAXPROCS];
    cbPKT_BANKINFO  bankinfo[cbMAXPROCS][cbMAXBANKS];
    cbPKT_GROUPINFO groupinfo[cbMAXPROCS][cbMAXGROUPS]; // sample group ID (1-4=proc1, 5-8=proc2, etc)
    cbPKT_FILTINFO  filtinfo[cbMAXPROCS][cbMAXFILTS];
    cbPKT_ADAPTFILTINFO adaptinfo[cbMAXPROCS];          //  Settings about adapting
    cbPKT_REFELECFILTINFO refelecinfo[cbMAXPROCS];      //  Settings about reference electrode filtering
    cbPKT_CHANINFO  chaninfo[cbMAXCHANS];
    cbSPIKE_SORTING isSortingOptions;   // parameters dealing with spike sorting
    cbPKT_NTRODEINFO isNTrodeInfo[cbMAXNTRODES];  // allow for the max number of ntrodes (if all are stereo-trodes)
    cbPKT_AOUT_WAVEFORM isWaveform[AOUT_NUM_GAIN_CHANS][cbMAX_AOUT_TRIGGER]; // Waveform parameters
    cbPKT_LNC       isLnc[cbMAXPROCS]; //LNC parameters
    cbPKT_NPLAY     isNPlay; // nPlay Info
    cbVIDEOSOURCE   isVideoSource[cbMAXVIDEOSOURCE]; // Video source
    cbTRACKOBJ      isTrackObj[cbMAXTRACKOBJ];       // Trackable objects
    cbPKT_FILECFG   fileinfo; // File recording status
    // This must be at the bottom of this structure because it is variable size 32-bit or 64-bit
    // depending on the compile settings e.g. 64-bit cbmex communicating with 32-bit Central
    HANDLE          hwndCentral;    // Handle to the Window in Central

} cbCFGBUFF;

// Latest CCF structure
typedef struct {
    cbPKT_CHANINFO isChan[cbMAXCHANS];
    cbPKT_ADAPTFILTINFO isAdaptInfo[cbMAXPROCS];
    cbPKT_SS_DETECT isSS_Detect;
    cbPKT_SS_ARTIF_REJECT isSS_ArtifactReject;
    cbPKT_SS_NOISE_BOUNDARY isSS_NoiseBoundary[cbNUM_ANALOG_CHANS];
    cbPKT_SS_STATISTICS isSS_Statistics;
    cbPKT_SS_STATUS isSS_Status;
    cbPKT_SYSINFO isSysInfo;
    cbPKT_NTRODEINFO isNTrodeInfo[cbMAXNTRODES];
    cbPKT_AOUT_WAVEFORM isWaveform[AOUT_NUM_GAIN_CHANS][cbMAX_AOUT_TRIGGER];
    cbPKT_FILTINFO filtinfo[cbNUM_DIGITAL_FILTERS];
    cbPKT_LNC isLnc[cbMAXPROCS];
} cbCCF;

// CCF processing state
typedef enum _cbStateCCF
{
    CCFSTATE_READ = 0,     // Reading in progress
    CCFSTATE_WRITE,        // Writing in progress
    CCFSTATE_SEND ,        // Sendign in progress
    CCFSTATE_CONVERT,      // Conversion in progress
    CCFSTATE_THREADREAD,   // Total threaded read progress
    CCFSTATE_THREADWRITE,  // Total threaded write progress
    CCFSTATE_UNKNOWN, // (Always the last) unknown state
} cbStateCCF;

// External Global Variables

extern HANDLE      cb_xmt_global_buffer_hnd[cbMAXOPEN];       // Transmit queues to send out of this PC
extern cbXMTBUFF*  cb_xmt_global_buffer_ptr[cbMAXOPEN];

extern HANDLE      cb_xmt_local_buffer_hnd[cbMAXOPEN];        // Transmit queues only for local (this PC) use
extern cbXMTBUFF*  cb_xmt_local_buffer_ptr[cbMAXOPEN];

extern HANDLE       cb_rec_buffer_hnd[cbMAXOPEN];
extern cbRECBUFF*   cb_rec_buffer_ptr[cbMAXOPEN];
extern HANDLE       cb_cfg_buffer_hnd[cbMAXOPEN];
extern cbCFGBUFF*   cb_cfg_buffer_ptr[cbMAXOPEN];
extern HANDLE       cb_pc_status_buffer_hnd[cbMAXOPEN];
extern cbPcStatus*  cb_pc_status_buffer_ptr[cbMAXOPEN];        // parameters dealing with local pc status
extern HANDLE       cb_spk_buffer_hnd[cbMAXOPEN];
extern cbSPKBUFF*   cb_spk_buffer_ptr[cbMAXOPEN];
extern HANDLE       cb_sig_event_hnd[cbMAXOPEN];

extern UINT32       cb_library_initialized[cbMAXOPEN];
extern UINT32       cb_library_index[cbMAXOPEN];

#endif // __cplusplus

#pragma pack(pop)

#endif      // end of include guard
