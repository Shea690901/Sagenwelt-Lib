# TODO

1. base directory structure
    - in progress
1. master
    - in progress
    - /var/cfg/*.acl need to be completed
1. simul_efuns
    - in progress
    - fast prng with seed for guarantied reproducable random numbers
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
            - bool is_elder(void)
            - bool is_wiz(void)
            - bool is_lord(string)
            - bool is_arch(void)
            - bool is_admin(void)
            - string *get_groups(void)
            - string query_uid(void)
            - int get_ed_setup(void)
            - int set_ed_setup(void)
1. standard inherits
    - needed functions for
        - master
            - string short(void)        short description
            - string the_short(void)    [the] short description
1. standard objects
1. base rooms
1. find some prime numbers (runtime config):
    - ~ 13107: hash table size
    - ~  4096: obiect table size
1. documentation for driver support headers (/include/driver/*)
