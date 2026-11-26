#include "button-toggle.h"

#include "constants.h"

ToggleButton::ToggleButton(size_t charPosX,
                           size_t charPosY,
                           const std::string& text,
                           TG::Printer& printerRef,
                           bool initialOn)
  : GUIControl(printerRef), _text(text)
{
  _transform.x = charPosX;
  _transform.y = charPosY;
  _transform.w = text.length() + 2;
  _transform.h = 2;

  _pixelPerfect.x = charPosX * Constants::CharWidth + Constants::CharWidth / 2 + 2;
  _pixelPerfect.y = charPosY * Constants::CharHeight + Constants::CharHeight / 2;
  _pixelPerfect.w = _transform.w * Constants::CharWidth - Constants::CharWidth - 4;
  _pixelPerfect.h = _transform.h * Constants::CharHeight - 2;

  _toggled = initialOn;

  _onMouseUp = [this]() { _toggled = not _toggled; };
}

// =============================================================================

ToggleButton::ToggleButton(int px,
                           int py,
                           TG::NameCP437 glyph,
                           uint16_t angle,
                           TG::Printer& printerRef,
                           bool initialOn)
  : GUIControl(printerRef), _glyph(glyph), _angle(angle)
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

  _toggled = initialOn;

  _onMouseUp = [this]() { _toggled = not _toggled; };
}

// =============================================================================

void ToggleButton::DrawDisabled()
{
  // Block fill.
  _printerRef.DrawFilledRect(
    _pixelPerfect.x,
    _pixelPerfect.y,
    _pixelPerfect.w,
    _pixelPerfect.h,
    Constants::GUIDisabledColor
  );

  _printerRef.DrawFilledRect(
    _pixelPerfect.x + kButtonBorderSize,
    _pixelPerfect.y + kButtonBorderSize,
    _pixelPerfect.w - kBorderSizeDoubled,
    _pixelPerfect.h - kBorderSizeDoubled,
    TG::Colors::Black
  );

  if (_glyph != TG::NameCP437::FIRST)
  {
    if (_angle != 0)
    {
      size_t offset = 0;

      if      (_angle == 90)  offset = 6;
      else if (_angle == 270) offset = 10;

      _printerRef.DrawGlyph(
        _transform.x + offset,
        _transform.y + 2,
        _glyph,
        Constants::GUIDisabledColor,
        TG::Colors::None,
        _angle
      );
    }
    else
    {
      _printerRef.PrintPP(
        _transform.x + Constants::CharWidthHalf,
        _transform.y + 2,
        _glyph,
        Constants::GUIDisabledColor,
        TG::Colors::None
      );
    }
  }
  else
  {
    _printerRef.PrintFB(
      _transform.x + 1,
      _transform.y + 1,
      _text,
      TG::Printer::kAlignLeft,
      Constants::GUIDisabledColor,
      TG::Colors::None
    );
  }
}

// =============================================================================

void ToggleButton::DrawToggled()
{
  // Block fill.
  _printerRef.DrawFilledRect(
    _pixelPerfect.x,
    _pixelPerfect.y,
    _pixelPerfect.w,
    _pixelPerfect.h,
    kActionColor
  );

  // Cut from block.
  if (_mdownFlag)
  {
    _printerRef.DrawFilledRect(
      _pixelPerfect.x + kButtonBorderSize,
      _pixelPerfect.y + kButtonBorderSize,
      _pixelPerfect.w - kBorderSizeDoubled,
      _pixelPerfect.h - kBorderSizeDoubled,
      kActionColor
    );
  }
  else
  {
    _printerRef.DrawFilledRect(
      _pixelPerfect.x + kButtonBorderSize,
      _pixelPerfect.y + kButtonBorderSize,
      _pixelPerfect.w - kBorderSizeDoubled,
      _pixelPerfect.h - kBorderSizeDoubled,
      (_hoverFlag ? kToggledHoverColor : kToggledColor)
    );
  }

  if (_glyph != TG::NameCP437::FIRST)
  {
    if (_angle != 0)
    {
      size_t offset = 0;

      if      (_angle == 90)  offset = 6;
      else if (_angle == 270) offset = 10;

      if (_mdownFlag)
      {
        _printerRef.DrawGlyph(
          _transform.x + offset,
          _transform.y + 2,
          _glyph,
          _fgColor,
          kActionColor,
          _angle
        );
      }
      else
      {
        _printerRef.DrawGlyph(
          _transform.x + offset,
          _transform.y + 2,
          _glyph,
          _fgColor,
          ( _hoverFlag ? kToggledHoverColor : TG::Colors::None ),
          _angle
        );
      }
    }
    else
    {
      if (_mdownFlag)
      {
        _printerRef.PrintPP(
          _transform.x + Constants::CharWidthHalf,
          _transform.y + 2,
          _glyph,
          _fgColor,
          kActionColor
        );
      }
      else
      {
        _printerRef.PrintPP(
          _transform.x + Constants::CharWidthHalf,
          _transform.y + 2,
          _glyph,
          _fgColor,
          ( _hoverFlag ? kToggledHoverColor : TG::Colors::None )
        );
      }
    }
  }
  else
  {
    if (_mdownFlag)
    {
      _printerRef.PrintFB(
        _transform.x + 1,
        _transform.y + 1,
        _text,
        TG::Printer::kAlignLeft,
        _fgColor,
        kActionColor
      );
    }
    else
    {
      _printerRef.PrintFB(
        _transform.x + 1,
        _transform.y + 1,
        _text,
        TG::Printer::kAlignLeft,
        TG::Colors::White,
        ( _hoverFlag ? kToggledHoverColor : kToggledColor )
      );
    }
  }
}

