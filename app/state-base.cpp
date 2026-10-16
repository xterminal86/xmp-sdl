#include "state-base.h"

AppStateBase::AppStateBase(const AppSettings& appSettings)
  : _appSettingsRef(appSettings) {}

// =============================================================================

void AppStateBase::Init() {}

// =============================================================================

void AppStateBase::SetDrawTime(const uint64_t dt)
{
  _drawTime = dt;
}
