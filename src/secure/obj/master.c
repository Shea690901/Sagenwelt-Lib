/// @addtogroup privileged
/// @{
/// @file master.c
/// @brief first object loaded
///
/// The master object is the first object loaded by the gamedriver, as
/// such it has complete control over the mudlib and needs to implement a
/// couple of functions for the gamedriver to work properly.
/// @author Gwenhwyvar
/// @version 0.0.0
/// @date 2015-11-27
/// @attention until this object is successfully loaded, there is no include path

#include "/secure/include/pragmas.h"    // setting standard pragmas
#include "/secure/include/privs.h"      // privlege related defines
#include "/secure/include/master.h"

private mapping  init_acl(string);
private int      check_acl(int, string, mixed);
private mapping  init_privileges(void);

// global vars {{{
private nosave int debug = FALSE;       ///< got we called with debug flag?

#ifdef __HAS_RUSAGE__
private nosave mapping startup_time;    ///< time needed to start the mud
#endif

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
                acl_write,              ///< acls for file write access
                acl_load;               ///< acls for file load access

private mapping privileges;             ///< object privileges
// }}}

// std functions {{{
private void create()
{
#ifdef __HAS_RUSAGE__
    startup_time = ([]);
#endif
    restore_object(MASTER_SAVE);
    if(!acl_read)
        acl_read   = init_acl("r");
    if(!acl_write)
        acl_write  = init_acl("w");
    if(!acl_load)
        acl_load   = init_acl("x");
    if(!privileges)
        privileges = init_privileges();
}

private int clean_up(int arg)
{
    return 0;
}

private void reset()
{
}
// }}}

// helper functions {{{
// init_acl() {{{
private mapping init_acl(string type)
{
    string   cfg,
            *lines,
             line;
    mapping  ret;

    ret = ([]);
    switch(type)
    {
        case "r":
            cfg = read_file(ACL_READ_CFG);
            break;
        case "w":
            cfg = read_file(ACL_WRITE_CFG);
            break;
        case "x":
            cfg = read_file(ACL_LOAD_CFG);
            break;
        default:
            error("Unknown argument '" + type + "' to init_acl");
            return;
    }
}
// }}}
// acl(request, info) {{{
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
        case _LOAD:
            acl_list = acl_load;
            break;
        default:
    }
    ret = match_path(acl_list, info[0]);
    return ret[info[1]];
}
// }}}
// check_acl(request, euid, info) {{{
// --------------------------------------------------------------------------
// request is one of _LINK, _READ or _WRITE
// euid is the effective uid of the object doing the request
// the contents of info depends on the request, refer to the apropriate
// valid_*
// returns TRUE if the access is granted, FALSE otherwise
// --------------------------------------------------------------------------
public int check_acl(int request, string euid, mixed info)
{
    string *access, // access control list for given file
            usr,    // user doing the request
            grp,    // the group the user belongs to, if any
           *t;

    switch(request) // check for valid request
    {
        case _LIHK:
        case _READ:
        case _WRITE:
            if(info && arrayp(info) &&          // we need info to be an array of two strings
                (sizeof(info) == 2) &&
                stringp(info[0]) && stringp(info[1]))
                break;
        default:                                // everything else can't be valid
            return FALSE;
    }

    // linking is allowed if euid may read the source and write the target
    if(request == _LINK)
        return check_acl(_READ, euid, ({ info[0], "ln" })) &&
            check_acl(_WRITE, euid, ({ info[1], "ln" }));

    // get the acl for the given path and request
    access = acl(request, info);

    // split euid in user and group part
    t   = explode(euid, ":");
    usr = t[0];
    grp = (sizeof(t) == 2) ? t[1] : "";

    // test if either user or group is allowed
    if((member_array(usr, access) != -1) || (member_array(grp, access) != -1))
        return TRUE;

    // everything else is disallowed
    return FALSE;
}
// }}}
// init_privileges() {{{
private mapping init_privileges(void)
{
    string   cfg,
            *lines,
             line,
             priv;
    mapping  ret;

    if(file_size(PRIVS_CFG) < 1)
    {
        error("privilege config file error");
        return;
    }
    cfg = read_file(ACL_READ_CFG);
    if(sizeof(cfg)
}
// }}}
// }}}

// master applies {{{
/// @name initialization
// {{{
/// @{
// flag {{{
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
    mapping before;         ///< rusage bevore loading object
    mapping after;          ///< rusage after loading object

    before = rusage();
#endif
    switch driver_flag
    {
        "debug":
            debug = TRUE;
            break;
        default:
            error(sprintf("Unknown driver-flag '%s'â€¦", driver_flag));
            break;
    }
#ifdef __HAS_RUSAGE__
    after = rusage();
    if(sizeof(before) && sizeof(after))
    {
        startup_time["utime"] = ["utime"] - before["utime"]
        startup_time["stime"] = ["utime"] - before["utime"]
    }
#endif
}
// }}}
// epilog {{{
// --------------------------------------------------------------------------
/// @brief epilog
///
/// The driver calls epilog() in master after the master object has been
/// loaded. Required initialization may be done at this point. If epilog()
/// returns an array of filenames, the driver will attempt to load those
/// files via the preload() function.
///
/// The variable 'load_empty' is non-zero if the -e option was specified when
/// starting up the driver, which has been historically used to mean no
/// preloads, although the mudlib is free to use another interpretation.
/// @Param load_empty - if true no preloading is done
/// @Returns array of files to preload
// --------------------------------------------------------------------------
private string *epilog(int load_empty)
{
    string  content;
    string  *ret;
    mapping seen;
#ifdef __HAS_RUSAGE__
    mapping before;         ///< rusage bevore loading object
    mapping after;          ///< rusage after loading object

    before = rusage();
#endif

    // either we shan't preload or the preload-file is empty
    if(load_empty || !(content = read_file(PRELOADS)))
        return ({});

    ret = ({});
    seen = ([]);
    // split content into array of single lines
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
        // add all other lines direct to return array
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
#ifdef __HAS_RUSAGE__
    after = rusage();
    if(sizeof(before) && sizeof(after))
    {
        startup_time["utime"] = ["utime"] - before["utime"]
        startup_time["stime"] = ["utime"] - before["utime"]
    }
#endif
    return ret;
}
// }}}
// preload {{{
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
    mapping before;         ///< rusage bevore loading object
    mapping after;          ///< rusage after loading object
    int     t;

    before = rusage();
