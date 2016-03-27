/// @addtogroup privileged
/// @{
/// @file master.c
/// @brief first object loaded
///
/// The master object is the second object (after the simul_efun object) to be
/// loaded by the gamedriver, it has complete control over the mudlib and
/// needs to implement a couple of functions (master applies) for the
/// gamedriver to work properly.
/// @author Gwenhwyvar
/// @version 0.1.0
/// @date 2015-11-27
/// @attention until this object is successfully loaded, there is only the
/// runtime configured include path

#include <pragmas.h>                // setting standard pragmas
#include <master.h>                 // master specific
#include <privs.h>                  // privlege related defines
#include <driver/parser_error.h>    // for master::parser_error_message

// private function forward declarations
private int      retrieve_ed_setup(object user);
private int      save_ed_setup(object user, int config);
private int      valid_bind(object doer, object owner, object victim);
private int      valid_hide(object ob);
private int      valid_link(string from, string to);
private int      valid_object(object ob);
private int      valid_override(string file, string efun_name, string mainfile);
private int      valid_seteuid(object ob, string t_euid);
private int      valid_shadow(object ob);
private int      valid_socket(object ob, string func, mixed *info);
private mapping  get_mud_stats(void);
private mapping  init_acl(string type);
private mapping  init_privileges(void);
private mixed    valid_database(object doer, string action, mixed *info);
private object   compile_object(string pathname);
private object   connect(int port);
private object  *parse_command_users(void);
private string   creator_file(string filename);
private string   get_bb_uid(void);
private string   get_root_uid(void);
private string   get_save_file_name(string file, object who);
private string   make_path_absolute(string rel_path);
private string   object_name(object ob);
private string   parse_command_all_word(void);
private string   parse_command_all_word(void);
private string   parser_error_message(int type, object ob, mixed arg, int flag);
private string   privs_file(string file);
private string  *acl(int request, mixed info);
private string  *epilog(int dummy);
private string  *get_include_path(string file);
private string  *get_include_path(string file);
private string  *parse_command_adjectiv_id_list(void);
private string  *parse_command_id_list(void);
private string  *parse_command_plural_id_list(void);
private string  *parse_command_prepos_list(void);
private string  *parse_command_prepos_list(void);
private void     crash(string crash_message, object command_giver, object current_object);
private void     error_handler(mapping err, int caught);
private void     flag(string driver_flag);
private void     log_error(string file, string message);
private void     preload(string str);
private void     save_master(void);
private void     startup_summary(void);

// global vars
private nosave int *startup_info;       ///< how many objects to be preloaded, errors...

/// @brief acl_*
///
/// data format:
/// ([
///    "path" : ([
///                 "function" : ({ "uid", ..., "gid", ... }),
///                 ...
///             ]),
///     ...
/// ])
private mapping acl_read,               ///< acls for file read access
                acl_write;              ///< acls for file write access

/// @brief privileges
///
/// data format:
/// ([
///    "path" : <assigned privileges>,
///     ...
/// ])
/// where assigned privileges is a bitfield (string)
private mapping privileges;

