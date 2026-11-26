#include "gui-control.h"

GUIControl::GUIControl(TG::Printer &printerRef) : _printerRef(printerRef) {}

// =============================================================================

GUIControl::~GUIControl() {}

// =============================================================================

void GUIControl::Move(int px, int py)
{
  _transform.x = px;
  _transform.y = py;

  _pixelPerfect.x = px;
  _pixelPerfect.y = py;
}

// =============================================================================

void GUIControl::MouseDown(const MouseInfo& mi)
{
  if (not _enabled)
  {
    return;
  }

  if (IsMouseInside(mi))
  {
    _mdownFlag = true;
    _fgColor = TG::Colors::Black;
    _bgColor = TG::Colors::White;

    if (_onMouseDown.target_type() != typeid(void))
    {
      _onMouseDown();
    }
  }
}

// =============================================================================

void GUIControl::MouseUp(const MouseInfo& mi)
{
  if (not _enabled)
  {
    return;
  }

  if (IsMouseInside(mi) or _mdownFlag)
  {
    _mdownFlag = false;
    _fgColor = TG::Colors::White;
    _bgColor = TG::Colors::Black;

    if (_onMouseUp.target_type() != typeid(void))
    {
      _onMouseUp();
    }

    if (OnClick.target_type() != typeid(void))
    {
      OnClick();
    }
  }
}

// =============================================================================

void GUIControl::MouseOver(const MouseInfo& mi)
{
  if (not _enabled)
  {
    return;
  }

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

// =============================================================================

const SDL_Rect& GUIControl::Transform()
{
  return _transform;
}

// =============================================================================

const SDL_Rect& GUIControl::TransformPP()
{
  return _pixelPerfect;
}

// =============================================================================

void GUIControl::SetEnabled(bool value)
{
  _enabled = value;
}

// =============================================================================

bool GUIControl::IsEnabled()
{
  return _enabled;
}
