#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <string>
#include <vector>

#include <SDL2/SDL.h>

#include <xmp.h>

#include "structs.h"

extern std::vector<uint8_t> SpectrogramSnapshot;

class SoundPlayer
{
  public:
    ~SoundPlayer();

    bool Init();

    bool Play(const std::string& uri);
    void Stop();

    xmp_module_info* GetCurrentModuleInfo();

  private:
    bool TestModule(const std::string& fname);
    bool LoadModule(const std::string& fname);

    void SetParam(const std::string& paramStr, int param, int value);

    SDL_AudioDeviceID _audioDevice;

    xmp_context _xmpContext;
    xmp_module_info _moduleInfo;

    bool _isPlaying = false;
};

#endif // SOUNDPLAYER_H
