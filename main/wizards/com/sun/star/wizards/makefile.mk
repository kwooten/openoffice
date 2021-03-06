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


PRJ		= ..$/..$/..$/..
PRJNAME = wizards
TARGET  = wizards_common
PACKAGE = com$/sun$/star$/wizards

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk
#.INCLUDE :  $(PRJ)$/util$/makefile.pmk

JARFILES= unoil.jar jurt.jar ridl.jar juh.jar jut.jar java_uno.jar java_uno_accessbridge 

.IF "$(SYSTEM_SAXON)" == "YES"
EXTRAJARFILES = $(SAXON_JAR)
.ELSE
JARFILES += saxon9.jar
.ENDIF


JARCLASSDIRS	=                                               \
	com$/sun$/star$/wizards$/common	       	 		\
	com$/sun$/star$/wizards$/db	       	 		\
	com$/sun$/star$/wizards$/ui	       	 		\
	com$/sun$/star$/wizards$/document     	 		\
	com$/sun$/star$/wizards$/text

JARTARGET	= commonwizards.jar

# --- Files --------------------------------------------------------

JAVAFILES=							\
	common$/HelpIds.java	                		\
	common$/Desktop.java	                		\
	common$/NumberFormatter.java                 		\
	common$/FileAccess.java                 		\
	common$/Helper.java                     		\
	common$/InvalidQueryException.java      		\
	common$/JavaTools.java                  		\
	common$/NoValidPathException.java       		\
	common$/Properties.java                 		\
	common$/PropertyNames.java                 		\
	common$/Resource.java                   		\
	common$/SystemDialog.java				\
	common$/Configuration.java				\
	common$/ConfigNode.java					\
	common$/ConfigGroup.java				\
	common$/ConfigSet.java					\
	common$/XMLProvider.java				\
	common$/XMLHelper.java					\
	common$/IRenderer.java					\
	common$/Indexable.java					\
	common$/TerminateWizardException.java			\
	common$/UCB.java					\
	common$/DebugHelper.java					\
	common$/PropertySetHelper.java					\
	common$/NumericalHelper.java					\
	common$/NamedValueCollection.java       \
	db$/DBMetaData.java					\
	db$/CommandMetaData.java				\
	db$/QueryMetaData.java					\
	db$/BlindtextCreator.java				\
	db$/FieldColumn.java					\
	db$/CommandName.java					\
	db$/RecordParser.java					\
	db$/TypeInspector.java					\
	db$/ColumnPropertySet.java				\
	db$/RelationController.java				\
	db$/TableDescriptor.java				\
	db$/SQLQueryComposer.java				\
	db$/DatabaseObjectWizard.java					\
	ui$/event$/AbstractListener.java			\
	ui$/event$/CommonListener.java				\
	ui$/event$/DataAware.java				\
	ui$/event$/DataAwareFields.java				\
	ui$/event$/EventNames.java				\
	ui$/event$/ListModelBinder.java				\
	ui$/event$/MethodInvocation.java			\
	ui$/event$/RadioDataAware.java				\
	ui$/event$/SimpleDataAware.java				\
	ui$/event$/Task.java					\
	ui$/event$/TaskEvent.java				\
	ui$/event$/TaskListener.java				\
	ui$/event$/UnoDataAware.java				\
	ui$/AggregateComponent.java				\
	ui$/CommandFieldSelection.java				\
	ui$/ControlScroller.java				\
	ui$/DBLimitedFieldSelection.java			\
	ui$/DocumentPreview.java				\
	ui$/FilterComponent.java				\
	ui$/FieldSelection.java					\
	ui$/ImageList.java					\
	ui$/ButtonList.java					\
	ui$/PathSelection.java					\
	ui$/PeerConfig.java					\
	ui$/SortingComponent.java				\
	ui$/TitlesComponent.java				\
	ui$/WizardDialog.java					\
	ui$/UIConsts.java					\
	ui$/UnoDialog.java					\
	ui$/UnoDialog2.java					\
	ui$/XCommandSelectionListener.java			\
	ui$/XFieldSelectionListener.java			\
	ui$/XPathSelectionListener.java				\
	ui$/XCompletion.java					\
	document$/Control.java					\
	document$/DatabaseControl.java				\
	document$/FormHandler.java				\
	document$/GridControl.java				\
	document$/OfficeDocument.java				\
	document$/Shape.java					\
	document$/TimeStampControl.java				\
	text$/TextDocument.java					\
	text$/TextFieldHandler.java				\
	text$/TextFrameHandler.java				\
	text$/TextSectionHandler.java				\
	text$/TextStyleHandler.java				\
	text$/TextTableHandler.java				\
	text$/ViewHandler.java


#JAVACLASSFILES = $(foreach,i,$(JAVAFILES) $(CLASSDIR)$/$(PACKAGE)$/$(i:s/.java/.class/))

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
