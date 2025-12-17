/* { system_windows.h } : system.h impl for windows
 */

#include "system.h"

#include <windows.h>
#include "stdio.h"

static BOOL ctrlproc(DWORD);

static BOOL ctrlproc(DWORD t)
{
    switch (t) {
    default:
        return FALSE;

    case CTRL_C_EVENT:
        g_bexit = !!1;
        break;
    }

    return TRUE;
}

bool_t g_bexit;

error console_init(void)
#ifdef NDEBUG
{
    fclose(stderr);
    (void)SetConsoleOutputCP(CP_UTF8);
    (void)SetConsoleCP(CP_UTF8);
    (void)SetConsoleCtrlHandler(ctrlproc, TRUE);

    return error_ok;
}
#else
{
    (void)SetConsoleOutputCP(CP_UTF8);
    (void)SetConsoleCP(CP_UTF8);
    (void)SetConsoleCtrlHandler(ctrlproc, TRUE);

    return error_ok;
}
#endif
