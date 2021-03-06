/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_framework.hxx"

//_________________________________________________________________________________________________________________
//	my own includes
//_________________________________________________________________________________________________________________
#include <classes/servicemanager.hxx>
#include <classes/filtercache.hxx>
#include <macros/generic.hxx>
#include <macros/debug.hxx>
#include <services.h>

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>

//_________________________________________________________________________________________________________________
//	other includes
//_________________________________________________________________________________________________________________
#include <comphelper/processfactory.hxx>
#include <vos/process.hxx>
#include <rtl/ustring.hxx>
#include <rtl/ustrbuf.hxx>

#include <vcl/event.hxx>
#include <vcl/svapp.hxx>
#include <vcl/wrkwin.hxx>
#include <vcl/msgbox.hxx>
#include <stdio.h>

//_________________________________________________________________________________________________________________
//	const
//_________________________________________________________________________________________________________________

#define	RDBFILE									DECLARE_ASCII("typecfg.rdb"		)
#define	ARGUMENT_GENERATE_CFGVIEW				DECLARE_ASCII("-cfgview"		)
#define	ARGUMENT_CHECK_FILTERREGISTRATION		DECLARE_ASCII("-registerfilter"	)
#define	ARGUMENT_GENERATE_TYPEDETECTION_XCD		DECLARE_ASCII("-generatexcd"	)

//_________________________________________________________________________________________________________________
//	namespace
//_________________________________________________________________________________________________________________

using namespace ::std						;
using namespace ::vos						;
using namespace ::rtl						;
using namespace ::framework					;
using namespace ::comphelper				;
using namespace ::com::sun::star::uno		;
using namespace ::com::sun::star::lang		;
using namespace ::com::sun::star::container	;
using namespace ::com::sun::star::beans		;

//_________________________________________________________________________________________________________________
//	defines
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
//	declarations
//_________________________________________________________________________________________________________________

enum EMode
{
	E_GENERATE_CFGVIEW		   		,
	E_CHECK_FILTERREGISTRATION		,
	E_GENERATE_TYPEDETECTION_XCD
};

/*-***************************************************************************************************************/
class TypeApplication : public Application
{
	//*************************************************************************************************************
	public:

		void Main();

	//*************************************************************************************************************
	private:

		void impl_parseCommandLine			();
		void impl_generateCFGView			();
		void impl_checkFilterRegistration	();
		void impl_generateTypeDetectionXCD	();

	//*************************************************************************************************************
	private:

		EMode								m_eMode				;
		Reference< XMultiServiceFactory >	m_xServiceManager	;

		FilterCache*						m_pCache			;

};	//	class FilterApplication

//_________________________________________________________________________________________________________________
//	global variables
//_________________________________________________________________________________________________________________

TypeApplication gApplication;

//*****************************************************************************************************************
void TypeApplication::Main()
{
	// Init global servicemanager and set it.
	ServiceManager aManager;
	m_xServiceManager = aManager.getSharedUNOServiceManager( RDBFILE );
	setProcessServiceFactory( m_xServiceManager );

	m_pCache = new FilterCache;

	impl_parseCommandLine();

	switch( m_eMode )
	{
		case E_GENERATE_CFGVIEW					:	impl_generateCFGView();
													break;
		case E_CHECK_FILTERREGISTRATION			:	impl_checkFilterRegistration();
													break;
		case E_GENERATE_TYPEDETECTION_XCD  		:	impl_generateTypeDetectionXCD();
													break;
	}

	delete m_pCache;
	m_pCache = NULL;
}

//*****************************************************************************************************************
void TypeApplication::impl_parseCommandLine()
{
	OStartupInfo	aInfo		;
	OUString		sArgument	;
	sal_Int32		nArgument	= 0							;
	sal_Int32		nCount		= aInfo.getCommandArgCount();

	while( nArgument<nCount )
	{
		aInfo.getCommandArg( nArgument, sArgument );

		if( sArgument == ARGUMENT_GENERATE_CFGVIEW )
		{
			m_eMode = E_GENERATE_CFGVIEW;
			break;
		}
		else
		if( sArgument == ARGUMENT_CHECK_FILTERREGISTRATION )
		{
			m_eMode = E_CHECK_FILTERREGISTRATION;
			break;
		}
		else
		if( sArgument == ARGUMENT_GENERATE_TYPEDETECTION_XCD )
		{
			m_eMode = E_GENERATE_TYPEDETECTION_XCD;
			break;
		}

		++nArgument;
	}
}

//*****************************************************************************************************************
void TypeApplication::impl_generateCFGView()
{
	#ifdef ENABLE_FILTERCACHEDEBUG
		// Cache use ref count!
		FilterCache aCache;
		aCache.impldbg_generateHTMLView();
	#endif	//ENABLE_FILTERCACHEDEBUG
}

//*****************************************************************************************************************
void TypeApplication::impl_checkFilterRegistration()
{
	Reference< XNameContainer > xFilterContainer( m_xServiceManager->createInstance( SERVICENAME_FILTERFACTORY ), UNO_QUERY );
	LOG_ASSERT2( xFilterContainer.is()==sal_False, "TypeApplication::impl_checkFilterRegistration()", "Couldn't create filter factory!" )
	if( xFilterContainer.is() == sal_True )
	{
		Sequence< PropertyValue > lProperties( 8 );

		lProperties[0].Name		=	DECLARE_ASCII("Type")				;
		lProperties[0].Value	<<=	DECLARE_ASCII("MeinType")			;

		lProperties[1].Name		=	DECLARE_ASCII("UIName")				;
		lProperties[1].Value	<<=	DECLARE_ASCII("MeinUIName")			;

		lProperties[2].Name		=	DECLARE_ASCII("UINames")	   		;
		lProperties[2].Value	<<=	Sequence< PropertyValue >()			;

		lProperties[3].Name		=	DECLARE_ASCII("DocumentService")	;
		lProperties[3].Value	<<=	DECLARE_ASCII("MeinDocService")		;

		lProperties[4].Name		=	DECLARE_ASCII("FilterService")		;
		lProperties[4].Value	<<=	DECLARE_ASCII("MeinFilterService")	;

		lProperties[5].Name		=	DECLARE_ASCII("Flags")				;
		lProperties[5].Value	<<=	(sal_Int32)256						;

		lProperties[6].Name		=	DECLARE_ASCII("UserData")			;
		lProperties[6].Value	<<=	Sequence< OUString >()				;

		lProperties[7].Name		=	DECLARE_ASCII("FileFormatVersion")	;
		lProperties[7].Value	<<=	(sal_Int32)0						;

		lProperties[8].Name		=	DECLARE_ASCII("TemplateName")		;
		lProperties[8].Value	<<=	DECLARE_ASCII("MeinTemplate")		;
	}
}

//*****************************************************************************************************************
void TypeApplication::impl_generateTypeDetectionXCD()
{
	#ifdef ENABLE_GENERATEFILTERCACHE
	// Cache use ref count!
	FilterCache	aCache					;
	sal_Bool	bWriteable	=	sal_True;
	sal_Unicode	cSeparator	=	','		;
	aCache.impldbg_generateXCD(	"org.openoffice.Office.TypeDetection.xcd", bWriteable, cSeparator );
	#endif	//ENABLE_GENERATEFILTERCACHE
}
