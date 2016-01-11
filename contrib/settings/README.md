# Sagenwelt Base-MudLib
## contrib/settings

This directory contains both, the compiletime and runtime settings used by
this lib.

+ local_options.sagenwelt-lib

compiletime options.
copy to the appropriate place within the FluffOS sources prior to compiling
the driver. (see build instructions there).
Depending on your system, this file still needs some changes!

+ Config.sagenwelt-lib

runtime options.

DON'T CHANGE, with the exception of of the mud name!!
This file is given to the driver as commandline argument (see install
instructions there)
