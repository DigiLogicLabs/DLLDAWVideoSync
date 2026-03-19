#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "IControls.h"
#include "VLC_Config.h"
#include <vector>
#include <string>

// VLC SDK integration
#ifdef WIN32
#include <windows.h>
#include <commdlg.h>  // For GetOpenFileName
#include <shellapi.h> // For ShellExecute
#include <shlobj.h>   // For SHGetFolderPath (Documents folder)
// Windows Media Foundation for video decoding
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>
#include <combaseapi.h>
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "comdlg32.lib")  // For GetOpenFileName
#pragma comment(lib, "shell32.lib")   // For ShellExecute
#endif

// VLC Media Player headers (when available)
#ifdef USE_VLC_SDK
#include <vlc/vlc.h>
#include <vlc/libvlc_media.h>
#include <vlc/libvlc_media_player.h>
#endif

const int kNumPresets = 1;

// ============================================================================
// TIMECODE FORMAT ENUM - Industry standard display options
// ============================================================================
enum ETimecodeFormat {
  kTimecodeMMSS = 0,      // MM:SS.ms (default simple)
  kTimecodeSMPTE,         // HH:MM:SS:FF (SMPTE non-drop)
  kTimecodeSMPTEDF,       // HH:MM:SS;FF (SMPTE drop-frame for 29.97)
  kTimecodeFrames,        // Total frames only
  kTimecodeBeats,         // Bars:Beats:Ticks
  kNumTimecodeFormats
};

// ============================================================================
// PERFORMANCE MODE ENUM - Multiple quality/performance levels
// ============================================================================
enum EPerformanceMode {
  kPerfUltraHD = 0,       // 1x1 pixels, bilinear (highest quality)
  kPerfHD,                // 1x1 pixels, nearest neighbor
  kPerfQuality,           // 2x2 blocks
  kPerfBalanced,          // 3x3 blocks
  kPerfPerformance,       // 4x4 blocks
  kPerfPreview,           // 8x8 blocks (lowest quality, fastest)
  kNumPerfModes
};

// Performance mode names for UI
static const char* kPerfModeNames[] = {
  "Ultra HD", "HD", "Quality", "Balanced", "Performance", "Preview"
};

// Performance mode block sizes
static const int kPerfModeBlockSizes[] = { 1, 1, 2, 3, 4, 8 };

enum EParams
{
  kVideoPosition = 0, // Keep for internal sync, but don't show as user control
  kNumParams
};

using namespace iplug;
using namespace igraphics;

class DLLDAWVideoSync final : public Plugin
{
public:
  DLLDAWVideoSync(const InstanceInfo& info);
  ~DLLDAWVideoSync();

#if IPLUG_DSP
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnReset() override;
  void OnParamChange(int paramIdx) override;
#endif

  void OnUIOpen() override;
  void OnUIClose() override;

  // State serialization - persist video path with DAW session
  bool SerializeState(IByteChunk& chunk) const override;
  int UnserializeState(const IByteChunk& chunk, int startPos) override;

private:
  double mLastTransportTime = 0.0;
  bool mVideoLoaded = false;

  // Transport state change detection for cleaner video sync
  bool mWasTransportRunning = false;
  double mLastTransportPosition = 0.0;
  bool mTransportJustStarted = false;
  bool mTransportJustStopped = false;
  bool mTransportDidSeek = false;
  WDL_String mVideoFilePath;
  double mVideoDuration = 7200.0; // Support up to 2 hours by default, will auto-detect actual length

  // UI control references for real-time updates
  ITextControl* mTransportStatusControl = nullptr;
  IControl* mProgressBarControl = nullptr;
  IControl* mVideoPreviewControl = nullptr; // Changed to IControl* for custom display
  ITextControl* mUploadStatusControl = nullptr;
  ITextControl* mVideoStatusControl = nullptr;
  ITextControl* mErrorDisplayControl = nullptr;
  ITextControl* mTimecodeDisplayControl = nullptr;    // SMPTE timecode display
  ITextControl* mBPMGridInfoControl = nullptr;        // BPM-to-frame grid info
  ITextControl* mSyncStatusControl = nullptr;         // Sync drift/status
  ITextControl* mMetadataDisplayControl = nullptr;    // Video metadata

  // Error handling
  void ShowError(const char* errorMessage);
  void ClearError();

  // Video frame display and decoding
  bool mShowVideoFrames = false;
  WDL_String mLastVideoFrame;

  // ============================================================================
  // INDUSTRY-STANDARD FEATURES
  // ============================================================================

  // Video Start Offset - allows video to start at a specific DAW time
  double mVideoStartOffset = 0.0;       // Offset in seconds (video starts at this DAW time)
  double mVideoStartOffsetBeats = 0.0;  // Offset in beats (alternative)
  bool mUseBeatsOffset = false;         // Use beats instead of seconds

  // Video In/Out Loop Points
  double mVideoInPoint = 0.0;           // Loop start point (video time)
  double mVideoOutPoint = -1.0;         // Loop end point (-1 = use duration)
  bool mLoopEnabled = false;            // Enable looping between in/out

  // Timecode Display Format
  ETimecodeFormat mTimecodeFormat = kTimecodeSMPTE;

