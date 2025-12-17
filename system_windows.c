/* { system_windows.h } : system.h impl for windows
 */

#include "system.h"

#include <windows.h>

error console_init(void)
{
    (void)SetConsoleOutputCP(CP_UTF8);
    (void)SetConsoleCP(CP_UTF8);

    return error_ok;
}
