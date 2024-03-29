//////////////////////////////////////////////////////////////////////////////
//
// (c) Copyright 2010 - 2022 Blackrock Microsystems, LLC
//
// $Workfile: cbsdk.h $
// $Archive: /Cerebus/Human/WindowsApps/cbmex/cbsdk.h $
// $Revision: 1 $
// $Date: 2/17/11 3:15p $
// $Author: Ehsan $
//
// $NoKeywords: $
//
//////////////////////////////////////////////////////////////////////////////
//
// Notes:
//  Only functions are exported, no data, and no classes
//  No exception is thrown, every function returns an error code that should be consulted by caller
//  Each instance will run one light thread that is responsible for networking, buffering and callbacks
//  Additional threads may be created by Qt based on platforms
// Note for developers:
//   Data structure in cbsdk should be decoupled from cbhlib as much as possible because cbhlib may change at any version
//   for some functions it is easier to just rely on the data structure as defined in cbhwlib (e.g. callbacks and CCF),
//   but this may change in future to give cbsdk a more stable and independent API
//
/**
* \file cbsdk.h
* \brief Cerebus SDK - This header file is distributed as part of the SDK.
*/

#ifndef CBSDK_H_INCLUDED
#define CBSDK_H_INCLUDED

#include "cbhwlib.h"

#ifdef STATIC_CBSDK_LINK
#undef CBSDK_EXPORTS
#endif

#ifdef WIN32
// Windows shared library
#ifdef CBSDK_EXPORTS
    #define CBSDKAPI __declspec(dllexport)
#elif ! defined(STATIC_CBSDK_LINK)
    #define CBSDKAPI __declspec(dllimport)
#else
    #define CBSDKAPI
#endif
#else
// Non Windows shared library
#ifdef CBSDK_EXPORTS
    #define CBSDKAPI __attribute__ ((visibility ("default")))
#else
    #define CBSDKAPI
#endif
#endif

/**
* \brief Library version information.
*/
typedef struct _cbSdkVersion
{
    // Library version
    UINT32 major;
    UINT32 minor;
    UINT32 release;
    UINT32 beta;
    // Protocol version
    UINT32 majorp;
    UINT32 minorp;
    // NSP version
    UINT32 nspmajor;
    UINT32 nspminor;
    UINT32 nsprelease;
    UINT32 nspbeta;
} cbSdkVersion;

