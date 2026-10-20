#ifndef PLAYLISTSTATE_H
#define PLAYLISTSTATE_H

#include <vector>
#include <memory>

#include "constants.h"
#include "state-base.h"

#include "list-box.h"
#include "button.h"

class PlaylistState : public AppStateBase
{
  public:
    using AppStateBase::AppStateBase;

    void Init() override;

    void HandleEvent(const SDL_Event& evt) override;
    void DrawStuff() override;

  private:
    void DrawPlaylist();
    void UpdatePlaylist();
    void TryToPlay();

#ifdef DEBUG_BUILD
    void DrawDimensions();
    void DrawFPS();
    void DrawWindowSize();
    void DrawMouseInfo();
#endif

    std::vector<PlaylistItem> _playlist;

    std::string _droppedFile;

    std::unique_ptr<ListBox<PlaylistItem>> _listBox;

    std::vector<std::unique_ptr<Button>> _buttons;

    size_t _listItemWidth = 0;

    std::string _itemBackground;
};

#endif // PLAYLISTSTATE_H
