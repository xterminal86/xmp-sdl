#include "playlist-state.h"

#include "constants.h"
#include "utils.h"

#include <format>
#include <filesystem>

namespace fs = std::filesystem;

// =============================================================================

const size_t kPageMaxItems = Constants::WindowCharHeightVisible - 2;
const std::string kHighlighter(Constants::WindowCharWidthVisible - 2, ' ');

// =============================================================================

void PlaylistState::Init()
{
  for (size_t i = 0; i < 30; i++)
  {
    PlaylistItem item;
    item.Filename = std::format("File {}", (i + 1));
    _playlist.push_back(item);
  }
}

// =============================================================================

void PlaylistState::HandleEvent(const SDL_Event& evt)
{
  switch (evt.type)
  {
    case SDL_MOUSEMOTION:
    {
      _mouseX = evt.motion.x;
      _mouseY = evt.motion.y;
    }
    break;

    // ---------------------------------------------------------------------

    case SDL_MOUSEBUTTONDOWN:
    {
      if (evt.button.timestamp - _mouseClickData.Timestamp <= 250)
      {
        _mouseClickData.WasDoubleClick = true;
        _mouseClickData.Timestamp = 0;
      }
      else
      {
        _mouseClickData.Timestamp = evt.button.timestamp;
        _mouseClickData.WasDoubleClick = false;
      }

      Utils::GetMouseInfo(evt.button, _mouseClickData);

      bool mousePosCheck = (
        _mouseClickData.CharPosX >= 1
    and _mouseClickData.CharPosX <= Constants::WindowCharWidthVisible - 1
    and _mouseClickData.CharPosY >= 1
    and _mouseClickData.CharPosY <= Constants::WindowCharHeightVisible - 1
      );

      if (mousePosCheck)
      {
        _cursorPosY = _mouseClickData.CharPosY;
      }
    }
    break;

    // ---------------------------------------------------------------------

    case SDL_KEYDOWN:
    {
      switch(evt.key.keysym.sym)
      {
        // -----------------------------------------------------------------

        // -----------------------------------------------------------------

        case SDLK_DOWN:
        {
          if (_cursorPosY == -1)
          {
            _cursorPosY = 1;
          }
          else
          {
            _cursorPosY++;

            if (_cursorPosY <= 1)
            {
              _cursorPosY = 1;
            }

            if (_cursorPosY > Constants::WindowCharHeightVisible - 1)
            {
              if (_cursorPosY == Constants::WindowCharHeightVisible)
              {
                if (_playlistOffset + kPageMaxItems < (_playlist.size() - 1))
                {
                  _playlistOffset++;
                }
              }

              _cursorPosY = Constants::WindowCharHeightVisible - 1;
            }
          }
        }
        break;

        // -----------------------------------------------------------------

        case SDLK_UP:
        {
          if (_cursorPosY == -1)
          {
            _cursorPosY = 1;
          }
          else
          {
            _cursorPosY--;

            if (_cursorPosY < 1)
            {
              if (_playlistOffset > 0)
              {
                _playlistOffset--;
              }

              _cursorPosY = 1;
            }
          }
        }
        break;

        // -----------------------------------------------------------------

        default:
          break;
      }
    }
    break;

    // ---------------------------------------------------------------------

    case SDL_DROPFILE:
    {
      _droppedFile = evt.drop.file;

      SDL_free(evt.drop.file);

      // FIXME:
      //RefreshList();
    }
    break;

    // ---------------------------------------------------------------------

    default:
      break;
  }
}

#ifdef DEBUG_BUILD
// =============================================================================

void PlaylistState::DrawFPS(TG::Printer* const printer)
{
  printer->PrintFB(0,
                   Constants::WindowCharHeightVisible,
                   std::format(" {} ", _drawTime),
                   TG::Printer::kAlignLeft,
                   TG::Colors::Yellow);
}

// =============================================================================

