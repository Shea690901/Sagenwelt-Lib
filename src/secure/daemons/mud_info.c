/// @addtogroup privileged
/// @{
/// @file mud_info.c
/// @brief this daemon holds status about all mud internal infos (finger, who, mssp, ...)
/// @author Gwenhwyvar
/// @version 0.0.0
/// @date 2015-12-20

private mapping domains;            // everything domain related
private mapping players;            // everything player related
private mapping help_files;         // which help file contains what?

// --------------------------------------------------------------------------
/// @brief mssp_telopt
///
/// MSSP-info formated for telopt interface provided by the driver via
/// master::get_mud_status()
/// @Returns mapping with appropriate key:value pairs
// --------------------------------------------------------------------------
public string mssp_telopt(void)
{
    string  *intermud = ({});           // which intermud versions are
                                        // supported?

    /*
    if(find_object(INTERMUD_D))         // intermud v3
        intermud += ({ "i3" });
    if(find_object(IMC2_D))             // intermud v2
        intermud += ({ "IMC2" });
    // others ???
    */

    return ([
            // required
        "NAME"                  : __MUD_NAME__,
        "PLAYERS"               : itoa(sizeof(users())),        // currently online
        "UPTIME"                : itoa(time() - uptime()),

            // generic
        "CRAWL DELAY"           : "-1",                         // use crawler default
        "HOSTNAME"              : __MUD_IP__,   ///< @todo host_lookup?
        "PORT"                  : itoa(query_host_port()),
        "CODEBASE"              : "Sagenwelt 0.0.0",            // Please don't change this one...
        "CONTACT"               : __ADMIN_EMAIL,                // look into 'secure/include/config.h'
        "CREATED"               : "2015",
        "ICON"                  : "http://"+__MUD_IP__+"/favicon.ico",
        "IP"                    : __MUD_IP__,
        "LANGUAGE"              : __LANGUAGE,
        "LOCATION"              : __LOCATION,
        "MINIMUM AGE"           : "13",         ///< @todo check age limits
        "WEBSITE"               : "http://"+__MUD_IP__+"/",

            // categorization
        "FAMILY"                : "LPMud",
        "GENRE"                 : "Fantasy",
        "SUBGENRE"              : "Adventure",
        "GAMEPLAY"              : "Adventure",
        "GAMEPLAY-NOTES"        : "Part Educational",
        "STATUS"                : "Alpha",
        "GAMESYSTEM"            : "Custom",
        "GAMESYSTEM-NOTE§"      : "D&D-based",
        "INTERMUD"              : (sizeof(intermud) ? implode(intermud, "\t") : "0"),

            // world
        "AREAS"                 : sizeof(keys(domains)),
        "HELPFILES"             : sizeof(keys(help_files)),
        "HELPFILES-NOTES"       : "each command, and other docs",
        "MOBILES"               : "N/A",
        "MOBILES-NOTES"         : "npc's can be cloned, so there can be thousands",
        "OBJECTS"               : "N/A",
        "OBJECTS-NOTES"         : "objects can be cloned, so there can be thousands",
        "ROOMS"                 : "N/A",
        "ROOMS-NOTES"           : "areas have generated rooms, could be millions",
        "CLASSES"               : "0",
        "LEVELS"                : "0",
        "RACES"                 : "0",
        "RACES-NOTES"           : "not all are playable",
        "SKILLS"                : "N/A",
        "SKILLS-NOTES"          : "many, but not like Diku",

            // protocols
        "ANSI"                  : "0",
        "GMCP"                  : "0",
        "MCCP"                  : "0",
        "MCP"                   : "0",
        "MSDP"                  : "0",
        "MSP"                   : "0",
        "MXP"                   : "0",
        "PUEBLO"                : "0",
        "UTF-8"                 : "1",
        "VT100"                 : "1",
        "XTERM 256 COLORS"      : "0",

            // commercial
        "PAY TO PLAY"           : "0",                          // !!! disallowed by driver !!!
        "PAY FOR PERKS"         : "0",

            // hiring
        "HIRING BUILDERS"       : "0",
        "HIRING BUILDERS-NOTES" : "not yet",
        "HIRING CODERS"         : "0",
        "HIRING CODERS-NOTES"   : "not yet",

            // uncategorized
        "ADULT MATERIAL"        : "0",
        "DBSIZE"                : "N/A",
        "DESCRIPTION"           : "A mud to preserve fairy tales, legends and the like from around the world!",
        "EQUIPMENT SYSTEM"      : "N/A",
        "EXITS"                 : "N/A",
        "EXTRA DESCRIPTIONS"    : "N/A",
        "MUDPROGS"              : "N/A",
        "MUDPROGS-NOTES"        : "doesn't apply, LPMud",
        "MUDTRIGS"              : "N/A",
        "MUDTRIGS-NOTES"        : "doesn't apply, LPMud",
        "MULTICLASSING"         : "0",
        "MULTIPLAYING"          : "Full",
        "PLAYER CITIES"         : "0",
        "PLAYER CLANS"          : "0",
        "PLAYER CRAFTING"       : "0",
        "PLAYER GUILDS"         : "0",
        "PLAYERKILLING"         : "Full",
        "QUEST SYSTEM"          : "Automated\tIntegrated",
        "RESETS"                : "N/A",
        "RESETS-NOTES"          : "doesn't apply, LPMud",
        "ROLEPLAYING"           : "None",
        "SSL"                   : "0",
        "TRAINING SYSTEM"       : "Skill",
        "WHO"                   : implode(map(filter(users(), (: (environment($1) && !($1->GetInvis())) :)), (: $1->GetCapName():)), "\t"),
        "WORLD ORIGINALITY"     : "Mostly Original",
    ]);
}
// --------------------------------------------------------------------------
/// @brief mssp_login
///
/// This function takes the mapping used for the telopt-interface and converts
/// it to a string representation which may be used for the login-interface.
/// @Returns MSSP-info formated as string
// --------------------------------------------------------------------------
public string mssp_login(void)
{
    mapping mssp = mssp_telopt();
    string  ret  = "MSSP-REPLY-START\r\n";

    foreach(string key, val in mssp)
        ret += sprintf("%s\t%s\r\n", key, val);

    ret += "MSSP-REPLY-END\r\n";

    return ret;
}
// --------------------------------------------------------------------------
/// @brief author_file
///
/// This is a helper function for sefun::author_of with the same semantics
/// as master::author_file
/// @Param file - absolute path to source of some object
/// @Returns name of author - for the security system this will be the uid
/// @Todo this is a preliminary implementation without using any mud_info_d
/// functionality
// --------------------------------------------------------------------------
public string author_file(string file)
{
    string *path;
    int     sp,
            fs;

    path = explode(file, "/");          // path[0] == "" !!!
    sp   = sizeof(path);
    fs   = file_size(file);

    switch(path[1])
    {
        case "players":                 // some player file
            // ""/"players"/"w"/"wiz"/"file.c"
            // 0  1         2   3     4
            if((sp > 4) || (sp == 4) && (fs == -2))
                return path[3];         // file/directory is owned by some player
            else if(fs == -2)
                return BB_UID;          // the other directories belong to backbone
            break;
        case "Domains":                 // file belongs to some domain
            // ""/"Domains"/"Example"/"members"/"wiz"/"file.c"
            // 0  1         2         3         4     5
            if(((sp >= 5) && (path[3] == "members")) &&
                ((sp > 5) || ((sp == 5) && (fs == -2))))
                return path[4];         // but is owned by one of it's members
            // ""/"Domains"/"Example"/"file.c"
            // 0  1         2         3
            else if((sp > 3) || ((sp == 3) && (fs == -2)))
                return path[2];         // this file/directory truly belongs to the domain
            else if(fs == -2)
                return BB_UID;          // the other directories belong to backbone
            break;
        case "var":                     // some special cases
            if((sp > 3) && (path[2] == "spool"))
            {
                // ""/"var"/"spool"/"mail"/"p"/"player"/"mail_dirs"
                // 0  1     2       3      4   5        6
                if((sp > 5) && (path[3] == "mail"))
                    return path[5];
                else
                    return BB_UID;
            }
    }
    return UNKNOWN_UID;     // everything else is unknown to this function
}
// --------------------------------------------------------------------------
/// @brief domain_file
///
/// This is a helper function for sefun::domain_of with the same semantics
/// as master::domain_file
/// @Param file
/// @Returns domain the file belongs to - for the security system this will be
/// the gid
/// @Todo this is a preliminary implementation without using full mud_info_d
/// functionality
// --------------------------------------------------------------------------
public string domain_file(string file)
{
    string *path;
    int     sp,
            fs;

    path = explode(file, "/");      // path[0] == "" !!!
    sp   = sizeof(path);
    fs   = file_size(file);

    switch(path[1])
    {
        case "players":                 // some player file
            // ""/"players"/"w"/"wiz"/"file.c"
            // 0  1         2   3     4
            if((sp > 4) || (sp == 4) && (fs == -2))
                return (is_wiz(path[3])) ?
                        WIZARD_DOMAIN : // wizard
                        PLAYER_DOMAIN;  // mortal
            else if(fs == -2)
                return BB_DOMAIN;       // the other directories belong to backbone
            break;
        case "Domains":                 // file belongs to some domain
            // ""/"Domains"/"Example"/"file.c"
            // 0  1         2         3
            if((sp > 3) || ((sp == 3) && (fs == -2)))
                return path[2];         // this file truly belongs to the domain
            else if(sp == 2)
                return BB_DOMAIN;       // the '/Domains' directory belongs to backbone
            break;
    }
    return UNKNOWN_DOMAIN;              // everything else is unknown tl this function
}

