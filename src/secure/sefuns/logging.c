/// @addtogroup sefun
/// @{
/// @file logging.c
/// @brief miscalenious simul_efuns
/// @author Gwenhwyvar
/// @version 0.0.0
/// @date 2015-12-13

#include "/secure/include/syslog.h"

// syslog {{{
// --------------------------------------------------------------------------
/// @brief syslog
///
/// Provides a standard interface to log messages analog to the one provided
/// by the standard C-library
/// The facility will select the log file and message channel used, while
/// level will provide the urgency of the message.
/// All messages will be prepended by date/time
/// @Param priority - facility | level as defined in syslog.h
/// @Param format - sprintf format string
/// @Param args... - argumemts for the format string
/// @Returns -
// --------------------------------------------------------------------------
varargs void syslog(int priority, string format, mixed *args...)
{
    int     facility;   // the supplied facility
    int     level;      // the supplied level
    object  po;         // who calls? (as object)
    string *logger;     // who calls? (as path)
    string  level_s;    // log level as text
    string  msg;        // complete message to log

    logger      = explode(file_name(po = PO()), "/");
    facility    = priority & LOG_FACILITY;
    level       = (priority & LOG_LEVEL);

    if(facility < LOG_USER)         // privileged facilities
    {
         switch(logger[1])
         {
             case "secure":
             case "std":
                 break;
             default:               // but not privileged object/room
                 error(sprintf("illegal argument to syslog: <facility>%d", facility));
         }
    }

    switch(priority & LOG_LEVEL)
    {
        case LOG_EMERG:
            level_s = "emergency";
            break;
        case LOG_ALERT:
            level_s = "alert";
            break;
        case LOG_CRIT:
            level_s = "critical";
            break;
        case LOG_ERR:
            level_s = "error";
            break;
        case LOG_WARNING:
            level_s = "warning";
            break;
        case LOG_NOTICE:
            level_s = "notice";
            break;
        case LOG_INFO:
            level_s = "info";
            break;
        case LOG_DEBUG:
            level_s = "debug";
            break;
        case LOG_USER1:
            level_s = "user1";
            break;
        case LOG_USER2:
            level_s = "user2";
            break;
        case LOG_USER3:
            level_s = "user3";
            break;
        case LOG_USER4:
            level_s = "user4";
            break;
        case LOG_USER5:
            level_s = "user5";
            break;
        case LOG_USER6:
            level_s = "user6";
            break;
        case LOG_USER7:
            level_s = "user7";
            break;
        default:
            level_s = "unknown";
            break;
    }
    msg = sprintf("%s [%s]: '" + format + "'", ctime(time()), level_s, args...);
    SYSLOGD->log(po, facility, msg);
}
// }}}
///  @}