/// cbSdk return values
typedef enum _cbSdkResult
{
    CBSDKRESULT_WARNCONVERT            =     3, ///< If file conversion is needed
    CBSDKRESULT_WARNCLOSED             =     2, ///< Library is already closed
    CBSDKRESULT_WARNOPEN               =     1, ///< Library is already opened
    CBSDKRESULT_SUCCESS                =     0, ///< Successful operation
    CBSDKRESULT_NOTIMPLEMENTED         =    -1, ///< Not implemented
    CBSDKRESULT_UNKNOWN                =    -2, ///< Unknown error
    CBSDKRESULT_INVALIDPARAM           =    -3, ///< Invalid parameter
    CBSDKRESULT_CLOSED                 =    -4, ///< Interface is closed cannot do this operation
    CBSDKRESULT_OPEN                   =    -5, ///< Interface is open cannot do this operation
    CBSDKRESULT_NULLPTR                =    -6, ///< Null pointer
    CBSDKRESULT_ERROPENCENTRAL         =    -7, ///< Unable to open Central interface
    CBSDKRESULT_ERROPENUDP             =    -8, ///< Unable to open UDP interface (might happen if default)
    CBSDKRESULT_ERROPENUDPPORT         =    -9, ///< Unable to open UDP port
    CBSDKRESULT_ERRMEMORYTRIAL         =   -10, ///< Unable to allocate RAM for trial cache data
    CBSDKRESULT_ERROPENUDPTHREAD       =   -11, ///< Unable to open UDP timer thread
    CBSDKRESULT_ERROPENCENTRALTHREAD   =   -12, ///< Unable to open Central communication thread
    CBSDKRESULT_INVALIDCHANNEL         =   -13, ///< Invalid channel number
    CBSDKRESULT_INVALIDCOMMENT         =   -14, ///< Comment too long or invalid
    CBSDKRESULT_INVALIDFILENAME        =   -15, ///< Filename too long or invalid
    CBSDKRESULT_INVALIDCALLBACKTYPE    =   -16, ///< Invalid callback type
    CBSDKRESULT_CALLBACKREGFAILED      =   -17, ///< Callback register/unregister failed
    CBSDKRESULT_ERRCONFIG              =   -18, ///< Trying to run an unconfigured method
    CBSDKRESULT_INVALIDTRACKABLE       =   -19, ///< Invalid trackable id, or trackable not present
    CBSDKRESULT_INVALIDVIDEOSRC        =   -20, ///< Invalid video source id, or video source not present
    CBSDKRESULT_ERROPENFILE            =   -21, ///< Cannot open file
    CBSDKRESULT_ERRFORMATFILE          =   -22, ///< Wrong file format
    CBSDKRESULT_OPTERRUDP              =   -23, ///< Socket option error (possibly permission issue)
    CBSDKRESULT_MEMERRUDP              =   -24, ///< Socket memory assignment error
    CBSDKRESULT_INVALIDINST            =   -25, ///< Invalid range or instrument address
    CBSDKRESULT_ERRMEMORY              =   -26, ///< library memory allocation error
    CBSDKRESULT_ERRINIT                =   -27, ///< Library initialization error
    CBSDKRESULT_TIMEOUT                =   -28, ///< Conection timeout error
    CBSDKRESULT_BUSY                   =   -29, ///< Resource is busy
    CBSDKRESULT_ERROFFLINE             =   -30, ///< Instrument is offline
} cbSdkResult;

/// cbSdk Connection Type (Central, UDP, other)
typedef enum _cbSdkConnectionType
{
    CBSDKCONNECTION_DEFAULT = 0, ///< Try Central then UDP
    CBSDKCONNECTION_CENTRAL,     ///< Use Central
    CBSDKCONNECTION_UDP,         ///< Use UDP
    CBSDKCONNECTION_GEMININSP,   ///< Connect to Gemini NSP
    CBSDKCONNECTION_GEMINIHUB,   ///< Connect to Gemini Hub
    CBSDKCONNECTION_GEMINIHUB2,  ///< Connect to a second Gemini Hub
    CBSDKCONNECTION_CLOSED,      ///< Closed
    CBSDKCONNECTION_COUNT ///< Allways the last value (Unknown)
} cbSdkConnectionType;

/// Instrument Type
typedef enum _cbSdkInstrumentType
{
    CBSDKINSTRUMENT_NSP = 0,       ///< NSP
    CBSDKINSTRUMENT_NPLAY,         ///< Local nPlay
    CBSDKINSTRUMENT_LOCALNSP,      ///< Local NSP
    CBSDKINSTRUMENT_REMOTENPLAY,   ///< Remote nPlay
    CBSDKINSTRUMENT_GEMININSP,     ///< Gemini NSP
    CBSDKINSTRUMENT_GEMINIHUB,     ///< Gemini Hub
    CBSDKINSTRUMENT_COUNT ///< Allways the last value (Invalid)
} cbSdkInstrumentType;

/// CCF operation progress and status
typedef struct _cbSdkCCFEvent
{
    cbStateCCF state;           ///< CCF state
    cbSdkResult result;         ///< Last result
    LPCSTR szFileName;          ///< CCF filename under operation
    UINT8 progress; ///< Progress (in percent)
} cbSdkCCFEvent;

