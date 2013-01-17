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
#include "precompiled_sw.hxx"

#include <SwSpellDialogChildWindow.hxx>
#include <vcl/msgbox.hxx>
#include <editeng/svxacorr.hxx>
#include <editeng/acorrcfg.hxx>
#include <svx/svxids.hrc>
#include <sfx2/app.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/dispatch.hxx>
#include <editeng/unolingu.hxx>
#include <editeng/editeng.hxx>
#include <editeng/editview.hxx>
#include <wrtsh.hxx>
#include <sfx2/printer.hxx>
#include <svx/svdoutl.hxx>
#include <svx/svdview.hxx>
#include <svx/svditer.hxx>
#include <svx/svdogrp.hxx>
#include <unotools/linguprops.hxx>
#include <unotools/lingucfg.hxx>
#include <doc.hxx>
#include <docsh.hxx>
#include <docary.hxx>
#include <frmfmt.hxx>
#include <dcontact.hxx>
#include <edtwin.hxx>
#include <pam.hxx>
#include <drawbase.hxx>
#include <unotextrange.hxx>
#include <dialog.hrc>
#include <cmdid.h>
#include <svx/svdlegacy.hxx>
#include <svx/fmmodel.hxx>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::linguistic2;
using namespace ::com::sun::star::beans;

SFX_IMPL_CHILDWINDOW(SwSpellDialogChildWindow, FN_SPELL_GRAMMAR_DIALOG)


#define SPELL_START_BODY        0   // body text area
#define SPELL_START_OTHER       1   // frame, footnote, header, footer
#define SPELL_START_DRAWTEXT    2   // started in a draw text object

struct SpellState
{
    bool                m_bInitialCall;
    bool                m_bLockFocus; //lock the focus notification while a modal dialog is active
    bool                m_bLostFocus;

    //restart and progress information
    sal_uInt16              m_SpellStartPosition;
    bool                m_bBodySpelled;  //body already spelled
    bool                m_bOtherSpelled; //frames, footnotes, headers and footers spelled
    bool                m_bStartedInOther; //started the spelling insided of the _other_ area
    bool                m_bStartedInSelection; // there was an initial text selection
    SwPaM*              pOtherCursor; // position where the spelling inside the _other_ area started
    bool                m_bDrawingsSpelled; //all drawings spelled
    Reference<XTextRange> m_xStartRange; //text range that marks the start of spelling
    const SdrObject*    m_pStartDrawing; //draw text object spelling started in
    ESelection          m_aStartDrawingSelection; //draw text start selection
    bool                m_bRestartDrawing; // the first selected drawing object is found again

    //lose/get focus information to decide if spelling can be continued
    ShellModes          m_eSelMode;
    const SwNode*       m_pPointNode;
    const SwNode*       m_pMarkNode;
    xub_StrLen          m_nPointPos;
    xub_StrLen          m_nMarkPos;
    const SdrOutliner*  m_pOutliner;
    ESelection          m_aESelection;

    //iterating over draw text objects
    std::list<SdrTextObj*> m_aTextObjects;
    bool                m_bTextObjectsCollected;

    SpellState() :
        m_bInitialCall(true),
        m_bLockFocus(false),
        m_bLostFocus(false),
        m_SpellStartPosition(SPELL_START_BODY),
        m_bBodySpelled(false),
        m_bOtherSpelled(false),
        m_bStartedInOther(false),
        m_bStartedInSelection(false),
        pOtherCursor(0),
        m_bDrawingsSpelled(false),
        m_pStartDrawing(0),
        m_bRestartDrawing(false),

        m_eSelMode(SHELL_MODE_OBJECT), //initially invalid
        m_pPointNode(0),
        m_pMarkNode(0),
        m_nPointPos(0),
        m_nMarkPos(0),
        m_pOutliner(0),
        m_bTextObjectsCollected(false)
        {}

    ~SpellState() {delete pOtherCursor;}

    // reset state in ::InvalidateSpellDialog
    void    Reset()
            {   m_bInitialCall = true;
                m_bBodySpelled = m_bOtherSpelled = m_bDrawingsSpelled = false;
                m_xStartRange = 0;
                m_pStartDrawing = 0;
                m_bRestartDrawing = false;
				m_bTextObjectsCollected = false;
                m_aTextObjects.clear();
                m_bStartedInOther = false;
                delete pOtherCursor;
                pOtherCursor = 0;
            }
};
/*-- 30.10.2003 14:33:26---------------------------------------------------

  -----------------------------------------------------------------------*/