// =============================================================================

void ToggleButton::DrawUntoggled()
{
  // Block fill.
  _printerRef.DrawFilledRect(
    _pixelPerfect.x,
    _pixelPerfect.y,
    _pixelPerfect.w,
    _pixelPerfect.h,
    TG::Colors::White
  );

  // Cut from block.
  if (_mdownFlag)
  {
    _printerRef.DrawFilledRect(
      _pixelPerfect.x + kButtonBorderSize,
      _pixelPerfect.y + kButtonBorderSize,
      _pixelPerfect.w - kBorderSizeDoubled,
      _pixelPerfect.h - kBorderSizeDoubled,
      kActionColor
    );
  }
  else
  {
    _printerRef.DrawFilledRect(
      _pixelPerfect.x + kButtonBorderSize,
      _pixelPerfect.y + kButtonBorderSize,
      _pixelPerfect.w - kBorderSizeDoubled,
      _pixelPerfect.h - kBorderSizeDoubled,
      (_hoverFlag ? kToggledHoverColor : TG::Colors::Black)
    );
  }

  if (_glyph != TG::NameCP437::FIRST)
  {
    if (_angle != 0)
    {
      size_t offset = 0;

      if      (_angle == 90)  offset = 6;
      else if (_angle == 270) offset = 10;

      if (_mdownFlag)
      {
        _printerRef.DrawGlyph(
          _transform.x + offset,
          _transform.y + 2,
          _glyph,
          _fgColor,
          kActionColor,
          _angle
        );
      }
      else
      {
        _printerRef.DrawGlyph(
          _transform.x + offset,
          _transform.y + 2,
          _glyph,
          _fgColor,
          ( _hoverFlag ? kToggledHoverColor : TG::Colors::None ),
          _angle
        );
      }
    }
    else
    {
      if (_mdownFlag)
      {
        _printerRef.PrintPP(
          _transform.x + Constants::CharWidthHalf,
          _transform.y + 2,
          _glyph,
          _fgColor,
          kActionColor
        );
      }
      else
      {
        _printerRef.PrintPP(
          _transform.x + Constants::CharWidthHalf,
          _transform.y + 2,
          _glyph,
          _fgColor,
          ( _hoverFlag ? kToggledHoverColor : TG::Colors::None )
        );
      }
    }
  }
  else
  {
    if (_mdownFlag)
    {
      _printerRef.PrintFB(
        _transform.x + 1,
        _transform.y + 1,
        _text,
        TG::Printer::kAlignLeft,
        _fgColor,
        kActionColor
      );
    }
    else
    {
      _printerRef.PrintFB(
        _transform.x + 1,
        _transform.y + 1,
        _text,
        TG::Printer::kAlignLeft,
        _fgColor,
        (_hoverFlag ? kToggledHoverColor : TG::Colors::Black)
      );
    }
  }
}

// =============================================================================

void ToggleButton::Draw()
{
  if (not _enabled)
  {
    DrawDisabled();
  }
  else
  {
    if (_toggled)
    {
      DrawToggled();
    }
    else
    {
      DrawUntoggled();
    }
  }
}

// =============================================================================

bool ToggleButton::IsToggled()
{
  return _toggled;
}