#endif

    write("Preloading: '" + str + "'...");

    // everything either has
    if(err = catch(load_object(str)))
        // compile errors
        write("Got error: '" + err + "'...");
    else
        // or loads fine...
        write("Done!...");
#ifdef __HAS_RUSAGE__
    after = rusage();
    if(sizeof(before) && sizeof(after))
    {
        t = after["utime"] - before["utime"];
        write("utime: " + t + "ms...")
        startup_time["utime"] += t;
        t = after["stime"] - before["stime"];
        write("utime: " + t + "ms")
        startup_time["stime"] += t;
    }
#endif
    write("\n");
}
// }}}
///  @} }}}

/// @name deinitialization
// {{{
/// @{
// crash {{{
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
    syslog(LOG_KERN|LOG_EMERG, "master::crash(\"%s\", %O, %O)", crsh_message, commmd_giver, current_object);
    // tell_users("Game Driver shouts: Ack! I think the game is crashing!\n");
    users()->quit();
}
// }}}
///  @} }}}

/// @name uid and security handling
// {{{
/// @{
// get_bb_uid {{{
// --------------------------------------------------------------------------
/// @brief get_bb_uid
///
/// This master apply is called by the driver on startup, after it has loaded
/// the master object, to get the backbone uid defined by the mud. The
/// function should return a string, eg "BACKBONE"
/// @Returns backbone uid
/// @Attention This routine is only used if PACKAGE_UIDS is used.
// --------------------------------------------------------------------------
private string get_bb_uid()
{
    return creator_file("/std");;
}
// }}}
// get_root_uid {{{
// --------------------------------------------------------------------------
/// @brief get_root_uid
///
/// This master apply is called by the driver each time it loads the master
/// object, to verify that the master object has loaded, and to get the root
/// uid defined by the mud. The function should return a string, eg "ROOT"
/// @Returns root uid
/// @Attention This routine is only used if PACKAGE_UIDS is used.
// --------------------------------------------------------------------------
private string get_root_uid()
{
    return credit("/secure");;
}
// }}}
// author_file {{{
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
/// Also this midlib uses this function to get ownership of directories for
/// the 'ls' commamd
/// @Attention This routine is only used by the driver if PACKAGE_MUDLIB_STATS
/// is used.
/// @Param file - absolute path to source of some object
/// @Returns name of author
// --------------------------------------------------------------------------
public string author_file (string file)
{
    string *path;
    int     sp,
            fs;

    if(file[0] != '/')                  // strange argument, without any '/'...
        return UNKNOWN_UID;

    path = explode(file, "/");          // path[0] == "" !!!
    sp   = sizeof(path);
    fs   = file_size(file);

    switch(path[1])
    {
        case "players":                 // some player file
            // ""/"players"/"w"/"wiz"/"file.c"
            // 0  1         2   3     4
            if((sp > 4) ||
                    (sp == 4) && (fs == -2))
                return path[3];         // file/directory is owned by some player
            else if(fs == -2)
                return BB_UID;          // the other directories belong to backbone
            break;
        case "Domains":                 // file belongs to some domain
            // ""/"Domains"/"Example"/"members"/"wiz"/"file.c"
            // 0  1         2         3         4     5
            if((sp > 4) &&
                (path[3] == "members") &&
                ((sp > 5) ||
                ((sp == 5) && (fs == -2))))
                return path[4];         // but is owned by one of it's members
            // ""/"Domains"/"Example"/"file.c"
            // 0  1         2         3
            else if((sp > 3) ||
                    ((sp == 3) && (fs == -2)))
                return path[2];         // this f£ile truly belongs to the domain
            else if(fs == -2)
                return BB_UID;          // the other directories belong to backbone
            break;
        case "std":                     // everything here belongs backbone
            return BB_UID;
            break;
        case "secure":                  // this all security relevant!!!
            return ROOT_UID;
            break;
    }
    return UNKNOWN_UID;                 // everything else shouldn't have any privileges
}
// }}}
// domain_file {{{
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
/// Also this midlib uses this function to get groupmembership of directories
/// for the 'ls' commamd
/// @Attention This routine is only used by the driver if PACKAGE_MUDLIB_STATS
/// is used.
/// @Param file
/// @Returns domain the file belongs to
// --------------------------------------------------------------------------
public string domain_file(string file)
{
    string *path;
    int     sp,
            fs;

    if(file[0] != '/')                  // strange argument, without any '/'...
        return UNKNOWN_DOMAIN;

    path = explode(file, "/");      // path[0] == "" !!!
    sp   = sizeof(path);
    fs   = file_size(file);

    switch(path[1])
    {
        case "players":                 // some player file
            // ""/"players"/"w"/"wiz"/"file.c"
            // 0  1         2   3     4
            if((sp > 4) ||
                    (sp == 4) && (fs == -2))
                return (MUD_INFO_D->is_wiz(path[3])) ?
                        WIZARD_DOMAIN : // wizard
                        PLAYER_DOMAIN;  // mortal
            else if(fs == -2)
                return BB_DOMAIN;       // the other directories belong to backbone
            break;
        case "Domains":                 // file belongs to some domain
            // ""/"Domains"/"Example"/"file.c"
            // 0  1         2         3
            if((sp > 3) ||
                    ((sp == 3) && (fs == -2)))
                return path[2];         // this f£ile truly belongs to the domain
            else if(sp == 2)
                return BB_DOMAIN;       // the '/Domains' directory belongs to backbone
            break;
        case "std":                     // everything here belongs backbone
        case "secure":                  // this to is backbone
            return BB_DOMAIN;
            break;
    }
    return UNKNOWN_DOMAIN;              // everything else shouldn't have any privileges
}
// }}}
// creator_file {{{
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
/// @Param filename - absolute path to object o be loaded/cloned
/// @Returns name of creator
// --------------------------------------------------------------------------
private string creator_file(string filename)
{
    string *path;
    int     sp;
    string  uid = BB_UID;
    string  gid = BB_DOMAIN;

    path = explode(filename, "/");
    sp   = sizeof(path);

    switch(path[1])
    {
        "secure":
            uid = ROOT_UID; // privileged object
            break;
        "std":              // standard object
            break;
        "players":
            if(sp > 4)      // object belongs to some player
            {
                uid = author_file(filename);
                gid = MUD_INFO_D->get_group(path[3]);
            }
            else            // here shouldn't be any object stored
            {
                uid = UNKNOWN_UID;
                gid = UNKNOWN_DOMAIN;
            }
            break;
        "Domains":
            if(sp > 3)      // object belongs to some domain
            {
                uid = author_file(filename);
                gid = MUD_INFO_D->get_group(path[3]);
            }
            else            // here shouldn't be any object stored
            {
                uid = UNKNOWN_UID;
                gid = UNKNOWN_DOMAIN;
            }
            break;
        default:
            uid = UNKNOWN_UID;
            gid = UNKNOWN_DOMAIN;
            break;
    }
    return sprintf("%s:%s", uid, gid));
}
// }}}
// privs_file {{{
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
// }}}
// valid_bind {{{
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
    if((doer == TO()) || (doer == find_object(SEFUNS)))
        return TRUE;

    // also allowed with PRIV_BIND
    if(test_bit(query_privs(doer), PRIV_BIND))
        return TRUE;

    // everything else fails
    syslog(LOG_AUTH|LOG_ERR, "Privilege violation: valid_bind(%O, ...)", doer);
    return FALSE;
}
// }}}
// valid_database {{{
// --------------------------------------------------------------------------
/// @brief valid_database
/// @Param doer
/// @Param action
/// @Param info
/// @Returns
// --------------------------------------------------------------------------
private mixed valid_database(object doer, string action, mixed *info)
{
    switch(action)
    {
        case "connect": // new database connection only by master for now
        case "close":   // the same for closing
            if(doer == TO())
                return (action == "connect") ? "password" : 1;
            break;
        // every other action for now only master and PRIV_DB
        default:
            if((doer == TO()) || (test_bit(query_privs(doer), PRIV_DB)))
                return TRUE;
    }

    // everything else fails
    syslog(LOG_AUTH|LOG_ERR,
            "Privilege violation: valid_database(%O,\"%s\", %O)",
            doer, action, info);
    return FALSE;
}
// }}}
// valid_hide {{{
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
// }}}
// valid_link {{{
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
    object  ob;
    string  euid;

    // who want's to link something?
    ob = TP();

    // it has to be done by someone
    if(!ob)
        return FALSE;

    // check acl's
    if(check_acl(_LINK, euid = geteuid(ob), ({ from, to })))
        return TRUE;

    // everything else fails
    syslog(LOG_AUTH|LOG_ERR,
            "Privilege violation: valid_link(\"%s\", \"%s\") by %O[%s]",
            from, to, ob, euid);
    return FALSE;
}
// }}}
// valid_object {{{
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
    string file,
           dir,
           euid;
    object po;
    int    clone;

    file  = file_name(ob);
    clone = clonep(ob);

    // no objects stored in hidden files
    // everything from /secure and it's subdirs may be loaded(!)
    // otherwise we ask the acl's
    if((strsrch(file, "/.") == -1) && (
        ((file[0..7] == "/secure/") && !clone) ||
        check_acl(_LOAD, euid = geteuid(po = PO()), ({ po, file}))
        ))
        return TRUE;

    // everything else fails
    syslog(LOG_AUTH|LOG_ERR,
            "Privilege violation: valid_object(%O) by %O[%s]",
            ob, po, euid);
    return FALSE;
}
// }}}
// valid_override {{{
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
    // everything within "/secure" or with correct privilege is allowed
    // we don't have an object yet!!!
    if(!strsrch(mainfile, "/secure") || test_bit(privs_file(mainfile), PRIV_OVERRIDE))
        return TRUE;

    // everything else fails
    syslog(LOG_AUTH|LOG_ERR,
            "Privilege violation: valid_oberride(\"%s\", \"%s\", \"%s\")",
            file, efun_name, mainfile);
    return FALSE;
}
// }}}
// valid_read {{{
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
    string  euid;

    // master is allowed
    if(ob == TO())
        return TRUE;

    // otherwise we check the acl
    if(check_acl(_READ, euid = geteuid(ob), ({ file, func }) ))
        return TRUE;

    // everything else fails
    if(origin() == ORIGIN_DIVER)        // log only for actual requets
        syslog(LOG_AUTH|LOG_ERR,
                "Privilege violation: valid_read(\"%s\", %O[%s], \"%s\")",
                file, ob, euid, func);
    return FALSE;
}
// }}}
// valid_seteuid {{{
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
// --------------------------------------------------------------------------
private int valid_seteuid(object ob, string t_euid)
{
    string  uid,
           *uids
            euid,
           *euids,
           *t_euids,
           *grps;

    uid     = getuid(ob);
    euid    = geteuid(ob);

    // as long as euid isn't set or one has the right privilege one can become
    // oneself
    if((!euid || test_bit(query_privs(ob), PRIV_SETEUID)) && (uid == t_euid))
        return TRUE;

    uids    = explode(uid, ":");
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
            grps = MUD_INFO_D->get_groups(uids[0]);
            if(member_array(t_euids[1], grps) != -1)
                return TRUE;
        }
    }

    // one can allways lower ones privileges
    else if((t_euids[0] == NOBODY_UID) || (t_euids[1] == NOBODY_DOMAIN))
        return TRUE;

    // everything else fails
    syslog(LOG_AUTH|LOG_ERR,
            "Privilege violation: valid_seteuid(%O[%s/%s], \"%s\")",
            ob, uid, euid ? euid : "-", t_euid);
    return FALSE;
}
// }}}
// valid_shadow {{{
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
    object po  = PO();
    int    ret = TRUE;
    string fn  = basename(file_name(po));

    // shadows are required to have a corresponding name
    if(fn[<7..] != "_shadow")
        ret = FALSE;

    // ROOT_UID can't be shadowed
    else if(getuid(ob) == ROOT_UID)
        ret = FALSE;

    // ob might forbid shadowing
    else if(ob->query_prevent_shadow(po))
        ret = FALSE;

    // all tests passed?
    if(ret)
        return TRUE;

    // everything else fails
    syslog(LOG_AUTH|LOG_ERR, "Privilege violation: valid_shadow(%O) by %O",
            ob, PO());
    return FALSE;
}
// }}}
// valid_socket {{{
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
// }}}
// valid_write {{{
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
    string  euid;

    // master is allowed
    if(ob == TO())
        return TRUE;

    // otherwise we check the acl
    if(check_acl(_READ, euid = geteuid(ob), ({ file, func }) ))
        return TRUE;

    // everything else fails
    if(origin() == ORIGIN_DIVER)    // log only for actual requets
        syslog(LOG_AUTH|LOG_ERR,
                "Privilege violation: valid_write(\"%s\", %O[%s], \"%s\")",
                file, ob, euid, func);
    return FALSE;
}
// }}}
//  @} }}}

