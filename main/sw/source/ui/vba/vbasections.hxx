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


#ifndef SW_VBA_SECTIONS_HXX
#define SW_VBA_SECTIONS_HXX

#include <vbahelper/vbacollectionimpl.hxx>
#include <ooo/vba/word/XSections.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>
#include <vbahelper/vbahelperinterface.hxx>

typedef CollTestImplHelper< ooo::vba::word::XSections > SwVbaSections_BASE;

class SwVbaSections : public SwVbaSections_BASE
{
private:
    css::uno::Reference< css::frame::XModel > mxModel;

public:
	SwVbaSections( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext > & xContext, const css::uno::Reference< css::frame::XModel >& xModel );
	virtual ~SwVbaSections() {}

	// XEnumerationAccess
	virtual css::uno::Type SAL_CALL getElementType() throw (css::uno::RuntimeException);
	virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration() throw (css::uno::RuntimeException);

    virtual css::uno::Any SAL_CALL PageSetup(  ) throw (css::uno::RuntimeException);

	// SwVbaSections_BASE
	virtual css::uno::Any createCollectionObject( const css::uno::Any& aSource ); 
	virtual rtl::OUString& getServiceImplName();
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};    

#endif /* SW_VBA_SECTIONS_HXX */