void lcl_LeaveDrawText(SwWrtShell& rSh)
{
    if(rSh.GetDrawView())
    {
        rSh.GetDrawView()->SdrEndTextEdit( sal_True );
        //go out of the frame
        rSh.SelectObj(basegfx::B2DPoint(LONG_MIN, LONG_MIN), SW_LEAVE_FRAME);
        rSh.EnterStdMode();
        rSh.GetView().AttrChangedNotify(&rSh);
    }
}
/*-- 09.09.2003 10:39:22---------------------------------------------------

  -----------------------------------------------------------------------*/
SwSpellDialogChildWindow::SwSpellDialogChildWindow (
            Window* _pParent,
            sal_uInt16 nId,
            SfxBindings* pBindings,
            SfxChildWinInfo* pInfo) :
                svx::SpellDialogChildWindow (
                    _pParent, nId, pBindings, pInfo),
                    m_pSpellState(new SpellState)
{

    String aPropName( String::CreateFromAscii(UPN_IS_GRAMMAR_INTERACTIVE ) );
    SvtLinguConfig().GetProperty( aPropName ) >>= m_bIsGrammarCheckingOn;
}
/*-- 09.09.2003 10:39:22---------------------------------------------------

  -----------------------------------------------------------------------*/
SwSpellDialogChildWindow::~SwSpellDialogChildWindow ()
{
    SwWrtShell* pWrtShell = GetWrtShell_Impl();
    if(!m_pSpellState->m_bInitialCall && pWrtShell)
        pWrtShell->SpellEnd();
    delete m_pSpellState;
}

/*-- 09.09.2003 12:40:07---------------------------------------------------

  -----------------------------------------------------------------------*/
SfxChildWinInfo SwSpellDialogChildWindow::GetInfo (void) const
{
    SfxChildWinInfo aInfo = svx::SpellDialogChildWindow::GetInfo();
    aInfo.bVisible = sal_False;
    return aInfo;
}

/*-- 09.09.2003 10:39:40---------------------------------------------------


  -----------------------------------------------------------------------*/
