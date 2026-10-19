#include "sound-player.h"

#include "utils.h"

#define STR(what) #what

SoundPlayer::~SoundPlayer()
{
  SDL_CloseAudioDevice(_audioDevice);
  xmp_free_context(_xmpContext);
}

// =============================================================================

static void AudioCallback(void* udata, unsigned char* stream, int len)
{
  //
  // NOTE: last argument is the loop count. 0 - means play forever.
  // If not 0, module reaches XMP_END state after specified number of repeats.
  //
  int ec = xmp_play_buffer((xmp_context)udata, stream, len, 1);
  switch (ec)
  {
    case 0:
      break;

    case -XMP_END:
    {
      //
      // Write silence to prevent repeated playback of the tail.
      //
      SDL_memset(stream, 0, len);
    }
    break;

    case -XMP_ERROR_STATE:
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Not in playing state!");
      break;

    default:
      SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Unhandled error code %d", ec);
      break;
  }
}

void SoundPlayer::SetParam(const std::string &paramStr, int param, int value)
{
  int ec = xmp_set_player(_xmpContext, param, value);
  switch (ec)
  {
    case 0:
    {
      SDL_Log("'%s' param value is set to %d",
              paramStr.data(),
              value);
    }
    break;

    case -XMP_ERROR_INVALID:
    {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                   "Invalid value %d for param '%s'",
                   value, paramStr.data());
    }
    break;

    case -XMP_ERROR_STATE:
    {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                   "'%s' - player is not in playing state",
                   paramStr.data());
    }
    break;

    default:
      break;
  }
}

// =============================================================================

bool SoundPlayer::Init()
{
  if ((_xmpContext = xmp_create_context()) == nullptr)
  {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "xmp_create_context()");
    return false;
  }

  SDL_AudioSpec a;

  a.freq     = 44100;
  a.format   = (AUDIO_S8);
  a.channels = 2;
  a.samples  = 2048;
  a.callback = AudioCallback;
  a.userdata = _xmpContext;

  _audioDevice = SDL_OpenAudioDevice(nullptr, 0, &a, nullptr, 0);
  if (_audioDevice == 0)
  {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", SDL_GetError());
    return false;
  }

  return true;
}

// =============================================================================

bool SoundPlayer::TestModule(const std::string& fname)
{
  bool res = false;

  xmp_test_info ti;
  int ec = xmp_test_module(fname.data(), &ti);
  switch (ec)
  {
    case 0:
      res = true;
      break;

    case -XMP_ERROR_FORMAT:
    {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                   "File '%s' is not recognized as a module",
                   fname.data());
    }
    break;

    case -XMP_ERROR_DEPACK:
    {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                   "Failed to decompress '%s'",
                   fname.data());
    }
    break;

    case -XMP_ERROR_SYSTEM:
    {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                   "System error: '%s'",
                   strerror(xmp_syserrno()));
    }
    break;

    default:
      break;
  }

  return res;
}

// =============================================================================

bool SoundPlayer::LoadModule(const std::string& fname)
{
  bool res = false;

  int ec = xmp_load_module(_xmpContext, fname.data());
  switch (ec)
  {
    case 0:
    {
      res = true;
    }
    break;

    case -XMP_ERROR_FORMAT:
    {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                   "File '%s' is not recognized as a module",
                   fname.data());
    }
    break;

    case -XMP_ERROR_DEPACK:
    {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                   "Failed to decompress '%s'",
                   fname.data());
    }
    break;

    case -XMP_ERROR_LOAD:
    {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                   "File '%s' is recognized but failed to load",
                   fname.data());
    }
    break;

    case -XMP_ERROR_SYSTEM:
    {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                   "System error: '%s'",
                   strerror(xmp_syserrno()));
    }
    break;

    default:
      break;
  }

  return res;
}

// =============================================================================

xmp_module_info* SoundPlayer::GetCurrentModuleInfo()
{
  return _isPlaying ? &_moduleInfo : nullptr;
}

// =============================================================================

bool SoundPlayer::Play(const std::string& uri)
{
  bool res = false;

  if (not TestModule(uri))
  {
    return res;
  }

  if (not LoadModule(uri))
  {
    return res;
  }

  xmp_get_module_info(_xmpContext, &_moduleInfo);

  int ec = xmp_start_player(_xmpContext, 44100, XMP_FORMAT_8BIT);
  switch (ec)
  {
    case 0:
    {
      int v = xmp_get_player(_xmpContext, XMP_PLAYER_DEFPAN);
      SDL_Log("defpan %d", v);

      v = xmp_get_player(_xmpContext, XMP_PLAYER_INTERP);
      SDL_Log("interp %d", v);

      // NOTE: no effect?
      SetParam(STR(XMP_PLAYER_DEFPAN), XMP_PLAYER_DEFPAN, 50);
      SetParam(STR(XMP_PLAYER_INTERP), XMP_PLAYER_INTERP, XMP_INTERP_SPLINE);

      SDL_PauseAudioDevice(_audioDevice, 0);
      res = true;
      _isPlaying = true;
    }
    break;

    case -XMP_ERROR_INTERNAL:
    {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                   "Failed to start player: internal error");
    }
    break;

    case -XMP_ERROR_SYSTEM:
    {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                   "Failed to start player: system error ('%s')",
                   strerror(xmp_syserrno()));
    }
    break;

    default:
      break;
  }

  return res;
}

// =============================================================================

void SoundPlayer::Stop()
{
  if (_isPlaying)
  {
    _isPlaying = false;

    SDL_LockAudioDevice(_audioDevice);
    SDL_PauseAudioDevice(_audioDevice, 1);
    SDL_UnlockAudioDevice(_audioDevice);

    xmp_end_player(_xmpContext);
    xmp_release_module(_xmpContext);
  }
}
