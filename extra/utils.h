#ifndef UTILS_H
#define UTILS_H

#include <SDL2/SDL.h>

#include "structs.h"

namespace Utils
{
  void GetMouseInfo(const SDL_MouseButtonEvent& in, MouseInfo& out);

  template <typename T>
  T Clamp(const T v, const T min, const T max)
  {
    return std::max(min, std::min(v, max));
  }
} // namespace Utils

#endif // UTILS_H
