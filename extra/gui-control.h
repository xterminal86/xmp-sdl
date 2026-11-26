#ifndef GUICONTROL_H
#define GUICONTROL_H

#include "printer.h"
#include "structs.h"
#include "constants.h"

class GUIControl
{
  public:
    enum class GlyphRotation
    {
        A_0   = 0
      , A_90  = 90
      , A_180 = 180
      , A_270 = 270
    };

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

    virtual void SetEnabled(bool value);

    bool IsEnabled();

  protected:
    bool IsMouseInside(const MouseInfo& mi);

    Callback _onMouseDown;
    Callback _onMouseUp;

    TG::Printer& _printerRef;

    SDL_Rect _transform;
    SDL_Rect _pixelPerfect;

    uint32_t _fgColor = TG::Colors::White;
    uint32_t _bgColor = TG::Colors::Black;

    bool _hoverFlag = false;
    bool _mdownFlag = false;

    const uint32_t kHoverColor = TG::Colors::ShadesOfGrey::Five;

    bool _enabled = true;
};

#endif // GUICONTROL_H
