#ifndef BUTTON_H
#define BUTTON_H

#include "gui-control.h"
#include "printer.h"

#include "structs.h"
#include "constants.h"

class Button : public GUIControl
{
  public:
    Button(size_t charPosX,
           size_t charPosY,
           const std::string& text,
           TG::Printer& printerRef);

    Button(int px,
           int py,
           TG::NameCP437 glyph,
           uint16_t angle,
           TG::Printer& printerRef);

    void Draw() override;

  private:
    std::string _text;

    TG::NameCP437 _glyph = TG::NameCP437::FIRST;

    uint16_t _angle = 0;

    const size_t kButtonBorderSize = 2;
    const size_t kBorderSizeDoubled = kButtonBorderSize * 2;
};

#endif // BUTTON_H