/// Reason for packet loss
typedef enum _cbSdkPktLostEventType
{
    CBSDKPKTLOSTEVENT_UNKNOWN = 0,       ///< Unknown packet lost
    CBSDKPKTLOSTEVENT_LINKFAILURE,       ///< Link failure
    CBSDKPKTLOSTEVENT_PC2NSP,            ///< PC to NSP connection lost
    CBSDKPKTLOSTEVENT_NET,               ///< Network error
} cbSdkPktLostEventType;

/// Packet lost event
typedef struct _cbSdkPktLostEvent
{
    cbSdkPktLostEventType type;     ///< packet lost event type
} cbSdkPktLostEvent;

/// Instrument information
typedef struct _cbSdkInstInfo
{
    UINT32 instInfo;     ///< bitfield of cbINSTINFO_* (0 means closed)
} cbSdkInstInfo;

/// Packet type information
typedef enum _cbSdkPktType
{
    cbSdkPkt_PACKETLOST = 0, ///< will be received only by the first registered callback
                             ///< data points to cbSdkPktLostEvent
    cbSdkPkt_INSTINFO,       ///< data points to cbSdkInstInfo
    cbSdkPkt_SPIKE,          ///< data points to cbPKT_SPK
    cbSdkPkt_DIGITAL,        ///< data points to cbPKT_DINP
    cbSdkPkt_SERIAL,         ///< data points to cbPKT_DINP
    cbSdkPkt_CONTINUOUS,     ///< data points to cbPKT_GROUP
    cbSdkPkt_TRACKING,       ///< data points to cbPKT_VIDEOTRACK
    cbSdkPkt_COMMENT,        ///< data points to cbPKT_COMMENT
    cbSdkPkt_GROUPINFO,      ///< data points to cbPKT_GROUPINFO
    cbSdkPkt_CHANINFO,       ///< data points to cbPKT_CHANINFO
    cbSdkPkt_FILECFG,        ///< data points to cbPKT_FILECFG
    cbSdkPkt_POLL,           ///< data points to cbPKT_POLL
    cbSdkPkt_SYNCH,          ///< data points to cbPKT_VIDEOSYNCH
    cbSdkPkt_NM,             ///< data points to cbPKT_NM
    cbSdkPkt_CCF,            ///< data points to cbSdkCCFEvent
    cbSdkPkt_IMPEDANCE,      ///< data points to cbPKT_IMPEDANCE
    cbSdkPkt_SYSHEARTBEAT,   ///< data points to cbPKT_SYSHEARTBEAT
    cbSdkPkt_LOG,		     ///< data points to cbPKT_LOG
    cbSdkPkt_COUNT ///< Always the last value
} cbSdkPktType;

/// Type of events to monitor
typedef enum _cbSdkCallbackType
{
    CBSDKCALLBACK_ALL = 0,      ///< Monitor all events
    CBSDKCALLBACK_INSTINFO,     ///< Monitor instrument connection information
    CBSDKCALLBACK_SPIKE,        ///< Monitor spike events
    CBSDKCALLBACK_DIGITAL,      ///< Monitor digital input events
    CBSDKCALLBACK_SERIAL,       ///< Monitor serial input events
    CBSDKCALLBACK_CONTINUOUS,   ///< Monitor continuous events
    CBSDKCALLBACK_TRACKING,     ///< Monitor video tracking events
    CBSDKCALLBACK_COMMENT,      ///< Monitor comment or custom events
    CBSDKCALLBACK_GROUPINFO,    ///< Monitor channel group info events
    CBSDKCALLBACK_CHANINFO,     ///< Monitor channel info events
    CBSDKCALLBACK_FILECFG,      ///< Monitor file config events
    CBSDKCALLBACK_POLL,         ///< respond to poll
    CBSDKCALLBACK_SYNCH,        ///< Monitor video synchronizarion events
    CBSDKCALLBACK_NM,           ///< Monitor NeuroMotive events
    CBSDKCALLBACK_CCF,          ///< Monitor CCF events
    CBSDKCALLBACK_IMPEDENCE,    ///< Monitor impedence events
    CBSDKCALLBACK_SYSHEARTBEAT, ///< Monitor system heartbeats (100 times a second)
    CBSDKCALLBACK_LOG,		    ///< Monitor system heartbeats (100 times a second)
    CBSDKCALLBACK_COUNT  ///< Always the last value
} cbSdkCallbackType;

