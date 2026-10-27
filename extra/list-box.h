#ifndef LISTBOX_H
#define LISTBOX_H

#include <vector>
#include <format>
#include <concepts>

#include <SDL2/SDL.h>

#include "structs.h"
#include "constants.h"
#include "printer.h"

template <typename T>
concept HasToString = requires(const T& a)
{
  { a.ToString() } -> std::same_as<const std::string&>;
};

template <typename T>
concept IsString = std::is_same_v< std::remove_cvref_t<T>, std::string >;

template <typename T>
concept StringRepresentable = HasToString<T> or IsString<T>;

template <StringRepresentable T>
class ListBox
{
  public:
    ListBox(int charPosX,
            int charPosY,
            int charWidth,
            int charHeight,
            TG::Printer& printerRef,
            uint8_t borderSize = 1)
      : _printerRef(printerRef), _borderSize(borderSize)
    {
      _transform.x = charPosX;
      _transform.y = charPosY;
      _transform.w = charWidth;
      _transform.h = charHeight;

      _pixelPerfect.x = charPosX * Constants::CharWidth + Constants::CharWidth;
      _pixelPerfect.y = charPosY * Constants::CharHeight + Constants::CharHeight;
      _pixelPerfect.w = _transform.w * Constants::CharWidth - Constants::CharWidth;
      _pixelPerfect.h = _transform.h * Constants::CharHeight - Constants::CharHeight;

      _listItemWidth = _transform.w - 1;

      _itemBackground = std::string(_listItemWidth, ' ');

      const size_t& cw  = Constants::CharWidth;
      const size_t& ch  = Constants::CharHeight;
      const size_t& cwh = Constants::CharWidthHalf;

      int xstart = (int)_transform.x + (int)_transform.w;
      int yend   = (int)_transform.y + (int)_transform.h;

      _scrollButtonsPositions =
      {
        xstart * (int)cw + (int)cwh,
        (int)_transform.y * (int)ch - (int)_borderSize,
        xstart * (int)cw + (int)cwh,
        yend * (int)ch
      };
    }

    // =========================================================================

    void SetMouseHoverFlag(const MouseInfo& mi)
    {
      _hoverFlag = IsMouseInside(mi);
    }

    // =========================================================================

    void Clear()
    {
      _items.clear();
      _itemIndex = -1;
      _offset = 0;
    }

    // =========================================================================

    int MouseToItemIndex(const MouseInfo& mouseInfo)
    {
      bool mouseInside = (
        mouseInfo.CharPosX >= _transform.x + 1
    and mouseInfo.CharPosX <= (_transform.x + _transform.w - 1)
    and mouseInfo.CharPosY >= _transform.y + 1
    and mouseInfo.CharPosY <= (_transform.y + _transform.h - 1)
      );

      int itemIndex = mouseInfo.CharPosY - (_transform.y + 1);

      if (not mouseInside or _items.empty() or (itemIndex >= (int)_items.size()))
      {
        return -1;
      }
      else
      {
        _itemIndex = itemIndex + _offset;
      }

      return _itemIndex;
    }

    // =========================================================================

    void Add(const T& item)
    {
      _items.push_back(item);
    }

    // =========================================================================

    void ScrollUp()
    {
      if (_offset > 0)
      {
        _offset--;
      }
    }

    // =========================================================================

    void ScrollDown()
    {
      if (_offset + _transform.h - 1 < _items.size())
      {
        _offset++;
      }
    }

    // =========================================================================

    void SelectItem(int index)
    {
      if (index < 0 or index >= (int)_items.size())
      {
        return;
      }

      _itemIndex = index;
    }

    // =========================================================================

    void SelectNextItem()
    {
      if (_items.empty())
      {
        return;
      }

      if (_itemIndex == -1)
      {
        SelectItem(0);
      }
      else
      {
        SelectItem(_itemIndex + 1);
      }
    }

    // =========================================================================

    void SelectPreviousItem()
    {
      if (_items.empty())
      {
        return;
      }

      if (_itemIndex == -1)
      {
        SelectItem(0);
      }
      else
      {
        SelectItem(_itemIndex - 1);
      }
    }

    int GetCurrentItemIndex()
    {
      return _itemIndex;
    }

    // =========================================================================

    int GetTopItemIndex()
    {
      return _items.empty() ? -1 : _offset;
    }

    // =========================================================================

    int GetBottomItemIndex()
    {
      int ind = -1;

      if (_items.empty())
      {
        return ind;
      }

      if (_offset + _transform.h - 2 < _items.size())
      {
        ind = _offset + _transform.h - 2;
      }
      else
      {
        ind = _items.size() - 1;
      }

      return ind;
    }

    // =========================================================================

    size_t GetListSize()
    {
      return _items.size();
    }

    // =========================================================================

    const SDL_Rect& Transform()
    {
      return _transform;
    }

    // =========================================================================

    size_t Offset()
    {
      return _offset;
    }

    // =========================================================================

    T* GetItem(size_t index)
    {
      return (index < 0 or index >= _items.size()) ? nullptr : &_items[index];
    }

    // =========================================================================

