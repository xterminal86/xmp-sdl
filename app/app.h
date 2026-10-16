#ifndef APP_H
#define APP_H

#include <unordered_map>
#include <memory>

#include <xmp.h>

#include "printer.h"

#include "structs.h"
#include "playlist-state.h"

class Application
{
  public:
    enum class AppState
    {
        UNDEFINED = 0
      , PLAYLIST
    };

    Application();
    ~Application();

    bool InitSDL();
    bool InitSound();
    bool InitDrawing();

    void Run();

    void ChangeState(AppState changeTo);

  private:
    AppState _currentState  = AppState::PLAYLIST;
    AppState _stateToChange = AppState::UNDEFINED;

    SDL_Renderer* _renderer = nullptr;
    SDL_Window* _window = nullptr;

    SDL_AudioDeviceID _audioDevice;

    SDL_Event _event;

    xmp_context _xmpContext;

    std::unordered_map<AppState, std::unique_ptr<AppStateBase>> _appStates;

    TG::Printer _printer;

    bool _stopPlaying = false;

    AppSettings _appSettings;
};

#endif // APP_H