// helper functions
// --------------------------------------------------------------------------
/// @brief save_master
/// the master is a _very_ important object, as such we must be extremly
/// careful when saving!
/// @Returns -
// --------------------------------------------------------------------------
private void save_master(void)
{
    // move old backup copy out of the way
    if(file_size(MASTER_SAVE + ".bak") >= 0)
        rename(MASTER_SAVE + ".bak", MASTER_SAVE + ".bak~");

    // make backup of old save file
    if((file_size(MASTER_SAVE) >= 0) && (cp(MASTER_SAVE, MASTER_SAVE + ".bak") != 1))
    {
        syslog(LOG_KERN|LOG_EMERG, "master::save_master: can't save save file!");
        efun::shutdown(-1);
    }
    else if(!save_obiect(MASTER_SAVE))  // save current values
    {
        // panic! couldn't save...
        syslog(LOG_KERN|LOG_EMERG, "master::save_master: can't save master!");
        efun::shutdown(-1);
    }
    else                                // now we can remove old backup
        rm(MASTER_SAVE + ".bak~");
}
private mapping init_acl(string type)
{
    string  cfg = "",
            fn;
    mapping ret = ([]);

    // read the proper config file
    switch(type)
    {
        case "r":
            if(file_size(fn = ACL_READ_CFG) > 0)
                cfg = read_file(ACL_READ_CFG);
            break;
        case "w":
            if(file_size(fn = ACL_WRITE_CFG) > 0)
                cfg = read_file(ACL_WRITE_CFG);
            break;
        default:
            error("Unknown argument '" + type + "' to init_acl");
            return;
    }

    // and parse it
    if(cfg != "")
    {
        string path = "";
        int    ln = 0;

        foreach(string line in explode(cfg, "\n"))
        {
            string *entries = ({});
            ln++;
            if(!line || (line == "") || (line[0] == '#'))   // skip empty lines and comments
                continue;
            switch(line[0])
            {
                case '/':                                   // new path entry
                    path = line;
                    break;
                case '\t':                                  // new function entry
                    if(path == "")                          // malformed config file
                    {
                        syslog(LOG_KERN|LOG_WARNING, "malformed config file: %s[%d]", fn, ln);
                        return ({});
                    }
                    entries = explode(trim(line), " ");
                    if(!ret[path])
                        ret[path] = ([]);
                    ret[path][entries[0]] = entries[1..];
                    break;
                default:
                    syslog(LOG_KERN|LOG_WARNING, "malformed config file: %s[%d]", fn, ln);
                    return ({});
            }
        }
    }
    else
        syslog(LOG_KERN|LOG_WARNING, "empty config file: %s", fn);

    return ret;
}
private string *acl(int request, mixed info)
{
    mapping acl_list;
    mixed   ret;

    switch(request)
    {
        case _READ:
            acl_list = acl_read;
            break;
        case _WRITE:
            acl_list = acl_write;
            break;
        default:
            syslog(LOG_KERN|LOG_INFO, "master::acl(%d,...): unknown request type!", request);
            return ({});
    }
    ret = match_path(acl_list, info[0]);
    return ret[info[1]];
}
// --------------------------------------------------------------------------
// request currently supported:
// - _READ
// - _WRITE
// euid is the effective uid of the object doing the request
// egid is the effective gid of the object doing the request
// the contents of info depends on the request, refer to the apropriate
// valid_*
// returns TRUE if the access is granted, FALSE otherwise
// --------------------------------------------------------------------------
public int check_acl(int request, string euid, string egid, mixed info)
{
    string *access, // access control list for given file
            author, // author
            domain; // and domain of the requested path
    object  ti;

    switch(request) // check for valid request
    {
        case _READ:
        case _WRITE:
            if(info && arrayp(info) &&          // we need info to be an array of two strings
                (sizeof(info) == 2) &&
                stringp(info[0]) && stringp(info[1]))
            {
                switch(info[1])                 // map driver supplied function names
                {
                    case "compress_file":
                    case "ed_start":
                    case "include":
                    case "move_file":
                    case "read_bytes":
                    case "read_file":
                    case "rename":
                    case "write_bytes":
                    case "write_file":
                        info[1] = "driver_open";
                        break;
                    case "get_dir":
                    case "mkdir":
                    case "remove_file":
                    case "rmdir":
                        info[1] = "driver_dopen";
                        break;
                    case "file_size":
                    case "stat":
                        info[1] = "driver_stat";
                        break;
                    case "debug_malloc":
                    case "dumpallobj":
                        info[1] = "driver_debug";

                    case "load_object":
                    case "restore_object":
                    case "save_object":
                    default:
                        break;
                }
                break;
            }
            else                                // incorrect parameter
                return FALSE;
        default:                                // everything else can't be valid
            return FALSE;
    }

    // get the acl for the given path and request
    if(!sizeof(access = acl(request, info)))
        return FALSE;                           // oops! nobody allowed!

    // everyone allowed?
    if(member_array(UPRIV_ALL, access) >= 0)
        return TRUE;

    // test if either user or group is allowed
    if((member_array(euid, access) >= 0) || (member_array(egid, access) >= 0))
        return TRUE;

    // check if owner
    author = author_file(info[0]);
    if((member_array(UPRIV_AUTHOR, access) >= 0) && (author == euid))
        return TRUE;

    // check if domain
    domain = author_file(info[0]);
    if((member_array(UPRIV_DOMAIN, access) >= 0) && (domain == egid))
        return TRUE;

    // check interactives
    if(ti = TI())
    {
        if(member_array(UPRIV_MORTAL, access) >= 0)
            return TRUE;
        if((member_array(UPRIV_ELDER, access) >= 0) && elderp(ti))
            return TRUE;
        if((member_array(UPRIV_WIZARD, access) >= 0) && creatorp(ti))
            return TRUE;
        if((member_array(UPRIV_D_WIZ, access) >= 0) && Dcreatorp(ti, domain))
            return TRUE;
        if((member_array(UPRIV_D_LORD, access) >= 0) && Dlordp(ti, domain))
            return TRUE;
        if((member_array(UPRIV_ARCH, access) >= 0) && archp(ti))
            return TRUE;
        if((member_array(UPRIV_ADMIN, access) >= 0) && adminp(ti))
            return TRUE;
    }

    // everything else is disallowed
    return FALSE;
}
private mapping init_privileges(void)
{
    string  cfg = "";
    mapping ret = ([]);

    // read config file
    if(file_size(PRIVS_CFG) > 0)
        cfg = read_file(PRIVS_CFG);

    // and parse it
    if(cfg != "")
    {
        int    ln = 0;

        foreach(string line in explode(cfg, "\n"))
        {
            ln++;
            if(!line || (line == "") || (line[0] == '#'))   // skip empty lines and comments
                continue;
            else if(line[0] == '/')
            {
                string *t = explode(line, "\t")
                ret[t[0]] = t[1];
            }
            else
                syslog(LOG_KERN|LOG_WARNING, "malformed config file: " + PRIVS_CFG + "[%d]", ln);
        }
    }
    else
        syslog(LOG_KERN|LOG_WARNING, "empty config file: " + PRIVS_CFG);

    return ret;
}

private void startup_summary(void)
{
    string out;

    out  = sprintf("\n%:'='80s\nStartup Summary:\n%:'-'80s\n", "=", "-");
    out += sprintf(
            "# of preloaded objects: %:03d\n" +
            "# of successful loads : %:03d\n" +
            "# of errors           : %:03d\n",
            startup_info[0], startup_info[1] - startup_info[2], startup_info[2]);
#ifdef __HAS_RUSAGE__
    out += sprintf("%:'-'80s\nStartup Time:\n", "-");
    out += sprintf("  user time: %:6d\nsystem time: %:6d\n",
            startup_info[3], startup_info[4]);
#endif
    out += sprintf("%:'='80s\n", "=");
    debug_message(out);

    // now the mudlib is up and running, tell the simul_efuns so...
    done_startup();
}

// parse_info_refresh {{{
// --------------------------------------------------------------------------
/// @brief parse_info_refresh
/// called from login and connection object whenever the contents of
/// efun::users changes
/// @Returns
// --------------------------------------------------------------------------
public void parse_info_refresh(void)
{
    parse_refresh();
}

