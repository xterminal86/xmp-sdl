#ifndef STATEBASE_H
#define STATEBASE_H

#include <SDL2/SDL.h>

#include "structs.h"

namespace TG {
class Printer;
};

class SoundPlayer;

class AppStateBase
{
  public:
    AppStateBase(const AppSettings& appSettings,
                 TG::Printer& printerRef,
                 SoundPlayer& soundPlayerRef);
    virtual ~AppStateBase() = default;

    virtual void Init();

    virtual void HandleEvent(const SDL_Event& evt) = 0;
    virtual void DrawStuff() = 0;

    void SetDrawTime(const uint64_t dt);

  protected:
    const AppSettings& _appSettingsRef;

    TG::Printer& _printerRef;
    SoundPlayer& _soundPlayerRef;

    int32_t _mouseX = 0;
    int32_t _mouseY = 0;

    MouseInfo _mouseClickData;

    uint64_t _drawTime = 0;
};

#endif // STATEBASE_H
