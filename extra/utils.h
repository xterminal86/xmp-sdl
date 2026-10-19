#ifndef UTILS_H
#define UTILS_H

#include <SDL2/SDL.h>

#include "structs.h"

namespace Utils
{
  void GetMouseInfo(const SDL_MouseButtonEvent& in, MouseInfo& out);

} // namespace Utils

#endif // UTILS_H
