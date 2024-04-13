#pragma once

#if defined(TRACY_ENABLE)
#include <modules/godot_tracy/profiler.h>

#define SM_PROFILE_ZONE ZoneScoped
#define SM_PROFILE_ZONE_NAMED(name) ZoneScopedN(name)
#define SM_PROFILE_SET_THREAD_NAME(name) tracy::SetThreadName(name)
#define SM_PROFILE_PLOT(text, value) TracyPlot(text, value)
#define SM_PROFILE_LOG(text, size) TracyMessage(text, size)

#else

#define SM_PROFILE_ZONE
#define SM_PROFILE_ZONE_NAMED(name)
#define SM_PROFILE_SET_THREAD_NAME(name)
#define SM_PROFILE_PLOT(text, value)
#define SM_PROFILE_LOG(text, size)
#endif
