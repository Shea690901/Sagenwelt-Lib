/// @addtogroup sefun
/// @{
/// @file simul_efun.c
/// @brief simulated efuns
/// @author Gwenhwyvar
/// @version 0.0.0
/// @date 2015-11-29

#include    <simul_efun.h>
#include    <simul_efun_intern.h>

private int startup_finished;

private void create(void)
{
    startup_finished = FALSE;

    init_object_efuns();
}

public void done_startup(void)
{
    startup_finished = TRUE;
}

#include    SEFUN_DIR "arrays"
#include    SEFUN_DIR "driver"
#include    SEFUN_DIR "file_system"
#include    SEFUN_DIR "general"
#include    SEFUN_DIR "logging"
#include    SEFUN_DIR "math"
#include    SEFUN_DIR "objects"
#include    SEFUN_DIR "room"
#include    SEFUN_DIR "security"

/// @}
