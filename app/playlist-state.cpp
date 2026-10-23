#include "playlist-state.h"

#include "sound-player.h"
#include "printer.h"
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
  _playlist = std::make_unique<ListBox<PlaylistItem>>(
    0, 0, 40, Constants::WindowCharHeightVisible, _printerRef
  );

  //_listItemWidth = (_playlist->Transform().w - 1);

  //_itemBackground = std::string(_listItemWidth, ' ');

  // ---------------------------------------------------------------------------
  auto _create_button = [this](int x, int y, const std::string& text)
  {
    _buttons.push_back(
      std::make_unique<Button>(x, y, text, _printerRef)
    );
  };
  // ---------------------------------------------------------------------------

  _create_button(41, 5, " CLEAR ");
  _buttons.back()->OnClick = [this]()
  {
    _playlist->Clear();
    _soundPlayerRef.Stop();
  };

  //
  //_create_button(58, 5, " STOP ");
  //_buttons.back()->OnClick = []() { SDL_Log("Button stop"); };
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

      Utils::GetMouseInfo(evt.button, _mouseClickData);
      for (auto& b : _buttons)
      {
        b->MouseOver(_mouseClickData);
      }

      _playlist->MouseOver(_mouseClickData);
    }
    break;

    // ---------------------------------------------------------------------

    case SDL_MOUSEBUTTONDOWN:
    {
      if (evt.button.button == 1)
      {
        if (evt.button.timestamp - _mouseClickData.Timestamp <= 250)
        {
          _mouseClickData.WasDoubleClick = true;
          _mouseClickData.Timestamp = 0;

          TryToPlay();
        }
        else
        {
          _mouseClickData.Timestamp = evt.button.timestamp;
          _mouseClickData.WasDoubleClick = false;
        }

        Utils::GetMouseInfo(evt.button, _mouseClickData);

        _playlist->MouseToItemIndex(_mouseClickData);
        //SDL_Log("List box index = %d", ind);

        for (auto& b : _buttons)
        {
          b->MouseDown(_mouseClickData);
        }
      }
    }
    break;

    // ---------------------------------------------------------------------

    case SDL_MOUSEBUTTONUP:
    {
      if (evt.button.button == 1)
      {
        Utils::GetMouseInfo(evt.button, _mouseClickData);

        for (auto& b : _buttons)
        {
          b->MouseUp(_mouseClickData);
        }
      }
    }
    break;

    // ---------------------------------------------------------------------

    case SDL_KEYDOWN:
    {
      switch(evt.key.keysym.sym)
      {
        // -----------------------------------------------------------------
        // FIXME: debug
        case SDLK_a:
        {
          size_t s = _playlist->GetListSize();

          PlaylistItem item;

          item.Filename = std::format("Item {}", s++);

          _playlist->Add(item);
        }
        break;

        case SDLK_RETURN:
        case SDLK_RETURN2:
        case SDLK_x:
        {
          TryToPlay();
        }
        break;

        case SDLK_v:
        {
          _soundPlayerRef.Stop();
        }
        break;

        // -----------------------------------------------------------------

        case SDLK_DOWN:
        {
          _playlist->SelectNextItem();

          int curItem = _playlist->GetCurrentItemIndex();
          if (curItem != -1 and curItem >= _playlist->GetBottomItemIndex())
          {
            _playlist->ScrollDown();
          }

          SDL_Log("List box index = %d", curItem);
        }
        break;

        // -----------------------------------------------------------------

        case SDLK_UP:
        {
          _playlist->SelectPreviousItem();

          int curItem = _playlist->GetCurrentItemIndex();
          if (curItem != -1 and curItem <= _playlist->GetTopItemIndex())
          {
            _playlist->ScrollUp();
          }

          SDL_Log("List box index = %d", curItem);
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

      UpdatePlaylist();

      SDL_RaiseWindow(_windowRef);
    }
    break;

    // ---------------------------------------------------------------------

    default:
      break;
  }
}

// =============================================================================

