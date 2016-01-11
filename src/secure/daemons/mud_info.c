/// @addtogroup privileged
/// @{
/// @file mud_info.c
/// @brief this daemon holds status about all mud internal infos (finger, who, mssp, ...)
/// @author Gwenhwyvar
/// @version 0.0.0
/// @date 2015-12-20

#include "/include/driver/runtime_config.h"

// --------------------------------------------------------------------------
/// @brief mssp_telopt
///
/// MSSP-info formated for telopt interface provided by the driver via
/// master::get_mud_status()
/// @Returns mapping with appropriate key:value pairs
// --------------------------------------------------------------------------
string mssp_telopt()
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
        "CONTACT"               : "your@email.address",
        "CREATED"               : "2015",
        "ICON"                  : "http://"+__MUD_IP__+"/favicon.ico",
        "IP"                    : __MUD_IP__,
        "LANGUAGE"              : "English",
        "LOCATION"              : "Germany",
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
        "INTERMUD"              : (sizeof(intermud) ? implode(intermud, ,"\t") : "0"),

            // world
        "AREAS"                 : "0",
        "HELPFILES"             : "1",
        "HELPFILES-NOTES"       : "each command, and hundreds of other docs",
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
        "VT100"                 : "0",
        "XTERM 256 COLORS"      : "0",

            // commercial
        "PAY TO PLAY"           : "0",                          // disallowed by driver
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
/// it to a string representation which may be us3d for the login-interface.
/// @Returns MSSP-info formated as string
// --------------------------------------------------------------------------
string mssp_login()
{
    mapping mssp = mssp_telopt();
    string  ret  = "MSSP-REPLY-START\r\n";

    foreach(string key, val in mssp)
        ret += sprintf("%s\t%s\r\n", key, val);

    ret += "MSSP-REPLY-END\r\n";

    return ret;
}
