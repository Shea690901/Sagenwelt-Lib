/// @file login.c
/// @brief primary connection object
/// @author Gwenhwyvar
/// @version 0.0.0
/// @date 2016-01-27

#include <pragmas.h>
#include <message.h>
#include <login.h>

// helper functions {{{
// timeout {{{
// --------------------------------------------------------------------------
/// @brief timeout
///
/// handles login timeout
/// @Returns
// --------------------------------------------------------------------------
private void timeout(void)
{
    // we aren't interactive anymore?
    if(!interactive(TO()))
        destruct();
    // timeout reached?
    else if(query_idle(TO()) > LOGIN_TIMEOUT)
    {
        receive("Timeout\nClosing connection...\n");
        destruct();
    }
    // restart timeout
    else
        call_out( (: timeout :), LOGIN_TIMEOUT);
}
// }}}
// login_name {{{
private void login_name(string arg)
{
    if(!arg || (arg == ""))
    {
        receive("You must supply either a user name oryour email address...\n" LOGIN_PROMPT);
        input_to( (: login_name :), TRUE);
        return;
    }
    if(strsrch(arg, '@') != -1)
    {
        arg = (string)MUD_INFO_D->get_email_name(arg);
}
// }}}
// }}}
// interactive applies {{{
// catch_tell {{{
// --------------------------------------------------------------------------
/// @brief catch_tell
///
/// If INTERACTIVE_CATCH_TELL is defined in options.h, whenever the driver has
/// something to tell an object due to say(), shout(), tell_object(), etc.
/// catch_tell in the player object will be called with the message to be
/// printed.  The message can be displayed, discarded, or modified in any way
/// desired.  This is a useful way to have flexible earmuffs, or to support
/// intelligent clients.
/// @Param msg
/// @Returns
// --------------------------------------------------------------------------
private void catch_tell(string msg)
{
    receive(message);
}
// }}}
// logon {{{
// --------------------------------------------------------------------------
/// @brief logon
///
/// This apply is called on the object returned by the connect() master apply.
/// It should set up the new user.
/// @Returns
// --------------------------------------------------------------------------
private object logon(void)
{
    string buf;

    // without eids we don't have any permissions
    init_eids();

    // display welcome message
    buf = read_file(WELCOME_MSG);
    receive(buf);;

    // start timeout
    call_out( (: timeout :), LOGIN_TIMEOUT);

    // wrife prompt and wait for input
    receive(LOGIN_PROMP);
    input_to( (: login_name :), TRUE);
}
// }}}
// net_dead {{{
// --------------------------------------------------------------------------
/// @brief net_dead
///
/// If an interactive object (i.e. the player object) suddenly loses its
/// connection (i.e. it goes "net dead"), then the driver calls this function
/// on that object giving it a chance to clean up, notify its environment etc.
/// Be aware that functions that depend on the object being interactive will
/// not work as expected.
/// @Returns
// --------------------------------------------------------------------------
private void net_dead(void)
{
    // we don't have anything to loose
    destruct();
}
// }}}
// receive_message {{{
// --------------------------------------------------------------------------
/// @brief receive_message
///
/// The message() efun calls this method in the player object.  The class
/// parameter is typically used to indicate the class (say, tell, emote,
/// combat, room description, etc) of the message.  The receive_message()
/// apply together with the message() efun can provide a good mechanism for
/// interfacing with a "smart" client.
/// @Param class
/// @Param message
/// @Returns
// --------------------------------------------------------------------------
private void receive_message(string class, string message)
{
    // only system messages (e.g. shutdown) or messages concerning the logon
    // process
    if((class == MSGCLASS_LOGON) || (class == MSGCLASS_SYSTEM))
        receive(message);
}
// }}}
// telnet_suboption {{{
// --------------------------------------------------------------------------
/// @brief telnet_suboption
///
/// This apply is called on the interactive object with the parameter given by
/// the SE telnet suboption, for mudlib defined processing.  For example, this
/// allows for future support of terminal types.
///
/// The first byte of the buffer is typically a type descriptor, ie
/// TELOPT_TTYPE. The next byte is a procession option, such as TELQUAL_IS.
/// Following this is the type dependent data.  In the case of a terminal type
/// call, the data will be routed through the terminal_type() apply instead.
///
/// Possible suboptions:
///
/// #define TELQUAL_IS    0 // option is...
/// #define TELQUAL_SEND  1 // send options
/// #define TELQUAL_INFO  2 // ENVIRON: informational version of IS
/// #define TELQUAL_REPLY 3 // AUTHENTICATION: client version of IS
/// #define TELQUAL_NAME  4 // AUTHENTICATION: client version of IS
/// @Param buffer
/// @Returns
// --------------------------------------------------------------------------
private void telnet_suboption(string buffer)
{
}
// }}}
// terminal_type {{{
// --------------------------------------------------------------------------
/// @brief terminal_type
///
/// This apply is called on the interactive object with term set to the
/// terminal type for the user, as reported by telnet negotiation.  If the
/// user's client never responds (it's not telnet, for example) this will
/// never be called.
/// @Param term
/// @Returns
// --------------------------------------------------------------------------
private void terminal_type(string term)
{
}
// }}}
// write_prompt {{{
// --------------------------------------------------------------------------
/// @brief write_prompt
///
/// If write_prompt is present in the player object, the driver will call it
/// whenever the default prompt would normally be printed.  The driver will
/// not call write_prompt when the player is in input_to or ed.
/// @Returns
// --------------------------------------------------------------------------
private void write_prompt(void)
{
}
// }}}
// }}}

// event handler {{{
// destruct {{{
public void event_destruct(void)
{
    if(origin() != ORIGIN_EFUN)
        return;
    destruct();
}
// }}}
// shutdown {{{
public void event_shutdown(void)
{
    if(origin() != ORIGIN_EFUN)
        return;
    receive("\nSorry, we're shutting down...\nPlease try again in a few
            minutes...\n");
    destruct();
}
// }}}
// }}}
