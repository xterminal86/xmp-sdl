#include "utils.h"

#include <cerrno>
#include <cstring>

#include <SDL2/SDL.h>

namespace Utils {

bool GetModuleInfo(const std::string& filename, PlaylistItem& out)
{
  xmp_test_info ti;
  int ec = xmp_test_module(filename.data(), &ti);
  switch (ec)
  {
    case 0:
    {
      out.URI       = filename;
      out.SongTitle = ti.name;
      out.SongType  = ti.type;
      return true;
    }
    break;

    case -XMP_ERROR_FORMAT:
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unrecognized file format!");
      break;

    case -XMP_ERROR_DEPACK:
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "File uncompression failed!");
      break;

    case -XMP_ERROR_SYSTEM:
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "System error! errno = %d ('%s')",
                   errno, std::strerror(errno));
      break;

    default:
      SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Unhandled error code %d", ec);
      break;
  }

  return false;
}

} // namespace Utils
