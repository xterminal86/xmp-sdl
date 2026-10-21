#include "button.h"

#include "constants.h"

Button::Button(size_t charPosX,
               size_t charPosY,
               const std::string& text,
               TG::Printer& printerRef)
  : _printerRef(printerRef)
{
  _transform.x = charPosX;
  _transform.y = charPosY;
  _transform.w = text.length() + 2;
  _transform.h = 2;

  _text = text;

  _pixelPerfect.x = charPosX * Constants::CharWidth + Constants::CharWidth / 2 + 2;
  _pixelPerfect.y = charPosY * Constants::CharHeight + Constants::CharHeight / 2;
  _pixelPerfect.w = _transform.w * Constants::CharWidth - Constants::CharWidth - 4;
  _pixelPerfect.h = _transform.h * Constants::CharHeight - 2;

  //SDL_Log("%d %d - %d %d",
  //        _pixelPerfect.x,
  //        _pixelPerfect.y,
  //        _pixelPerfect.w,
  //        _pixelPerfect.h);
}

// =============================================================================

bool Button::IsMouseInside(const MouseInfo& mi)
{
  return (
        mi.X >= _pixelPerfect.x
    and mi.X <= _pixelPerfect.x + _pixelPerfect.w
    and mi.Y >= _pixelPerfect.y
    and mi.Y <= _pixelPerfect.y + _pixelPerfect.h
  );
}

// =============================================================================

void Button::MouseDown(const MouseInfo& mi)
{
  if (IsMouseInside(mi))
  {
    _mdownFlag = true;
    _fgColor = TG::Colors::Black;
    _bgColor = TG::Colors::White;
  }
}

// =============================================================================

void Button::MouseUp(const MouseInfo& mi)
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

void Button::MouseOver(const MouseInfo& mi)
{
  _hoverFlag = IsMouseInside(mi);
}

// =============================================================================

void Button::Draw()
{
  _printerRef.DrawFilledRect(_pixelPerfect.x,
                             _pixelPerfect.y,
                             _pixelPerfect.w,
                             _pixelPerfect.h,
                             TG::Colors::White);

  if (_mdownFlag)
  {
    _printerRef.DrawFilledRect(_pixelPerfect.x + kButtonBorderSize,
                               _pixelPerfect.y + kButtonBorderSize,
                               _pixelPerfect.w - kBorderSizeDoubled,
                               _pixelPerfect.h - kBorderSizeDoubled,
                               _bgColor);

    _printerRef.PrintFB(_transform.x + 1,
                        _transform.y + 1,
                        _text,
                        TG::Printer::kAlignLeft,
                        _fgColor,
                        _bgColor);
  }
  else
  {
    _printerRef.DrawFilledRect(_pixelPerfect.x + kButtonBorderSize,
                               _pixelPerfect.y + kButtonBorderSize,
                               _pixelPerfect.w - kBorderSizeDoubled,
                               _pixelPerfect.h - kBorderSizeDoubled,
                               _hoverFlag ?
                               TG::Colors::ShadesOfGrey::Four :
                               _bgColor);

    _printerRef.PrintFB(_transform.x + 1,
                        _transform.y + 1,
                        _text,
                        TG::Printer::kAlignLeft,
                        _fgColor,
                        _hoverFlag ?
                        TG::Colors::ShadesOfGrey::Four :
                        _bgColor);
  }
}
