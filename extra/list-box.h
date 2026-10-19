#ifndef LISTBOX_H
#define LISTBOX_H

#include <vector>
#include <format>

#include <SDL2/SDL.h>

#include "structs.h"
#include "constants.h"
#include "printer.h"

template <typename T>
class ListBox
{
  public:
    ListBox(int charPosX, int charPosY, int charWidth, int charHeight)
    {
      _transform.x = charPosX;
      _transform.y = charPosY;
      _transform.w = charWidth;
      _transform.h = charHeight;
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

  private:
    SDL_Rect _transform;

    std::vector<T> _items;

    int _itemIndex = -1;

    size_t _offset = 0;
};

#endif // LISTBOX_H
