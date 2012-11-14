#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2000, 2011 Oracle and/or its affiliates.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# This file is part of OpenOffice.org.
#
# OpenOffice.org is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# only, as published by the Free Software Foundation.
#
# OpenOffice.org is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.  If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#
#*************************************************************************

$(eval $(call gb_CppunitTest_CppunitTest,basegfx_test))

$(eval $(call gb_CppunitTest_add_exception_objects,basegfx_test, \
	basegfx/test/basegfx1d \
	basegfx/test/basegfx2d \
	basegfx/test/basegfx3d \
	basegfx/test/boxclipper \
	basegfx/test/basegfxtools \
	basegfx/test/clipstate \
	basegfx/test/genericclipper \
	basegfx/test/testtools \
))

$(eval $(call gb_CppunitTest_add_linked_libs,basegfx_test, \
	basegfx \
	sal \
	stl \
	cppuhelper \
	cppunit \
	cppu \
    $(gb_STDLIBS) \
))

$(eval $(call gb_CppunitTest_add_api,basegfx_test,\
	udkapi \
	offapi \
))

$(eval $(call gb_CppunitTest_set_include,basegfx_test,\
	$$(INCLUDE) \
	-I$(SRCDIR)/basegfx/inc/pch \
))

# vim: set noet sw=4 ts=4: