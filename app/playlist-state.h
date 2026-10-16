#ifndef PLAYLISTSTATE_H
#define PLAYLISTSTATE_H

#include "state-base.h"

class PlaylistState : public AppStateBase
{
  public:
    using AppStateBase::AppStateBase;

    void Init() override;

    void HandleEvent(const SDL_Event& evt) override;
    void DrawStuff(TG::Printer* const printer) override;

  private:
#ifdef DEBUG_BUILD
    void DrawFPS(TG::Printer* const printer);
    void DrawWindowSize(TG::Printer* const printer);
    void DrawMouseInfo(TG::Printer* const printer);
#endif

    void DrawPlaylist(TG::Printer* const printer, const size_t offset);
    void RefreshList();

    int _cursorPosY = -1;
    size_t _playlistOffset = 0;

    std::vector<PlaylistItem> _playlist;

    std::string _droppedFile;
};

#endif // PLAYLISTSTATE_H
