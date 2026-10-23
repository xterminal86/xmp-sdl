#ifndef STRUCTS_H
#define STRUCTS_H

#include <cstdint>
#include <string>

#include <SDL2/SDL.h>

#include <xmp.h>

struct PlaylistItem
{
  std::string URI;
  std::string Filename;
  std::string SongTitle;
  std::string SongType;

  xmp_module_info ModuleInfo;

  std::string ToString()
  {
    return Filename;
  }
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

struct AppSettings
{
  SDL_bool AlwaysOnTop = SDL_FALSE;
};

#endif // STRUCTS_H
