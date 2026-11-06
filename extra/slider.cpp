#include "slider.h"

#include <format>

Slider::Slider(int px,
               int py,
               size_t barSize,
               size_t values,
               bool vertical,
               TG::Printer& printerRef)
  : GUIControl(printerRef),
    _px(px),
    _py(py),
    _barSize(barSize),
    _values(values),
    _isVertical(vertical)
{
  //
  // Fool's guard.
  //
  if (_barSize == 0)
  {
    _barSize = 1;
  }

  _valueCounter = 0;

  // TODO: unify sizes and draw the same rotated glyphs (LARROW_1 and RARROW_1)
  // for vertical variant.

  if (_isVertical)
  {
    _transform.x = px;
    _transform.y = py;
    _transform.h = _barSize;

    _btnLeft = std::make_unique<Button>(_transform.x,
                                        _transform.y  - Constants::CharWidth * 2 - 50,
                                        TG::NameCP437::UARROW_1,
                                        0,
                                        printerRef);

    _btnRight = std::make_unique<Button>(_transform.x,
                                         _transform.y  + _transform.h + 50,
                                         TG::NameCP437::DARROW_1,
                                         0,
                                         printerRef);

    _transform.w = _btnLeft->TransformPP().w;

    _pixelPerfect.x = px;
    _pixelPerfect.y = py;
    _pixelPerfect.w = _btnLeft->TransformPP().w;
    _pixelPerfect.h = _barSize;
  }
  else
  {
    _transform.x = px;
    _transform.y = py;
    _transform.w = _barSize;

    _btnLeft = std::make_unique<Button>(_transform.x - Constants::CharWidth * 2 - 50,
                                        _transform.y,
                                        TG::NameCP437::LARROW_1,
                                        0,
                                        printerRef);

    _btnRight = std::make_unique<Button>(_transform.x + _transform.w + 50,
                                         _transform.y,
                                         TG::NameCP437::RARROW_1,
                                         0,
                                         printerRef);

    _transform.h = _btnLeft->TransformPP().h;

    _pixelPerfect.x = px;
    _pixelPerfect.y = py;
    _pixelPerfect.w = _barSize;
    _pixelPerfect.h = _btnLeft->TransformPP().h;
  }

  if (_values != 0)
  {
    if (_barSize > _values)
    {
      _scrollAmount = 1;

      _scrollStep     = _barSize / _values;
      _scrollLeftover = _barSize % _values;
    }
    else
    {
      _scrollAmount = _values / _barSize;

      _scrollStep     = 1;
      _scrollLeftover = 0;
    }

    SDL_Log("scroll amount = %lu scroll step = %lu, scroll leftover = %lu",
            _scrollAmount, _scrollStep, _scrollLeftover);
  }

  _btnLeft->OnClick = [this]()
  {
    _valueCounter -= (int)_scrollAmount;

    if (_valueCounter < 0)
    {
      _valueCounter = 0;
    }
  };

  _btnRight->OnClick = [this]()
  {
    _valueCounter += (int)_scrollAmount;

    if (_valueCounter >= (int)_values)
    {
      _valueCounter = _values;
    }
  };

  _markerTransform.x = _transform.x;
  _markerTransform.y = _transform.y;
  _markerTransform.w = _markerSize;
  _markerTransform.h = _markerSize;
}

// =============================================================================

void Slider::Draw()
{
  _printerRef.DrawFilledRect(_transform.x,
                             _transform.y,
                             _transform.w,
                             _transform.h,
                             TG::Colors::White);

  _printerRef.DrawFilledRect(_transform.x + 2,
                             _transform.y + 2,
                             _transform.w - 4,
                             _transform.h - 4,
                             (_values == 0) ? TG::Colors::Black :
                             _hoverFlag ? kHoverColor : TG::Colors::Black);

  size_t markerPosX = (
    _isVertical ?
    _markerTransform.x :
    _markerTransform.x + (_scrollStep * _valueCounter)
  );

  size_t markerPosY = (
    _isVertical ?
    _markerTransform.y + (_scrollStep * _valueCounter) :
    _markerTransform.y
  );

  _printerRef.DrawFilledRect(
    markerPosX,
    markerPosY,
    _markerSize,
    _markerSize,
    TG::Colors::White
  );

  _btnLeft->Draw();
  _btnRight->Draw();

  // DEBUG:
  _printerRef.PrintFB(_transform.x / Constants::CharWidth,
                      _transform.y / Constants::CharHeight,
                      std::format("{}", _ratio),
                      TG::Printer::kAlignLeft,
                      TG::Colors::Yellow);
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

  _btnLeft->MouseDown(mi);
  _btnRight->MouseDown(mi);

  _markerGrabbed = InsideMarker(mi);

  if (not _markerGrabbed and _mdownFlag)
  {
    ClampMarker(mi);

    _markerGrabbed = true;
  }
}

// =============================================================================

void Slider::MouseUp(const MouseInfo& mi)
{
  GUIControl::MouseUp(mi);

  _btnLeft->MouseUp(mi);
  _btnRight->MouseUp(mi);

  if (_markerGrabbed)
  {
    _markerGrabbed = false;
    AlignMarker();
  }
}

// =============================================================================

void Slider::MouseOver(const MouseInfo& mi)
{
  GUIControl::MouseOver(mi);

  _btnLeft->MouseOver(mi);
  _btnRight->MouseOver(mi);

  if (_markerGrabbed)
  {
    ClampMarker(mi);
  }
}

// =============================================================================

void Slider::ClampMarker(const MouseInfo& mi)
{
  if (_isVertical)
  {
    _markerTransform.y = mi.Y;

    if (_markerTransform.y < _transform.y)
    {
      _markerTransform.y = _transform.y;
    }

    if (_markerTransform.y > _transform.y + _transform.h)
    {
      _markerTransform.y = _transform.y + _transform.h;
    }

    _ratio = ((double)_markerTransform.y / (double)(_transform.y + _transform.h));
  }
  else
  {
    _markerTransform.x = mi.X;

    if (_markerTransform.x < _transform.x)
    {
      _markerTransform.x = _transform.x;
    }

    if (_markerTransform.x > _transform.x + _transform.w)
    {
      _markerTransform.x = _transform.x + _transform.w;
    }

    _ratio = ((double)_markerTransform.x / (double)(_transform.x + _transform.w));
  }
}

// =============================================================================

void Slider::AlignMarker()
{
  if (_isVertical)
  {
    if (_scrollStep != 1)
    {
      size_t rem = _markerTransform.y % _scrollStep;
      if (rem != 0)
      {
        _markerTransform.y -= rem;
      }
    }
  }
  else
  {
    if (_scrollStep != 1)
    {
      size_t rem = _markerTransform.x % _scrollStep;
      if (rem != 0)
      {
        _markerTransform.x -= rem;
      }
    }
  }
}


