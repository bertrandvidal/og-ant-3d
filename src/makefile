#--------------------------------------------------------------------------------
#Copyright (C) 2007 Jocelyn Buisson

#This program is free software; you can redistribute it and/or
#modify it under the terms of the GNU General Public License
#as published by the Free Software Foundation; either version 2
#of the License, or (at your option) any later version.

#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.

#You should have received a copy of the GNU General Public License
#along with this program; if not, write to the Free Software
#Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#--------------------------------------------------------------------------------
bin=Ant3D_EA
includes=${HOME}/include .
subdirs=.
LDLIBS= -lGL -lglut -lGLU
CXXFLAGS= -g -O3
RM=rm -f

.DEFAULT_GOAL := $(bin)
.PHONY: $(subdirs) clean clean_all cleanobj print_src print_obj $(lib)

CXXFLAGS+=$(addprefix -I, $(includes))

###    Automatic source search    ###
cxxsrc=$(foreach dir,$(subdirs),$(wildcard $(dir)/*.cpp))
objects=$(cxxsrc:.cpp=.o)

###    explicit rules    ###

all: $(bin)

first: $(bin)

$(bin): $(objects)
	$(CXX) -o $@ $^ $(LDFLAGS) $(LDLIBS)

-include $(cxxsrc:.cpp=.d)

%.d: %.cpp
	@echo generating $(<F) prerequisites
	@$(CXX) -o $@ -MM $<

###    informations    ###

print_src:
	@echo $(cxxsrc)

print_obj:
	@echo $(OBJ)

###    cleaning    ###

clean: cleanobj
	@-$(RM) $(cxxsrc:.cpp=.d) *~

cleanall: clean
	@-$(RM) $(bin) *~

cleanobj:
	@-$(RM) $(objects)
