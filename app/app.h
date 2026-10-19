#ifndef APP_H
#define APP_H

#include <unordered_map>
#include <memory>

#include "printer.h"
#include "sound-player.h"

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

    SDL_Event _event;

    std::unordered_map<AppState, std::unique_ptr<AppStateBase>> _appStates;

    TG::Printer _printer;
    SoundPlayer _soundPlayer;

    AppSettings _appSettings;
};

#endif // APP_H