svx::SpellPortions SwSpellDialogChildWindow::GetNextWrongSentence(bool bRecheck)
{
    svx::SpellPortions aRet;
    SwWrtShell* pWrtShell = GetWrtShell_Impl();
    if(pWrtShell)
    {
        if (!bRecheck)
        {
            // first set continuation point for spell/grammar check to the
            // end of the current sentence
            pWrtShell->MoveContinuationPosToEndOfCheckedSentence();
        }

        ShellModes  eSelMode = pWrtShell->GetView().GetShellMode();
        bool bDrawText = SHELL_MODE_DRAWTEXT == eSelMode;
        bool bNormalText =
            SHELL_MODE_TABLE_TEXT == eSelMode ||
            SHELL_MODE_LIST_TEXT == eSelMode ||
            SHELL_MODE_TABLE_LIST_TEXT == eSelMode ||
            SHELL_MODE_TEXT == eSelMode;
        //Writer text outside of the body
        bool bOtherText = false;

        if( m_pSpellState->m_bInitialCall )
        {
            //if no text selection exists the cursor has to be set into the text
            if(!bDrawText && !bNormalText)
            {
                if(!MakeTextSelection_Impl(*pWrtShell, eSelMode))
                    return aRet;
                else
                {
                    // the selection type has to be checked again - both text types are possible
                    if(0 != (pWrtShell->GetSelectionType()& nsSelectionType::SEL_DRW_TXT))
                        bDrawText = true;
                    bNormalText = !bDrawText;
                }
            }
            if(bNormalText)
            {
                //set cursor to the start of the sentence
                if(!pWrtShell->HasSelection())
                    pWrtShell->GoStartSentence();
                else
                {
                    pWrtShell->ExpandToSentenceBorders();
                    m_pSpellState->m_bStartedInSelection = true;
                }
                //determine if the selection is outside of the body text
                bOtherText = !(pWrtShell->GetFrmType(0,sal_True) & FRMTYPE_BODY);
                m_pSpellState->m_SpellStartPosition = bOtherText ? SPELL_START_OTHER : SPELL_START_BODY;
                if(bOtherText)
                {
                    m_pSpellState->pOtherCursor = new SwPaM(*pWrtShell->GetCrsr()->GetPoint());
                    m_pSpellState->m_bStartedInOther = true;
                    pWrtShell->SpellStart( DOCPOS_OTHERSTART, DOCPOS_OTHEREND, DOCPOS_CURR, sal_False );
                }
                else
				{
					SwPaM* pCrsr = pWrtShell->GetCrsr();
					//mark the start position only if not at start of doc
					if(!pWrtShell->IsStartOfDoc())
					{
                        m_pSpellState->m_xStartRange =
                            SwXTextRange::CreateXTextRange(
                                *pWrtShell->GetDoc(),
                                *pCrsr->Start(), pCrsr->End());
					}
                    pWrtShell->SpellStart( DOCPOS_START, DOCPOS_END, DOCPOS_CURR, sal_False );
				}
            }
            else
            {
                SdrView* pSdrView = pWrtShell->GetDrawView();
                m_pSpellState->m_SpellStartPosition = SPELL_START_DRAWTEXT;
                m_pSpellState->m_pStartDrawing = pSdrView->getSelectedIfSingle();
                OutlinerView* pOLV = pSdrView->GetTextEditOutlinerView();
                // start checking at the top of the drawing object
                pOLV->SetSelection( ESelection() );
                m_pSpellState->m_aStartDrawingSelection = ESelection();
/*
Note: spelling in a selection only, or starting in a mid of a drawing object requires
further changes elsewhere. (Especially if it should work in sc and sd as well.) 
The code below would only be part of the solution. 
(Keeping it a as a comment for the time being)
                ESelection aCurSel( pOLV->GetSelection() );
                ESelection aSentenceSel( pOLV->GetEditView().GetEditEngine()->SelectSentence( aCurSel ) );
                if (!aCurSel.HasRange())
                {
                    aSentenceSel.nEndPara = aSentenceSel.nStartPara;
                    aSentenceSel.nEndPos  = aSentenceSel.nStartPos;
                }
                pOLV->SetSelection( aSentenceSel );
                m_pSpellState->m_aStartDrawingSelection = aSentenceSel;
*/
            }

            m_pSpellState->m_bInitialCall = false;
        }
        if( bDrawText )
        {
            // spell inside of the current draw text
            if(!SpellDrawText_Impl(*pWrtShell, aRet))
            {
                if(!FindNextDrawTextError_Impl(*pWrtShell) || !SpellDrawText_Impl(*pWrtShell, aRet))
                {
                    lcl_LeaveDrawText(*pWrtShell);
                    //now the drawings have been spelled
                    m_pSpellState->m_bDrawingsSpelled = true;
                    //the spelling continues at the other content
                    //if there's any that has not been spelled yet
                    if(!m_pSpellState->m_bOtherSpelled && pWrtShell->HasOtherCnt())
                    {
                        pWrtShell->SpellStart(DOCPOS_OTHERSTART, DOCPOS_OTHEREND, DOCPOS_OTHERSTART, sal_False );
                        if(!pWrtShell->SpellSentence(aRet, m_bIsGrammarCheckingOn))
						{
                            pWrtShell->SpellEnd();
							m_pSpellState->m_bOtherSpelled = true;
						}
                    }
                    else
                        m_pSpellState->m_bOtherSpelled = true;
                    //if no result has been found try at the body text - completely
                    if(!m_pSpellState->m_bBodySpelled && !aRet.size())
                    {
                        pWrtShell->SpellStart(DOCPOS_START, DOCPOS_END, DOCPOS_START, sal_False );
                        if(!pWrtShell->SpellSentence(aRet, m_bIsGrammarCheckingOn))
                        {
                            m_pSpellState->m_bBodySpelled = true;
                            pWrtShell->SpellEnd();
                        }
                    }

                }
            }
        }
        else
        {
            //spell inside of the Writer text
            if(!pWrtShell->SpellSentence(aRet, m_bIsGrammarCheckingOn))
            {
                // if there is a selection (within body or header/footer text) 
                // then spell/grammar checking should not move outside of it.
                if (!m_pSpellState->m_bStartedInSelection)
                {
                    //find out which text has been spelled body or other
                    bOtherText = !(pWrtShell->GetFrmType(0,sal_True) & FRMTYPE_BODY);
                    if(bOtherText && m_pSpellState->m_bStartedInOther && m_pSpellState->pOtherCursor)
                    {
                        m_pSpellState->m_bStartedInOther = false;
                        pWrtShell->SetSelection(*m_pSpellState->pOtherCursor);
                        pWrtShell->SpellEnd();
                        delete m_pSpellState->pOtherCursor;
                        m_pSpellState->pOtherCursor = 0;
                        pWrtShell->SpellStart(DOCPOS_OTHERSTART, DOCPOS_CURR, DOCPOS_OTHERSTART, sal_False );
                        pWrtShell->SpellSentence(aRet, m_bIsGrammarCheckingOn);
                    }
                    if(!aRet.size())
                    {
                        //end spelling
                        pWrtShell->SpellEnd();
                        if(bOtherText)
                        {
                            m_pSpellState->m_bOtherSpelled = true;
                            //has the body been spelled?
                            if(!m_pSpellState->m_bBodySpelled)
                            {
                                pWrtShell->SpellStart(DOCPOS_START, DOCPOS_END, DOCPOS_START, sal_False );
                                if(!pWrtShell->SpellSentence(aRet, m_bIsGrammarCheckingOn))
                                {
                                    m_pSpellState->m_bBodySpelled = true;
                                    pWrtShell->SpellEnd();
                                }
                            }
                        }
                        else
                        {
                             m_pSpellState->m_bBodySpelled = true;
                             if(!m_pSpellState->m_bOtherSpelled && pWrtShell->HasOtherCnt())
                             {
                                pWrtShell->SpellStart(DOCPOS_OTHERSTART, DOCPOS_OTHEREND, DOCPOS_OTHERSTART, sal_False );
                                if(!pWrtShell->SpellSentence(aRet, m_bIsGrammarCheckingOn))
                                {
                                    pWrtShell->SpellEnd();
                                    m_pSpellState->m_bOtherSpelled = true;
                                }
                             }
                             else
                                 m_pSpellState->m_bOtherSpelled = true;
                        }
                    }

                    //search for a draw text object that contains error and spell it
                    if(!aRet.size() &&
                            (m_pSpellState->m_bDrawingsSpelled ||
                            !FindNextDrawTextError_Impl(*pWrtShell) || !SpellDrawText_Impl(*pWrtShell, aRet)))
                    {
                        lcl_LeaveDrawText(*pWrtShell);
                        m_pSpellState->m_bDrawingsSpelled = true;
                    }
                }
            }
        }
        // now only the rest of the body text can be spelled -
        // if the spelling started inside of the body
        //
        bool bCloseMessage = true;
        if(!aRet.size() && !m_pSpellState->m_bStartedInSelection)
        {
            DBG_ASSERT(m_pSpellState->m_bDrawingsSpelled &&
                        m_pSpellState->m_bOtherSpelled && m_pSpellState->m_bBodySpelled,
                        "not all parts of the document are already spelled");
            if(m_pSpellState->m_xStartRange.is())
            {
                LockFocusNotification( true );
                sal_uInt16 nRet = QueryBox( GetWindow(),  SW_RES(RID_QB_SPELL_CONTINUE)).Execute();
                if(RET_YES == nRet)
                {
                    SwUnoInternalPaM aPam(*pWrtShell->GetDoc());
                    if (::sw::XTextRangeToSwPaM(aPam,
                                m_pSpellState->m_xStartRange))
                    {
                        pWrtShell->SetSelection(aPam);
                        pWrtShell->SpellStart(DOCPOS_START, DOCPOS_CURR, DOCPOS_START);
                        if(!pWrtShell->SpellSentence(aRet, m_bIsGrammarCheckingOn))
                            pWrtShell->SpellEnd();
                    }
                    m_pSpellState->m_xStartRange = 0;
                    LockFocusNotification( false );
                    //take care that the now valid selection is stored
                    LoseFocus();
                }
                else
                    bCloseMessage = false; //no closing message if a wrap around has been denied
            }
        }
        if(!aRet.size())
        {
            if(bCloseMessage)
			{
				LockFocusNotification( true );
                String sInfo(SW_RES(STR_SPELLING_COMPLETED));
                //#i84610#
                Window* pTemp = GetWindow();    // temporary needed for g++ 3.3.5
                InfoBox(pTemp, sInfo ).Execute();
                LockFocusNotification( false );
                //take care that the now valid selection is stored
                LoseFocus();
			}

            //close the spelling dialog
            GetBindings().GetDispatcher()->Execute(FN_SPELL_GRAMMAR_DIALOG, SFX_CALLMODE_ASYNCHRON);
        }
    }
    return aRet;

}
/*-- 09.09.2003 10:39:40---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwSpellDialogChildWindow::ApplyChangedSentence(const svx::SpellPortions& rChanged, bool bRecheck)
{
    SwWrtShell* pWrtShell = GetWrtShell_Impl();
    DBG_ASSERT(!m_pSpellState->m_bInitialCall, "ApplyChangedSentence in initial call or after resume");
    if(pWrtShell && !m_pSpellState->m_bInitialCall)
    {
		ShellModes  eSelMode = pWrtShell->GetView().GetShellMode();
		bool bDrawText = SHELL_MODE_DRAWTEXT == eSelMode;
		bool bNormalText =
			SHELL_MODE_TABLE_TEXT == eSelMode ||
			SHELL_MODE_LIST_TEXT == eSelMode ||
			SHELL_MODE_TABLE_LIST_TEXT == eSelMode ||
			SHELL_MODE_TEXT == eSelMode;

        // evaluate if the same sentence should be rechecked or not.
        // Sentences that got grammar checked should always be rechecked in order
        // to detect possible errors that get introduced with the changes
        bRecheck |= pWrtShell->HasLastSentenceGotGrammarChecked();

		if(bNormalText)
			pWrtShell->ApplyChangedSentence(rChanged, bRecheck);
        else if(bDrawText )
        {
            SdrView* pDrView = pWrtShell->GetDrawView();
            SdrOutliner *pOutliner = pDrView->GetTextEditOutliner();
            pOutliner->ApplyChangedSentence(pDrView->GetTextEditOutlinerView()->GetEditView(), rChanged, bRecheck);
        }
	}
}
/*-- 21.10.2003 09:33:57---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwSpellDialogChildWindow::AddAutoCorrection(
        const String& rOld, const String& rNew, LanguageType eLanguage)
{
    SvxAutoCorrect* pACorr = SvxAutoCorrCfg::Get()->GetAutoCorrect();
    pACorr->PutText( rOld, rNew, eLanguage );
}
/*-- 21.10.2003 09:33:59---------------------------------------------------

  -----------------------------------------------------------------------*/
