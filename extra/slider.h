#ifndef SLIDER_H
#define SLIDER_H

#include <memory>

#include "gui-control.h"
#include "printer.h"
#include "button.h"

#include "structs.h"
#include "constants.h"

class Slider : public GUIControl
{
  public:
    Slider(int px,
           int py,
           size_t barSize,
           size_t values,
           bool vertical,
           TG::Printer& printerRef);

    void Draw() override;

    void MouseDown(const MouseInfo& mi);
    void MouseUp(const MouseInfo& mi);
    void MouseOver(const MouseInfo& mi);

    int GetValue();

  private:
    bool InsideMarker(const MouseInfo& mi);

    void MoveMarker(const MouseInfo& mi);
    void ClampMarker();

    void AlignMarker();
    void UpdateMarkerPos();
    void UpdateValueCounter();

    int _px;
    int _py;

    size_t _barSize;
    size_t _values;

    size_t _zeroMarkerPosX;
    size_t _zeroMarkerPosY;

    int _valueCounter;

    size_t _scrollAmount = 1;
    size_t _scrollStep = 1;
    size_t _scrollLeftover = 0;

    bool _isVertical;

    SDL_Rect _markerTransform;

    std::unique_ptr<Button> _btnLeft;
    std::unique_ptr<Button> _btnRight;

    const size_t _markerSize = 32;

    bool _markerGrabbed = false;

    double _ratio = 0.0;
};

#endif // SLIDER_H
