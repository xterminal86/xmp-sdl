#include "button.h"

#include "constants.h"

Button::Button(size_t charPosX,
               size_t charPosY,
               const std::string& text,
               TG::Printer& printerRef)
  : _printerRef(printerRef), _text(text)
{
  SDL_Log("<Button '%s'>", text.data());

  _transform.x = charPosX;
  _transform.y = charPosY;
  _transform.w = text.length() + 2;
  _transform.h = 2;

  SDL_Log("T: %d %d - %d %d",
          _transform.x,
          _transform.y,
          _transform.w,
          _transform.h);

  _pixelPerfect.x = charPosX * Constants::CharWidth + Constants::CharWidth / 2 + 2;
  _pixelPerfect.y = charPosY * Constants::CharHeight + Constants::CharHeight / 2;
  _pixelPerfect.w = _transform.w * Constants::CharWidth - Constants::CharWidth - 4;
  _pixelPerfect.h = _transform.h * Constants::CharHeight - 2;

  SDL_Log("PP: %d %d - %d %d",
          _pixelPerfect.x,
          _pixelPerfect.y,
          _pixelPerfect.w,
          _pixelPerfect.h);
}

// =============================================================================

Button::Button(int px, int py, TG::NameCP437 glyph, TG::Printer& printerRef)
  : _printerRef(printerRef), _glyph(glyph)
{
  _transform.x = px;
  _transform.y = py;
  _transform.w = Constants::CharWidth * 2;

  // To compensate for glyph's background.
  _transform.h = Constants::CharHeight + 4;

  _pixelPerfect.x = _transform.x;
  _pixelPerfect.y = _transform.y;
  _pixelPerfect.w = _transform.w;
  _pixelPerfect.h = _transform.h;

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

    if (_glyph != TG::NameCP437::FIRST)
    {
      _printerRef.PrintPP(_transform.x + Constants::CharWidthHalf,
                          _transform.y + 2,
                          _glyph,
                          _fgColor,
                          _bgColor);
    }
    else
    {
      _printerRef.PrintFB(_transform.x + 1,
                          _transform.y + 1,
                          _text,
                          TG::Printer::kAlignLeft,
                          _fgColor,
                          _bgColor);
    }
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

    if (_glyph != TG::NameCP437::FIRST)
    {
      _printerRef.PrintPP(_transform.x + Constants::CharWidthHalf,
                          _transform.y + 2,
                          _glyph,
                          _fgColor,
                          _hoverFlag ?
                          TG::Colors::ShadesOfGrey::Four :
                          _bgColor);
    }
    else
    {
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
}