/// Trial type
typedef enum _cbSdkTrialType
{
    CBSDKTRIAL_CONTINUOUS,
    CBSDKTRIAL_EVENTS,
    CBSDKTRIAL_COMMETNS,
    CBSDKTRIAL_TRACKING,
} cbSdkTrialType;

/** Callback details.
* \n pEventData points to a cbPkt_* structure depending on the type
* \n pCallbackData is what is used to register the callback
*/
typedef void (* cbSdkCallback)(UINT32 nInstance, const cbSdkPktType type, const void* pEventData, void* pCallbackData);


/// The default number of continuous samples that will be stored per channel in the trial buffer
#define cbSdk_CONTINUOUS_DATA_SAMPLES 102400 // multiple of 4096
/// The default number of events that will be stored per channel in the trial buffer
#define cbSdk_EVENT_DATA_SAMPLES (2 * 8192) // multiple of 4096

/// Maximum file size (in bytes) that is allowed to upload to NSP
#define cbSdk_MAX_UPOLOAD_SIZE (1024 * 1024 * 1024)

/// \todo these should become functions as we may introduce different instruments
#define cbSdk_TICKS_PER_SECOND  30000.0
/// The number of seconds corresponding to one cb clock tick
#define cbSdk_SECONDS_PER_TICK  (1.0 / cbSdk_TICKS_PER_SECOND)

/// Trial spike events
typedef struct _cbSdkTrialEvent
{
    UINT16 count; ///< Number of valid channels in this trial (up to cbMAXCHANS)
    UINT16 chan[cbMAXCHANS]; ///< channel numbers (1-based)
    UINT32 num_samples[cbMAXCHANS][cbMAXUNITS + 1]; ///< number of samples
    void * timestamps[cbMAXCHANS][cbMAXUNITS + 1];   ///< Buffer to hold time stamps
    void * waveforms[cbMAXCHANS]; ///< Buffer to hold waveforms or digital values
} cbSdkTrialEvent;

/// Connection information
typedef struct _cbSdkConnection
{
    _cbSdkConnection()
    {
        nInPort = cbNET_UDP_PORT_BCAST;
        nOutPort = cbNET_UDP_PORT_CNT;
        nRecBufSize = (4096 * 2048); // 8MB default needed for best performance
        szInIP = "";
        szOutIP = "";
        nRange = 0;
    }
    int nInPort;  ///< Client port number
    int nOutPort; ///< Instrument port number
    int nRecBufSize; ///< Receive buffer size (0 to ignore altogether)
    LPCSTR szInIP;  ///< Client IPv4 address
    LPCSTR szOutIP; ///< Instrument IPv4 address
    int nRange;     ///< Range of IP addresses to try to open
} cbSdkConnection;

/// Trial continuous data
typedef struct _cbSdkTrialCont
{
    UINT16 count; ///< Number of valid channels in this trial (up to cbNUM_ANALOG_CHANS)
    UINT16 chan[cbNUM_ANALOG_CHANS]; ///< Channel numbers (1-based)
    UINT16 sample_rates[cbNUM_ANALOG_CHANS]; ///< Current sample rate (samples per second)
    UINT32 num_samples[cbNUM_ANALOG_CHANS]; ///< Number of samples
    UINT32 time;  ///< Start time for trial continuous data
    void * samples[cbNUM_ANALOG_CHANS]; ///< Buffer to hold sample vectors
} cbSdkTrialCont;

/// Trial comment data
typedef struct _cbSdkTrialComment
{
    UINT16 num_samples; ///< Number of comments
    UINT8 * charsets;   ///< Buffer to hold character sets
    UINT32 * rgbas;     ///< Buffer to hold rgba values
    UINT8 * * comments; ///< Pointer to comments
    void * timestamps;  ///< Buffer to hold time stamps
} cbSdkTrialComment;