void PlaylistState::DrawWindowSize(TG::Printer* const printer)
{
  printer->PrintFB(
    Constants::WindowCharWidthVisible + 1,
    Constants::WindowCharHeightVisible,
    std::format(
      " {}x{} ",
      Constants::WindowCharWidthVisible,
      Constants::WindowCharHeightVisible
    ),
    TG::Printer::kAlignRight,
    TG::Colors::Yellow
  );
}

// =============================================================================

void PlaylistState::DrawMouseInfo(TG::Printer* const printer)
{
  printer->PrintFB(
    Constants::WindowCharWidthVisible + 1,
    0,
    std::format(
      " x: {} y: {} cx: {} cy: {} {} ",
      _mouseClickData.X,
      _mouseClickData.Y,
      _mouseClickData.CharPosX,
      _mouseClickData.CharPosY,
      (_mouseClickData.WasDoubleClick ? "DBL" : " - ")
    ),
    TG::Printer::kAlignRight,
    TG::Colors::Yellow
  );

  printer->PrintFB(
    Constants::WindowCharWidthVisible + 1,
    1,
    std::format(" {} {} ", _mouseX, _mouseY),
    TG::Printer::kAlignRight,
    TG::Colors::Cyan
  );
}
#endif

// =============================================================================

void PlaylistState::DrawPlaylist(TG::Printer* const printer,
                                 const size_t offset)
{
  if (_playlist.empty())
  {
    return;
  }

  size_t itemIndex = 0;
  size_t itemPosY  = 1;
  for (;;)
  {
    if (_cursorPosY != -1 and _cursorPosY == itemPosY)
    {
      printer->PrintFB(2,
                      itemPosY,
                      kHighlighter,
                      TG::Printer::kAlignLeft,
                      TG::Colors::ShadesOfGrey::Four,
                      TG::Colors::Blue);

      printer->PrintFB(2,
                       itemPosY,
                       _playlist[offset + itemIndex].Filename,
                       TG::Printer::kAlignLeft,
                       TG::Colors::White,
                       TG::Colors::Blue);
    }
    else
    {
      printer->PrintFB(2,
                       itemPosY,
                       _playlist[offset + itemIndex].Filename,
                       TG::Printer::kAlignLeft,
                       TG::Colors::White);
    }

    itemPosY++;
    itemIndex++;

    if (
         (itemPosY >= Constants::WindowCharHeightVisible)
      or (itemPosY >= _playlist.size())
    )
    {
      break;
    }
  }
}

// =============================================================================

void PlaylistState::DrawStuff(TG::Printer* const printer)
{
  printer->Clear();

  //DrawDimensionHints();

  printer->DrawWindow(
    { 0, 0 },
    {
      (int)Constants::WindowCharWidthVisible,
      (int)Constants::WindowCharHeightVisible
    }
  );

  DrawPlaylist(printer, _playlistOffset);

  if (_appSettingsRef.AlwaysOnTop == SDL_TRUE)
  {
    printer->PrintFB(Constants::WindowCharWidthVisible,
                     1,
                     "T",
                     TG::Printer::kAlignLeft,
                     TG::Colors::Yellow,
                     TG::Colors::Red);
  }

#ifdef DEBUG_BUILD
  DrawFPS(printer);
  DrawWindowSize(printer);
  DrawMouseInfo(printer);
#endif

  printer->Render();
}

// =============================================================================

void PlaylistState::RefreshList()
{
  if (fs::is_directory(_droppedFile))
  {
    SDL_Log("Dir");

    /*
    for (auto& entry : fs::recursive_directory_iterator(_droppedFile))
    {
      std::string uri = entry.path().string();
      if (fs::is_regular_file(uri))
      {
        PlaylistItem item;
        if (Utils::GetModuleInfo(uri, item))
        {
          item.Filename = entry.path().filename().string();
          _playlist.push_back(item);
        }
      }
    }
    */
  }
  else if (fs::is_regular_file(_droppedFile))
  {
    SDL_Log("File");

    /*
    PlaylistItem item;
    if (Utils::GetModuleInfo(_droppedFile, item))
    {
      _playlist.push_back(item);
    }
    */
  }
}
