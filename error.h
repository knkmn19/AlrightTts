/* { error.h } : errors
 */

#ifndef ALRTTS_ERROR
#define ALRTTS_ERROR

#if (_MSC_VER > 1000)
#   pragma once
#
#endif /* (_MSC_VER > 1000) */

typedef enum error {
    error_ok    = 0,
    error_fail  = 1,
    error_unset = 2,

    error_reserved = 999,
    error_badalloc,

    error_audio_reserved = 1999,

    error_tts_reserved = 2999,
    error_invalidtextinput,
} error;

static inline char const* error_what(error e)
{
    switch (e) {
    default:
        break;

    case error_invalidtextinput:
        return "you cant say that (cause it has no wordds or somehing";

    case error_badalloc:
        return "mem allocation fail...........";

    case error_unset:
        return "result isnt ready yet bro";

    case error_fail:
        return "failed............";

    case error_ok:
        return "ok!!!!!!";
    }

    return "idk";
}

#endif /* !ALRTTS_ERROR */
