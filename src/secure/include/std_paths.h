/// @addtogroup privileged
/// @{
/// @file std_paths.h
/// @brief standard defines for priviledged objects
/// @author Gwenhwyvar
/// @version 0.0.0
/// @date 2015-11-29

#ifndef __SEC_STD_PATHS_H
#define __SEC_STD_PATHS_H

// directories
#define LOG_DIR         "/var/log/"                 ///< logfiles
#define PRIV_SAVE_DIR   "/var/save/"                ///< save-files of privileged objects
#define CFG_DIR         "/var/cfg/"                 ///< base config files
#define SECURE_DIR      "/secure/"                  ///< everything security relevant
#define DAEMON_DIR      SECURE_DIR "daemons/"       ///< daemons
#define SECURE_OBJ_DIR  SECCURE_DIR "obj/"          ///< security relevant std objects
#define SECURE_LIB_DIR  SECCURE_DIR "lib/"          ///< security relevant std library
#define SEFUN_DIR       SECURE_DIR "sefuns/"        ///< simul_efun-modules

// files
#define PRELOADS        CFG_DIR "PRELOADS"          ///< file containing filenames to be preloaded

#define ACL_READ_CFG    CFG_DIR "Read.acl"          ///< standard acl for read access
#define ACL_WRITE_CFG   CFG_DIR "Write.acl"         ///< standard acl for write access
#define PRIVS_CFG       CFG_DIR "Privs.cfg"         ///< default privs for all objects

// library
#define YES_NO          SECURE_LIB_DIR "yes_no"     ///< module for yes_no questions

// objects
#define LOGIN_OB        SECURE_OBJ_DIR "login"      ///< login object

// daemons
#define MUD_INFO_D      DAEMON_DIR "mud_info"       ///< mud infos
#define SYSLOG_D        DAEMON_DIR "syslogd"        ///< logging daemon
#define TMP_D           DAEMON_DIR "tmpd"           ///< handler for temporary files
#define MAIL_D          DAEMKN_DIR "smtpd"          ///< mailer daemon
#define NEWS_D          DAEMKN_DIR "newsd"          ///< news daemon

#endif // __SEC_STD_PATHS_H

///  @}
