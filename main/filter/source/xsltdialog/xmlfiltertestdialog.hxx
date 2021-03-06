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


#ifndef _XMLFILTERTESTDIALOG_HXX_
#define _XMLFILTERTESTDIALOG_HXX_

#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/document/XEventBroadcaster.hpp>
#include <vcl/dialog.hxx>

#ifndef _SV_BUTTON_HXX 
#include <vcl/button.hxx>
#endif
#include <vcl/fixed.hxx>
#include <svl/poolitem.hxx>

class filter_info_impl;
class XMLSourceFileDialog;

class XMLFilterTestDialog : public ModalDialog
{
public:
	XMLFilterTestDialog( Window* pParent, ResMgr& rResMgr, const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >& rxMSF  );
	virtual ~XMLFilterTestDialog();

	void test( const filter_info_impl& rFilterInfo );

	void updateCurrentDocumentButtonState( com::sun::star::uno::Reference< com::sun::star::lang::XComponent > * pRef = NULL );

private:
	DECL_LINK(ClickHdl_Impl, PushButton * );

	void onExportBrowse();
	void onExportCurrentDocument();
	void onImportBrowse();
	void onImportRecentDocument();
	void initDialog();

	com::sun::star::uno::Reference< com::sun::star::lang::XComponent > getFrontMostDocument( const rtl::OUString& rServiceName );
	void import( const rtl::OUString& rURL );
	void displayXMLFile( const rtl::OUString& rURL );
	void doExport( com::sun::star::uno::Reference< com::sun::star::lang::XComponent > xComp );

private:
	com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory > mxMSF;
	com::sun::star::uno::Reference< com::sun::star::document::XEventBroadcaster > mxGlobalBroadcaster;
	com::sun::star::uno::Reference< com::sun::star::document::XEventListener > mxGlobalEventListener;
	com::sun::star::uno::WeakReference< com::sun::star::lang::XComponent > mxLastFocusModel;

	ResMgr&				mrResMgr;

	rtl::OUString	maImportRecentFile;
	rtl::OUString	maExportRecentFile;

	FixedLine	maFLExport;
	FixedText	maFTExportXSLT;
	FixedText	maFTExportXSLTFile;
	FixedText	maFTTransformDocument;
	PushButton	maPBExportBrowse;
	PushButton	maPBCurrentDocument;
	FixedText	maFTNameOfCurentFile;
	FixedLine	maFLImport;
	FixedText	maFTImportXSLT;
	FixedText	maFTImportXSLTFile;
	FixedText	maFTImportTemplate;
	FixedText	maFTImportTemplateFile;
	FixedText	maFTTransformFile;
	CheckBox	maCBXDisplaySource;
	PushButton	maPBImportBrowse;
	PushButton	maPBRecentDocument;
	FixedText	maFTNameOfRecentFile;
	PushButton	maPBClose;
	HelpButton	maPBHelp;

	XMLSourceFileDialog*	mpSourceDLG;
	filter_info_impl*		mpFilterInfo;

	String maDialogTitle;

	::rtl::OUString sDTDPath;
};

#endif
