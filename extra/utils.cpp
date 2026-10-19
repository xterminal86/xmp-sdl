#include "utils.h"

#include "constants.h"

#include <cerrno>
#include <cstring>

namespace Utils {

void GetMouseInfo(const SDL_MouseButtonEvent& in, MouseInfo& out)
{
  switch (in.button)
  {
    case SDL_BUTTON_LEFT:
      out.MouseButton = MouseInfo::Button::LEFT;
      break;

    case SDL_BUTTON_RIGHT:
      out.MouseButton = MouseInfo::Button::RIGHT;
      break;

    case SDL_BUTTON_MIDDLE:
      out.MouseButton = MouseInfo::Button::MIDDLE;
      break;

    default:
      break;
  }

  out.WasPressed = (in.state == SDL_PRESSED);

  out.X = in.x;
  out.Y = in.y;

  out.CharPosX = in.x / Constants::CharWidth;
  out.CharPosY = in.y / Constants::CharHeight;
}

} // namespace Utils
