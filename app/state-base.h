#ifndef STATEBASE_H
#define STATEBASE_H

#include <SDL2/SDL.h>

#include "printer.h"
#include "structs.h"

class AppStateBase
{
  public:
    explicit AppStateBase(const AppSettings& appSettings);
    virtual ~AppStateBase() = default;

    virtual void Init();

    virtual void HandleEvent(const SDL_Event& evt) = 0;
    virtual void DrawStuff(TG::Printer* const printerRef) = 0;

    void SetDrawTime(const uint64_t dt);

  protected:
    const AppSettings& _appSettingsRef;

    int32_t _mouseX = 0;
    int32_t _mouseY = 0;

    MouseInfo _mouseClickData;

    uint64_t _drawTime = 0;
};

#endif // STATEBASE_H
