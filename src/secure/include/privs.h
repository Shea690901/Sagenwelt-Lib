/// @addtogroup privileged
/// @{
/// @file privs.h
/// @brief privilege system
/// @author Gwenhwyvar
/// @version 0.0.0
/// @date 2015-12-18

#ifndef __SEC_PRIVS_H
#define __SEC_PRIVS_H

// user privileges
#define UPRIV_ALL       "__ALL__"                       // no restrictions
#define UPRIV_AUTHOR    "__AUTHOR__"                    // matching uid
#define UPRIV_DOMAIN    "__DOMAIN__"                    // matching gid

#define UPRIV_MORTAL    "__Player__"                    // normal player
#define UPRIV_ELDER     "__Elder__"                     // high level mortal
#define UPRIV_WIZARD    "__Wizard__"                    // normal wizard
#define UPRIV_D_WIZ     "__D_Wizard__"                  // domain member
#define UPRIV_D_LORD    "__D_Lord__"                    // domain lord
#define UPRIV_ARCH      "__Archwiz__"                   // arch wizard
#define UPRIV_ADMIN     "__ADMIN__"                     // mud admin

// standard uids
#define BB_UID          "__backbone__"                  // backbone uid
#define ROOT_UID        "__root__"                      // root uid
#define NOBODY_UID      "__nobody__"                    // uid with no special privileges
#define UNKNOWN_UID     "__unknown__"                   // invalid uid

// standard gids
#define BB_DOMAIN       "__Backbone__"                  // backbone domain
#define PLAYER_DOMAIN   "__Mortals__"                   // for statistics and access privileges
#define WIZARD_DOMAIN   "__Wizards__"                   // for statistics and access privileges
#define NOBODY_DOMAIN   "__Nodomain__"                  // gid with no special privileges
#define UNKNOWN_DOMAIN  "__Unknown__"                   // invalid domain

// requests for check_acl
#define _LINK   0
#define _READ   1
#define _WRITE  2
#define _LOAD   3

// efun privileges (master::valid_*)
// these are indices into a bitfield!
// used values must __never__ be reused!!!
#define PRIV_BIND       0                               // efun::bind
#define PRIV_DB         1                               // efun::db_*
#define PRIV_HIDE       2                               // efun::set_hide
#define PRIV_OVERRIDE   3                               // function prefix efun::
#define PRIV_SETEUID    4                               // efun::seteuid
#define PRIV_SOCK_SERV  5                               // allowed to use server sockets
#define PRIV_SOCK_CLNT  6                               // allowed to use client sockets

// __NEXT_PRIV always shows the next free PRIV number
// this value must __never__ decrease!!!
#define __NEXT_PRIV     7

#endif // __SEC_PRIVS_H

///  @}