// master applies
/// @name initialization
/// @{
// --------------------------------------------------------------------------
/// @brief flag
///
/// This master apply is called for each command line option passed to the
/// driver with the -f flag. For example, invoking the driver via:
///
///     ./driver -fdebug
///
/// will call flag("debug") in the master object during initialization.
/// @Param driver_flag
// --------------------------------------------------------------------------
private void flag(string driver_flag)
{
#ifdef __HAS_RUSAGE__
    mapping before,         ///< rusage before loading object
            after;          ///< rusage after loading object

    before = rusage();
#endif
    switch driver_flag
    {
        "debug":
            set_debug(TRUE);        // set simul_efun internal debug flag
            break;
        default:
            error(sprintf("Unknown driver-flag '%s'…", driver_flag));
            break;
    }
#ifdef __HAS_RUSAGE__
    after = rusage();
    if(sizeof(before) && sizeof(after))
    {
        startup_info[3] = after["utime"] - before["utime"]
        startup_info[4] = after["stime"] - before["stime"]
    }
#endif
}
// --------------------------------------------------------------------------
/// @brief epilog
///
/// The driver calls epilog() in master after the master object has been
/// loaded. Required initialization may be done at this point. If epilog()
/// returns an array of filenames, the driver will attempt to load those
/// files via the preload() function.
///
/// @Param dummy - if true no preloading is done
/// @Returns array of files to preload
/// @Attention FluffOS no longer supports '-e', as such the parameter is always '0'!
// --------------------------------------------------------------------------
private string *epilog(int dummy)
{
    string   content,
            *ret;
    mapping  seen;
#ifdef __HAS_RUSAGE__
    mapping  before,         ///< rusage before loading object
             after;          ///< rusage after loading object

    before = rusage();
#endif

    ret  = ({});
    seen = ([]);

    if(content = read_file(PRELOADS))
    {
        // split content into array of single lines
        // and process each line
        foreach(string line in explode(content, "\n"))
        {
            // ignore empty lines or comment lines
            if(!line || line == "" || line[0] == '#')
                continue;
            // include all "*.c" from directories
            if(line[<1] == '/')
            {
                foreach(string entry in get_dir(line + "*.c"))
                {
                    entry = entry[0..<3];       // cut of '.c' file ending
                    if(!seen[entry])
                    {
                        ret += ({ entry });
                        seen[entry] = 1;
                    }
                    else
                        seen[entry] += 1;
                }
            }
            // add all other lines directly to return array
            else
            {
                if(line[<2..<1] == ".c")
                    line = line[0..<3];         // cut of ".c" if present
                if(!seen[line])
                {
                    ret += ({ line });
                    seen[line] = 1;
                }
                else
                    seen[line] += 1;
            }
        }
    }

    startup_info[0] = sizeof(ret);            // preload needs to know when to print summary

#ifdef __HAS_RUSAGE__
    after = rusage();
    if(sizeof(before) && sizeof(after))
    {
        startup_info[3] = after["utime"] - before["utime"]
            startup_info[4] = after["stime"] - before["stime"]
    }
#endif

    // nothing to preload
    if(!startup_info[0])
        startup_summary();

    return ret;
}
// --------------------------------------------------------------------------
/// @brief preload
///
/// For each string in the array returned by epilog, the driver calls
/// preload(filename). Note that there is the equivalent of a catch() around
/// these calls at the driver level, so it is not neccessary for the mudlib to
/// worry about the sequence being terminated by an error.
///
/// Typical behavoir is to use load_object() to attempt to load the file.
/// @Param str - file to be preloaded
// --------------------------------------------------------------------------
private void preload(string str)
{
    string  err;            ///< error while loading object
#ifdef __HAS_RUSAGE__
    mapping before,         ///< rusage before loading object
            after;          ///< rusage after loading object
    int     t;

    before = rusage();
#endif

    startup_info[1]++;
    debug_message("Preloading: '" + str + "'...");

    // everything either has
    if(err = catch(load_object(str)))
    {
        // compile errors
        debug_message("Got error: '" + err + "'...");
        startup_info[2]++;
    }
    else
        // or loads fine...
        debug_message("Done!...");
#ifdef __HAS_RUSAGE__
    after = rusage();
    if(sizeof(before) && sizeof(after))
    {
        t = after["utime"] - before["utime"];
        debug_message("utime: " + t + "ms...")
        startup_info[3] += t;
        t = after["stime"] - before["stime"];
        debug_message("utime: " + t + "ms")
        startup_info[4] += t;
    }
#endif

    debug_message("\n");
    if(startup_info[0] == startup_info[1])  // all objects tried to load?
        startup_summary();
}
///  @}

/// @name deinitialization
/// @{
// --------------------------------------------------------------------------
/// @brief crash
///
/// The driver calls crash() in master in the event that the driver crashes
/// (segmentation fault, bus error, etc). This function offers a way to shutdown the
/// mudlib (safe players and other important data) before the driver crashes. It
/// also lets you log various useful information such as what signal crashed the
/// driver, what object was active, who the current player was etc.
/// @Param crash_message
/// @Param command_giver
/// @Param current_object
/// @todo make use of parameters
// --------------------------------------------------------------------------
private void crash(string crash_message, object command_giver, object current_object)
{
    object *usr = users();

    syslog(LOG_KERN|LOG_EMERG, "master::crash(\"%s\", %O, %O)", crsh_message, commmd_giver, current_object);

    // inform the player
    reset_eval_cost();                  // we might need to do a lot of calls...
    message(MSGCLASS_SYSTEM, "Game Driver shouts: Ack! I think the game is crashing!\n", usr);

    // and send them the logout shutdown event
    reset_eval_cost();                  // we might need to do a lot of calls...
    event(usr, "shutdown");             // force every user to logout

    // terminate daemons and other secure objects gracefully
    reset_eval_cost();                  // we might need to do a lot of calls...
    event(objects( (: file_name($1)[0..8] == "/secure/" :) ), "shutdown");
}
///  @}