bool SwSpellDialogChildWindow::HasAutoCorrection()
{
    return true;
}
/*-- 16.06.2008 11:59:17---------------------------------------------------

  -----------------------------------------------------------------------*/
bool SwSpellDialogChildWindow::HasGrammarChecking()
{
    return SvtLinguConfig().HasGrammarChecker();
}
/*-- 18.06.2008 12:27:11---------------------------------------------------

  -----------------------------------------------------------------------*/
bool SwSpellDialogChildWindow::IsGrammarChecking()
{
    return m_bIsGrammarCheckingOn;
}
/*-- 18.06.2008 12:27:11---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwSpellDialogChildWindow::SetGrammarChecking(bool bOn)
{
    uno::Any aVal;
    aVal <<= bOn;
    m_bIsGrammarCheckingOn = bOn;
    String aPropName( C2S(UPN_IS_GRAMMAR_INTERACTIVE ) );
    SvtLinguConfig().SetProperty( aPropName, aVal );
    // set current spell position to the start of the current sentence to
    // continue with this sentence after grammar checking state has been changed
    SwWrtShell* pWrtShell = GetWrtShell_Impl();
    if(pWrtShell)
    {
        ShellModes  eSelMode = pWrtShell->GetView().GetShellMode();
        bool bDrawText = SHELL_MODE_DRAWTEXT == eSelMode;
        bool bNormalText =
            SHELL_MODE_TABLE_TEXT == eSelMode ||
            SHELL_MODE_LIST_TEXT == eSelMode ||
            SHELL_MODE_TABLE_LIST_TEXT == eSelMode ||
            SHELL_MODE_TEXT == eSelMode;
        if( bNormalText )
            pWrtShell->PutSpellingToSentenceStart();
        else if( bDrawText )
        {
            SdrView*     pSdrView = pWrtShell->GetDrawView();
            SdrOutliner* pOutliner = pSdrView ? pSdrView->GetTextEditOutliner() : 0;
            DBG_ASSERT(pOutliner, "No Outliner in SwSpellDialogChildWindow::SetGrammarChecking");
            if(pOutliner)
            {
                pOutliner->PutSpellingToSentenceStart( pSdrView->GetTextEditOutlinerView()->GetEditView() );
            }
        }
    }
}
/*-- 28.10.2003 08:41:09---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwSpellDialogChildWindow::GetFocus()
{
    if(m_pSpellState->m_bLockFocus)
        return;
    bool bInvalidate = false;
    SwWrtShell* pWrtShell = GetWrtShell_Impl();
    if(pWrtShell && !m_pSpellState->m_bInitialCall)
    {
        ShellModes  eSelMode = pWrtShell->GetView().GetShellMode();
        if(eSelMode != m_pSpellState->m_eSelMode)
        {
            //prevent initial invalidation
            if(m_pSpellState->m_bLostFocus)
                bInvalidate = true;
        }
        else
        {
            switch(m_pSpellState->m_eSelMode)
            {
                case SHELL_MODE_TEXT:
                case SHELL_MODE_LIST_TEXT:
                case SHELL_MODE_TABLE_TEXT:
                case SHELL_MODE_TABLE_LIST_TEXT:
                {
                    SwPaM* pCursor = pWrtShell->GetCrsr();
                    if(m_pSpellState->m_pPointNode != pCursor->GetNode(sal_True) ||
                        m_pSpellState->m_pMarkNode != pCursor->GetNode(sal_False)||
                        m_pSpellState->m_nPointPos != pCursor->GetPoint()->nContent.GetIndex()||
                        m_pSpellState->m_nMarkPos != pCursor->GetMark()->nContent.GetIndex())
                            bInvalidate = true;
                }
                break;
                case SHELL_MODE_DRAWTEXT:
                {
                    SdrView*     pSdrView = pWrtShell->GetDrawView();
                    SdrOutliner* pOutliner = pSdrView ? pSdrView->GetTextEditOutliner() : 0;
                    if(!pOutliner || m_pSpellState->m_pOutliner != pOutliner)
                        bInvalidate = true;
                    else
                    {
                        OutlinerView* pOLV = pSdrView->GetTextEditOutlinerView();
                        DBG_ASSERT(pOLV, "no OutlinerView in SwSpellDialogChildWindow::GetFocus()");
                        if(!pOLV || !m_pSpellState->m_aESelection.IsEqual(pOLV->GetSelection()))
                            bInvalidate = true;
                    }
                }
                break;
                default: bInvalidate = true;
            }
        }
    }
    else
    {
        bInvalidate = true;
    }
    if(bInvalidate)
        InvalidateSpellDialog();
}
/*-- 28.10.2003 08:41:09---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwSpellDialogChildWindow::LoseFocus()
{
    //prevent initial invalidation
    m_pSpellState->m_bLostFocus = true;
    if(m_pSpellState->m_bLockFocus)
        return;
    SwWrtShell* pWrtShell = GetWrtShell_Impl();
    if(pWrtShell)
    {
        m_pSpellState->m_eSelMode = pWrtShell->GetView().GetShellMode();
        m_pSpellState->m_pPointNode = m_pSpellState->m_pMarkNode = 0;
        m_pSpellState->m_nPointPos = m_pSpellState->m_nMarkPos = 0;
        m_pSpellState->m_pOutliner = 0;

        switch(m_pSpellState->m_eSelMode)
        {
            case SHELL_MODE_TEXT:
            case SHELL_MODE_LIST_TEXT:
            case SHELL_MODE_TABLE_TEXT:
            case SHELL_MODE_TABLE_LIST_TEXT:
            {
                //store a node pointer and a pam-position to be able to check on next GetFocus();
                SwPaM* pCursor = pWrtShell->GetCrsr();
                m_pSpellState->m_pPointNode = pCursor->GetNode(sal_True);
                m_pSpellState->m_pMarkNode = pCursor->GetNode(sal_False);
                m_pSpellState->m_nPointPos = pCursor->GetPoint()->nContent.GetIndex();
                m_pSpellState->m_nMarkPos = pCursor->GetMark()->nContent.GetIndex();

            }
            break;
            case SHELL_MODE_DRAWTEXT:
            {
                SdrView*     pSdrView = pWrtShell->GetDrawView();
                SdrOutliner* pOutliner = pSdrView->GetTextEditOutliner();
                m_pSpellState->m_pOutliner = pOutliner;
                OutlinerView* pOLV = pSdrView->GetTextEditOutlinerView();
                DBG_ASSERT(pOutliner && pOLV, "no Outliner/OutlinerView in SwSpellDialogChildWindow::LoseFocus()");
                if(pOLV)
                {
                    m_pSpellState->m_aESelection = pOLV->GetSelection();
                }
            }
            break;
            default:;//prevent warning
        }
    }
    else
        m_pSpellState->m_eSelMode = SHELL_MODE_OBJECT;
}
/*-- 18.09.2003 12:50:18---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwSpellDialogChildWindow::InvalidateSpellDialog()
{
    SwWrtShell* pWrtShell = GetWrtShell_Impl();
    if(!m_pSpellState->m_bInitialCall && pWrtShell)
        pWrtShell->SpellEnd(0, false);
    m_pSpellState->Reset();
    svx::SpellDialogChildWindow::InvalidateSpellDialog();
}

/*-- 18.09.2003 12:54:59---------------------------------------------------

  -----------------------------------------------------------------------*/
