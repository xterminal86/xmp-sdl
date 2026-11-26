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
           TG::Printer& printerRef,
           bool drawArrows = true);

    void Draw() override;

    void MouseDown(const MouseInfo& mi);
    void MouseUp(const MouseInfo& mi);
    void MouseOver(const MouseInfo& mi);

    int GetValue();

    void SetEnabled(bool value) override;

  private:
    bool InsideMarker(const MouseInfo& mi);

    void MoveMarker(const MouseInfo& mi);

    void UpdateMarkerPos();

    void ClampMarker();
    void AlignMarker();

    int _px;
    int _py;

    size_t _barSize;
    size_t _values;

    int _zeroMarkerPosX;
    int _zeroMarkerPosY;

    int _valueCounter;

    bool _isVertical;
    bool _drawArrows;

    SDL_Rect _markerTransform;

    std::unique_ptr<Button> _btnLeft;
    std::unique_ptr<Button> _btnRight;

    const size_t _markerSize = 32;
    const size_t _markerSizeHalf = _markerSize / 2;

    bool _markerGrabbed = false;

    double _ratio = 0.0;
};

#endif // SLIDER_H
