0. Introduction
---------------

   wxZoom is a program which allows you to magnify parts of screen. It is
written using wxWindows cross-platform GUI toolkit and so works on all
platforms supported by it (currently tested under Win32 and Unix/GTK+).

WARNING: THIS IS STILL AN ALPHA VERSION - YOU KNOW WHAT THIS MEANS

   Please contact me at vadim@wxwindows.org with any comments/suggestions.
This program is so far only a quick (~30 minutes) hack which I wrote for my
personal needs but if there is any interest in it I will work on improving it.

1. Installation
---------------

   If you have downloaded the binary, just run it. Note that you will need
wxWindows libraries for your platform, see the project home page for more
instructions.

   If you only have a source distribution, you need to build the program
yourself.

a) Building under Unix

   You must have wxWindows development package (i.e. not only the libraries
but the headers and wx-config) installed before doing this!

   Simply execute the following commands in the directory where you untarred
the archive:

	./configure
	make
	make install

If you had obtained the sources by checking them out from the repository, you
also need to run "./bootstrap" command before the ones above.

You will usually need super user (root) priveleges to install it in the
default location (/usr/local, may be changed by giving --prefix option to
configure).

b) Building under Win32

   Currently you can only build it using VC++ or the cygwin tools. For the
latter, please refer to the Unix instructions above.

   To build the program with VC++ 6.0 or 7.1, just open the provided workspace
file (wxZoom.dsw for the former and wxZoom.sln for the latter) and choose build.

2. Using the program
--------------------

   After launch the program you can press the left mouse button in the window
and drag it to select the screen area to view. You can increase/decrease the
zoom level with Ctrl-I/Ctrl-O (also from the menu) and you can also refresh
the window either manually (F5) or automatically (Ctrl-A) using a configurable
interval.

   Miscellaneous other features:

   * you can toggle the grid lines display with Ctrl-G.
   * when the mouse is inside the window, the second pane of the status bar
     displays the colour of the pixel under the pointer in hex RGB format.
   * you can open new program windows with Ctrl-N