/// Trial video tracking data
typedef struct _cbSdkTrialTracking
{
    UINT16 count;								///< Number of valid trackable objects (up to cbMAXTRACKOBJ)
    UINT16 ids[cbMAXTRACKOBJ];					///< Node IDs (holds count elements)
    UINT16 max_point_counts[cbMAXTRACKOBJ];		///< Maximum point counts (holds count elements)
    UINT16 types[cbMAXTRACKOBJ];				///< Node types (can be cbTRACKOBJ_TYPE_* and determines coordinate counts) (holds count elements)
    UINT8  names[cbMAXTRACKOBJ][cbLEN_STR_LABEL + 1];   ///< Node names (holds count elements)
    UINT16 num_samples[cbMAXTRACKOBJ];			///< Number of samples
    UINT16 * point_counts[cbMAXTRACKOBJ];		///< Buffer to hold number of valid points (up to max_point_counts) (holds count*num_samples elements)
    void * * coords[cbMAXTRACKOBJ] ;			///< Buffer to hold tracking points (holds count*num_samples tarackables, each of max_point_counts points
    UINT32 * synch_frame_numbers[cbMAXTRACKOBJ];///< Buffer to hold synch frame numbers (holds count*num_samples elements)
    UINT32 * synch_timestamps[cbMAXTRACKOBJ];   ///< Buffer to hold synchronized tracking time stamps (in milliseconds) (holds count*num_samples elements)
    void  * timestamps[cbMAXTRACKOBJ];          ///< Buffer to hold tracking time stamps (holds count*num_samples elements)
} cbSdkTrialTracking;

/// Output waveform type
typedef enum _cbSdkWaveformType
{
    cbSdkWaveform_NONE = 0,
    cbSdkWaveform_PARAMETERS,     ///< Parameters
    cbSdkWaveform_SINE,           ///< Sinusoid
    cbSdkWaveform_COUNT, ///< Always the last value
} cbSdkWaveformType;

/// Trigger type
typedef enum _cbSdkWaveformTriggerType
{
    cbSdkWaveformTrigger_NONE = 0, ///< Instant software trigger
    cbSdkWaveformTrigger_DINPREG,      ///< Digital input rising edge trigger
    cbSdkWaveformTrigger_DINPFEG,      ///< Digital input falling edge trigger
    cbSdkWaveformTrigger_SPIKEUNIT,    ///< Spike unit
    cbSdkWaveformTrigger_COMMENTCOLOR, ///< Custom colored event (e.g. NeuroMotive event)
    cbSdkWaveformTrigger_SOFTRESET,    ///< Soft-reset trigger (e.g. file recording start)
    cbSdkWaveformTrigger_EXTENSION,    ///< Extension trigger
    cbSdkWaveformTrigger_COUNT, ///< Always the last value
} cbSdkWaveformTriggerType;

/// Extended pointer-form of cbWaveformData
typedef struct _cbSdkWaveformData
{
    cbSdkWaveformType type;
    UINT32  repeats;
    cbSdkWaveformTriggerType  trig;
    UINT16  trigChan;
    UINT16  trigValue;
    UINT8   trigNum;
    INT16   offset;
    union {
        struct {
            UINT16 sineFrequency;
            INT16  sineAmplitude;
        };
        struct {
            UINT16 phases;
            UINT16  * duration;
            INT16   * amplitude;
        };
    };
} cbSdkWaveformData;

/// Analog output monitor
typedef struct _cbSdkAoutMon
{
    UINT16  chan; ///< (1-based) channel to monitor
    BOOL bTrack; ///< If should monitor last tracked channel
    BOOL bSpike; ///< If spike or continuous should be monitored
} cbSdkAoutMon;

/// CCF data
typedef struct _cbSdkCCF
{
    int ccfver; ///< CCF internal version
    cbCCF data;
} cbSdkCCF;

