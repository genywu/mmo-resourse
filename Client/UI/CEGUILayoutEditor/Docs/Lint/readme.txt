This directory contains the PC-Lint 8.00w (http://www.gimpel.com/) configuration files used in CELayoutEditor. We recommend using Visual Lint as well (but it's not strictly necessary).

std.lnt is the main configuration file (read directly by PC-Lint). It is constructed for VS2005 (VC8) but it can easily be adapted to VS2003 (VC7) and to other environments. You should change the include directories stored within to suit your own machine configuration before using it.

options.lnt is the configuration file with settings specific to CELayoutEditor, CEGUI and wxWidgets use. It is called internally by std.lnt.

Both files are to be located at [PC-LINT DIR]\CEGUI. All other referenced configuration files can be obtained directly from the PC-Lint site and from the Visual Lint one (see std.lnt for more information).

Frederico Jerónimo (fjeronimo),
22/11/2007.

