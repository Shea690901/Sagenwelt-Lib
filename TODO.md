# TODO

1. base directory structure
    - in progress
1. master
    - in progress
    - check_acl needs additional code for missing UPRIVs
    - /var/cfg/*.acl need to be completed
1. simul_efuns
    - in progress
    - fast prng with seed for guarantied reproducable random numbers
    - [sg]et_e?uid
1. daemons
    - mud_info (needed for master functionality!)
        holds
        - wiz list
        - domain list
        - group memberships
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
1. player object
    - needed functions for
        - master
            - bool is_elder(void)
            - bool is_wiz(void)
            - bool is_lord(string)
            - bool is_arch(void)
            - bool is_admin(void)
            - string *get_groups(void)
1. standard inherits
1. standard objects
1. base rooms
1. find some prime numbers (runtime config):
    - ~ 13107: hash table size
    - ~  4096: obiect table size
1. documentation for driver support headers (/include/driver/*)
