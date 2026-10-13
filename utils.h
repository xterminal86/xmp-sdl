#ifndef UTILS_H
#define UTILS_H

#include <xmp.h>

#include "structs.h"

namespace Utils
{
  bool GetModuleInfo(const std::string& filename,
                     PlaylistItem& out);

} // namespace Utils

#endif // UTILS_H
