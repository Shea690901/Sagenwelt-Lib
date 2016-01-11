/// @addtogroup privileged
/// @{
/// @file master.h
/// @brief master specific macros and function declaratios
/// @author Gwenhwyvar
/// @version 0.0.0
/// @date 2015-11-29

#ifndef __SEC_MASTER_H
#define __SEC_MASTER_H

#define __SECURE

#include "/secure/include/std_paths.h"  // standard paths used by various objects

#define MASTER_SAVE     PRIV_SAVE_DIR "master"
#define ACL_READ_CFG    CFG_DIR "acl_read.cfg"
#define ACL_WRITE_CFG   CFG_DIR "acl_write.cfg"
#define ACL_LOAD_CFG    CFG_DIR "acl_load.cfg"

// function declarations {{{
string  *acl(int request, mixed info);
void     flag(string driver_flag);
string  *epilog(int load_empty);
void     preload(string str);
void     crash(string crash_message, object command_giver, object current_object);
string   get_bb_uid();
string   get_root_uid();
string   author_file (string file);
string   domain_file(string file);
string   creator_file(string filename);
string   privs_file(string filename);
int      valid_bind(object doer, object owner, object victim);
mixed    valid_database(object doer, string action, mixed *info);
int      valid_hide(object ob);
int      valid_link(string from, string to);
int      valid_object(object ob);
int      valid_override(string file, string efun_name, string mainfile);
int      valid_read(string file, object ob, string func);
int      valid_seteuid(object ob, string euid);
int      valid_shadow(object ob);
int      valid_socket(object ob, string func, mixed *info);
int      valid_write(string file, object ob, string func);
object   connect(int port);
mapping  get_mud_stats();
void     error_handler(mapping error, int caught);
void     log_error(string file, string message);
string   get_save_file_name(string file, object who);
string   make_path_absolute(string rel_path);
string   parse_command_all_word ();
string  *parse_command_prepos_list ();
object   compile_object(string pathname);
string   object_name (object ob);
int      retrieve_ed_setup(object user);
int      save_ed_setup(object user, int config);
// }}}

#endif // __SEC_MASTER_H

///  @}