  // Performance Mode (replaces simple HD toggle)
  EPerformanceMode mPerformanceMode = kPerfHD;

  // UI state preservation (survives resize) - LEGACY compatibility
  bool mHDMode = true;        // HD quality mode (now derived from mPerformanceMode)
  bool mShowStats = false;    // Stats overlay visibility

  // Frame Rate & Sync Analysis
  double mProjectFrameRate = 30.0;      // DAW project frame rate (if detectable)
  bool mFrameRateMismatch = false;      // True if video fps doesn't match project
  double mSyncDrift = 0.0;              // Accumulated sync drift in ms
  double mMaxSyncDrift = 0.0;           // Maximum drift observed

  // Frame Drop Detection
  uint64_t mTotalFramesRequested = 0;
  uint64_t mFramesDecoded = 0;
  uint64_t mFramesDropped = 0;
  double mLastFrameDecodeTime = 0.0;
  double mAverageDecodeTime = 0.0;

  // Video Metadata (extracted on load)
  WDL_String mVideoCodec;
  WDL_String mVideoColorSpace;
  int mVideoBitrate = 0;                // In kbps
  bool mVideoHasAudio = false;
  int mVideoAudioChannels = 0;
  int mVideoAudioSampleRate = 0;

  // ============================================================================
  // TIMECODE & GRID HELPERS
  // ============================================================================

  // Format time as SMPTE timecode string
  void FormatSMPTETimecode(double timeInSeconds, double fps, bool dropFrame, char* buffer, int bufferSize) const;

  // Format time in current timecode format
  void FormatTimecode(double timeInSeconds, char* buffer, int bufferSize) const;

  // Calculate frames per beat at current BPM
  double GetFramesPerBeat() const;

  // Check if video fps creates perfect beat alignment
  bool IsBeatAligned() const;

  // Get frame number at specific beat
  int GetFrameAtBeat(double beatNumber) const;

  // Check for frame rate mismatch and calculate drift
  void AnalyzeFrameRateSync();

  // Get drift per hour in frames
  double GetDriftPerHour() const;

  // Video file handling
  bool mVideoFileValid = false;
  double mVideoFrameRate = 30.0;
  int mVideoWidth = 640;
  int mVideoHeight = 480;

  // Frame extraction methods
  bool ExtractVideoFrame(double timeInSeconds, unsigned char* frameBuffer, int bufferSize);
  bool LoadVideoFile(const char* filepath);

#ifdef USE_VLC_SDK
  // VLC SDK members
  libvlc_instance_t* mVLCInstance;
  libvlc_media_t* mVLCMedia;
  libvlc_media_player_t* mVLCPlayer;
  unsigned char* mVideoFrameBuffer;
  bool InitializeVLC();
  void CleanupVLC();
  bool LoadVideoWithVLC(const char* filepath);
  bool ExtractVLCFrame(double timeInSeconds, unsigned char* frameBuffer, int bufferSize);
#endif

  // Windows Media Foundation fallback
  bool LoadVideoWithWMF(const char* filepath);
  bool ExtractWMFFrame(double timeInSeconds, unsigned char* frameBuffer, int bufferSize);

#ifdef WIN32
  // WMF members
  IMFSourceReader* mWMFSourceReader;
  LONGLONG mWMFVideoDuration;
  bool mWMFInitialized;
  bool InitializeWMF();
  void CleanupWMF();

  // Sequential reading optimization - avoid seeking when playing forward
  double mLastWMFDecodedTime = -1.0;
  int mLastWMFDecodedFrame = -1;
  std::vector<unsigned char> mWMFFrameCache;  // Cached frame data
  bool mWMFHasCachedFrame = false;

  // PERFORMANCE: Memory limits for long videos
  static constexpr size_t kMaxFrameCacheSize = 16 * 1024 * 1024;  // 16MB max cache
  static constexpr size_t kMaxFloatingBufferSize = 8 * 1024 * 1024;  // 8MB floating window buffer

  // Floating video window - stays visible when plugin UI closes
  static HWND sFloatingWindow;
  static DLLDAWVideoSync* sFloatingWindowOwner;
  static LRESULT CALLBACK FloatingWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
  bool CreateFloatingVideoWindow();
  void DestroyFloatingVideoWindow();
  void UpdateFloatingWindow();
  static void CALLBACK FloatingWindowTimer(HWND hwnd, UINT msg, UINT_PTR idEvent, DWORD dwTime);
  UINT_PTR mFloatingTimerID = 0;
  std::vector<unsigned char> mFloatingFrameBuffer;
  HBITMAP mFloatingBitmap = NULL;
  HDC mFloatingMemDC = NULL;
  bool mFloatingWindowActive = false;

  // Fullscreen support for floating window
  bool mFloatingFullscreen = false;
  RECT mFloatingWindowedRect = { 0 };   // Store windowed position for restore
  LONG mFloatingWindowedStyle = 0;       // Store windowed style for restore
  void ToggleFloatingFullscreen();

  // Reference track creation
  bool CreateReferenceAudioTrack();
  bool ExtractAudioFromVideo();         // Extract actual audio (not just silent)
  WDL_String mLastReferenceTrackPath;

  // Audio extraction via WMF
  bool mVideoHasExtractableAudio = false;
#endif
};