/// @name connection handling
// {{{
/// @{
// connect {{{
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

    // port specific initialization (if any)
    switch(port)
    {
        case TELNET_PORT:
        case SSL_PORT:
            break;
        default:
            write(sprintf("This shouldn't have happened¦ got portnr.: %05d\n", port));
            return 0;
    }

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

    // give the driver the object this connection should be associated with
    return l_ob;
}
// }}}
// get_mud_stats {{{
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
private mapping get_mud_stats()
{
    syslog(LOG_KERN|LOG_INFO, "mssp-telopt query");
    return MUD_INFO_D->mssp_telopt();
}
// }}}
// }}}

/// @name error and logfile handling
// {{{
/// @{
// error_handler {{{
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
///Each line of traceback is a mapping containing the following:
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
///arguments and local variables are only available if ARGUMENTS_IN_TRACEBACK and LOCALS_IN_TRACEBACK are defined.
///
///The 'caught' flag is 1 if the error was trapped by catch().
/// @Param error
/// @Param caught
/// @attention This function is only called if MUDLIB_ERROR_HANDLER is defined.
// --------------------------------------------------------------------------
// error_handler copied from testsuite
private void error_handler(mapping error, int caught)
{
    object ob;
    string str;

    ob = TI() || TP();

    if(flag)
        str = "*Error caught\n";
    else
        str = "";
    str += sprintf("Error: %s\nCurrent object: %O\nCurrent program: %s\nFile: %O Line: %d\n%O\n",
            map["error"], (map["object"] || "No current object"),
            (map["program"] || "No current program"),
            map["file"], map["line"],
            implode(map_array(map["trace"],
            (: sprintf("Line: %O  File: %O Object: %O Program: %O", $1["line"], $1["file"], $1["object"] || "No object", $1["program"] ||
            "No program") :)), "\n"));
    write_file("/var/log/error.log", str);
    if(!flag && ob)
        tell_object(ob, str);
}
// }}}
// log_error {{{
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
    syslog(LOG_USER|LOG_ERR, "(%s): %s", file, message);
}
// }}}
///  @}
// }}}

