#include "slider.h"

#include <format>

Slider::Slider(int px,
               int py,
               size_t barSize,
               size_t values,
               bool vertical,
               TG::Printer& printerRef,
               bool drawArrows)
  : GUIControl(printerRef),
    _px(px),
    _py(py),
    _barSize(barSize),
    _values(values),
    _isVertical(vertical),
    _drawArrows(drawArrows)
{
  //
  // Fool's guard.
  //
  if (_barSize == 0)
  {
    _barSize = 1;
  }

  _valueCounter = 0;

  if (_isVertical)
  {
    _transform.x = px;
    _transform.y = py;
    _transform.h = _barSize;

    _btnLeft = std::make_unique<Button>(_transform.x,
                                        _transform.y  - _markerSize,
                                        TG::NameCP437::UARROW_1,
                                        0,
                                        printerRef);

    _btnRight = std::make_unique<Button>(_transform.x,
                                         _transform.y  + _transform.h + _markerSize,
                                         TG::NameCP437::DARROW_1,
                                         0,
                                         printerRef);

    _transform.w = _btnLeft->TransformPP().w;

    _pixelPerfect.x = px;
    _pixelPerfect.y = py;
    _pixelPerfect.w = _btnLeft->TransformPP().w;
    _pixelPerfect.h = _barSize + _markerSize;
  }
  else
  {
    _transform.x = px;
    _transform.y = py;
    _transform.w = _barSize;

    _btnLeft = std::make_unique<Button>(_transform.x - _markerSize,
                                        _transform.y,
                                        TG::NameCP437::UARROW_1,
                                        270,
                                        printerRef);

    _btnRight = std::make_unique<Button>(_transform.x + _transform.w + _markerSize,
                                         _transform.y,
                                         TG::NameCP437::UARROW_1,
                                         90,
                                         printerRef);

    _transform.h = _btnLeft->TransformPP().h;

    _pixelPerfect.x = px;
    _pixelPerfect.y = py;
    _pixelPerfect.w = _barSize + _markerSize;
    _pixelPerfect.h = _btnLeft->TransformPP().h;
  }

  if (_values != 0)
  {
    _scrollStep = (_barSize > _values) ? (_barSize / _values) : 1;
  }
  else
  {
    _scrollStep = 0;
  }

  SDL_Log("step = %lu", _scrollStep);

  _btnLeft->OnClick = [this]()
  {
    _valueCounter--;

    if (_valueCounter < 0)
    {
      _valueCounter = 0;
    }

    _ratio = (_values != 0) ? ((double)_valueCounter / (double)_values) : 0;

    UpdateMarkerPos();
  };

  _btnRight->OnClick = [this]()
  {
    _valueCounter++;

    if (_valueCounter >= (int)_values)
    {
      _valueCounter = _values;
    }

    _ratio = (_values != 0) ? ((double)_valueCounter / (double)_values) : 0;

    UpdateMarkerPos();
  };

  _markerTransform.x = _transform.x;
  _markerTransform.y = _transform.y;
  _markerTransform.w = _markerSize;
  _markerTransform.h = _markerSize;

  _zeroMarkerPosX = _markerTransform.x;
  _zeroMarkerPosY = _markerTransform.y;
}

// =============================================================================

void Slider::Draw()
{
  _printerRef.DrawFilledRect(_transform.x,
                             _transform.y,
                             (not _isVertical ?
                              (_transform.w + _markerSize) :
                              _transform.w),
                             (_isVertical ?
                              (_transform.h + _markerSize) :
                              _transform.h),
                             TG::Colors::White);

  _printerRef.DrawFilledRect(_transform.x + 2,
                             _transform.y + 2,
                             (not _isVertical ?
                              (_transform.w - 4 + _markerSize) :
                              _transform.w - 4),
                             (_isVertical ?
                              (_transform.h - 4 + _markerSize) :
                              _transform.h - 4),
                             (_values == 0) ? TG::Colors::Black :
                             _hoverFlag ? kHoverColor : TG::Colors::Black);

  if (_values != 0)
  {
    _printerRef.DrawFilledRect(
      _markerTransform.x,
      _markerTransform.y,
      _markerTransform.w,
      _markerTransform.h,
      TG::Colors::White
    );
  }

  if (_drawArrows)
  {
    _btnLeft->Draw();
    _btnRight->Draw();
  }

  //
  // DEBUG:
  //
  if (_isVertical)
  {
    _printerRef.PrintFB(
        _transform.x / Constants::CharWidth + 3
      , _transform.y / Constants::CharHeight + 1
      , std::format("{} ({:.2f})", _valueCounter, _ratio)
      , TG::Printer::kAlignLeft
      , TG::Colors::Yellow
    );
  }
  else
  {
    _printerRef.PrintFB(
        _transform.x / Constants::CharWidth + 2
      , _transform.y / Constants::CharHeight - 1
      , std::format("{} ({:.2f})", _valueCounter, _ratio)
      , TG::Printer::kAlignLeft
      , TG::Colors::Yellow
    );
  }
}

