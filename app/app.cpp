#include "app.h"

#include "constants.h"

Application::Application()
{
  _appStates[AppState::PLAYLIST] = std::make_unique<PlaylistState>(_appSettings);

  for (auto& kvp : _appStates)
  {
    kvp.second->Init();
  }
}

// =============================================================================

Application::~Application()
{
  _appStates.clear();

  xmp_free_context(_xmpContext);
  SDL_Quit();
}

// =============================================================================

bool Application::InitSDL()
{
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
  {
    fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
    return false;
  }

  return true;
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

// =============================================================================


bool Application::InitSound()
{
  if ((_xmpContext = xmp_create_context()) == nullptr)
  {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "xmp_create_context()");
    return false;
  }

  SDL_AudioSpec a;

  a.freq     = 44100;
  a.format   = (AUDIO_S16);
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

  // TODO: check out other params, subsequently maybe move them to settings.
  //xmp_set_player(_xmpContext, XMP_PLAYER_DEFPAN, 50);

  return true;
}

// =============================================================================

bool Application::InitDrawing()
{
  _window = SDL_CreateWindow("xmp-sdl music player",
                             100, 100,
                             Constants::WindowWidth,
                             Constants::WindowHeight,
                             SDL_WINDOW_SHOWN);
  if (_window == nullptr)
  {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "Failed to create SDL window: '%s'",
                 SDL_GetError());
    return false;
  }

  int drivers = SDL_GetNumRenderDrivers();

  for (int i = 0; i < drivers; i++)
  {
    SDL_RendererInfo info;
    SDL_GetRenderDriverInfo(i, &info);

    //
    // This is important since printer uses render to texture
    //
    if (info.flags & SDL_RENDERER_TARGETTEXTURE)
    {
      _renderer = SDL_CreateRenderer(_window, i, info.flags);
      if (_renderer != nullptr)
      {
        break;
      }
    }
  }

  if (_renderer == nullptr)
  {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "Couldn't create renderer: "
                 "maybe render to texture isn't supported.\n");
    return false;
  }

  SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);

  SDL_DisplayMode sdm;
  if (SDL_GetCurrentDisplayMode(0, &sdm) != 0)
  {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", SDL_GetError());
    return false;
  }

  SDL_Log("Window size: %dx%d", sdm.w, sdm.h);
  SDL_Log("Window size (chars): %lux%lu",
          Constants::WindowCharWidth,
          Constants::WindowCharHeight);

  if (not _printer.Init(_renderer,
                        Constants::WindowWidth,
                        Constants::WindowHeight,
                        Constants::GlobalScale))
  {
    return false;
  }

  return true;
}

// =============================================================================

void Application::Run()
{
  uint64_t drawTime = 0;

  while (true)
  {
    while (SDL_PollEvent(&_event))
    {
      switch (_event.type)
      {
        case SDL_KEYDOWN:
        {
          switch (_event.key.keysym.sym)
          {
            // TODO: remove afterwards
            case SDLK_ESCAPE:
            {
              return;
            }

            // -----------------------------------------------------------------

            case SDLK_t:
            {
              if (_appSettings.AlwaysOnTop == SDL_TRUE)
              {
                _appSettings.AlwaysOnTop == SDL_FALSE;
              }
              else if (_appSettings.AlwaysOnTop == SDL_FALSE)
              {
                _appSettings.AlwaysOnTop = SDL_TRUE;
              }

              SDL_SetWindowAlwaysOnTop(_window, _appSettings.AlwaysOnTop);
            }
            break;
          }
        }
        break;

        case SDL_QUIT:
          return;
          break;

        default:
          break;
      }

      _appStates[_currentState]->HandleEvent(_event);
    }

    if (_stopPlaying)
    {
      _stopPlaying = false;

      SDL_LockAudioDevice(_audioDevice);
      SDL_PauseAudioDevice(_audioDevice, 1);
      SDL_UnlockAudioDevice(_audioDevice);

      xmp_end_player(_xmpContext);
      xmp_release_module(_xmpContext);
    }

    drawTime = SDL_GetTicks64();

    _appStates[_currentState]->DrawStuff(&_printer);

    _appStates[_currentState]->SetDrawTime(SDL_GetTicks64() - drawTime);
  }
}
