// *****************************************************************************
//
// Build from host system's terminal if Qt Creator from flatpak misbehaves.
//
// *****************************************************************************

#include <filesystem>
#include <vector>
#include <string>

#include "printer.h"
#include "structs.h"
#include "utils.h"

#include <xmp.h>

namespace fs = std::filesystem;

TG::Printer Printer;

const size_t kWindowWidth  = 800;
const size_t kWindowHeight = 600;

std::string DroppedFile = "<drag and drop a file>";

SDL_bool TopFlag = SDL_FALSE;

xmp_context XmpContext;

std::vector<PlaylistItem> Playlist;

SDL_AudioDeviceID AudioDevice;

bool StopPlaying = false;

// =============================================================================

void TryToPlay(const PlaylistItem& item)
{
  xmp_load_module(XmpContext, item.URI.data());
  xmp_start_player(XmpContext, 44100, 0);

  StopPlaying = false;

  SDL_PauseAudioDevice(AudioDevice, 0);
}

// =============================================================================

void RefreshList()
{
  if (fs::is_directory(DroppedFile))
  {
    SDL_Log("Dir");

    for (auto& entry : fs::recursive_directory_iterator(DroppedFile))
    {
      std::string uri = entry.path().string();
      if (fs::is_regular_file(uri))
      {
        PlaylistItem item;
        if (Utils::GetModuleInfo(uri, item))
        {
          Playlist.push_back(item);
        }
      }
    }
  }
  else if (fs::is_regular_file(DroppedFile))
  {
    PlaylistItem item;
    if (Utils::GetModuleInfo(DroppedFile, item))
    {
      Playlist.push_back(item);
    }
  }
}

// =============================================================================

void Display()
{
  Printer.Clear();

  for (size_t i = 0; i < 60; i++)
  {
    Printer.PrintFB(i,
                    0,
                    std::to_string((i % 10)),
                    TG::Printer::kAlignLeft,
                    TG::Colors::White);
  }

  Printer.PrintFB(0,
                  2,
                  DroppedFile,
                  TG::Printer::kAlignLeft,
                  TG::Colors::White);

  if (TopFlag == SDL_TRUE)
  {
    Printer.PrintFB(45,
                    1,
                    "T",
                    TG::Printer::kAlignLeft,
                    TG::Colors::Yellow,
                    TG::Colors::Red);
  }

  Printer.Render();
}

// =============================================================================

void AudioCallback(void* udata, unsigned char* stream, int len)
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
      StopPlaying = true;
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

bool InitSound()
{
  SDL_AudioSpec a;

  a.freq     = 44100;
  a.format   = (AUDIO_S16);
  a.channels = 2;
  a.samples  = 2048;
  a.callback = AudioCallback;
  a.userdata = XmpContext;

  AudioDevice = SDL_OpenAudioDevice(nullptr, 0, &a, nullptr, 0);
  if (AudioDevice == 0)
  {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", SDL_GetError());
    return false;
  }

  return true;
}

// =============================================================================

int main()
{
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
  {
    printf("SDL_Init Error: %s\n", SDL_GetError());
    return 1;
  }

  if ((XmpContext = xmp_create_context()) == nullptr)
  {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "xmp_create_context()");
    return 1;
  }

  if (not InitSound())
  {
    return 1;
  }

  // TODO: tune other params
  xmp_set_player(XmpContext, XMP_PLAYER_DEFPAN, 50);

  SDL_Renderer* renderer = nullptr;

  SDL_Window* window = SDL_CreateWindow("Printer test",
                                        100, 100,
                                        kWindowWidth, kWindowHeight,
                                        SDL_WINDOW_SHOWN);

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
      renderer = SDL_CreateRenderer(window, i, info.flags);
      if (renderer != nullptr)
      {
        break;
      }
    }
  }

  if (renderer == nullptr)
  {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "Couldn't create renderer: "
                 "maybe render to texture isn't supported.\n");
    return 1;
  }

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

  SDL_DisplayMode sdm;
  if (SDL_GetCurrentDisplayMode(0, &sdm) != 0)
  {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", SDL_GetError());
    return 1;
  }

  SDL_Log("%dx%d", sdm.w, sdm.h);

  if (!Printer.Init(renderer, kWindowWidth, kWindowHeight, 2.0))
  {
    return 1;
  }

  SDL_Event evt;

  xmp_frame_info fi;

  bool running = true;
  while (running)
  {
    while (SDL_PollEvent(&evt))
    {
      switch (evt.type)
      {
        case SDL_QUIT:
          running = false;
          break;

        case SDL_KEYDOWN:
        {
          switch(evt.key.keysym.sym)
          {
            case SDLK_ESCAPE:
              running = false;
              break;

            case SDLK_t:
            {
              if      (TopFlag == SDL_TRUE)  TopFlag = SDL_FALSE;
              else if (TopFlag == SDL_FALSE) TopFlag = SDL_TRUE;

              SDL_SetWindowAlwaysOnTop(window, TopFlag);
            }
            break;

            default:
              break;
          }
        }
        break;

        case SDL_DROPFILE:
        {
          DroppedFile = evt.drop.file;

          SDL_free(evt.drop.file);

          RefreshList();
        }
        break;

        default:
          break;
      }
    }

    if (StopPlaying)
    {
      StopPlaying = false;

      SDL_LockAudioDevice(AudioDevice);
      SDL_PauseAudioDevice(AudioDevice, 1);
      SDL_UnlockAudioDevice(AudioDevice);

      xmp_end_player(XmpContext);
      xmp_release_module(XmpContext);
    }

    Display();
  }

  xmp_free_context(XmpContext);

  SDL_Quit();

  return 0;
}