    const SDL_Rect& GetScrollButtonsPositions()
    {
      return _scrollButtonsPositions;
    }

    // =========================================================================

    void Draw()
    {
      if (_borderSize != 0)
      {
        _printerRef.DrawFilledRect(_pixelPerfect.x - _borderSize,
                                   _pixelPerfect.y - _borderSize,
                                   _pixelPerfect.w + _borderSize * 2,
                                   _pixelPerfect.h + _borderSize * 2,
                                   TG::Colors::White);

        _printerRef.DrawFilledRect(_pixelPerfect.x,
                                   _pixelPerfect.y,
                                   _pixelPerfect.w,
                                   _pixelPerfect.h,
                                   TG::Colors::Black);
      }

      size_t yPos = 0;
      for (size_t ind = _offset; ind < _items.size(); ind++)
      {
        if ((int)ind == _itemIndex)
        {
          _printerRef.PrintFB(_transform.x + 1,
                              _transform.y + 1 + yPos,
                              _itemBackground,
                              TG::Printer::kAlignLeft,
                              TG::Colors::Blue,
                              TG::Colors::Blue);

          T* item = GetItem(ind);

          if (item != nullptr)
          {
            if constexpr (std::is_same_v<std::remove_cvref_t<T>, std::string>)
            {
              _printerRef.PrintFB(_transform.x + 2,
                                  _transform.y + 1 + yPos,
                                  *item,
                                  TG::Printer::kAlignLeft,
                                  TG::Colors::White,
                                  TG::Colors::Blue);
            }
            else
            {
              _printerRef.PrintFB(_transform.x + 2,
                                  _transform.y + 1 + yPos,
                                  item->ToString(),
                                  TG::Printer::kAlignLeft,
                                  TG::Colors::White,
                                  TG::Colors::Blue);
            }

            if constexpr (not std::is_same_v<std::remove_cvref_t<T>, std::string>)
            {
              if (not item->IsValid)
              {
                _printerRef.DrawFilledRect(
                  _transform.x * Constants::CharWidth,
                  (_transform.y + 1 + yPos) * Constants::CharHeight + Constants::CharHeightHalf,
                  _transform.w * Constants::CharWidth,
                  4,
                  TG::Colors::Red
                );
              }
            }
          }
          else
          {
            _printerRef.PrintFB(_transform.x + 2,
                                _transform.y + 1 + yPos,
                                std::format("{:p} - (null)", (void*)item),
                                TG::Printer::kAlignLeft,
                                TG::Colors::White,
                                TG::Colors::Blue);
          }
        }
        else
        {
          T* item = GetItem(ind);

          if (item != nullptr)
          {
            if constexpr (std::is_same_v<std::remove_cvref_t<T>, std::string>)
            {
              _printerRef.PrintFB(_transform.x + 2,
                                  _transform.y + 1 + yPos,
                                  *item,
                                  TG::Printer::kAlignLeft,
                                  TG::Colors::White);
            }
            else
            {
              _printerRef.PrintFB(_transform.x + 2,
                                  _transform.y + 1 + yPos,
                                  item->ToString(),
                                  TG::Printer::kAlignLeft,
                                  TG::Colors::White);
            }

            if constexpr (not std::is_same_v<std::remove_cvref_t<T>, std::string>)
            {
              if (not item->IsValid)
              {
                _printerRef.DrawFilledRect(
                  _transform.x * Constants::CharWidth,
                  (_transform.y + 1 + yPos) * Constants::CharHeight + Constants::CharHeightHalf,
                  _transform.w * Constants::CharWidth,
                  4,
                  TG::Colors::Red
                );
              }
            }
          }
          else
          {
            _printerRef.PrintFB(_transform.x + 2,
                                _transform.y + 1 + yPos,
                                std::format("{:p} - (null)", (void*)item),
                                TG::Printer::kAlignLeft,
                                TG::Colors::White);
          }
        }

        yPos++;

        if ((int)yPos > _transform.h - 2)
        {
          break;
        }
      }
    }

    // =========================================================================

    bool IsMouseInside(int mx, int my)
    {
      return (
            mx >= _pixelPerfect.x
        and mx <= _pixelPerfect.x + _pixelPerfect.w
        and my >= _pixelPerfect.y
        and my <= _pixelPerfect.y + _pixelPerfect.h
      );
    }

    // =========================================================================

    bool IsMouseInside(const MouseInfo& mi)
    {
      return (
            mi.X >= _pixelPerfect.x
        and mi.X <= _pixelPerfect.x + _pixelPerfect.w
        and mi.Y >= _pixelPerfect.y
        and mi.Y <= _pixelPerfect.y + _pixelPerfect.h
      );
    }

  private:
    bool _hoverFlag = false;

    SDL_Rect _transform;
    SDL_Rect _pixelPerfect;
    SDL_Rect _scrollButtonsPositions;

    std::vector<T> _items;

    int _itemIndex = -1;

    size_t _offset = 0;

    TG::Printer& _printerRef;

    size_t _listItemWidth = 0;

    uint8_t _borderSize = 1;

    std::string _itemBackground;
};

#endif // LISTBOX_H
