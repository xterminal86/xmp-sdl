#ifndef GUICONTROL_H
#define GUICONTROL_H

#include "printer.h"
#include "structs.h"
#include "constants.h"

class GUIControl
{
  public:
    GUIControl(TG::Printer& printerRef);
    virtual ~GUIControl();

    virtual void Draw() = 0;

    void MouseDown(const MouseInfo& mi);
    void MouseUp(const MouseInfo& mi);
    void MouseOver(const MouseInfo& mi);

    Callback OnClick;

    void Move(int px, int py);

    const SDL_Rect& Transform();
    const SDL_Rect& TransformPP();

  protected:
    bool IsMouseInside(const MouseInfo& mi);

    TG::Printer& _printerRef;

    SDL_Rect _transform;
    SDL_Rect _pixelPerfect;

    uint32_t _fgColor = TG::Colors::White;
    uint32_t _bgColor = TG::Colors::Black;

    bool _hoverFlag = false;
    bool _mdownFlag = false;

    const uint32_t kHoverColor = TG::Colors::ShadesOfGrey::Five;
};

#endif // GUICONTROL_H