SwWrtShell* SwSpellDialogChildWindow::GetWrtShell_Impl()
{
    SfxDispatcher* pDispatch = GetBindings().GetDispatcher();
    SwView* pView = 0;
    if(pDispatch)
    {
        sal_uInt16 nShellIdx = 0;
        SfxShell* pShell;
        while(0 != (pShell = pDispatch->GetShell(nShellIdx++)))
            {
            pView = dynamic_cast< SwView* >(pShell);
            
			if(pView)
            {
                break;
            }
    }
    }
    return pView ? pView->GetWrtShellPtr(): 0;
}

/*-- 13.10.2003 15:19:04---------------------------------------------------
    set the cursor into the body text - necessary if any object is selected
    on start of the spelling dialog
  -----------------------------------------------------------------------*/
bool SwSpellDialogChildWindow::MakeTextSelection_Impl(SwWrtShell& rShell, ShellModes  eSelMode)
{
    SwView& rView = rShell.GetView();
    switch(eSelMode)
    {
        case SHELL_MODE_TEXT:
        case SHELL_MODE_LIST_TEXT:
        case SHELL_MODE_TABLE_TEXT:
        case SHELL_MODE_TABLE_LIST_TEXT:
        case SHELL_MODE_DRAWTEXT:
            DBG_ERROR("text already active in SwSpellDialogChildWindow::MakeTextSelection_Impl()");
        break;

        case SHELL_MODE_FRAME:
        {
            rShell.UnSelectFrm();
            rShell.LeaveSelFrmMode();
            rView.AttrChangedNotify(&rShell);
        }
        break;

        case SHELL_MODE_DRAW:
        case SHELL_MODE_DRAW_CTRL:
        case SHELL_MODE_DRAW_FORM:
        case SHELL_MODE_BEZIER:
            if(FindNextDrawTextError_Impl(rShell))
			{
                rView.AttrChangedNotify(&rShell);
                break;
			}
        //otherwise no break to deselect the object
        case SHELL_MODE_GRAPHIC:
        case SHELL_MODE_OBJECT:
        {
            if ( rShell.IsDrawCreate() )
            {
                rView.GetDrawFuncPtr()->BreakCreate();
                rView.AttrChangedNotify(&rShell);
            }
            else if ( rShell.HasSelection() || rView.IsDrawMode() )
            {
                SdrView *pSdrView = rShell.GetDrawView();
                if(pSdrView && pSdrView->areSdrObjectsSelected() &&
                    pSdrView->GetHdlList().GetFocusHdl())
                {
                    ((SdrHdlList&)pSdrView->GetHdlList()).ResetFocusHdl();
                }
                else
                {
                    rView.LeaveDrawCreate();
                    //go out of the frame
                    rShell.SelectObj(basegfx::B2DPoint(LONG_MIN, LONG_MIN), SW_LEAVE_FRAME);
                    SfxBindings& rBind = rView.GetViewFrame()->GetBindings();
                    rBind.Invalidate( SID_ATTR_SIZE );
                    rShell.EnterStdMode();
                    rView.AttrChangedNotify(&rShell);
                }
            }
        }
        break;
        default:; //prevent warning
    }
    return true;
}
/*-- 13.10.2003 15:20:09---------------------------------------------------
    select the next draw text object that has a spelling error
  -----------------------------------------------------------------------*/
