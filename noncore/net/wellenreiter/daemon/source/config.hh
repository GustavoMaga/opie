/*
 *
 * Global configuration for wellenreiter
 *
 * $Id: config.hh,v 1.1 2002-11-12 22:12:33 mjm Exp $
 *
 * Written by Martin J. Muench <mjm@codito.de>
 *
 */

#ifndef CONFIG_HH
#define CONFIG_HH

#define PROGNAME "wellenreiter"  /* Name of program (for syslog et.al.) */
#define VERSION  "0.2"           /* Version of wellenreiter             */

#define DAEMONPORT    37772            /* Port of Daemon                           */

#define GUIADDR  "127.0.0.1"     /* Adress of GUI, later specified in configfile */
#define GUIPORT  37773          /* Port of GUI, "   " */

#endif /* CONFIG_HH */
