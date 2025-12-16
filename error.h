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
} error;

static inline char const* error_what(error e)
{
    switch (e) {
    default:
        break;

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
static inline error error_errorfromhr(long hr)
{
    switch (hr) {
    default:
        break;


    }
    return error_fail;
}

#endif /* !ALRTTS_ERROR */
