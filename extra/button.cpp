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
  _pixelPerfect.w = _pixelPerfect.x + _transform.w * Constants::CharWidth - Constants::CharWidth - 4;
  _pixelPerfect.h = _pixelPerfect.y + _transform.h * Constants::CharHeight - 2;
}

// =============================================================================

void Button::MouseDown(const MouseInfo& mi)
{
  bool inside = (
      mi.X >= _pixelPerfect.x
  and mi.X <= _pixelPerfect.w
  and mi.Y >= _pixelPerfect.y
  and mi.Y <= _pixelPerfect.h
  );

  if (inside)
  {
    _mdownFlag = true;
    _fgColor = TG::Colors::Black;
    _bgColor = TG::Colors::White;
  }
}

// =============================================================================

void Button::MouseUp(const MouseInfo& mi)
{
  bool inside = (
      mi.X >= _pixelPerfect.x
  and mi.X <= _pixelPerfect.w
  and mi.Y >= _pixelPerfect.y
  and mi.Y <= _pixelPerfect.h
  );

  if (inside or _mdownFlag)
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
  bool inside = (
      mi.X > _pixelPerfect.x
  and mi.X < _pixelPerfect.w
  and mi.Y > _pixelPerfect.y
  and mi.Y < _pixelPerfect.h
  );

  _hoverFlag = inside;
}

// =============================================================================

void Button::Draw()
{
  _printerRef.PrintFB(_transform.x,
                      _transform.y,
                      (int)TG::NameCP437::ULCORNER_1,
                      TG::Colors::White);

  _printerRef.PrintFB(_transform.x,
                      _transform.y + _transform.h,
                      (int)TG::NameCP437::DLCORNER_1,
                      TG::Colors::White);

  _printerRef.PrintFB(_transform.x + _transform.w - 1,
                      _transform.y + _transform.h,
                      (int)TG::NameCP437::DRCORNER_1,
                      TG::Colors::White);

  _printerRef.PrintFB(_transform.x + _transform.w - 1,
                      _transform.y,
                      (int)TG::NameCP437::URCORNER_1,
                      TG::Colors::White);

  for (size_t x = _transform.x + 1; x < (_transform.x + _transform.w - 1); x++)
  {
    _printerRef.PrintFB(x,
                        _transform.y,
                        (int)TG::NameCP437::HBAR_1,
                        TG::Colors::White);

    _printerRef.PrintFB(x,
                        _transform.y + _transform.h,
                        (int)TG::NameCP437::HBAR_1,
                        TG::Colors::White);
  }

  for (size_t y = _transform.y + 1; y < (_transform.y + _transform.h); y++)
  {
    _printerRef.PrintFB(_transform.x,
                        y,
                        (int)TG::NameCP437::VBAR_1,
                        TG::Colors::White);

    _printerRef.PrintFB(_transform.x + _transform.w - 1,
                        y,
                        (int)TG::NameCP437::VBAR_1,
                        TG::Colors::White);
  }

  /*
  if (_hoverFlag)
  {
    _printerRef.DrawFilledRect(_pixelPerfect.x,
                               _pixelPerfect.y,
                               _pixelPerfect.w - _pixelPerfect.x,
                               _pixelPerfect.h - _pixelPerfect.y,
                               TG::Colors::ShadesOfGrey::Eight);
  }
  */

  if (_mdownFlag)
  {
    _printerRef.DrawFilledRect(_pixelPerfect.x,
                               _pixelPerfect.y,
                               _pixelPerfect.w - _pixelPerfect.x,
                               _pixelPerfect.h - _pixelPerfect.y,
                               TG::Colors::White);
  }

  _printerRef.PrintFB(_transform.x + 1,
                      _transform.y + 1,
                      _text,
                      TG::Printer::kAlignLeft,
                      _fgColor,
                      _bgColor);
}
