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
  List = std::make_unique<ListBox<std::string>>(10, 5, 30, 10, Printer, 4);

  Buttons.push_back(
    std::make_unique<Button>(0, 0, " + ", Printer)
  );

  Buttons.back()->OnClick = []()
  {
    Counter++;
    List->Add(std::format("Item {}", Counter));
  };

  Buttons.push_back(
    std::make_unique<Button>(0, 4, " - ", Printer)
  );

  Buttons.back()->OnClick = []()
  {
    Counter--;
  };
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

  List->Draw();

  Printer.DrawGlyph(100, 30, TG::NameCP437::UARROW_1, TG::Colors::White, TG::Colors::Red, 0);
  Printer.DrawGlyph(100, 62, TG::NameCP437::UARROW_1, TG::Colors::White, TG::Colors::Red, 45);
  Printer.DrawGlyph(100, 94, TG::NameCP437::UARROW_1, TG::Colors::White, TG::Colors::Red, 90);

  Printer.DrawLine(800, 200, 850, 300, TG::Colors::Yellow);

  // delta time
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

  uint64_t updateAfter = 0;

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

            // -----------------------------------------------------------------

            case SDLK_DOWN:
            {
              List->SelectNextItem();

              int curItem = List->GetCurrentItemIndex();
              if (curItem != -1 and curItem >= List->GetBottomItemIndex())
              {
                List->ScrollDown();
              }

              SDL_Log("List box index = %d", curItem);
            }
            break;

            // -----------------------------------------------------------------

            case SDLK_UP:
            {
              List->SelectPreviousItem();

              int curItem = List->GetCurrentItemIndex();
              if (curItem != -1 and curItem <= List->GetTopItemIndex())
              {
                List->ScrollUp();
              }

              SDL_Log("List box index = %d", curItem);
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

            List->MouseToItemIndex(mi);
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

          List->SetMouseHoverFlag(mi);
        }
        break;

        // ---------------------------------------------------------------------

        case SDL_MOUSEWHEEL:
        {
          if (List->IsMouseInside(evt.wheel.mouseX, evt.wheel.mouseY))
          {
            if (evt.wheel.y < 0)
            {
              List->ScrollDown();
            }
            else
            {
              List->ScrollUp();
            }
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

    if (SDL_GetTicks64() - updateAfter > 500)
    {
      updateAfter = SDL_GetTicks64();
      DeltaTime = SDL_GetTicks64() - dt;
    }
  }

l_exit:

  SDL_Quit();

  return 0;
}
