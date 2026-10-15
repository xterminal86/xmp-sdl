#ifndef UTILS_H
#define UTILS_H

#include <SDL2/SDL.h>

#include <xmp.h>

#include "structs.h"

namespace Utils
{
  bool GetModuleInfo(const std::string& filename,
                     PlaylistItem& out);

  void GetMouseInfo(const SDL_MouseButtonEvent& in, MouseInfo& out);

} // namespace Utils

#endif // UTILS_H
