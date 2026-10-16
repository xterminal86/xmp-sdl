#include "constants.h"

namespace Constants
{
  const size_t GlobalScale = 2;

  //
  // In multiples of chars.
  //
  const size_t CharWidth  =  8 * GlobalScale;
  const size_t CharHeight = 16 * GlobalScale;

  const size_t WindowCharWidth  = 80;
  const size_t WindowCharHeight = 20;

  const size_t WindowCharWidthVisible  = WindowCharWidth  - 1;
  const size_t WindowCharHeightVisible = WindowCharHeight - 1;

  const size_t WindowWidth  = WindowCharWidth  * CharWidth;
  const size_t WindowHeight = WindowCharHeight * CharHeight;

} // namespace Constants
