#pragma once

// Canli login soket yasam dongusu icin hedefli trace.
// Genel client loglarini acmadan yalnizca login/socket kapanma yolunu yazar.
#ifndef PHOENIX_LOGIN_SOCKET_TRACE_ENABLED
#define PHOENIX_LOGIN_SOCKET_TRACE_ENABLED 0
#endif

#if PHOENIX_LOGIN_SOCKET_TRACE_ENABLED
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

static inline void PhoenixLoginSocketTrace(const char* fmt, ...)
{
  FILE* fp = fopen("login_socket_trace.log", "a");
  if (fp == NULL) return;

  time_t now = time(NULL);
  struct tm tmNow;
  localtime_s(&tmNow, &now);
  fprintf(fp, "%04d-%02d-%02d %02d:%02d:%02d ",
          tmNow.tm_year + 1900, tmNow.tm_mon + 1, tmNow.tm_mday,
          tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec);

  va_list args;
  va_start(args, fmt);
  vfprintf(fp, fmt, args);
  va_end(args);

  fputc('\n', fp);
  fclose(fp);
}
#else
static inline void PhoenixLoginSocketTrace(const char*, ...) {}
#endif

// NewIDCreate account-create akisi icin hedefli trace.
// Varsayilan kapali; debug gerekirse build flag'i ile acilir.
// Password yazilmaz; sadece ID/status/socket/packet sonucu takip edilir.
#ifndef PHOENIX_REGISTRATION_TRACE_ENABLED
#define PHOENIX_REGISTRATION_TRACE_ENABLED 0
#endif

#if PHOENIX_REGISTRATION_TRACE_ENABLED
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

static inline void PhoenixRegistrationTrace(const char* fmt, ...)
{
  FILE* fp = fopen("newid_registration_trace.log", "a");
  if (fp == NULL) return;

  time_t now = time(NULL);
  struct tm tmNow;
  localtime_s(&tmNow, &now);
  fprintf(fp, "%04d-%02d-%02d %02d:%02d:%02d ",
          tmNow.tm_year + 1900, tmNow.tm_mon + 1, tmNow.tm_mday,
          tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec);

  va_list args;
  va_start(args, fmt);
  vfprintf(fp, fmt, args);
  va_end(args);

  fputc('\n', fp);
  fclose(fp);
}
#else
static inline void PhoenixRegistrationTrace(const char*, ...) {}
#endif
