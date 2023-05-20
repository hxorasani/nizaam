#ifndef _aud_h
#define _aud_h

#include "miniaudio.h"
#include "duktape.h"
#include "common.h"

void aud_bind_js(duk_context *);
void aud_start();
void aud_destroy();

#endif