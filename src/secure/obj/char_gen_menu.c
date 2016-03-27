/// @file char_gen_menu.c
/// @brief menu driven character generation/extension
/// @author Gwenhwyvar
/// @version 0.0.0
/// @date 2016-02-02

// current version of player data
// please update when ever some variable is added, removed or otherwise
// changed...
#define PLAYER_VERSION  1

inherit M_MENU;

private MENU      toplevel;

private MENU      race_sel;
private string   *races;

private MENU      gender_sel;
private string   *gender;

private mapping   player_info;
private function  call_back;        // call back function to be called after
                                    // all information is gathered

// once we've gathered everything we inform our caller and terminate
private void finished(void)
{
    evaluate(call_back, player_info);
    quit_menu_application();
}

// check if everything is gathered, if so we allow finished to be called
private int check_finished(void)
{
    if(                                 // we need:
       !player_info[MI_PI_NAME]   ||    // a name
       !player_info[MI_PI_PWD]    ||    // a password
       !player_info[MI_PI_RACE]   ||    // a race
       !player_info[MI_PI_GENDER]       // the gender
      )
        return FALSE;                   // still missing something
    return TRUE;
}

// someone choose to logout during character generation....
private void logout(void)
{
    environment()->quit();
    quit_menu_application();
}

// help
private void toplevel_help(string arg)
{
    write(
@END_HELP
For a new player it is mandatory to choose a race, a gender, a name and a password. Neither an email address, nor a web page is
necessary, although it is recommended to supply at least the email address!

If an email address is supplied and registered (the automatically send register email is answered) this email may be used to
either login, the password is still needed, or to reset a forgotten password. Without an email address a character with a
forgotten password is most likely lost!

The choosen race implies the base character values like strength, wisdom and so on, but also gives some perks, either positive
like the ability to see in the dark or negative like fear of open plains.  While most races have the 'normal' genders of
female/male some races might restricted to either one or might have a complete other set of genders available. As such as soon as
one chooses either a race or some gender the available choices for the other one will be restricted!  To circumvent any problems
with those choices it is recommended to visit the sub menus in the given order.

As soon as all mandatory choices are made the character may be finished. This last step in the character generation gives the
opportunity to slightly change both the base character values as well as the perks to individualize the character. Thereafter
neither race, gender or name may be changed anymore, while the character values and perks may be still changed through either
normal character development (most likely permanently) or the help of e.g. some spells or the participation in a adventurer group
(most likely temporary).

Either of password, email address and web page are available for change for as long as the character exists!
END_HELP
        );
}

private void race_sel_help(string arg)
{
    write(
@END_HELP
Here you might choose between any available race.

ATTENTION:
By choosing some gender before coming here said choice might be limited!

To get an overview of the capabilities of one race or another just choose it from the menu, you will be given the opportunity to
discard this choice.
END_HELP
        );
}

private void gender_sel_help(string arg)
{
    write(
@END_HELP
Depending on the choosen race, if any, you will be given a choice of available genders.

Furthermore depending on the given choices you might later either choose another gender to change your selection or choose the
same gender a second time to completely undo your choice.
END_HELP
        );
}

private void create(void)
{
    int idx;

    MENU_ITEM sep = new_seperator("");

    init_eids();

    player_info = ([]);

    toplevel    = new_menu(MUD_NAME + " CHARACTER GENEARATION");

    // toplevel menu
    add_menu_item(toplevel, sep);
    add_menu_item(toplevel, new_menu_item("Race selection",   race_sel,         "r"));
    add_menu_item(toplevel, new_menu_item("Gender selection", gender_sel,       "g"));
    add_menu_item(toplevel, new_menu_item("Name selection",   (: name_sel :),   "n"));
    add_menu_item(toplevel, new_menu_item("Password",         (: password :),   "p"));
    add_menu_item(toplevel, sep);
    add_menu_item(toplevel, new_menu_item("eMail address",    (: email :),      "e"));
    add_menu_item(toplevel, new_menu_item("Web page",         (: homepage :),   "w"));
    add_menu_item(toplevel, sep);
    add_menu_item(toplevel, new_menu_item("Help", (: toplevel_help :),          "h", 1));
    add_menu_item(toplevel, new_menu_item("Finished", (: finished :),           "f"), 0, (: check_finished :));
    add_menu_item(toplevel, new_menu_item("Logout", (: logout :),               "q"));

    // race selection menu
    race_sel    = new_menu("Race selection");
    add_menu_item(race_sel, sep);
    idx = 0;
    foreach(string entry in (races = (string*)RACES_D->get_races()))
    {
        add_menu_item(race_sel, new_menu_item(capitalize(entry), (: choose_race($(idx)) :)));
        idx++;
    }
    add_menu_item(race_sel, sep);
    add_menu_item(race_sel, new_menu_item("Help", (: race_sel_help :),          "h"));
    add_menu_item(race_sel, new_menu_item("Back", (: toplevel :),               "q"));

    // gender selection menu
    gender_sel  = new_menu("Gender selection");
    add_menu_item(gender_sel, sep);
    idx = 0;
    foreach(string entry in (gender = (string*)RACES_D->get_gender()))
    {
        add_menu_item(gender_sel, new_menu_item(capitalize(entry), (: choose_gender($(idx)) :)));
        idx++;
    }
    add_menu_item(gender_sel, sep);
    add_menu_item(gender_sel, new_menu_item("Help", (: gender_sel_help :),          "h"));
    add_menu_item(gender_sel, new_menu_item("Back", (: toplevel :),  "q"));
}

public void start_menu(mapping p_info, function cb)
{
    player_info = p_info;
    call_back   = cb;
    init_menu_application(toplevel);
}

// event handler
public void event_destruct(void)
{
    if(origin() != ORIGIN_EFUN)
        return;
    // someone destructed us :(
    write("\nSorry, but someone is playing around with the login process...\nPlease try again in a few minutes...\n");
    logout();
    destruct();
}

public void event_shutdown(void)
{
    if(origin() != ORIGIN_EFUN)
        return;
    destruct();
}
