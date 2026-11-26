#ifndef BUTTONTOGGLE_H
#define BUTTONTOGGLE_H

#include "gui-control.h"
#include "printer.h"

#include "structs.h"
#include "constants.h"

class ToggleButton : public GUIControl
{
  public:
    ToggleButton(size_t charPosX,
                 size_t charPosY,
                 const std::string& text,
                 TG::Printer& printerRef,
                 bool initialOn = false);

    ToggleButton(int px,
                 int py,
                 TG::NameCP437 glyph,
                 uint16_t angle,
                 TG::Printer& printerRef,
                 bool initialOn = false);

    void Draw() override;

    bool IsToggled();

  private:
    void DrawDisabled();
    void DrawToggled();
    void DrawUntoggled();

    std::string _text;

    TG::NameCP437 _glyph = TG::NameCP437::FIRST;

    uint16_t _angle = 0;

    const size_t kButtonBorderSize = 2;
    const size_t kBorderSizeDoubled = kButtonBorderSize * 2;

    const uint32_t kActionColor       = 0x00FF00;
    const uint32_t kToggledColor      = 0x006600;
    const uint32_t kToggledHoverColor = 0x009900;

    bool _toggled = false;
};

#endif // BUTTONTOGGLE_H