/// @name uid and security handling
/// @{
// --------------------------------------------------------------------------
/// @brief get_bb_uid
///
/// This master apply is called by the driver on startup, after it has loaded
/// the master object, to get the backbone uid defined by the mud. The
/// function should return a string, eg "BACKBONE"
/// @Returns backbone uid
/// @Attention This routine is only used if PACKAGE_UIDS is used.
// --------------------------------------------------------------------------
private string get_bb_uid(void)
{
    return creator_file("/std");;
}
// --------------------------------------------------------------------------
/// @brief get_root_uid
///
/// This master apply is called by the driver each time it loads the master
/// object, to verify that the master object has loaded, and to get the root
/// uid defined by the mud. The function should return a string, eg "ROOT"
/// @Returns root uid
/// @Attention This routine is only used if PACKAGE_UIDS is used.
// --------------------------------------------------------------------------
private string get_root_uid(void)
{
    return creator_file("/secure");;
}
// --------------------------------------------------------------------------
/// @brief author_file
///
/// This function must exist in the master object. It is called by the author
/// statistic functions in the driver to determine what author a given object
/// should be associated with. This is totally arbitrary and up to the mudlib
/// designers wishes. It should be noted that the author that the object is
/// assigned to will receive "credit" for all of the objects behavior (errors,
/// heart_beats, etc).
///
/// For this mudlib player or wizard names are lowercase while domain names
/// are capitalized
/// @Attention This routine is only used by the driver if PACKAGE_MUDLIB_STATS
/// is used.
/// @Param file - absolute path to source of some object
/// @Returns name of author - for the security system this will be the uid
// --------------------------------------------------------------------------
private string author_file(string file)
{
    return author_of(file);
}
// --------------------------------------------------------------------------
/// @brief domain_file
///
/// statistic functions in the driver to determine what domain a given object
/// should be associated with. This is totally arbitrary and up to the mudlib
/// designers wishes. It should be noted that the domain that the object is
/// assigned to will receive "credit" for all of the objects behavior (errors,
/// heart_beats, etc).
///
/// For this mudlib domain names are uppercase
/// @Attention This routine is only used by the driver if PACKAGE_MUDLIB_STATS
/// is used.
/// @Param file
/// @Returns domain the file belongs to - for the security system this will be
/// the gid
// --------------------------------------------------------------------------
private string domain_file(string file)
{
    return domain_of(file);
}
// --------------------------------------------------------------------------
/// @brief creator_file
///
/// The creator_file() function is called in the master object each time a new
/// object is created. The `filename' of the object is passed as the sole
/// parameter, and the string that creator_file() returns is set as the new
/// object's uid. If the AUTO_SETEUID option is enabled at compile-time of the
/// driver, it is also set as the new object's euid.
/// One exception: if the AUTO_TRUST_BACKBONE option is enabled at
/// compile-time of the driver, and creator_file() returns the backbone uid
/// (as specified by get_bb_uid() in the master object), the object is given
/// the uid and euid of the object that loaded it.
///
/// This mudlib uses neither AUTO_SETEUID nor AUTO_TRUST_BACKBONE!
/// Also creator is always <user>:<group>.
/// @Param filename - absolute path to object to be loaded/cloned
/// @Returns name of creator
// --------------------------------------------------------------------------
private string creator_file(string filename)
{
    return("%s:%s",
            author_of(filename),
            domain_of(filename));
}
// --------------------------------------------------------------------------
/// @brief privs_file
///
/// The privs_file() function is called in the master object when a new file
/// is created. The `filename' of the object is passed as the argument, and
/// the string that privs_file() returns is used as the new object's privs
/// string.
/// @Param file
/// @Returns privilege level
// --------------------------------------------------------------------------
private string privs_file(string file)
{
    return match_path(privileges, file);
}
// --------------------------------------------------------------------------
/// @brief valid_bind
///
/// This is called when 'doer' tries to use the bind() efun to bind a
/// function pointer owned by 'owner' to the object 'victim'. If this routine
/// returns zero, the operation is disallowed.
/// @Param doer
/// @Param owner
/// @Param victim
/// @Returns
// --------------------------------------------------------------------------
private int valid_bind(object doer, object owner, object victim)
{
    // master and simul_efuns are allowed
    if((doer == master()) || (doer == simul_efun()))
        return TRUE;

    // also allowed with PRIV_BIND
    if(test_bit(query_privs(doer), PRIV_BIND))
        return TRUE;

    // everything else fails
    syslog(LOG_AUTH|LOG_ERR, "Privilege violation: valid_bind(%O, %O, %O)",
            doer, owner, victim);
    return FALSE;
}
// --------------------------------------------------------------------------
/// @brief valid_database
/// @Param doer
/// @Param action
/// @Param info
/// @Returns
/// @Todo DB_d
// --------------------------------------------------------------------------
private mixed valid_database(object doer, string action, mixed *info)
{
    switch(action)
    {
        case "connect": // new database connection only by master for now
        case "close":   // the same for closing
            if(doer == master())
                return (action == "connect") ? "password" : 1;
            break;
        // every other action for now only master and PRIV_DB
        default:
            if((doer == master()) || test_bit(query_privs(doer), PRIV_DB))
                return TRUE;
    }

    // everything else fails
    syslog(LOG_AUTH|LOG_ERR,
            "Privilege violation: valid_database(%O,\"%s\", %O)",
            doer, action, info);
    return FALSE;
}
// --------------------------------------------------------------------------
/// @brief valid_hide
///
/// Add valid_hide to master.c in order to allow objects to hide themselves,
/// or see other objects that have hidden themselves. When an object tries to
/// use the set_hide() efun to hide itself, valid_hide will be called with
/// the object that is wanting to hide as the sole parameter. It should
/// return 1 to allow it, or 0 to not allow it. The same call takes place
/// when it needs to be determined if a certain object should be able to see
/// hidden objects.
/// @Param ob
/// @Returns
// --------------------------------------------------------------------------
private int valid_hide(object ob)
{
    // only with PRIV_HIDE
    if(test_bit(query_privs(ob), PRIV_HIDE))
        return TRUE;

    // everything else fails
    syslog(LOG_AUTH|LOG_ERR, "Privilege violation: valid_hide(%O)", ob);
    return FALSE;
}
// --------------------------------------------------------------------------
/// @brief valid_link
///
/// The driver calls valid_link(from, to) in the master object from inside
/// the link(from, to) efunction. If valid_link() returns 0, then the link()
/// will fail. If valid_link() returns 1, then the link will succeed if
/// rename() would succeed if called with the same arguments.
/// @Param from - make this file known as
/// @Param to   - this link
/// @Returns 1 if link granted, 0 otherwise
// --------------------------------------------------------------------------
private int valid_link(string from, string to)
{
    // we support linking, security check comes from within the driver
    return TRUE;
}
// --------------------------------------------------------------------------
/// @brief valid_object
///
/// After loading an object, the driver will call valid_object() with the
/// newly created object as its argument, in the master object. If the
/// function exists, and returns 0, then the object will be destructed and
/// the efun that caused it to load will error out. If it does not exist, or
/// returns 1, then loading will proceed as normal. This is called before the
/// object has a chance to execute any code, including create(), so not much
/// should be assumed about the object except that file_name(obj) is valid.
/// @Param ob
/// @Returns
// --------------------------------------------------------------------------
private int valid_object(object ob)
{
    string  file;
    int     clone,
            ret = TRUE;
    object  po = PO();
    object *anc;

    file  = file_name(ob, 1);
    clone = clonep(ob);

    // we need uid/gid to be known
    if((author_file(file) == UNKNOWN_UID) || (domain_file(file) == UNKNOWN_DOMAIN))
        ret = FALSE;

    // no objects stored in hidden files...
    else if(strsrch(file, "/.") != -1)
        ret = FALSE;

    // special consideration for /secure/*
    else if(file[0..7] == "/secure/")
    {
        switch(file)
        {
            // some may be cloned
            case LOGIN_OB:
            case PLAYER_OB:
                ret = TRUE;
                break;
            // most 'only' loaded
            default:
                ret = !clone;
                break;
        }
    }

    // world writeable dirs are forbidden
    else if((file[0..4] == "/tmp") || (file[0..8] == "/var/tmp"))
        ret = FALSE;

    // input handling objects aren't allowed to be standard objects or rooms
    else if((member_array(M_INPUT, anc = deep_inherit_list(po)) != -1) &&
            ((member_array(STD_OBJECT, anc) != -1) ||
             (member_array(ROOM, anc) != -1)))
        ret = FALSE;

    // everything passed?
    if(ret)
        return TRUE;

    // apparently not
    syslog(LOG_AUTH|LOG_ERR,
            "Privilege violation: valid_object(%O) by %O[%s]",
            ob, po, efun::geteuid(po));
    return FALSE;
}
// --------------------------------------------------------------------------
/// @brief valid_override
///
/// File will be the actual file the call appears in; mainfile will be the
/// file being compiled (the two can differ due to #include)
///
/// Add valid_override to master.c in order to control the use of the efun::
/// prefix. The valid_override function in master.c will be called each time
/// the driver attempts to compile a function call that begins with efun::.
/// If valid_override returns 0, then that compile will fail. Thus
/// valid_override provides a way to modify the behavior of efuns that isn't
/// circumventable via the efun:: prefix (by having a simul_efun of the same
/// name as the efun to be modified and having valid_override disallow that
/// simul_efun from being overriden).
///
/// If you wish to have the original 3.1.2 efun:: behavior, simply add a line to master.c that looks like this:
///
///     int valid_override(string file, string efun) { return 1; }
///
/// Here is an example valid_override that is more restrictive:
///
///     int valid_override(string file, string name)
///     {
///         if (file == "/adm/obj/simul_efun")
///             return 1;
///         if (name == "destruct")
///             return 0;
///         if (name == "shutdown")
///             return 0;
///         if (name == "snoop")
///             return 0;
///         if (name == "exec")
///             return 0;
///         return 1;
///     }
/// @Param file
/// @Param efun_name
/// @Param mainfile
/// @Returns
// --------------------------------------------------------------------------
private int valid_override(string file, string efun_name, string mainfile)
{
    // we don't have an object yet!!!
    // only the filename from which the object is compiled...

    // special cases 1
    // these are disallowed even for '/secure/*' as long as they aren't
    // contained in a special file
    switch(efun_name)
    {
        "move_object":
            return file == MOVE;
    }

    // everything within "/secure" or with correct privilege is allowed
    if((mainfile[0..6] == "/secure") || test_bit(privs_file(mainfile), PRIV_OVERRIDE))
        return TRUE;

    // special cases 2
    // these are allowed even without special privilege as long as they are
    // contained in a special file
    switch(efun_name)
    {
        case "get_char":
        case "input_to":
        case "getuid":
            return file == INPUT_SYSTEM;
    }

    // everything else fails
    syslog(LOG_AUTH|LOG_ERR,
            "Privilege violation: valid_oberride(\"%s\", \"%s\", \"%s\")",
            file, efun_name, mainfile);
    return FALSE;
}
// --------------------------------------------------------------------------
/// @brief valid_read
///
/// Every time an object tries to read a file, the driver calls valid_read in
/// the master object to check if the read should be allowed. The arguments
/// are the filename, the object making the read, and the calling function
/// name. If valid_read returns non-zero, the read is allowed.
/// @Param file
/// @Param ob
/// @Param func
/// @Returns
// --------------------------------------------------------------------------
public int valid_read(string file, object ob, string func)
{
    string euid,
           egid;

    // master is allowed
    if(ob == master())
        return TRUE;

    // otherwise we check the acl
    if(check_acl(_READ, euid = geteuid(ob), egid = getegid(ob), ({ file, func }) ))
        return TRUE;

    // everything else fails
    if(origin() == ORIGIN_DIVER)        // log only for actual requets
        syslog(LOG_AUTH|LOG_ERR,
                "Privilege violation: valid_read(\"%s\", %O[%s:%s], \"%s\")",
                file, ob, euid, egid, func);
    return FALSE;
}
// --------------------------------------------------------------------------
/// @brief valid_seteuid
///
/// The driver calls valid_seteuid(ob, euid) in the master object from inside
/// the seteuid(euid) efunction. If valid_seteuid() returns 0, then the
/// seteuid() call will fail. If valid_seteuid() returns 1, then the
/// seteuid() will succeed.
/// @Param ob - object atempting the seteuid
/// @Param t_euid - argument to seteuid
/// @Returns 1 if seteuid is granted, 0 otherwise
/// @Attention this function checks driver internal representations!!!
// --------------------------------------------------------------------------
private int valid_seteuid(object ob, string t_euid)
{
    string  uid,
           *uids
            euid,
           *euids,
           *t_euids;

    // special case: simul_efun is always allowed
    if(ob == simul_efun())
        return TRUE;

    uid     = efun::getuid(ob);
    euid    = efun::geteuid(ob);

    // as long as euid isn't set or one has the right privilege one can become
    // oneself
    if((!euid || test_bit(query_privs(ob), PRIV_SETEUID)) && (uid == t_euid))
        return TRUE;

    uids    = explode(uid, ":");
    if(!euid)
        euids = ({ UNKNOWN_UID, UNKNOWN_DOMAIN });
    else
        euids   = explode(euid, ":");
    t_euids = explode(t_euid, ":");

    // effective ROOT_UID may change it's euid to any other
    if(euids[0] == ROOT_UID)
        return TRUE;

    // interactives may change their current group to any they are a member of
    if(ob == TI())
    {
        if(uids[0] == t_euids[0])
        {
            string *grps = MUD_INFO_D->get_groups(uids[0]);
            if(member_array(t_euids[1], grps) != -1)
                return TRUE;
        }
        // todo:
        // allow 'su user' with correct credentials
    }

    // non interactives can allways lower their privileges
    else if((t_euids[0] == NOBODY_UID) || (t_euids[1] == NOBODY_DOMAIN))
        return TRUE;

    // everything else fails
    syslog(LOG_AUTH|LOG_ERR,
            "Privilege violation: valid_seteuid(%O[%s/%s], \"%s\")",
            ob, uid, euid ? euid : "-", t_euid);
    return FALSE;
}
// --------------------------------------------------------------------------
/// @brief valid_shadow
///
/// When an object attempts to shadow `ob' (with the shadow() efun),
/// valid_shadow in the master object is called. One object parameter is
/// passed, which is the object that previous_object() is attempting to
/// shadow.  valid_shadow() should return 0 if the shadow should not be
/// permitted, in which case the shadow() call will return 0 and fail. If
/// valid_shadow() returns 1, the shadow is allowed.
/// @Param ob
/// @Returns
// --------------------------------------------------------------------------
private int valid_shadow(object ob)
{
    object po    = PO();
    int    ret   = TRUE;
    string fn    = file_name(po, 1);

    // neither master nor simul_efun is allowed to be shadowed!!!
    if(ob == master()) || (ob == simul_efun())
        ret = FALSE;

    // shadows are required to have a corresponding name
    else if(fn[<7..] != "_shadow")
        ret = FALSE;

    // ROOT_UID can't be shadowed regardless of it's effective uid
    else if(getuid(ob) == ROOT_UID)
        ret = FALSE;

    // ob might forbid shadowing
    else if(ob->query_prevent_shadow(po))
        ret = FALSE;

    // all tests passed?
    if(ret)
        return TRUE;

    // everything else fails
    syslog(LOG_AUTH|LOG_ERR, "Privilege violation: valid_shadow(%O) by %O(%s)",
            ob, po, efun::geteuid(po));
    return FALSE;
}
// --------------------------------------------------------------------------
/// @brief valid_socket
///
/// Each of the socket efunctions calls valid_socket() prior to executing. If
/// valid_socket returns 0, then the socket efunction fails. If valid_socket
/// returns 1, then the socket efunction attempts to succeed. The first
/// argument 'caller' is the object that called the socket efunction. The
/// second argument is the name of the socket efunction that is being called
/// (e.g. socket_write() or socket_bind()). The third argument is an array of
/// information. The first element of the array (when applicable) is file
/// descriptor being referenced. The second element of the array is the owner
/// of the socket (object). The third element of the array is the address
/// (string) of the remote end of the socket. The fourth element of the array
/// is the port number associated with the socket.
/// @Param ob
/// @Param func
/// @Param info
/// @Returns
// --------------------------------------------------------------------------
private int valid_socket(object ob, string func, mixed *info)
{
    string privs = query_privs(ob);

    switch(func)
    {
        case "accept":          // server only
        case "listen":
            if(test_bit(privs, PRIV_SOCK_SERV))
                return TRUE;
            break;
        case "connect":         // client only
            if(test_bit(privs, PRIV_SOCK_CLNT))
                return TRUE;
            break;
        case "acquire":         // server or client
        case "bind":
        case "create":
        case "close":
        case "release":
        case "write":
            if(test_bit(privs, PRIV_SOCK_SERV) || test_bit(privs, PRIV_SOCK_CLNT))
                return TRUE;
            break;
    }

    // everything else fails
    syslog(LOG_AUTH|LOG_ERR, "Privilege violation: valid_socket(%O, \"%s\", %O)",
            ob, func, info);
    return FALSE;
}
// --------------------------------------------------------------------------
/// @brief valid_write
///
/// Every time an object tries to write a file, the driver calls valid_write
/// in the master object to check if the write should be allowed. The
/// arguments are the filename, the object making the write, and the calling
/// function name (usually the name of the efun being used). If valid_write
/// returns non-zero, the write is allowed.
/// @Param file
/// @Param ob
/// @Param func
/// @Returns
// --------------------------------------------------------------------------
public int valid_write(string file, object ob, string func)
{
    string euid,
           egid;

    // master is allowed
    if(ob == master())
        return TRUE;

    // otherwise we check the acl
    if(check_acl(_READ, euid = geteuid(ob), egid = getegid(ob), ({ file, func }) ))
        return TRUE;

    // everything else fails
    if(origin() == ORIGIN_DIVER)    // log only for actual requets, not some lib-internal stat
        syslog(LOG_AUTH|LOG_ERR,
                "Privilege violation: valid_write(\"%s\", %O[%s:%s], \"%s\")",
                file, ob, euid, egid, func);
    return FALSE;
}
//  @}