typedef enum _cbSdkSystemType
{
    cbSdkSystem_RESET = 0,
    cbSdkSystem_SHUTDOWN,
    cbSdkSystem_STANDBY,
} cbSdkSystemType;

/// Extension command type
typedef enum _cbSdkExtCmdType
{
    cbSdkExtCmd_RPC = 0,    // RPC command
    cbSdkExtCmd_UPLOAD,     // Upload the file
    cbSdkExtCmd_TERMINATE,  // Signal last RPC command to terminate
    cbSdkExtCmd_INPUT,		// Input to RPC command
    cbSdkExtCmd_END_PLUGIN, // Signal to end plugin
    cbSdkExtCmd_NSP_REBOOT, // Restart the NSP
    cbSdkExtCmd_PLUGINFO,   // Get plugin info
} cbSdkExtCmdType;

/// Extension command
typedef struct _cbSdkExtCmd
{
    cbSdkExtCmdType cmd;
    char szCmd[cbMAX_LOG];
} cbSdkExtCmd;

// Can call this even with closed library to get library information
/*! Get the library version (and nsp version if library is open) */
CBSDKAPI    cbSdkResult cbSdkGetVersion(UINT32 nInstance, cbSdkVersion * version);

/*! Read configuration file. */
CBSDKAPI    cbSdkResult cbSdkReadCCF(UINT32 nInstance, cbSdkCCF * pData, const char * szFileName, bool bConvert, bool bSend = false, bool bThreaded = false); // Read batch config from CCF (or nsp if filename is null)
/*! Write configuration file. */
CBSDKAPI    cbSdkResult cbSdkWriteCCF(UINT32 nInstance, cbSdkCCF * pData, const char * szFileName, bool bThreaded = false); // Write batch config to CCF (or nsp if filename is null)

/*! Open the library */
CBSDKAPI    cbSdkResult cbSdkOpen(UINT32 nInstance,
                                  cbSdkConnectionType conType = CBSDKCONNECTION_DEFAULT,
                                  cbSdkConnection con = cbSdkConnection());

CBSDKAPI    cbSdkResult cbSdkGetType(UINT32 nInstance, cbSdkConnectionType * conType, cbSdkInstrumentType * instType); // Get connection and instrument type

/*! Close the library */
CBSDKAPI    cbSdkResult cbSdkClose(UINT32 nInstance);

/*! Get the instrument sample clock time */
CBSDKAPI    cbSdkResult cbSdkGetTime(UINT32 nInstance, PROCTIME * cbtime);

/*! Get direct access to internal spike cache shared memory */
CBSDKAPI    cbSdkResult cbSdkGetSpkCache(UINT32 nInstance, UINT16 channel, cbSPKCACHE **cache);
// Note that spike cache is volatile, thus should not be used for critical operations such as recording

/*! Get trial setup configuration */
CBSDKAPI    cbSdkResult cbSdkGetTrialConfig(UINT32 nInstance,
                                         UINT32 * pbActive, UINT16 * pBegchan = NULL, UINT32 * pBegmask = NULL, UINT32 * pBegval = NULL,
                                         UINT16 * pEndchan = NULL, UINT32 * pEndmask = NULL, UINT32 * pEndval = NULL, bool * pbDouble = NULL,
                                         UINT32 * puWaveforms = NULL, UINT32 * puConts = NULL, UINT32 * puEvents = NULL,
                                         UINT32 * puComments = NULL, UINT32 * puTrackings = NULL, bool * pbAbsolute = NULL);
/*! Setup a trial */
CBSDKAPI    cbSdkResult cbSdkSetTrialConfig(UINT32 nInstance,
                                         UINT32 bActive, UINT16 begchan = 0, UINT32 begmask = 0, UINT32 begval = 0,
                                         UINT16 endchan = 0, UINT32 endmask = 0, UINT32 endval = 0, bool bDouble = false,
                                         UINT32 uWaveforms = 0, UINT32 uConts = cbSdk_CONTINUOUS_DATA_SAMPLES, UINT32 uEvents = cbSdk_EVENT_DATA_SAMPLES,
                                         UINT32 uComments = 0, UINT32 uTrackings = 0, bool bAbsolute = false); // Configure a data collection trial
