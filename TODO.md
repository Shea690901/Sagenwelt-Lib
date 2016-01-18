# TODO

1. base directory structure
    - in progress
1. master
    - in progress
    - check_acl needs additional code for missing UPRIVs
    - /var/cfg/*.acl need to be completed
1. simul_efuns
    - in progress
1. daemons
    - mud_info (needed for master functionality!)
        holds
        - wizlist
        - domainlist
        - groupmemberships
    - syslogd (needed for master functionality!)
    - crond
    - maild
    - newsd
    - mapd (world map)
    - ftpd (mostlikely support for external ftpd only)
1. privilege system (combined with daemons)
    - in progress
    - needs acl's for file/socket access inside master
1. login object
1. standard inherits
1. standard objects
1. base rooms
1. find some prime numbers (runtime config):
    - ~ 13107: hash table size
    - ~  4096: obiect table size
1. documentation for driver support headers (/include/driver/*)
