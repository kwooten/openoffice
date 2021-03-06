#**************************************************************
#  
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#  
#    http://www.apache.org/licenses/LICENSE-2.0
#  
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.
#  
#**************************************************************



PRJ	= ..$/.. 
PRJNAME = xmerge
#PACKAGE = com$/sun$/star$/documentconversion$/XSLTFilter
TARGET  = xmergesync 
ENABLE_EXCEPTIONS=TRUE
LIBTARGET=NO
.IF "$(POCKETPCSDK_HOME)" != ""
SOLARINC+=-I$(POCKETPCSDK_HOME)$/support$/ActiveSync$/inc
.ENDIF          # "$(POCKETPCSDK_HOME)" != ""

# --- Settings ----------------------------------------------------- 
.INCLUDE: settings.mk  

.IF 0
.IF "$(GUI)" == "WNT"
.IF "$(POCKETPCSDK_HOME)" != ""

RCFILES=XMergeSync.rc

SLOFILES= \
    $(SLO)$/XMergeFactory.obj \
    $(SLO)$/XMergeSync.obj \
    $(SLO)$/XMergeFilter.obj 
LIBNAME=$(TARGET)
SHL1TARGETDEPN=makefile.mk
SHL1OBJS=$(SLOFILES) $(RES)$/xmergesync.res
SHL1TARGET=$(LIBNAME)
SHL1IMPLIB=i$(LIBNAME)
SHL1DEF=XMergeSync.def
USE_DEFFILE=true

SHL1STDLIBS= uuid.lib Advapi32.lib

.ENDIF          # "$(POCKETPCSDK_HOME)" != ""
.ENDIF
.ENDIF


# --- Targets ------------------------------------------------------  
.INCLUDE :  target.mk 

ALLTAR :
	.IF "$(GUI)" == "WNT"
		$(COPY) BIN$/xmergesync.dll $(BIN)
	.ENDIF