/// @name filename handling
// {{{
/// @{
// get_save_file_name {{{
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
// }}}
// make_path_absolute {{{
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
    return canonical_path(absolute_path(rel_path));
}
// }}}
///  @} }}}

/// @name parser-helper
// {{{
/// @{
// parser_error_message {{{
// }}}
// parse_command_all_word {{{
// --------------------------------------------------------------------------
/// @brief parse_command_all_word
///
/// This routine is called in the master object to find out what word should
/// be considered to refer to everyting. It is used by the parse_command()
/// efun.
/// @Returns word
// --------------------------------------------------------------------------
string parse_command_all_word ()
{
    return "all";
}
// }}}
// parse_command_users {{{
// }}}
// parse_command_prepos_list {{{
// --------------------------------------------------------------------------
/// @brief parse_command_prepos_list
///
/// This routine is called in the master object to find out what words should
/// be considered prepositions. It is used by the parsing package and the
/// parse_command() efun.
/// @Returns
// --------------------------------------------------------------------------
private string *parse_command_prepos_list ()
{
    return ({ "down", "up", "in", "on", "out", "within", "without", "into",
            "onto", "inside", "outside", "across", "against", "around", "at",
            "between", "here", "over", "near", "through", "under", "with",
            "about", "enter", "every", "exit", "for", "from", "of", "off",
            "only", "room", "to", "-r", "-a" });
}
// }}}
// parse_get_first_inventory {{{
// }}}
// parse_get_next_inventory {{{
// }}}
// parse_get_environment {{{
// }}}
///  @} }}}

