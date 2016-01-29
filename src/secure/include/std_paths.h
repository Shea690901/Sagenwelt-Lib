/// @addtogroup privileged
/// @{
/// @file std_paths.h
/// @brief standard defines for priviledged objects
/// @author Gwenhwyvar
/// @version 0.0.0
/// @date 2015-11-29

#ifndef __SEC_STD_PATHS_H
#define __SEC_STD_PATHS_H

#include <config.h>

// directories
#define LOG_DIR         "/var/log/"                     ///< logfiles
#define PRIV_SAVE_DIR   "/var/save/"                    ///< save-files of privileged objects
#define CFG_DIR         "/var/cfg/"                     ///< base config files
#define SECURE_DIR      "/secure/"                      ///< everything security relevant
#define DAEMON_DIR      SECURE_DIR "daemons/"           ///< daemons
#define SECURE_OBJ_DIR  SECCURE_DIR "obj/"              ///< security relevant std objects
#define SECURE_LIB_DIR  SECCURE_DIR "lib/"              ///< security relevant std library
#define SEFUN_DIR       SECURE_DIR "sefuns/"            ///< simul_efun-modules

#define __HOME_DIR(x)   "/players/" x[0..0] "/" x "/"   ///< create home directory name

// files
#define __SAVE_FILE(x)      ((x) + __SAVE_EXT)          ///< create save file name (uncompressed)
#define __SAVE_GZ_FILE(x)   ((x) + __SAVE_GZ_EXT)       ///< create save file name (compressed)

#define PRELOADS        CFG_DIR "PRELOADS"              ///< file containing filenames to be preloaded

#define ACL_READ_CFG    CFG_DIR "Read.acl"              ///< standard acl for read access
#define ACL_WRITE_CFG   CFG_DIR "Write.acl"             ///< standard acl for write access
#define PRIVS_CFG       CFG_DIR "Privs.cfg"             ///< default privs for all objects

#define WELCOME_MSG     CFG_DIR "welcome.txt"           ///< welcome message, displayed @ logon
#define MOTD            CFG_DIR "motd.txt"              ///< message of the day, displayed @ logon

// library
#define INPUT_SYSTEM    SECURE_LIB_DIR "inp_sys"        ///< module for input handling
#define MESSAGE_SYSTEM  SECURE_LIB_DIR "messages"       ///< module for message handling
#define SHELL_SYSTEM    SECURE_LIB_DIR "shell"          ///< module for shell handling

// objects
#define LOGIN_OB        SECURE_OBJ_DIR "login"          ///< login object

// daemons
#define MUD_INFO_D      DAEMON_DIR "mud_info"           ///< mud infos
#define SYSLOG_D        DAEMON_DIR "syslogd"            ///< logging daemon
#define TMP_D           DAEMON_DIR "tmpd"               ///< handler for temporary files
#define MAIL_D          DAEMKN_DIR "smtpd"              ///< mailer daemon
#define NEWS_D          DAEMKN_DIR "newsd"              ///< news daemon

#endif // __SEC_STD_PATHS_H

///  @}
