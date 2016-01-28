/// @file mud_info.h
/// @brief macros concerning requests/answers from the mud_info daemon
/// @author Gwenhwyvar
/// @version 0.0.0
/// @date 2016-01-28

#ifndef __SEC_MUD_INFO_H
#define __SEC_MUD_INFO_H

#include <std_paths.h>

#define MUD_INFO_SAVE     __SAVE_FILE(PRIV_SAVE_DIR "mud_info")

// email handling {{{
#define MI_EMAIL_INVALID    "@inv@"
#define MI_EMAIL_INVALID    "@reg@"
// }}}

#endif  // __SEC_MUD_INFO_H
