//////////////////////////////////////////////////////////////////////////////
//
// (c) Copyright 2003 - 2008 Cyberkinetics, Inc.
// (c) Copyright 2008 - 2023 Cyberkinetics, Inc.
//
// $Workfile: Launching.h $
// $Archive: /Cerebus/WindowsApps/cbhwlib/Launching.h $
// $Revision: 3 $
// $Date: 4/08/04 3:23p $
// $Author: Kkorver $
//
// $NoKeywords: $
//
//////////////////////////////////////////////////////////////////////////////

#ifndef LAUNCHING_H_INCLUDED
#define LAUNCHING_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum enLaunchView
{
    LV_HARDWARE_CONFIG,
    LV_SPIKE_PANEL,
    LV_RASTER,
    LV_SINGLE,
    LV_ACTIVITY,
    LV_FILE,
    LV_THRESHOLDING,
    LV_DIGITAL_INPUT_WATCH,
    LV_MODULATION,              // The Neural Modulation application
    LV_SIG_TO_NOISE,            // Signal-to-noise application
    LV_AUTOIMPEDANCE,           // Auto Impedance application
    LV_CROSSTALK,               // Crosstalk application
    LV_NTRODE,                  // N-Trode application
    LV_NPLAY,                   // nPlay interface application
    LV_STIMCOMM,                // StimComm application for the Stim Switch
    LV_OSCILLOSCOPE,            // Oscilloscope application
    LV_DIGITAL_FILTER_EDITOR,   // Digital Filter Editor
    LV_CERVELLO,                // launch Cervello
    LV_SERIAL_INPUT_WATCH,      // Serial Input Watch application
    LV_DIGITAL_OUTPUT,          // Digout application
    LV_MAX                      // used for range checking
};

enum enLaunchOpts
{
    LO_NONE,
    LO_BACKGROUND,
    LO_SPECIAL
};

// Purpose: start one of our other applications, if possible, show
//  this channel as the active one
// Inputs:
//  enApp - the enumerated value of the program to start
//  nChannel - the 1 based channel index (-1 means no channel)
//  nLaunchOpts - LO_* launch options
//  nCerebusId - library ID to open this application against
//  szExtraParams - extra parameters to pass to the app
// Outputs:
//  same as WinExec in the windows API
HANDLE LaunchView(enLaunchView enApp, int nChannel = -1, enLaunchOpts nLaunchOpts = LO_NONE, int nCerebusId = 0, LPCSTR szExtraParams = NULL);

bool AppExists(enLaunchView enApp);
bool AppExists(const char * strAppName);

#endif // include guard