// helper functions {{{
// save_mud_info() {{{
// --------------------------------------------------------------------------
/// @brief save_mud_info
/// the mud_info_d is a _very_ important object, as such we must be extremly
/// careful when saving!
/// @Returns -
// --------------------------------------------------------------------------
private void save_mud_info(void)
{
    // move old backup copy out of the way
    if(file_size(MUD_INFO_SAVE + ".bak") >= 0)
        rename(MUD_INFO_SAVE + ".bak", MUD_INFO_SAVE + ".bak~");

    // make backup of old save file
    if((file_size(MUD_INFO_SAVE) >= 0) && (cp(MUD_INFO_SAVE, MUD_INFO_SAVE + ".bak") != 1))
    {
        syslog(LOG_MUD_INFO|LOG_EMERG, "save_mud_info: can't save save file!");
        efun::shutdown(-2);
    }
    else if(!save_obiect(MUD_INFO_SAVE))  // save current values
    {
        // panic! couldn't save...
        syslog(LOG_MUD_INFO|LOG_EMERG, "save_mud_info: can't save mud_info!");
        efun::shutdown(-2);
    }
    else                                // now we can remove old backup
        rm(MUD_INFO_SAVE + ".bak~");
}

// std applies
private void create(void)
{
    init_eids();
    restore_object(MUD_INFO_SAVE);
    if(!domains)
        domains = ([]);
    if(!players)
        players = ([]);
    if(!help_files)
        help_files = ([]);
}

private int clean_up(int arg)
{
    return 0;
}

private void reset()
{
    save_object(MUD_INFO_SAVE);
}

// event handler
public void event_destruct(void)
{
    if(origin() != ORIGIN_EFUN)
        return;
    save_mud_info();
}
public void event_shutdown(void)
{
    if(origin() != ORIGIN_EFUN)
        return;
    save_mud_info();
}
