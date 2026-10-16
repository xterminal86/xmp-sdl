// *****************************************************************************
//
// Build from host system's terminal if Qt Creator from flatpak misbehaves.
//
// *****************************************************************************

#include "app.h"

int main()
{
  Application app;

  bool initOk = app.InitSDL() and app.InitSound() and app.InitDrawing();
  if (not initOk)
  {
    return 1;
  }

  app.Run();

  return 0;
}