/// @name connection handling
/// @{
// --------------------------------------------------------------------------
/// @brief connect
///
/// The driver calls connect() in the master object whenever a new user logs
/// into the driver. port is the actual port connected to; for example if you have
/// two login ports defined as 2000 and 3000, either 2000 or 3000 will be passed
/// to this routine.
/// The object returned by connect() is used as the initial user object. Note
/// that it is possible to change the user object at a later time (for example,
/// after determining who is logging in) using the exec() efun.
/// @Param port - port number used by the new connection
/// @Returns either new player object or 0, in the latter case the connection
/// will be terminated.
// --------------------------------------------------------------------------
private object connect(int port)
{
    string  err;
    object  l_ob;

    switch(port)
    {
        case __MUD_PORT__:
            // try to create a new login object
            if(err = catch(l_ob = new(LOGIN_OB)))
            {
                // in case of error:
                // give the user a feed back
                write("Got error: '" + err + "'\n");

                // destroy any created object
                if(l_ob)
                    destruct(l_ob);

                // tell the driver to terminate the connection
                return 0;
            }
            else
                l_ob->set_port(port);

            // give the driver the object this connection should be associated with
            return l_ob;
        default:
            debug_message(sprintf("This shouldn't have happened!\nGot portnr.: %05d\n", port));
            return 0;
    }
}
// --------------------------------------------------------------------------
/// @brief get_mud_stats
///
/// This function is called by the driver when some client asks for
/// MSSP-telopt. MSSP is the
/// [Mud-Server-Status-Protocol](http://tintin.sourceforge.net/mssp/).
/// According to a [proposal](http://lpmuds.net/smf/index.php?topic=943.0) by
/// cratylus@deadsouls this is also available as MSSP-over-login using
/// ___MSSP-REQUEST___ as character name.
/// @Returns - a mapping containig the MSSP info
// --------------------------------------------------------------------------
private mapping get_mud_stats(void)
{
    syslog(LOG_KERN|LOG_INFO, "mssp-telopt query");
    return MUD_INFO_D->mssp_telopt();
}

