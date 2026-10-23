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
concept HasToString = requires(T a)
{
  { a.ToString() } -> std::convertible_to<std::string>;
};

template <typename T>
concept IsString = std::is_same_v<std::remove_cvref_t<T>, std::string>;

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
            TG::Printer& printerRef)
      : _printerRef(printerRef)
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
    }

    // =========================================================================

    void MouseOver(const MouseInfo& mi)
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

      if (not mouseInside or _items.empty() or (itemIndex >= _items.size()))
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
      if (index < 0 or index >= _items.size())
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

    void Draw()
    {
      _printerRef.DrawFilledRect(_pixelPerfect.x - 2,
                                 _pixelPerfect.y - 2,
                                 _pixelPerfect.w + 4,
                                 _pixelPerfect.h + 4,
                                 TG::Colors::White);

      _printerRef.DrawFilledRect(_pixelPerfect.x,
                                 _pixelPerfect.y,
                                 _pixelPerfect.w,
                                 _pixelPerfect.h,
                                 TG::Colors::Black);

      size_t yPos = 0;
      for (size_t ind = _offset; ind < _items.size(); ind++)
      {
        if (ind == _itemIndex)
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

        if (yPos > _transform.h - 2)
        {
          break;
        }
      }
    }

  private:
    bool IsMouseInside(const MouseInfo& mi)
    {
      return (
            mi.X >= _pixelPerfect.x
        and mi.X <= _pixelPerfect.x + _pixelPerfect.w
        and mi.Y >= _pixelPerfect.y
        and mi.Y <= _pixelPerfect.y + _pixelPerfect.h
      );
    }

    bool _hoverFlag = false;

    SDL_Rect _transform;
    SDL_Rect _pixelPerfect;

    std::vector<T> _items;

    int _itemIndex = -1;

    size_t _offset = 0;

    TG::Printer& _printerRef;

    size_t _listItemWidth = 0;

    std::string _itemBackground;
};

#endif // LISTBOX_H