bool SwSpellDialogChildWindow::FindNextDrawTextError_Impl(SwWrtShell& rSh)
{
    bool bNextDoc = false;
    SdrView* pDrView = rSh.GetDrawView();
    if(!pDrView)
        return bNextDoc;
    SwView& rView = rSh.GetView();
    SwDoc* pDoc = rView.GetDocShell()->GetDoc();

    //start at the current draw object - if there is any selected
    SdrTextObj* pCurrentTextObj = dynamic_cast< SdrTextObj* >(pDrView->getSelectedIfSingle());

    //at first fill the list of drawing objects
    if(!m_pSpellState->m_bTextObjectsCollected )
    {
        m_pSpellState->m_bTextObjectsCollected = true;
        std::list<SdrTextObj*> aTextObjs;
        SwDrawContact::GetTextObjectsFromFmt( aTextObjs, pDoc );
        if(pCurrentTextObj)
        {
            m_pSpellState->m_aTextObjects.remove(pCurrentTextObj);
            m_pSpellState->m_aTextObjects.push_back(pCurrentTextObj);
                                }
                            }
    if(m_pSpellState->m_aTextObjects.size())
    {
        Reference< XSpellChecker1 >  xSpell( GetSpellChecker() );
        while(!bNextDoc && m_pSpellState->m_aTextObjects.size())
        {
            std::list<SdrTextObj*>::iterator aStart = m_pSpellState->m_aTextObjects.begin();
            SdrTextObj* pTextObj = *aStart;
            if(m_pSpellState->m_pStartDrawing == pTextObj)
                m_pSpellState->m_bRestartDrawing = true;
            m_pSpellState->m_aTextObjects.erase(aStart);
            OutlinerParaObject* pParaObj = pTextObj->GetOutlinerParaObject();
            if ( pParaObj )
            {
                bool bHasSpellError = false;
                {
                    SdrOutliner aTmpOutliner(pDoc->GetDrawModel()->
                                             GetDrawOutliner().GetEmptyItemSet().GetPool(),
                                                OUTLINERMODE_TEXTOBJECT );
                    aTmpOutliner.SetRefDevice( pDoc->getPrinter( false ) );
                    MapMode aMapMode (MAP_TWIP);
                    aTmpOutliner.SetRefMapMode(aMapMode);
                    aTmpOutliner.SetPaperSize( sdr::legacy::GetLogicRect(*pTextObj).GetSize() );
                    aTmpOutliner.SetSpeller( xSpell );

                    OutlinerView* pOutlView = new OutlinerView( &aTmpOutliner, &(rView.GetEditWin()) );
                    pOutlView->GetOutliner()->SetRefDevice( rSh.getIDocumentDeviceAccess()->getPrinter( false ) );
                    aTmpOutliner.InsertView( pOutlView );
                    Point aPt;
                    Size aSize(1,1);
                    Rectangle aRect( aPt, aSize );
                    pOutlView->SetOutputArea( aRect );
                    aTmpOutliner.SetText( *pParaObj );
                    aTmpOutliner.ClearModifyFlag();
                    bHasSpellError = EE_SPELL_OK != aTmpOutliner.HasSpellErrors();
                    aTmpOutliner.RemoveView( pOutlView );
                    delete pOutlView;
                }
                if(bHasSpellError)
                {
                    //now the current one has to be deselected
                    if(pCurrentTextObj)
                        pDrView->SdrEndTextEdit( sal_True );
                    //and the found one should be activated
                    rSh.MakeVisible(sdr::legacy::GetLogicRect(*pTextObj));
                    Point aTmp( 0,0 );
                    rSh.SelectObj( aTmp, 0, pTextObj );
                    rView.BeginTextEdit( pTextObj, &rView.GetEditWin(), sal_False, sal_True );
                    rView.AttrChangedNotify(&rSh);
                    bNextDoc = true;
                }
            }
        }
    }
    return bNextDoc;
}

