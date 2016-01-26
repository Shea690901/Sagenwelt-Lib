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
// simul_efun_helper {{{
public void done_startup(void);
//  }}}
// array {{{
public         mixed  *distinct_array(mixed *arr);
public         mixed  *remove_member(mixed *arr, int i);
public         mixed  *scramble_array(mixed *arr);
// }}}
// driver {{{
public varargs mixed   debug_info(int req, mixed *args...);
public         int     exec(object to, object from);
public         void    reset_eval_cost(void);
public         void    set_eval_limit(int limit);
public         void    shutdown(int ret, string msg)
//  }}}
// file_system {{{
public         string  basename(string path);
public         string  canonical_path(string path);
public         string  dirname(string path);
public         string  get_cwd(object who);
// }}}
// general {{{
public         int     cmp(mixed a, mixed b);
public         int     get_debug(void);
public         int     set_debug(int val);
//  }}}
// logging {{{
public varargs void    syslog(int priority, string format, mixed *args...);
public varargs void    m_syslog(string uid, string gid, int priority, string format, mixed *args...)
// }}}
// math {{{
public         int     fib(int n);
public         int     gcd(int a, int b);
public         int     lcm(int a, int b);
// }}}
// objects {{{
public         string  author_of(string file);
public         string  domain_of(string file);
public varargs void    destruct(object ob);
public         string  file_name(object who = 0, int flag = 0);
public         object  simul_efun(void);
// }}}
// room {{{
public         string  opposite_dir(string str, int article = 0);
// }}}
// security {{{
public         int     init_eids(void);
public         string  getgid(mixed who);
public         string  getegid(mixed who);
public         string  setegid(string egid);
public         string  getuid(mixed who);
public         string  getugid(mixed who);
public         string  seteuid(string euid);
///  }}}
///  }}}
#endif // __SEC_SIMUL_EFUN_H
/// @}