/// @name error and logfile handling
/// @{
// --------------------------------------------------------------------------
/// @brief error_handler
///
/// This function allows the mudlib to handle runtime errors. The contents of
/// the 'error' mapping is:
///
///    ([
///        "error"   : string,        // the error
///        "program" : string,        // the program
///        "object"  : object,        // the current object
///        "line"    : int,           // the line number
///        "trace"   : mapping array  // a trace back
///    ])
///
/// Each line of traceback is a mapping containing the following:
///
///    ([
///        "function"  : string,   // the function name
///        "program"   : string,   // the program
///        "object"    : object,   // the object
///        "file"      : string,   // the file to which the line number refers
///        "line"      : int,      // the line number
///        "arguments" : array,    // function arguments
///        "locals"    : array     // local variables
///    ])
///
/// arguments and local variables are only available if ARGUMENTS_IN_TRACEBACK
/// and LOCALS_IN_TRACEBACK are defined.
///
/// The 'caught' flag is 1 if the error was trapped by catch().
/// @Param err
/// @Param caught
/// @attention This function is only called if MUDLIB_ERROR_HANDLER is defined.
// --------------------------------------------------------------------------
private void error_handler(mapping err, int caught)
{
    string str,
           ret,
           author = author_file(err["file"]),
           domain = domain_file(err["file"]);

    // remove trailing (and leading) whitespace
    err["error"] = trim(err["error"]);

    // If an object didn't load, they get compile errors.
    if(err["error"][0..23] == "*Error in loading object")
        return 0;

    str = sprintf("Error %:8s : %s\nCurrent object : %O\nCurrent program: %s\nFile           : %s[%05d]\n",
            (caught ? "(caught)" : ""),
            err["error"],
            (err["object"] || "<none>"),
            (err["program"] || "<none>"),
            err["file"], err["line"]);

    ret = caught ? "" : str;

    if(!caught)
    {
        // oops! we should inform someone about this error
        object *ob = ({ TI() || TP() });

        ob += MUD_INFO_D->error_author(author);
        ob += MUD_INFO_D->error_domain(domain);
        ob = distinct_array(ob);

        message(MSGCLASS_ERROR, str, ob, ({}));
    }

    str += sprintf("Call trace:\n%s\n",
            implode(map_array(err["trace"],
            (: sprintf("--\nObject: %O\nProgram: %O\nFile: %s[%05d]",
                       $1["object"] || "<none>",
                       $1["program"] || "<none>",
                       $1["file"], $1["line"]) :)), "\n"));

    m_syslog(author, domain, LOG_KERN|LOG_ERR, str);
}
// --------------------------------------------------------------------------
/// @brief log_error
///
/// Whenever an error occurs during _compilation_, the function log_error in
/// the master object is called with the filename that the error occurred in
/// and the error message itself. Then, log_error is free to do whatever it
/// thinks it should do with that information. Usually this is deciding based
/// on the filename where the error message should be logged, and then
/// writing it to that file. Warnings also pass through this routine, and can
/// be detected since they start with "Warning:"
/// @Param file - absolute path generating the error
/// @Param message - error message during compilation
/// @todo still needs work with sefun::syslog/syslogd
// --------------------------------------------------------------------------
private void log_error(string file, string message)
{
    int level;

    if(message[0..] == "Warning:")
        level = LOG_WARN;
    else
        level = LOG_ERR;
    m_syslog(author_file(file), domain_file(file), LOG_USER|level, "(%s): %s", file, message);
}
///  @}