/*-- 13.10.2003 15:24:27---------------------------------------------------

  -----------------------------------------------------------------------*/
bool SwSpellDialogChildWindow::SpellDrawText_Impl(SwWrtShell& rSh, ::svx::SpellPortions& rPortions)
{
    bool bRet = false;
    SdrView*     pSdrView = rSh.GetDrawView();
    SdrOutliner* pOutliner = pSdrView ? pSdrView->GetTextEditOutliner() : 0;
    DBG_ASSERT(pOutliner, "No Outliner in SwSpellDialogChildWindow::SpellDrawText_Impl");
    if(pOutliner)
    {
        bRet = pOutliner->SpellSentence(pSdrView->GetTextEditOutlinerView()->GetEditView(), rPortions, m_bIsGrammarCheckingOn);
        //find out if the current selection is in the first spelled drawing object
        //and behind the initial selection
        if(bRet && m_pSpellState->m_bRestartDrawing)
        {
            OutlinerView* pOLV = pSdrView->GetTextEditOutlinerView();
            ESelection aCurrentSelection = pOLV->GetSelection();
            if(m_pSpellState->m_aStartDrawingSelection.nEndPara < aCurrentSelection.nEndPara ||
               (m_pSpellState->m_aStartDrawingSelection.nEndPara ==  aCurrentSelection.nEndPara &&
                    m_pSpellState->m_aStartDrawingSelection.nEndPos <  aCurrentSelection.nEndPos))
			{
                bRet = false;
				rPortions.clear();
			}
        }
    }
    return bRet;
}
/*-- 30.10.2003 14:54:59---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwSpellDialogChildWindow::LockFocusNotification(bool bLock)
{
    DBG_ASSERT(m_pSpellState->m_bLockFocus != bLock, "invalid locking - no change of state");
    m_pSpellState->m_bLockFocus = bLock;
}

