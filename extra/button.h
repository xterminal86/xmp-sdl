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

    void Draw();
    void MouseDown(const MouseInfo& mi);
    void MouseUp(const MouseInfo& mi);
    void MouseOver(const MouseInfo& mi);

    Callback OnClick;

  private:
    TG::Printer& _printerRef;

    SDL_Rect _transform;
    SDL_Rect _pixelPerfect;

    std::string _text;

    uint32_t _fgColor = TG::Colors::White;
    uint32_t _bgColor = TG::Colors::Black;

    bool _hoverFlag = false;
    bool _mdownFlag = false;
};

#endif // BUTTON_H
