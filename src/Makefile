#
# Makefile - Copyright (c) 2024 - Olivier Poncet
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

# ----------------------------------------------------------------------------
# global environment
# ----------------------------------------------------------------------------

OPTLEVEL = -O2 -g
WARNINGS = -Wall
EXTRAS   = -fstack-protector-strong
CC       = gcc
CFLAGS   = -std=c99 $(OPTLEVEL) $(WARNINGS) $(EXTRAS)
CXX      = g++
CXXFLAGS = -std=c++14 $(OPTLEVEL) $(WARNINGS) $(EXTRAS)
CPPFLAGS = -I. -D_DEFAULT_SOURCE -D_FORTIFY_SOURCE=2
LD       = g++
LDFLAGS  = -L.
CP       = cp
CPFLAGS  = -f
RM       = rm
RMFLAGS  = -f

# ----------------------------------------------------------------------------
# default rules
# ----------------------------------------------------------------------------

.c.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $<

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $<

# ----------------------------------------------------------------------------
# global targets
# ----------------------------------------------------------------------------

all: build

build: build_kr_malloc
	@echo "=== $@ ok ==="

clean: clean_kr_malloc
	@echo "=== $@ ok ==="

# ----------------------------------------------------------------------------
# kr-malloc files
# ----------------------------------------------------------------------------

kr_malloc_PROGRAM = kr-malloc.bin

kr_malloc_SOURCES = \
	kr-malloc.cc \
	$(NULL)

kr_malloc_HEADERS = \
	kr-malloc.h \
	$(NULL)

kr_malloc_OBJECTS = \
	kr-malloc.o \
	$(NULL)

kr_malloc_LDFLAGS = \
	$(NULL)

kr_malloc_LDADD = \
	$(NULL)

# ----------------------------------------------------------------------------
# build kr-malloc
# ----------------------------------------------------------------------------

build_kr_malloc: $(kr_malloc_PROGRAM)

$(kr_malloc_PROGRAM): $(kr_malloc_OBJECTS)
	$(LD) $(LDFLAGS) $(kr_malloc_LDFLAGS) -o $(kr_malloc_PROGRAM) $(kr_malloc_OBJECTS) $(kr_malloc_LDADD)

# ----------------------------------------------------------------------------
# clean kr-malloc
# ----------------------------------------------------------------------------

clean_kr_malloc:
	$(RM) $(RMFLAGS) $(kr_malloc_OBJECTS) $(kr_malloc_PROGRAM)

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
