#ifndef BUTTON_H
#define BUTTON_H

#include "printer.h"

class Button
{
  public:
    Button(size_t charPosX,
           size_t charPosY,
           const std::string& text,
           TG::Printer& printerRef);

    void Draw();

  private:
    TG::Printer& _printerRef;

    SDL_Rect _transform;

    std::string _text;
};

#endif // BUTTON_H