// =============================================================================

bool Slider::InsideMarker(const MouseInfo& mi)
{
  return (mi.X > _markerTransform.x
      and mi.X < _markerTransform.x + _markerTransform.w
      and mi.Y > _markerTransform.y
      and mi.Y < _markerTransform.y + _markerTransform.h
  );
}

// =============================================================================

void Slider::MouseDown(const MouseInfo& mi)
{
  GUIControl::MouseDown(mi);

  if (_drawArrows)
  {
    _btnLeft->MouseDown(mi);
    _btnRight->MouseDown(mi);
  }

  _markerGrabbed = InsideMarker(mi);

  if (_values != 0 and not _markerGrabbed and _mdownFlag)
  {
    MoveMarker(mi);

    _markerGrabbed = true;
  }
}

// =============================================================================

void Slider::MouseUp(const MouseInfo& mi)
{
  GUIControl::MouseUp(mi);

  if (_drawArrows)
  {
    _btnLeft->MouseUp(mi);
    _btnRight->MouseUp(mi);
  }

  if (_values != 0 and _markerGrabbed)
  {
    _markerGrabbed = false;
    AlignMarker();
    UpdateValueCounter();
  }
}

// =============================================================================

void Slider::MouseOver(const MouseInfo& mi)
{
  GUIControl::MouseOver(mi);

  if (_drawArrows)
  {
    _btnLeft->MouseOver(mi);
    _btnRight->MouseOver(mi);
  }

  if (_values != 0 and _markerGrabbed)
  {
    MoveMarker(mi);
  }
}

// =============================================================================

int Slider::GetValue()
{
  return _valueCounter;
}

// =============================================================================

void Slider::MoveMarker(const MouseInfo& mi)
{
  if (_isVertical)
  {
    _markerTransform.y = mi.Y - _markerSizeHalf;
  }
  else
  {
    _markerTransform.x = mi.X - _markerSizeHalf;
  }

  ClampMarker();
  UpdateValueCounter();
}

// =============================================================================

void Slider::ClampMarker()
{
  if (_isVertical)
  {
    if (_markerTransform.y < _transform.y)
    {
      _markerTransform.y = _transform.y;
    }

    if (_markerTransform.y > _transform.y + _transform.h)
    {
      _markerTransform.y = _transform.y + _transform.h;
    }
  }
  else
  {
    if (_markerTransform.x < _transform.x)
    {
      _markerTransform.x = _transform.x;
    }

    if (_markerTransform.x > _transform.x + _transform.w)
    {
      _markerTransform.x = _transform.x + _transform.w;
    }
  }
}

// =============================================================================

void Slider::AlignMarker()
{
  // FIXME: fix snapping for edge cases and when _scrollStep != 1
  if (_isVertical)
  {
    if (_scrollStep != 1)
    {
      size_t rem = _markerTransform.y % _scrollStep;
      _markerTransform.y -= rem;
    }
  }
  else
  {
    if (_scrollStep != 1)
    {
      size_t rem = _markerTransform.x % _scrollStep;
      _markerTransform.x -= rem;
    }
  }

  ClampMarker();
}

// =============================================================================

void Slider::UpdateMarkerPos()
{
  if (_isVertical)
  {
    _markerTransform.y = _zeroMarkerPosY + (int)(_ratio * (double)_transform.h);
  }
  else
  {
    _markerTransform.x = _zeroMarkerPosX + (int)(_ratio * (double)_transform.w);
  }
}

// =============================================================================

void Slider::UpdateValueCounter()
{
  if (_isVertical)
  {
    _ratio =
      (double)(_markerTransform.y - _transform.y) / (double)(_transform.h);
  }
  else
  {
    _ratio =
      (double)(_markerTransform.x - _transform.x) / (double)(_transform.w);
  }

  _valueCounter = (int)(_ratio * (double)_values);

  _ratio = (double)_valueCounter / (double)_values;
}
