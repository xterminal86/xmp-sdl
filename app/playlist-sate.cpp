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
  _listBox = std::make_unique<ListBox<PlaylistItem>>(
    0, 0, 40, Constants::WindowCharHeightVisible
  );

  _listItemWidth = (_listBox->Transform().w - 1);

  _itemBackground = std::string(_listItemWidth, ' ');

  _button = std::make_unique<Button>(50, 5, " TEST BUTTON ", _printerRef);
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

        _listBox->MouseToItemIndex(_mouseClickData);
        //SDL_Log("List box index = %d", ind);
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
          size_t s = _listBox->GetListSize();

          PlaylistItem item;

          item.Filename = std::format("Item {}", s++);

          _listBox->Add(item);
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
          _listBox->SelectNextItem();

          int curItem = _listBox->GetCurrentItemIndex();
          if (curItem != -1 and curItem >= _listBox->GetBottomItemIndex())
          {
            _listBox->ScrollDown();
          }

          SDL_Log("List box index = %d", curItem);
        }
        break;

        // -----------------------------------------------------------------

        case SDLK_UP:
        {
          _listBox->SelectPreviousItem();

          int curItem = _listBox->GetCurrentItemIndex();
          if (curItem != -1 and curItem <= _listBox->GetTopItemIndex())
          {
            _listBox->ScrollUp();
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
  int ind = _listBox->GetCurrentItemIndex();
  if (ind != -1)
  {
    _soundPlayerRef.Stop();
    _soundPlayerRef.Play(_listBox->GetItem(ind)->URI);
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
      { _listBox->Transform().x, _listBox->Transform().y }
    , { _listBox->Transform().w, _listBox->Transform().h }
    , "PLAYLIST"
    , TG::Colors::Yellow
    , TG::Colors::Blue
  );

  size_t yPos = 0;
  for (size_t ind = _listBox->Offset(); ind < _listBox->GetListSize(); ind++)
  {
    if (ind == _listBox->GetCurrentItemIndex())
    {
      _printerRef.PrintFB(_listBox->Transform().x + 1,
                          _listBox->Transform().y + 1 + yPos,
                          _itemBackground,
                          TG::Printer::kAlignLeft,
                          TG::Colors::Blue,
                          TG::Colors::Blue);

      _printerRef.PrintFB(_listBox->Transform().x + 2,
                          _listBox->Transform().y + 1 + yPos,
                          std::format("{}", _listBox->GetItem(ind)->Filename),
                          TG::Printer::kAlignLeft,
                          TG::Colors::White,
                          TG::Colors::Blue);
    }
    else
    {
      _printerRef.PrintFB(_listBox->Transform().x + 2,
                          _listBox->Transform().y + 1 + yPos,
                          std::format("{}", _listBox->GetItem(ind)->Filename),
                          TG::Printer::kAlignLeft,
                          TG::Colors::White);
    }

    yPos++;

    if (yPos > _listBox->Transform().h - 2)
    {
      break;
    }
  }
}

// =============================================================================

void PlaylistState::DrawStuff()
{
  _printerRef.Clear();

  DrawPlaylist();

  _button->Draw();

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

          if (fname.length() >= _listBox->Transform().w - 2)
          {
            fname.resize(_listBox->Transform().w - 2);
          }

          PlaylistItem item;

          item.URI      = uri;
          item.Filename = fname;

          _listBox->Add(item);
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

      _listBox->Add(item);
    }
}
