/// @addtogroup sefun
/// @{
/// @file simul_efun.h
/// @brief simul_efun specific macros and function declaratios
/// @author Gwenhwyvar
/// @version 0.0.0
/// @date 2016-01-24

#ifndef __SEC_SIMUL_EFUN_H
#define  __SEC_SIMUL_EFUN_H

#include <pragmas.h>        // setting standard pragmas
#include <syslog.h>         // needed for logging

// function prototypes (ordered by groups{{{
// file_system {{{
public         string absolute_path(string path);
public         string basename(string path);
public         string canonical_path(string path);
public         string dirname(string path);
public         string get_cwd(object who);
// }}}
// general {{{
public         int get_debug(void);
public         int set_debug(int val);
//  }}}
// logging {{{
public varargs void syslog(int priority, string format, mixed *args...);
// }}}
// objects {{{
public         string file_name(object who, int flag = 0);
public         object simul_efun(void);
// }}}
// security {{{
public varargs string getgid(mixed who);
public varargs string getegid(mixed who);
public         string setegid(string egid);
public varargs string getuid(mixed who);
public varargs string getugid(mixed who);
public         string seteuid(string euid);
///  }}}
///  }}}
#endif // __SEC_SIMUL_EFUN_H
/// @}