void PlaylistState::TryToPlay()
{
  int ind = _playlist->GetCurrentItemIndex();
  if (ind != -1)
  {
    _soundPlayerRef.Stop();
    _soundPlayerRef.Play(_playlist->GetItem(ind)->URI);
  }
}

#ifdef DEBUG_BUILD
// =============================================================================

void PlaylistState::DrawDimensions()
{
  for (size_t x = 0; x < Constants::WindowCharWidthVisible; x++)
  {
    _printerRef.PrintFB(x,
                        0,
                        std::format("{}", (x % 10)),
                        TG::Printer::kAlignLeft,
                        ((x % 10 == 0) ?
                         TG::Colors::White :
                         TG::Colors::ShadesOfGrey::Eight));
  }

  for (size_t y = 0; y < Constants::WindowCharHeightVisible; y++)
  {
    _printerRef.PrintFB(0,
                        y,
                        std::format("{}", (y % 10)),
                        TG::Printer::kAlignLeft,
                        ((y % 10 == 0) ?
                         TG::Colors::White :
                         TG::Colors::ShadesOfGrey::Eight));
  }
}

// =============================================================================

void PlaylistState::DrawFPS()
{
  _printerRef.PrintFB(0,
                      Constants::WindowCharHeightVisible,
                      std::format(" {} ", _drawTime),
                      TG::Printer::kAlignLeft,
                      TG::Colors::Yellow);
}

// =============================================================================

void PlaylistState::DrawWindowSize()
{
  _printerRef.PrintFB(
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

void PlaylistState::DrawMouseInfo()
{
  _printerRef.PrintFB(
    Constants::WindowCharWidthVisible + 1,
    1,
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

  _printerRef.PrintFB(
    Constants::WindowCharWidthVisible + 1,
    2,
    std::format(" {} {} ", _mouseX, _mouseY),
    TG::Printer::kAlignRight,
    TG::Colors::Cyan
  );
}
#endif

// =============================================================================

void PlaylistState::DrawPlaylist()
{
  _printerRef.DrawWindow(
      { _playlist->Transform().x, _playlist->Transform().y }
    , { _playlist->Transform().w, _playlist->Transform().h }
    , "PLAYLIST"
    , TG::Colors::Yellow
    , TG::Colors::Blue
  );

  if (_playlist->GetListSize() == 0)
  {
    _printerRef.PrintFB(_playlist->Transform().x + _playlist->Transform().w / 2,
                        _playlist->Transform().y + _playlist->Transform().h / 2,
                        "drag and drop here",
                        TG::Printer::kAlignCenter,
                        TG::Colors::White);
    return;
  }

  _playlist->Draw();
}

// =============================================================================

void PlaylistState::DrawStuff()
{
  _printerRef.Clear();

  DrawPlaylist();

  for (auto& b : _buttons)
  {
    b->Draw();
  }

#ifdef DEBUG_BUILD
  //DrawDimensions();
  DrawFPS();
  //DrawWindowSize();
  DrawMouseInfo();
#endif

  _printerRef.Render();
}

// =============================================================================

void PlaylistState::UpdatePlaylist()
{
  if (fs::is_directory(_droppedFile))
    {
      SDL_Log("Dir");

      for (auto& entry : fs::recursive_directory_iterator(_droppedFile))
      {
        std::string uri = entry.path().string();
        if (fs::is_regular_file(uri))
        {
          std::string fname = entry.path().filename().string();

          if (fname.length() >= _playlist->Transform().w - 2)
          {
            fname.resize(_playlist->Transform().w - 2);
          }

          PlaylistItem item;

          item.URI      = uri;
          item.Filename = fname;

          _playlist->Add(item);
        }
      }
    }
    else if (fs::is_regular_file(_droppedFile))
    {
      SDL_Log("File");

      fs::path p(_droppedFile);

      PlaylistItem item;

      item.URI      = p.string();
      item.Filename = p.filename().string();

      _playlist->Add(item);
    }
}