/// @name filename handling
/// @{
// --------------------------------------------------------------------------
/// @brief get_save_file_name
///
/// This master apply is called by ed() when a player disconnects while in
/// the editor and editing a file. This function should return an alternate
/// file name for the file to be saved, to avoid overwriting the original.
/// @Param file
/// @Param who
/// @Returns filename
// --------------------------------------------------------------------------
private string get_save_file_name(string file, object who)
{
    return sprintf("%s.edsav", file);
}
// --------------------------------------------------------------------------
/// @brief make_path_absolute
///
/// This master apply is called by the ed() efun to resolve relative path
/// names of a file to read/write, to an absolute path name.
/// @Param rel_path - relative path to some file
/// @Returns - absolute path to same file (cleaned)
// --------------------------------------------------------------------------
private string make_path_absolute(string rel_path)
{
    return canonical_path(get_cwd(PO()) + "/" + rel_path);
}
///  @}

/// @name parser-helper
/// @{
// --------------------------------------------------------------------------
/// @brief parser_error_message
/// @Param type
/// @Param ob
/// @Param arg
/// @Param flag
/// @Returns
// --------------------------------------------------------------------------
private string parser_error_message(int type, object ob, mixed arg, int flag)
{
    string ret;

    if(ob)
        ret = ob->short() + ": ";
    else
        ret = "";

    switch(type)
    {
        case ERR_IS_NOT:
            if(flag)
                return ret + "There is no such " + arg + " here.\n";
            else
                return ret + "There is no " + arg + " here.\n";
            break;
        case ERR_NOT_LIVING:
            if(flag)
                return ret + "None of the " + pluralize(arg) + " are alive.\n";
            else
                return ret + "The " + arg + " isn't alive.\n";
            break;
        case ERR_NOT_ACCESSIBLE:
            if(flag)
                return ret + "You can't reach them.\n";
            else
                return ret + "You can't reach it.\n";
            break;
        case ERR_AMBIG:
            {
                mixed  *descs = unique_array(arg, (: $1->the_short() :));
                string  str;
                int     sz;

                if((sz = sizeof(descs)) == 1)
                    return ret + "Which " + descs[0][0]->short() + " do you mean?\n";
                str = ret + "Do you mean ";
                for(int i = 0; i < sz; i++)
                {
                    if(sizeof(descs[i]) > 1)
                        str += "one of ";
                    str += descs[i][0]->the_short();
                    if(i == sz - 1)
                        str += " or ";
                    else
                        str += ", ";
                }
                return str + "?\n";
            }
            break;
        case ERR_ORDINAL:
            if(arg > 1)
                return ret + "There are only " + arg + " of them.\n";
            else
                return ret + "There is only one of them.\n";
            break;
        case ERR_ALLOCATED:
            return ret + arg;
        case ERR_THERE_IS_NO:
            return ret + "There is no " + arg + " here.\n";
        case ERR_BAD_MULTIPLE:
            return ret + "You can't use more than one object at a time with that verb.\n";
        case ERR_MANY_PATHS:
            return ret + "Oops?!? What happened now?\n";
    }
}
// --------------------------------------------------------------------------
/// @brief parse_command_all_word
///
/// This routine is called in the master object to find out what word should
/// be considered to refer to everyting. It is used by the parse_command()
/// efun.
/// @Returns word
// --------------------------------------------------------------------------
private string parse_command_all_word(void)
{
    return "all";
}
// --------------------------------------------------------------------------
/// @brief parse_command_id_list
///
/// This routine is called in the master object to find out what words should
/// be considered to refer to something. It is used by the parse_command()
/// efun to get a default value for objects missing this function...
/// @Returns
// --------------------------------------------------------------------------
private string *parse_command_id_list(void)
{
    return ({ "one", "thing" });
}
// --------------------------------------------------------------------------
/// @brief parse_command_plural_id_list
///
/// This routine is called in the master object to find out what words should
/// be considered to refer to multiple somethings. It is used by the
/// parse_command() efun to get a default value for objects missing this
/// function...
/// @Returns word
// --------------------------------------------------------------------------
private string *parse_command_plural_id_list(void)
{
    return ({ "ones", "things", "them" });
}
// --------------------------------------------------------------------------
/// @brief parse_command_plural_id_list
///
/// This routine is called in the master object to find out what adjectives
/// should be considered to refer to something. It is used by the
/// parse_command() efun to get a default value for objects missing this
/// function...
/// @Returns word
// --------------------------------------------------------------------------
private string *parse_command_adjectiv_id_list(void)
{
    return ({ "iffish" });
}
// --------------------------------------------------------------------------
/// @brief parse_command_users
/// @Returns
// --------------------------------------------------------------------------
private object *parse_command_users(void)
{
    return users();
}
// --------------------------------------------------------------------------
/// @brief parse_command_prepos_list
///
/// This routine is called in the master object to find out what words should
/// be considered prepositions. It is used by the parsing package and the
/// parse_command() efun.
/// @Returns
// --------------------------------------------------------------------------
private string *parse_command_prepos_list(void)
{
    return ({ "aboard", "about", "above", "across", "against", "alongside",
            "around", "at", "behind", "below", "beneath", "beside", "besides",
            "between", "by", "down", "enter", "every", "exit", "for", "from",
            "here", "in", "inside", "into", "near", "of", "off", "on", "only",
            "onto", "out of", "out", "outside", "over", "room", "through",
            "to", "under", "underneath", "up", "upon", "with", "within",
            "without", "alongside of", "back of", "down from", "in back of",
            "in front of", "inside of", "near to", "next to", "off of",
            "on top of", "outside of", "over to", "round about", "up to",
            "-a", "-r"});
}
///  @}

