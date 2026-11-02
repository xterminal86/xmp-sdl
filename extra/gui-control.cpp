#include "gui-control.h"

GUIControl::GUIControl(TG::Printer &printerRef) : _printerRef(printerRef) {}

// =============================================================================

void GUIControl::MouseDown(const MouseInfo& mi)
{
  if (IsMouseInside(mi))
  {
    _mdownFlag = true;
    _fgColor = TG::Colors::Black;
    _bgColor = TG::Colors::White;
  }
}

// =============================================================================

void GUIControl::MouseUp(const MouseInfo& mi)
{
  if (IsMouseInside(mi) or _mdownFlag)
  {
    _mdownFlag = false;
    _fgColor = TG::Colors::White;
    _bgColor = TG::Colors::Black;

    if (OnClick.target_type() != typeid(void))
    {
      OnClick();
    }
  }
}

// =============================================================================

void GUIControl::MouseOver(const MouseInfo& mi)
{
  _hoverFlag = IsMouseInside(mi);
}

// =============================================================================

bool GUIControl::IsMouseInside(const MouseInfo& mi)
{
  return (
        mi.X >= _pixelPerfect.x
    and mi.X <= _pixelPerfect.x + _pixelPerfect.w
    and mi.Y >= _pixelPerfect.y
    and mi.Y <= _pixelPerfect.y + _pixelPerfect.h
  );
}
