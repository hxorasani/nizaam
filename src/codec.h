#ifndef _codec_h
#define _codec_h

#include "common.h"

void codec_bind_js(duk_context *);

char codec_start();
void codec_destroy();

#endif