/// @name general functions
/// @{
// --------------------------------------------------------------------------
/// @brief compile_object
///
/// The driver calls compile_object in the event that the mudlib instructs
/// the driver to load a file that does not exist. For example, the driver
/// will call compile_object("/obj/file.r") in master if the mudlib calls
/// call_other("/obj/file.r", "some_function") or new("/obj/file.r") and
/// /obj/file.r.c names a file that does not exist. The compile_object()
/// function is expected to return 0 if the mudlib does not wish to associate
/// an object with the file name "/obj/file.r". If the mudlib does wish to
/// associate an object with the filename "/obj/file.r", then the mudlib
/// should return the object it wishes associated. After an association is
/// made between an object and a filename, then it will be as if the file,
/// file.r.c, did exist (to the driver) and when loaded produced the object
/// that compile_object() returned.
/// @Param pathname
/// @Returns new object or 0
// --------------------------------------------------------------------------
private object compile_object(string pathname)
{
    if(pathname[0..8] == "/std/map/")           // a room on the map
        return MAP_D->get_room(pathname[9..]);  // ask the map daemon
    return 0;                                   // no others used as of yet
}
// --------------------------------------------------------------------------
/// @brief get_include_path
///
/// dynamic generation of include path based on the compiled object
/// @param file - absolute path to the object storage in disk
/// @return - array of include directories
/// ':DEFAULT:' will be replaced by runtime configuration
// --------------------------------------------------------------------------
private string *get_include_path(string file)
{
    string *path;

    path = explode(file, "/");
    switch(path[1])
    {
        case "secure":
            return ({ "/secure/include" });
            break;
        case "players":
            // ""/"players"/"w"/"wiz"/"file.c"
            // 0  1         2   3     4
            return ({ ".",
                      "/players/" + path[2] + "/" + path[3] + "/include",
                      "/std/include",
                      "/secure/include" });
            break;
        case "Domains":
            // ""/"Domains"/"Example"/"members"/"wiz"/"file.c"
            // 0  1         2         3         4     5
            if((sizeof(sp) > 5) && (path[3] == "members"))
                return ({ ".",
                          "/Domains/" + path[2] + "/members/" + path[4] + "/include",
                          "/Domains/" + path[2] + "/include",
                          "/std/include",
                          "/secure/include" });
            else
                return ({ "/Domains/" + path[2] + "/include",
                          "/std/include",
                          "/secure/include" });
                break;
    }
    return ({ "/std/include", "/secure/include" });
}
// --------------------------------------------------------------------------
/// @brief object_name
///
/// This master apply is called by the sprintf() efun, when printing the
/// "value" of an object. This function should return a string corresponding
/// to the name of the object (eg a user's name).
/// @Param ob - object
/// @Returns
// --------------------------------------------------------------------------
private string object_name(object ob)
{
    if(!ob)                     // ob no longer exists
        return "<destructed>";
    if(interactive(ob))         // player
        return sprintf("<%s's link>", getuid(ob));
    return file_name(ob);
}
// --------------------------------------------------------------------------
/// @brief retrieve_ed_setup
///
/// This master apply is called by the ed() efun to retrieve a user's ed
/// setup/configuration settings. This function should return the setup
/// (contained in an int).
/// @Param user
/// @Returns
// --------------------------------------------------------------------------
private int retrieve_ed_setup(object user)
{
    if(!user)
        return 0;
    return user->get_ed_setup();
}
// --------------------------------------------------------------------------
/// @brief save_ed_setup
///
/// This master apply is called by the ed() efun to save a user's ed
/// setup/configuration settings (contained in an int). This function should
/// return an int for success (1 or TRUE)/failure (0 or FALSE).
/// @Param user
/// @Param config
/// @Returns
// --------------------------------------------------------------------------
private int save_ed_setup(object user, int config)
{
    if(!user)
        return FALSE;
    return user->set_ed_setup(config);
}
///  @}

// std applies
private void create()
{
#ifdef __HAS_RUSAGE__
    mapping before,         ///< rusage before running create
            after;          ///< rusage after running create

    before = rusage();

    startup_info = allocate(5);
#else
    startup_info = allocate(3);
#endif

    // we use the efun directly, it's faster
    efun::seteuid(creator_file(__MASTER_FILE__));

    startup_info[0] =           // # to be preloaded objects
    startup_info[1] =           // # how often preload called
    startup_info[2] = 0;        // # how many errors

    restore_object(MASTER_SAVE);
    if(!acl_read)
        acl_read   = init_acl("r");
    if(!acl_write)
        acl_write  = init_acl("w");
    if(!privileges)
        privileges = init_privileges();

#ifdef __HAS_RUSAGE__
    after = rusage();
    if(sizeof(before) && sizeof(after))
    {
        startup_info[3] = after["utime"] - before["utime"]
        startup_info[4] = after["stime"] - before["stime"]
    }
#endif
}

private int clean_up(int arg)
{
    return 0;
}

private void reset()
{
    save_object(MASTER_SAVE);
}

// event handler
public void event_destruct(void)
{
    if(origin() != ORIGIN_EFUN)
        return;
    save_master();
}
public void event_shutdown(void)
{
    if(origin() != ORIGIN_EFUN)
        return;
    save_master();
}
///  @}
