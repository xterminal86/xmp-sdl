#include "button.h"

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

  _printerRef.PrintFB(_transform.x + 1,
                      _transform.y + 1,
                      _text,
                      TG::Printer::kAlignLeft,
                      TG::Colors::White);
}
