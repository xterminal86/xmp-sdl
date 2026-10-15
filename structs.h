#ifndef STRUCTS_H
#define STRUCTS_H

#include <cstdint>
#include <string>

struct PlaylistItem
{
  std::string URI;
  std::string Filename;
  std::string SongTitle;
  std::string SongType;
};

struct MouseInfo
{
  enum class Button
  {
    NONE = 0,
    LEFT,
    RIGHT,
    MIDDLE
  };

  uint32_t Timestamp = 0;

  int X = 0;
  int Y = 0;
  int CharPosX = 0;
  int CharPosY = 0;

  bool WasDoubleClick = false;
  bool WasPressed     = false;

  Button MouseButton = Button::NONE;
};

#endif // STRUCTS_H
