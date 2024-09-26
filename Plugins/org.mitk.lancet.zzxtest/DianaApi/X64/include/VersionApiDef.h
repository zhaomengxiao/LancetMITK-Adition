#pragma once

#if defined _WIN32
#ifdef VERSIONAPI_EXPORTS
#define VERSIONAPI_API extern "C" __declspec(dllexport)
#define VERSIONAPI_CLASS __declspec(dllexport)
#else
#define VERSIONAPI_API extern "C" __declspec(dllimport)
#define VERSIONAPI_CLASS __declspec(dllimport)
#endif
#else
#if __GNUC__ >= 4
#define VERSIONAPI_API extern "C" __attribute__((visibility("default")))
#define VERSIONAPI_CLASS __attribute__((visibility("default")))
#else
#define VERSIONAPI_API extern "C"
#define VERSIONAPI_CLASS
#endif
#endif
