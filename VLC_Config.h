#pragma once

// VLC SDK Configuration for DLL DAW VideoSync
// Enable this define when VLC SDK is available

// Uncomment this line when VLC SDK is installed and linked
// #define USE_VLC_SDK

// VLC SDK Installation Instructions:
//
// 1. Download VLC SDK from: https://www.videolan.org/vlc/download-windows.html
//    - Download the "Developer" package (not the regular installer)
//    - Extract to a folder like C:\VLC_SDK\
//
// 2. Add VLC include directory to project:
//    - Add C:\VLC_SDK\include to Additional Include Directories
//
// 3. Add VLC library directory to project:
//    - Add C:\VLC_SDK\lib to Additional Library Directories
//    - Add libvlc.lib to Additional Dependencies
//
// 4. Copy VLC DLLs to output directory:
//    - Copy libvlc.dll and libvlccore.dll from C:\VLC_SDK\bin\
//    - Place them in the same directory as the compiled plugin
//
// 5. Enable VLC SDK by uncommenting #define USE_VLC_SDK above
//
// Alternative: Use VLC installation directory if VLC is installed
// - VLC Install Directory: C:\Program Files\VideoLAN\VLC\
// - Include: C:\Program Files\VideoLAN\VLC\sdk\include\
// - Lib: C:\Program Files\VideoLAN\VLC\sdk\lib\

// Status messages for different configurations
#ifdef USE_VLC_SDK
    #define VIDEO_BACKEND_STATUS "VLC SDK Active - Real video decoding enabled"
#else
    #define VIDEO_BACKEND_STATUS "✅ WORKING! Sep19-v3.0 - ENHANCED DEBUG WITH PORTRAIT VIDEO FIXES"
#endif