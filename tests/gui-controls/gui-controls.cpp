#include <memory>
#include <vector>
#include <format>

#include <SDL2/SDL.h>

#include "printer.h"
#include "constants.h"
#include "button.h"
#include "list-box.h"
#include "utils.h"

SDL_Window* Window = nullptr;
SDL_Renderer* Renderer = nullptr;

TG::Printer Printer;

uint64_t DeltaTime = 0;

std::vector<std::unique_ptr<Button>> Buttons;

std::unique_ptr<ListBox<std::string>> List;

int64_t Counter = 0;

void InitControls()
{
  Buttons.push_back(
    std::make_unique<Button>(0, 0, " + ", Printer)
  );

  Buttons.back()->OnClick = []() { Counter++; };

  Buttons.push_back(
    std::make_unique<Button>(0, 4, " - ", Printer)
  );

  Buttons.back()->OnClick = []() { Counter--; };

  List = std::make_unique<ListBox<std::string>>(10, 0, 30, 30);
}

// =============================================================================

void Draw()
{
  Printer.Clear();

  for (auto& b : Buttons)
  {
    b->Draw();
  }

  Printer.PrintFB(2,
                  3,
                  std::format("{}", Counter),
                  TG::Printer::kAlignLeft,
                  TG::Colors::White);

  // FPS
  Printer.PrintFB(Constants::WindowCharWidthVisible,
                  Constants::WindowCharHeightVisible,
                  std::to_string(DeltaTime),
                  TG::Printer::kAlignRight,
                  TG::Colors::Yellow);

  Printer.Render();
}

// =============================================================================

int main(int argc, char* argv[])
{
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
  {
    fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
    return false;
  }

  Window = SDL_CreateWindow("GUI controls test",
                            100, 100,
                            Constants::WindowWidth,
                            Constants::WindowHeight,
                            SDL_WINDOW_SHOWN);
  if (Window == nullptr)
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
      Renderer = SDL_CreateRenderer(Window, i, info.flags);
      if (Renderer != nullptr)
      {
        break;
      }
    }
  }

  if (Renderer == nullptr)
  {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "Couldn't create renderer: "
                 "maybe render to texture isn't supported.\n");
    return false;
  }

  SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);

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

  if (not Printer.Init(Renderer,
                       Constants::WindowWidth,
                       Constants::WindowHeight,
                       Constants::GlobalScale))
  {
    return false;
  }

  InitControls();

  uint64_t dt = SDL_GetTicks64();

  SDL_Event evt;

  MouseInfo mi;

  while (true)
  {
    while (SDL_PollEvent(&evt))
    {
      switch (evt.type)
      {
        case SDL_KEYDOWN:
        {
          switch (evt.key.keysym.sym)
          {
            case SDLK_ESCAPE:
            {
              goto l_exit;
            }
            break;
          }
        }
        break;

        // ---------------------------------------------------------------------

        case SDL_MOUSEBUTTONDOWN:
        {
          if (evt.button.button == 1)
          {
            Utils::GetMouseInfo(evt.button, mi);

            for (auto& b : Buttons)
            {
              b->MouseDown(mi);
            }
          }
        }
        break;

        // ---------------------------------------------------------------------

        case SDL_MOUSEBUTTONUP:
        {
          if (evt.button.button == 1)
          {
            Utils::GetMouseInfo(evt.button, mi);

            for (auto& b : Buttons)
            {
              b->MouseUp(mi);
            }
          }
        }
        break;

          // ---------------------------------------------------------------------

          case SDL_MOUSEMOTION:
          {
            Utils::GetMouseInfo(evt.button, mi);

            for (auto& b : Buttons)
            {
              b->MouseOver(mi);
            }
          }
          break;

        // ---------------------------------------------------------------------

        case SDL_QUIT:
        {
          goto l_exit;
        }
        break;

        default:
          break;
      }

      //_appStates[_currentState]->HandleEvent(evt);
    }

    dt = SDL_GetTicks64();

    Draw();

    DeltaTime = SDL_GetTicks64() - dt;
  }

l_exit:

  SDL_Quit();

  return 0;
}
