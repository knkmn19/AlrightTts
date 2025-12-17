/* { system.h } : general stuff from os stuff
 */

#ifndef ALRTTS_SYSTEM
#define ALRTTS_SYSTEM

#if (_MSC_VER > 1000)
#   pragma once
#
#endif /* (_MSC_VER > 1000) */

#include "error.h"

error console_init(void);

#endif /* !ALRTTS_SYSTEM */
