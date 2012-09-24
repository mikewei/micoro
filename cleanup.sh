#!/bin/sh

# Copyright (c) 2012, Bin Wei <bin@vip.qq.com>
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
# 
#     * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#     * The name of of its contributors may not be used to endorse or 
# promote products derived from this software without specific prior 
# written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

cd "`dirname $0`"

if test -r Makefile; then
	make maintainer-clean
fi

find . -name Makefile.in -exec rm -f {} \;

rm -R -f \
	   autom4te.cache \
	   build-aux \
	   m4/libtool.m4 \
	   m4/ltoptions.m4 \
	   m4/ltsugar.m4 \
	   m4/ltversion.m4 \
	   m4/lt~obsolete.m4 \
	   aclocal.m4 \
	   autoscan.log \
	   config.guess \
	   config.h \
	   config.h.in \
	   config.h.in~ \
	   config.hin \
	   config.hin~ \
	   config.log \
	   config.status \
	   config.status.lineno \
	   config.sub \
	   configure \
	   configure.lineno \
	   configure.scan

