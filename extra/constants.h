#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdint>
#include <cstddef>
#include <unordered_map>

using PairUU = std::pair<uint64_t, uint64_t>;

namespace Constants
{
  extern const size_t GlobalScale;

  extern const size_t CharWidth;
  extern const size_t CharHeight;

  extern const size_t WindowCharWidth;
  extern const size_t WindowCharHeight;

  extern const size_t WindowCharWidthVisible;
  extern const size_t WindowCharHeightVisible;

  extern const size_t WindowWidth;
  extern const size_t WindowHeight;

} // namespace Constants

#endif // CONSTANTS_H