// begchan - first channel number (1-based), zero means all
// endchan - last channel number (1-based), zero means all

/*! Close given trial if configured */
CBSDKAPI    cbSdkResult cbSdkUnsetTrialConfig(UINT32 nInstance, cbSdkTrialType type);

/*! Get channel label */
// Pass NULL or allocate bValid[6] label[cbLEN_STR_LABEL] position[4]
CBSDKAPI    cbSdkResult cbSdkGetChannelLabel(UINT32 nInstance, UINT16 channel, UINT32 * bValid, char * label = NULL, UINT32 * userflags = NULL, INT32 * position = NULL); // Get channel label
/*! Set channel label */
CBSDKAPI    cbSdkResult cbSdkSetChannelLabel(UINT32 nInstance, UINT16 channel, const char * label, UINT32 userflags, INT32 * position); // Set channel label

/*! Retrieve data of a trial (NULL means ignore), user should allocate enough buffers beforehand, and trial should not be closed during this call */
CBSDKAPI    cbSdkResult cbSdkGetTrialData(UINT32 nInstance,
                                          UINT32 bActive, cbSdkTrialEvent * trialevent, cbSdkTrialCont * trialcont,
                                          cbSdkTrialComment * trialcomment, cbSdkTrialTracking * trialtracking);

/*! Initialize the structures (and fill with information about active channels, comment pointers and samples in the buffer) */
CBSDKAPI    cbSdkResult cbSdkInitTrialData(UINT32 nInstance, UINT32 bActive,
                                           cbSdkTrialEvent * trialevent, cbSdkTrialCont * trialcont,
                                           cbSdkTrialComment * trialcomment, cbSdkTrialTracking * trialtracking);

/*! Start/stop/open/close file recording */
CBSDKAPI    cbSdkResult cbSdkSetFileConfig(UINT32 nInstance, const char * filename, const char * comment, UINT32 bStart, UINT32 options = cbFILECFG_OPT_NONE);

/*! Get the state of file recording */
CBSDKAPI    cbSdkResult cbSdkGetFileConfig(UINT32 nInstance, char * filename, char * username, bool * pbRecording);

CBSDKAPI    cbSdkResult cbSdkSetPatientInfo(UINT32 nInstance, const char * ID, const char * firstname, const char * lastname, UINT32 DOBMonth, UINT32 DOBDay, UINT32 DOBYear);

CBSDKAPI    cbSdkResult cbSdkInitiateImpedance(UINT32 nInstance);

CBSDKAPI    cbSdkResult cbSdkSendPoll(UINT32 nInstance, const char* appname, UINT32 mode, UINT32 flags, UINT32 extra);

/*! This sends an arbitrary packet without any validation. Please use with care or it might break the system */
CBSDKAPI    cbSdkResult cbSdkSendPacket(UINT32 nInstance, void * ppckt);

/*! Get/Set the runlevel of the instrument */
CBSDKAPI    cbSdkResult cbSdkSetSystemRunLevel(UINT32 nInstance, UINT32 runlevel, UINT32 locked, UINT32 resetque);

CBSDKAPI    cbSdkResult cbSdkGetSystemRunLevel(UINT32 nInstance, UINT32 * runlevel, UINT32 * runflags = NULL, UINT32 * resetque = NULL);

/*! Send a digital output command */
CBSDKAPI    cbSdkResult cbSdkSetDigitalOutput(UINT32 nInstance, UINT16 channel, UINT16 value);

/*! Send a synch output waveform */
CBSDKAPI    cbSdkResult cbSdkSetSynchOutput(UINT32 nInstance, UINT16 channel, UINT32 nFreq, UINT32 nRepeats);

