#ifndef PA_WIN_VERSION_H
#define PA_WIN_VERSION_H

/*
 * $Id$
 * Portable Audio I/O Library
 * Win32 platform-specific support functions
 *
 * Based on the Open Source API proposed by Ross Bencina
 * Copyright (c) 1999-2008 Ross Bencina
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * The text above constitutes the entire PortAudio license; however,
 * the PortAudio community also makes the following non-binding requests:
 *
 * Any person wishing to distribute modifications to the Software is
 * requested to send the modifications to the original developer so that
 * they can be incorporated into the canonical version. It is also
 * requested that these non-binding requests be included along with the
 * license above.
 */

#include "portaudio.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 Windows OS version.
*/
typedef enum PaOsVersion
{
    paOsVersionWindowsUnknown = 0,
    paOsVersionWindows9x, // Windows 95, Windows 98, Windows ME
    paOsVersionWindowsNT4,
    paOsVersionWindows2000,
    paOsVersionWindowsXP,
    paOsVersionWindowsXPServer2003,
    paOsVersionWindowsVistaServer2008,
    paOsVersionWindows7Server2008R2,
    paOsVersionWindows8Server2012,
    paOsVersionWindows8_1Server2012R2,
    paOsVersionWindows10Server2016,
    // insert subsequent Windows versions below:
    // ...
    // paOsVersionWindowsFuture must be the last in the list
    paOsVersionWindowsFuture = 1000
} PaOsVersion;

/**
 Get Windows OS version.
 @return OS version via PaOsVersion enum.
 @see    PaOsVersion
*/
PaOsVersion PaWinUtil_GetOsVersion();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PA_WIN_VERSION_H */
