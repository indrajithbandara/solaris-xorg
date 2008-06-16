###############################################################################
#
# xterm 1.x Makefile
#
# Copyright 2008 Sun Microsystems, Inc.  All rights reserved.
# Use subject to license terms.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, and/or sell copies of the Software, and to permit persons
# to whom the Software is furnished to do so, provided that the above
# copyright notice(s) and this permission notice appear in all copies of
# the Software and that both the above copyright notice(s) and this
# permission notice appear in supporting documentation.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT
# OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
# HOLDERS INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL
# INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING
# FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
# NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
# WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
# 
# Except as contained in this notice, the name of a copyright holder
# shall not be used in advertising or otherwise to promote the sale, use
# or other dealings in this Software without prior written authorization
# of the copyright holder.
#
#

# Package name used in tarballs
APP_NAME=xterm

# Version number (used in path names)
APP_VERS=235

# Source tarball
SOURCE_TARBALL_NAME=$(APP_NAME)-$(APP_VERS).tar.gz
SOURCE_URL=ftp://invisible-island.net/xterm/xterm.tar.gz
SOURCE_URL_SET=yes
SOURCE_UNCOMPRESS=gzcat

# Patches to apply to source after unpacking, in order
SOURCE_PATCHES = \
	4251073.patch

# Man pages to apply Sun footer to & attributes to list
SUNTOUCHED_MANPAGES=*.man
SUNTOUCH_MAN_FLAGS= -p $(APP_prefix)/bin/ \
 -a '{Availability, SUNWxwopt} {Interface Stability, Committed}'

# Paths to find Xft/fontconfig/freetype headers libraries at build time
# and run time 
FT_CFLAGS= -I$(PROTODIR)/usr/include -I$(PROTODIR)/usr/include/freetype2 \
	-I$(PROTODIR)/usr/X11/include
FT_LIBS= \
  -L$(PROTODIR)/usr/X11/lib$(LIBSUBDIR) -R/usr/X11/lib$(LIBSUBDIR) -lXft \
  -L$(PROTODIR)/usr/lib$(LIBSUBDIR) -R/usr/lib$(LIBSUBDIR) -lfontconfig

# Additional flags to pass to configure script
APP_CONFIG_OPTS = --enable-256-color --enable-88-color --enable-wide-chars \
  --with-freetype-cflags="$(FT_CFLAGS)" --with-freetype-libs="$(FT_LIBS)" \
  --disable-setuid --disable-setgid  --enable-luit \
  --x-includes=$(PROTODIR)/usr/X11/include --x-libraries=/usr/X11/lib \
  --with-icondir=/usr/share/pixmaps

# Tell configure where to find luit binary
APP_CONFIG_ENV = LUIT=/usr/X11/bin/luit

include ../Makefile.inc