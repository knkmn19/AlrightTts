/* { system_windows.h } : system.h impl for windows
 */

#include "system.h"

#include <windows.h>
#include "stdio.h"

error console_init(void)
#ifdef NDEBUG
{
    fclose(stderr);
    (void)SetConsoleOutputCP(CP_UTF8);
    (void)SetConsoleCP(CP_UTF8);

    return error_ok;
}
#else
{
    (void)SetConsoleOutputCP(CP_UTF8);
    (void)SetConsoleCP(CP_UTF8);

    return error_ok;
}
#endif
