/* { annotations.h } : annotation macros
 */

#ifndef ALRTTS_ANNOTATIONS
#define ALRTTS_ANNOTATIONS

#if (_MSC_VER > 1000)
#   pragma once
#
#endif /* (_MSC_VER > 1000) */

#define FN_DEPRECATED
#define FN_CONST
#define FN_NOTIMPLEMENTED_PRIORITYLOW
#define FN_NOTIMPLEMENTED_PRIORITYMID
#define FN_NOTIMPLEMENTED_PRIORITYMAX
#define FN_UNKNOWNLINKAGE extern
#define FN_CXXLINKAGE extern
#define FN_ASMLINKAGE extern

#define CMP_LIKELY(x)   (x)
#define CMP_UNLIKELY(x) (x)

#endif /* !ALRTTS_ANNOTATIONS */
