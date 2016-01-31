# TODO

1. base directory structure
    - in progress
1. simul_efuns
    - in progress
    - fast prng with seed for guarantied reproducable random numbers
    - call\_out handler (prevention of 'too many call\_outs)
    - clone_object (prevention of object flooding)
    - support function "event_shutdown" in livings/daemons/...
    - termcap support?
1. daemons
    - mud_info (needed for master functionality!)
        - holds
            - wiz list
            - domain list
            - group memberships
        - needed functions for
            - master
                - string *error_author(string) - returns array of users
                    responsible for errors by author
                - string *error_domain(string) - returns array of users
                    responsible for errors by domain
                - string *get_groups(string) - returns all groups for suplied user
    - tmpd (needed for master functionality!)
        - needed functions for
            - master
                - author\_file - signature/semantic like master::author\_file
                - domain\_file - signature/semantic like master::domain\_file
    - syslogd (needed for master functionality!)
    - tmp_d holds ownership of files/dirs within public tmp-dirs (needed for master functionality!)
    - crond
    - maild
    - newsd
    - mapd (world map)
    - ftpd (mostlikely support for external ftpd only)
    - httpd (mostlikely support for external ftpd only)
1. privilege system (combined with daemons)
    - in progress
    - needs acl's for file/socket access inside master
1. login object
    - needed functions for
        - master
            - void set_port(int)
1. player object
    - needed functions for
        - master
            - string query_uid(void)
            - int get_ed_setup(void)
            - int set_ed_setup(void)
1. standard inherits
    - needed functions for
        - master
            - string short(void)        short description
            - string the_short(void)    [the] short description
1. standard objects
    - player
        - in progress
1. base rooms
1. find some prime numbers (runtime config):
    - ~ 13107: hash table size
    - ~  4096: obiect table size
1. documentation for driver support headers (/include/driver/*)
1. master
    - /var/cfg/*.acl need to be completed
1. telnet protocol
    read up what those do and decide if we support them (driver support is
    present!)
    - mxp \[Mud eXtension Protocol\] (probably yes)
        - applies: mxp\_enable, mxp\_tag
        - efuns  : int has\_mxp(object=TO), void act\_mxp(void)
    - gmcp (code states **yawn** ???)
        - applies: gmcp\_enable, gmcp
        - efuns  : int has\_gmcp(object=TO), void send\_gmcp(string)
    - zmp \[Zenith Mud Protocol\] (http://zmp.sourcemud.org)
        - applies: zmp\_command
        - efuns  : int has\_zmp(object=TO), void send\_zmp(string, string*)
1. user shell
    - set command
        - set env var=val
        - set email address
