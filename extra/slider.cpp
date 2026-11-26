#include "slider.h"
#include "utils.h"

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

  _enabled = (_values > 1);

  _valueCounter = 0;

  if (_isVertical)
  {
    _transform.x = px;
    _transform.y = py;
    _transform.h = _barSize;

    _btnLeft = std::make_unique<Button>(_transform.x,
                                        _transform.y  - _markerSize,
                                        TG::NameCP437::UARROW_1,
                                        GlyphRotation::A_0,
                                        printerRef);

    _btnRight = std::make_unique<Button>(_transform.x,
                                         _transform.y  + _transform.h + _markerSize,
                                         TG::NameCP437::DARROW_1,
                                         GlyphRotation::A_0,
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
                                        GlyphRotation::A_270,
                                        printerRef);

    _btnRight = std::make_unique<Button>(_transform.x + _transform.w + _markerSize,
                                         _transform.y,
                                         TG::NameCP437::UARROW_1,
                                         GlyphRotation::A_90,
                                         printerRef);

    _transform.h = _btnLeft->TransformPP().h;

    _pixelPerfect.x = px;
    _pixelPerfect.y = py;
    _pixelPerfect.w = _barSize + _markerSize;
    _pixelPerfect.h = _btnLeft->TransformPP().h;
  }

  // ---------------------------------------------------------------------------
  _btnLeft->OnClick = [this]()
  {
    if (not _enabled)
    {
      return;
    }

    _valueCounter--;

    if (_valueCounter < 0)
    {
      _valueCounter = 0;
    }

    _ratio = (double)_valueCounter / (double)(_values - 1);

    UpdateMarkerPos();
  };

  _btnRight->OnClick = [this]()
  {
    if (not _enabled)
    {
      return;
    }

    _valueCounter++;

    if (_valueCounter > (int)(_values - 1))
    {
      _valueCounter = _values - 1;
    }

    _ratio = (double)_valueCounter / (double)(_values - 1);

    UpdateMarkerPos();
  };
  // ---------------------------------------------------------------------------

  if (_values == 1)
  {
    _markerTransform.x = _isVertical ? _transform.x : (_transform.x + _barSize / 2);
    _markerTransform.y = _isVertical ? (_transform.y + _barSize / 2) : _transform.y;
    _markerTransform.w = _markerSize;
    _markerTransform.h = _markerSize;
  }
  else
  {
    _markerTransform.x = _transform.x;
    _markerTransform.y = _transform.y;
    _markerTransform.w = _markerSize;
    _markerTransform.h = _markerSize;
  }

  _zeroMarkerPosX = _markerTransform.x;
  _zeroMarkerPosY = _markerTransform.y;

  SetEnabled(_enabled);
}

// =============================================================================

void Slider::Draw()
{
  if (not _enabled)
  {
    _printerRef.DrawFilledRect(_transform.x,
                               _transform.y,
                               (not _isVertical ?
                                (_transform.w + _markerSize) :
                                _transform.w),
                               (_isVertical ?
                                (_transform.h + _markerSize) :
                                _transform.h),
                               Constants::GUIDisabledColor);

    _printerRef.DrawFilledRect(_transform.x + 2,
                               _transform.y + 2,
                               (not _isVertical ?
                                (_transform.w - 4 + _markerSize) :
                                _transform.w - 4),
                               (_isVertical ?
                                (_transform.h - 4 + _markerSize) :
                                _transform.h - 4),
                               TG::Colors::Black);
    if (_drawArrows)
    {
      _btnLeft->Draw();
      _btnRight->Draw();
    }

    return;
  }

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

#ifdef DEBUG_BUILD
  // ---------------------------------------------------------------------------
  if (_isVertical)
  {
    _printerRef.PrintFB(
        _transform.x / Constants::CharWidth + 3
      , _transform.y / Constants::CharHeight + 1
      , std::format("{} ({:.2f})", GetValue(), _ratio)
      , TG::Printer::kAlignLeft
      , TG::Colors::Yellow
    );
  }
  else
  {
    _printerRef.PrintFB(
        _transform.x / Constants::CharWidth + 2
      , _transform.y / Constants::CharHeight - 1
      , std::format("{} ({:.2f})", GetValue(), _ratio)
      , TG::Printer::kAlignLeft
      , TG::Colors::Yellow
    );
  }
  // ---------------------------------------------------------------------------
#endif
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
  if (not _enabled)
  {
    return;
  }

  GUIControl::MouseDown(mi);

  if (_drawArrows)
  {
    _btnLeft->MouseDown(mi);
    _btnRight->MouseDown(mi);
  }

  _markerGrabbed = InsideMarker(mi);

  if (_values > 1 and not _markerGrabbed and _mdownFlag)
  {
    MoveMarker(mi);

    _markerGrabbed = true;
  }
}

// =============================================================================

void Slider::MouseUp(const MouseInfo& mi)
{
  if (not _enabled)
  {
    return;
  }

  GUIControl::MouseUp(mi);

  if (_drawArrows)
  {
    _btnLeft->MouseUp(mi);
    _btnRight->MouseUp(mi);
  }

  if (_values > 1 and _markerGrabbed)
  {
    _markerGrabbed = false;
    AlignMarker();
  }
}

// =============================================================================

void Slider::MouseOver(const MouseInfo& mi)
{
  if (not _enabled)
  {
    return;
  }

  GUIControl::MouseOver(mi);

  if (_drawArrows)
  {
    _btnLeft->MouseOver(mi);
    _btnRight->MouseOver(mi);
  }

  if (_values > 1 and _markerGrabbed)
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
  if (not _enabled)
  {
    return;
  }

  if (_isVertical)
  {
    _markerTransform.y = mi.Y - _markerSizeHalf;

    ClampMarker();

    _ratio = (double)(_markerTransform.y - _zeroMarkerPosY) / (double)_transform.h;
  }
  else
  {
    _markerTransform.x = mi.X - _markerSizeHalf;

    ClampMarker();

    _ratio = (double)(_markerTransform.x - _zeroMarkerPosX) / (double)_transform.w;
  }

  //_valueCounter = (int)(_ratio * (double)(_values - 1));

  // Seems to be working.
  _valueCounter = (int)std::round(_ratio * (double)(_values - 1));
}

// =============================================================================

void Slider::UpdateMarkerPos()
{
  if (not _enabled)
  {
    return;
  }

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

void Slider::ClampMarker()
{
  if (not _enabled)
  {
    return;
  }

  if (_isVertical)
  {
    if (_markerTransform.y < _zeroMarkerPosY)
    {
      _markerTransform.y = _zeroMarkerPosY;
    }
    else if (_markerTransform.y > _zeroMarkerPosY + _transform.h)
    {
      _markerTransform.y = _zeroMarkerPosY + _transform.h;
    }
  }
  else
  {
    if (_markerTransform.x < _zeroMarkerPosX)
    {
      _markerTransform.x = _zeroMarkerPosX;
    }
    else if (_markerTransform.x > _zeroMarkerPosX + _transform.w)
    {
      _markerTransform.x = _zeroMarkerPosX + _transform.w;
    }
  }
}

// =============================================================================

void Slider::AlignMarker()
{
  if (not _enabled)
  {
    return;
  }

  _ratio = (double)_valueCounter / (double)(_values - 1);

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

void Slider::SetEnabled(bool value)
{
  _enabled = value;
  _btnLeft->SetEnabled(_enabled);
  _btnRight->SetEnabled(_enabled);
}