/*! Send an extension command */
CBSDKAPI    cbSdkResult cbSdkExtDoCommand(UINT32 nInstance, cbSdkExtCmd * extCmd);

/*! Send a analog output waveform or monitor a given channel, disable channel if both are null */
CBSDKAPI    cbSdkResult cbSdkSetAnalogOutput(UINT32 nInstance, UINT16 channel, cbSdkWaveformData * wf, cbSdkAoutMon * mon);

/*! * Mask channels (for both trial and callback)
* @param[in] channel channel number (1-based), zero means all channels
*/
CBSDKAPI    cbSdkResult cbSdkSetChannelMask(UINT32 nInstance, UINT16 channel, UINT32 bActive);

/*! Send a comment or custom event */
CBSDKAPI    cbSdkResult cbSdkSetComment(UINT32 nInstance, UINT32 rgba, UINT8 charset, const char * comment = NULL);

/*! Send a full channel configuration packet */
CBSDKAPI    cbSdkResult cbSdkSetChannelConfig(UINT32 nInstance, UINT16 channel, cbPKT_CHANINFO * chaninfo);
/*! Get a full channel configuration packet */
CBSDKAPI    cbSdkResult cbSdkGetChannelConfig(UINT32 nInstance, UINT16 channel, cbPKT_CHANINFO * chaninfo);

/*! Get filter description (proc = 1 for now) */
CBSDKAPI    cbSdkResult cbSdkGetFilterDesc(UINT32 nInstance, UINT32 proc, UINT32 filt, cbFILTDESC * filtdesc);

/*! Get sample group list (proc = 1 for now) */
CBSDKAPI    cbSdkResult cbSdkGetSampleGroupList(UINT32 nInstance, UINT32 proc, UINT32 group, UINT32 *length, UINT16 *list);

CBSDKAPI    cbSdkResult cbSdkGetSampleGroupInfo(UINT32 nInstance, UINT32 proc, UINT32 group, char *label, UINT32 *period, UINT32 *length);

/*!
* Get information about given trackable object
* @param[in] id		trackable ID (1 to cbMAXTRACKOBJ)
* @param[in] name	string of length cbLEN_STR_LABEL
*/
CBSDKAPI    cbSdkResult cbSdkGetTrackObj(UINT32 nInstance, char *name, UINT16 *type, UINT16 *pointCount, UINT32 id);

/*! Get video source information */
// id     video source ID (1 to cbMAXVIDEOSOURCE)
// name   string of length cbLEN_STR_LABEL
CBSDKAPI    cbSdkResult cbSdkGetVideoSource(UINT32 nInstance, char *name, float *fps, UINT32 id);

/// Send global spike configuration
CBSDKAPI    cbSdkResult cbSdkSetSpikeConfig(UINT32 nInstance, UINT32 spklength, UINT32 spkpretrig);
/// Get global system configuration
CBSDKAPI    cbSdkResult cbSdkGetSysConfig(UINT32 nInstance, UINT32 * spklength, UINT32 * spkpretrig = NULL, UINT32 * sysfreq = NULL);
/// Perform given system command
CBSDKAPI    cbSdkResult cbSdkSystem(UINT32 nInstance, cbSdkSystemType cmd);

CBSDKAPI    cbSdkResult cbSdkRegisterCallback(UINT32 nInstance, cbSdkCallbackType callbacktype, cbSdkCallback pCallbackFn, void* pCallbackData);
CBSDKAPI    cbSdkResult cbSdkUnRegisterCallback(UINT32 nInstance, cbSdkCallbackType callbacktype);
// At most one callback per each callback type per each connection

/// Convert volts string (e.g. '5V', '-65mV', ...) to its raw digital value equivalent for given channel
CBSDKAPI    cbSdkResult cbSdkAnalogToDigital(UINT32 nInstance, UINT16 channel, const char * szVoltsUnitString, INT32 * digital);

#endif /* CBSDK_H_INCLUDED */