/// @name general functions
// {{{
/// @{
// compile_object {{{
// --------------------------------------------------------------------------
/// @brief compile_object
///
/// The driver calls compile_object in the event that the mudlib instructs
/// the driver to load a file that does not exist. For example, the driver
/// will call compile_object("/obj/file.r") in master if the mudlib calls
/// call_other("/obj/file.r", "some_function") or new("/obj/file.r") and
/// obj/file.r.c names a file that does not exist. The compile_object()
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
// }}}
// get_include_path {{{
/// @brief get_include_path
///
/// dynamic generation of include path based on the compiled object
/// @param file - absolute path to the object storage in disk
/// @return - array of include directories
/// ':DEFAULT:' will be replaced by runtime configuration
private string *get_include_path(string file)
{
    string *path;

    path = explode(file, "/");
    switch(path[1])
    {
        case "secure":
            return ({ ".", "/secure/include" });
            break;
        case "players":
            // ""/"players"/"w"/"wiz"/"file.c"
            // 0  1         2   3     4
            return ({ ".",
                        "/players/" + path[2] + "/" + path[3] + "/include",
                        ":DEFAULT:" });
            break;
        case "Domains":
            // ""/"Domains"/"Example"/"members"/"wiz"/"file.c"
            // 0  1         2         3         4     5
            if((sizeof(sp) > 5) && (path[3] == "members"))
                return ({ ".",
                            "/Domains/" + path[2] + "/members/" + path[4] + "/include",
                            "/Domains/" + path[2] + "/include",
                            ":DEFAULT:" });
            else
                return ({ ".",
                            "/Domains/" + path[2] + "/include",
                            ":DEFAULT:" });
                break;
    }
    return ({ ":DEFAULT:" });
}
// }}}
// object_name {{{
// --------------------------------------------------------------------------
/// @brief object_name
///
/// This master apply is called by the sprintf() efun, when printing the
/// "value" of an object. This function should return a string corresponding
/// to the name of the object (eg a user's name).
/// @Param ob - object
/// @Returns
// --------------------------------------------------------------------------
private string object_name (object ob)
{
    if(!ob)                     // ob no longer exists
        return "<destructed>";
    if(interactive(ob))         // player
        return sprintf("<%s's link>", ob->query_uid());
    return file_name(ob);
}
// }}}
// retrieve_ed_setup {{{
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
// }}}
// save_ed_setup {{{
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
// }}}
///  @} }}}
///  }}}

///  @}
