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



#ifndef OOX_XLS_EXCELHANDLERS_HXX
#define OOX_XLS_EXCELHANDLERS_HXX

#include "oox/core/fragmenthandler2.hxx"
#include "oox/xls/worksheethelper.hxx"

namespace oox {
namespace xls {

// ============================================================================
// ============================================================================

/** Context handler derived from the WorkbookHelper helper class.

    Used to import contexts in global workbook fragments.
 */
class WorkbookContextBase : public ::oox::core::ContextHandler2, public WorkbookHelper
{
public:
    template< typename ParentType >
    inline explicit     WorkbookContextBase( ParentType& rParent ) :
                            ::oox::core::ContextHandler2( rParent ), WorkbookHelper( rParent ) {}
};

// ============================================================================

/** Context handler derived from the WorksheetHelper helper class.

    Used to import contexts in sheet fragments.
 */
class WorksheetContextBase : public ::oox::core::ContextHandler2, public WorksheetHelper
{
public:
    template< typename ParentType >
    inline explicit     WorksheetContextBase( ParentType& rParent ) :
                            ::oox::core::ContextHandler2( rParent ), WorksheetHelper( rParent ) {}
};

// ============================================================================

/** Fragment handler derived from the WorkbookHelper helper class.

    Used to import global workbook fragments.
 */
class WorkbookFragmentBase : public ::oox::core::FragmentHandler2, public WorkbookHelper
{
public:
    explicit            WorkbookFragmentBase(
                            const WorkbookHelper& rHelper,
                            const ::rtl::OUString& rFragmentPath );
};

// ============================================================================

/** Fragment handler derived from the WorksheetHelper helper class.

    Used to import sheet fragments.
 */
class WorksheetFragmentBase : public ::oox::core::FragmentHandler2, public WorksheetHelper
{
public:
    explicit            WorksheetFragmentBase(
                            const WorksheetHelper& rHelper,
                            const ::rtl::OUString& rFragmentPath );
};

// ============================================================================
// ============================================================================

/** Base class for all BIFF context handlers.

    Derived handlers have to implement the importRecord() function that has to
    import the record the passed BIFF input stream currently points to.
 */
class BiffContextHandler
{
public:
    virtual             ~BiffContextHandler();

    /** Derived classes have to implement importing the current record. */
    virtual void        importRecord( BiffInputStream& rStrm ) = 0;
};

// ----------------------------------------------------------------------------

/** Context handler derived from the WorkbookHelper helper class.

    Used to import contexts in global workbook fragments.
 */
class BiffWorkbookContextBase : public BiffContextHandler, public WorkbookHelper
{
protected:
    explicit            BiffWorkbookContextBase( const WorkbookHelper& rHelper );
};

// ----------------------------------------------------------------------------

/** Context handler derived from the WorksheetHelper helper class.

    Used to import contexts in sheet fragments.
 */
class BiffWorksheetContextBase : public BiffContextHandler, public WorksheetHelper
{
protected:
    explicit            BiffWorksheetContextBase( const WorksheetHelper& rHelper );
};

// ============================================================================

/** An enumeration for all types of fragments in a BIFF workbook stream. */
enum BiffFragmentType
{
    BIFF_FRAGMENT_GLOBALS,      /// Workbook globals fragment.
    BIFF_FRAGMENT_WORKSHEET,    /// Worksheet fragment.
    BIFF_FRAGMENT_CHARTSHEET,   /// Chart sheet fragment.
    BIFF_FRAGMENT_MACROSHEET,   /// Macro sheet fragment.
    BIFF_FRAGMENT_MODULESHEET,  /// BIFF5 VB module fragment.
    BIFF_FRAGMENT_EMPTYSHEET,   /// Sheet fragment of unsupported type.
    BIFF_FRAGMENT_WORKSPACE,    /// BIFF4 workspace/workbook globals.
    BIFF_FRAGMENT_UNKNOWN       /// Unknown fragment/error.
};

// ----------------------------------------------------------------------------

class BiffFragmentHandler
{
public:
    /** Opens the stream with the passed full name. */
    explicit            BiffFragmentHandler(
                            const ::oox::core::FilterBase& rFilter,
                            const ::rtl::OUString& rStrmName );

    virtual             ~BiffFragmentHandler();

    /** Imports the fragment, returns true, if EOF record has been reached. */
    virtual bool        importFragment() = 0;

protected:
    /** Returns the BIFF input stream of this fragment. */
    inline BiffInputStream& getInputStream() { return *mxBiffStrm; }
    
    /** Starts a new fragment in a workbbok stream and returns the fragment type.

        The passed stream must point before a BOF record. The function will
        try to start the next record and read the contents of the BOF record,
        if extant.

        @return  Fragment type according to the imported BOF record.
     */
    BiffFragmentType    startFragment( BiffType eBiff );

    /** Skips the current fragment up to its trailing EOF record.

        Skips all records until next EOF record. When this function returns,
        stream points to the EOF record, and the next call of startNextRecord()
        at the stream will start the record following the EOF record.

        Embedded fragments enclosed in BOF/EOF records (e.g. embedded chart
        objects) are skipped correctly.

        @return  True = stream points to the EOF record of the current fragment.
     */
    bool                skipFragment();

private:
    typedef ::boost::shared_ptr< BinaryXInputStream >   XInputStreamRef;
    typedef ::boost::shared_ptr< BiffInputStream >      BiffInputStreamRef;

    XInputStreamRef     mxXInStrm;
    BiffInputStreamRef  mxBiffStrm;
};

// ----------------------------------------------------------------------------

/** Fragment handler derived from the WorkbookHelper helper class.

    Used to import global workbook fragments.
 */
class BiffWorkbookFragmentBase : public BiffFragmentHandler, public WorkbookHelper
{
protected:
    explicit            BiffWorkbookFragmentBase(
                            const WorkbookHelper& rHelper,
                            const ::rtl::OUString& rStrmName,
                            bool bCloneDecoder = false );
};

// ----------------------------------------------------------------------------

/** Fragment handler derived from the WorksheetHelper helper class.

    Used to import sheet fragments.
 */
class BiffWorksheetFragmentBase : public BiffFragmentHandler, public WorksheetHelper
{
protected:
    explicit            BiffWorksheetFragmentBase(
                            const WorksheetHelper& rHelper,
                            const BiffWorkbookFragmentBase& rParent );
};

// ----------------------------------------------------------------------------

/** Special fragment handler for worksheets that have to be skipped.
 */
class BiffSkipWorksheetFragment : public BiffWorksheetFragmentBase
{
public:
    explicit            BiffSkipWorksheetFragment(
                            const WorksheetHelper& rHelper,
                            const BiffWorkbookFragmentBase& rParent );

    virtual bool        importFragment();
};

// ============================================================================
// ============================================================================

} // namespace xls
} // namespace oox

#endif
