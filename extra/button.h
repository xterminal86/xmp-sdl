#ifndef BUTTON_H
#define BUTTON_H

#include "printer.h"

#include "structs.h"
#include "constants.h"

class Button
{
  public:
    Button(size_t charPosX,
           size_t charPosY,
           const std::string& text,
           TG::Printer& printerRef);

    Button(int px,
           int py,
           TG::NameCP437 glyph,
           TG::Printer& printerRef);

    void Draw();
    void MouseDown(const MouseInfo& mi);
    void MouseUp(const MouseInfo& mi);
    void MouseOver(const MouseInfo& mi);

    Callback OnClick;

  private:
    bool IsMouseInside(const MouseInfo& mi);

    TG::Printer& _printerRef;

    SDL_Rect _transform;
    SDL_Rect _pixelPerfect;

    std::string _text;

    TG::NameCP437 _glyph = TG::NameCP437::FIRST;

    uint32_t _fgColor = TG::Colors::White;
    uint32_t _bgColor = TG::Colors::Black;

    bool _hoverFlag = false;
    bool _mdownFlag = false;

    const size_t kButtonBorderSize = 2;
    const size_t kBorderSizeDoubled = kButtonBorderSize * 2;
};

#endif // BUTTON_H
