#include "state-base.h"

#include "printer.h"
#include "sound-player.h"

AppStateBase::AppStateBase(const AppSettings& appSettings,
                           TG::Printer& printerRef,
                           SoundPlayer& soundPlayerRef)
  : _appSettingsRef(appSettings),
    _printerRef(printerRef),
    _soundPlayerRef(soundPlayerRef)
{}

// =============================================================================

void AppStateBase::Init() {}

// =============================================================================

void AppStateBase::SetDrawTime(const uint64_t dt)
{
  _drawTime = dt;
}
