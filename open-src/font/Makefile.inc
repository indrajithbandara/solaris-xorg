# -*- Makefile -*- rules common to most X.Org font modules
#
# Copyright (c) 2006, 2016, Oracle and/or its affiliates. All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice (including the next
# paragraph) shall be included in all copies or substantial portions of the
# Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
#
#

### Makefile settings/rules common to all X.Org font modules

PWD:sh=pwd
TOP=$(PWD)/../../..

MODTYPE=font

### Include common definitions
include $(TOP)/open-src/common/Makefile.init

# Default package name for font packages
MODTYPE_PKGHIER_DEFAULT         = $(MODTYPE_PKGHIER_SET:yes=$(POUND_SIGN))
$(MODTYPE_PKGHIER_DEFAULT)	MODTYPE_PKGHIER=system/font
MODTYPE_PKGHIER_SET=yes
MODULE_PKGTAIL_DEFAULT		= $(MODULE_PKGTAIL_SET:yes=$(POUND_SIGN))
$(MODULE_PKGTAIL_DEFAULT)	MODULE_PKGTAIL:sh=basename $(pwd)
MODULE_PKGTAIL_SET	      	= yes

# Package classification
MODULE_PKGCLASS_DEFAULT		= $(MODULE_PKGCLASS_SET:yes=$(POUND_SIGN))
$(MODULE_PKGCLASS_DEFAULT)	MODULE_PKGCLASS=System/Fonts
MODULE_PKGCLASS_SET		= yes

# Default module description for packages in this area
MODTYPE_DESC = the $(MODULE_NAME:font-%=%) fonts

# Default TPNO & license text for X.Org fonts
UPSTREAM_DEFAULT	= $(UPSTREAM_SET:yes=$(POUND_SIGN))	
$(UPSTREAM_DEFAULT)	ORACLE_TPNO = $(ORACLE_TPNO_xorg_fonts)
$(UPSTREAM_DEFAULT)	ORACLE_TPNO_SET = yes
$(UPSTREAM_DEFAULT)	XORG_FONT_LICENSE  = ../X11-font-license.txt
$(UPSTREAM_DEFAULT) 	ADD_LICENSE_HEADER = ../X11-font-license-add.txt
$(UPSTREAM_DEFAULT)	LICENSE_APPEND = $(ADD_LICENSE_FILES)

# If font makefile contains AUTO_ADD_LICENSE = no, we just display the X.Org
# font license, as there is no other attribution notice required for the 
# package.  Otherwise we display the X11 license, plus a header and the 
# package's own COPYING file.
ADD_LICENSE_YES  = $(AUTO_ADD_LICENSE:no=$(POUND_SIGN)) 
$(ADD_LICENSE_YES) ADD_LICENSE_NO = $(POUND_SIGN)

$(UPSTREAM_DEFAULT)$(ADD_LICENSE_YES)	MODULE_LICENSE_PREPEND  = $(XORG_FONT_LICENSE)
$(UPSTREAM_DEFAULT)$(ADD_LICENSE_YES)	MODULE_LICENSE_PREPEND += $(ADD_LICENSE_HEADER)
$(UPSTREAM_DEFAULT)$(ADD_LICENSE_NO)	LICENSE_FILE = $(XORG_FONT_LICENSE)
$(UPSTREAM_DEFAULT)$(ADD_LICENSE_NO)	LICENSE_FILE_SET = yes

# Most font modules have no man pages
MODTYPE_SUNTOUCH_MAN_FLAGS= 

# No common dependencies
MODTYPE_CONFIGURE_DEPS=

# Set default installation path for fonts
MODTYPE_CONFIG_OPTS = --with-fontrootdir="$(X11_FONT_DIR)"
MODTYPE_CONFIG_OPTS += $(MODULE_FONTDIR:%=--with-fontdir="%")

# Disable compression of pcf fonts
MODTYPE_CONFIG_OPTS += --without-compression

# Avoid races between modules that install to the same directory by not 
# running mkfontdir at build time, but waiting for later (either in the
# sun-aliases module or during packaging in generate_font_metadata.pl).
MODTYPE_CONFIG_OPTS += MKFONTDIR=/usr/bin/true

# Most font modules have nothing to build, but font-util builds some programs
MODTYPE_CFLAGS=$(PROG_CFLAGS)
MODTYPE_CPPFLAGS=$(PROG_CPPFLAGS)
MODTYPE_CXXFLAGS=$(PROG_CXXFLAGS)
MODTYPE_LDFLAGS=$(PROG_LDFLAGS)
MODTYPE_LD_OPTIONS=

# Paths to find mkfontscale, fc-cache & the libraries they need in build area
FONT_UTILS_PATH=$(PROTODIR)/usr/bin
FONT_LIBS_PATH=$(PROTODIR)$(X11_SERVERLIBS_DIR)$(LIBSUBDIR_64):$(PROTODIR)/usr/lib$(LIBSUBDIR_64)

# Additional environment variable options to GNU autoconf configure script
MODTYPE_CONFIG_ENV = PATH="$(FONT_UTILS_PATH):$$PATH" \
	LD_LIBRARY_PATH="$(FONT_LIBS_PATH)"

# Environment variable options needed when building
MODTYPE_BUILD_ENV = PATH="$(FONT_UTILS_PATH):$$PATH" \
	LD_LIBRARY_PATH="$(FONT_LIBS_PATH)"

# Need to install links in fontpath.d directory
MODTYPE_ADD_INSTALL_TARGETS = install_fontpath_links

### Include common rulesets
include $(TOP)/open-src/common/Makefile.inc

# Can't use Makefile substitutions or dependencies on the links themselves
#  due to the : characters & shell math expansion in the link names
# Ignores errors since /usr/bin/ln -sf can fail due to race conditions
#  when parallel makes try making the same symlink at the same time
install_fontpath_links: $(PROTODIR)$(X11_FONT_CATALOGUE)
	@for l in $(FONTPATH_LINKS) ; do \
	    if [[ ! -L $(PROTODIR)$(X11_FONT_CATALOGUE)/$$l ]] ; then \
		( $(START_CMD_ECHO) ; ln -sf ../../..$(MODULE_FONTDIR) \
		  $(PROTODIR)$(X11_FONT_CATALOGUE)/$$l || true ); \
	    fi \
	done

$(PROTODIR)$(X11_FONT_CATALOGUE):
	mkdir -p $@
