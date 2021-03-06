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


#ifndef _SVX_NUMITEM_HXX
#define _SVX_NUMITEM_HXX

// include ---------------------------------------------------------------

#include <tools/link.hxx>
#include <tools/string.hxx>
#include <svl/poolitem.hxx>
#include <editeng/svxenum.hxx>
#include <tools/gen.hxx>
#include <editeng/numdef.hxx>
#include <tools/color.hxx>
#include <cppuhelper/weakref.hxx>
#include <com/sun/star/lang/Locale.hpp>
#include <com/sun/star/uno/Reference.h>
#include <com/sun/star/style/NumberingType.hpp>
#include <unotools/fontcvt.hxx>
#include <editeng/editengdllapi.h>

class SvxBrushItem;
class Font;
class Graphic;
class SvxNodeNum;
class BitmapEx;
namespace com{namespace sun{ namespace star{
	namespace text{
		class XNumberingFormatter;
	}
}}}

// -----------------------------------------------------------------------
//Feature-Flags (only sal_uInt16!)
#define NUM_CONTINUOUS 			0x0001 // fortlaufende Numerierung moeglich?
#define NUM_CHAR_TEXT_DISTANCE 	0x0002 // Abstand Symbol<->Text?
#define NUM_CHAR_STYLE			0x0004 // Zeichenvorlagen?
#define NUM_BULLET_REL_SIZE		0x0008 // rel. Bulletgroesse?
#define NUM_BULLET_COLOR		0x0010 // Bullet color
#define NUM_SYMBOL_ALIGNMENT	0x0040 // alignment soll unter den Optionen angezeigt werden
#define NUM_NO_NUMBERS			0x0080 // Numberierungen sind nicht erlaubt
#define NUM_ENABLE_LINKED_BMP	0x0100 // linked bitmaps are available
#define NUM_ENABLE_EMBEDDED_BMP	0x0200 // embedded bitmaps are available

#define SVX_NO_NUM				200 // Markierung fuer keine Numerierung
#define SVX_NO_NUMLEVEL         0x20

#define LINK_TOKEN 	0x80 //indicate linked bitmaps - for use in dialog only
class EDITENG_DLLPUBLIC SvxNumberType
{
	static sal_Int32 nRefCount;
	static com::sun::star::uno::Reference<com::sun::star::text::XNumberingFormatter> xFormatter;

	sal_Int16		nNumType;
	sal_Bool		bShowSymbol;		// Symbol auch anzeigen?

public:
	SvxNumberType(sal_Int16 nType = com::sun::star::style::NumberingType::ARABIC);
	SvxNumberType(const SvxNumberType& rType);
	~SvxNumberType();

	String 			GetNumStr( sal_uLong nNo ) const;
	String 			GetNumStr( sal_uLong nNo, const com::sun::star::lang::Locale& rLocale ) const;

	void			SetNumberingType(sal_Int16 nSet) {nNumType = nSet;}
	sal_Int16		GetNumberingType() const {return nNumType;}

	void 			SetShowSymbol(sal_Bool bSet) {bShowSymbol = bSet;}
	sal_Bool		IsShowSymbol()const{return bShowSymbol;}

	sal_Bool		IsTxtFmt() const
					{
						return com::sun::star::style::NumberingType::NUMBER_NONE != nNumType &&
		   					com::sun::star::style::NumberingType::CHAR_SPECIAL != nNumType &&
		   					com::sun::star::style::NumberingType::BITMAP != nNumType;
					}
};

class EDITENG_DLLPUBLIC SvxNumberFormat : public SvxNumberType
{
public:
    // --> OD 2008-01-08 #newlistlevelattrs#
    enum SvxNumPositionAndSpaceMode
    {
        LABEL_WIDTH_AND_POSITION,
        LABEL_ALIGNMENT
    };
    enum SvxNumLabelFollowedBy
    {
        LISTTAB,
        SPACE,
        NOTHING
    };
    // <--

private:
	String 				sPrefix;
	String 				sSuffix;

	SvxAdjust			eNumAdjust;

	sal_uInt8				nInclUpperLevels;	//Nummern aus der vorigen Ebenen uebernehmen
	sal_uInt16 				nStart;				//Start der Zaehlung

	sal_Unicode 		cBullet;			//Symbol
	sal_uInt16				nBulletRelSize;		//proz. Groesse des Bullets
	Color				nBulletColor;		//Bullet color

    // --> OD 2008-01-08 #newlistlevelattrs#
    // mode indicating, if the position and spacing of the list label is
    // determined by the former attributes (nFirstLineOffset, nAbsLSpace,
    // nLSpace and nCharTextDistance) called position and spacing via label
    // width and position (LABEL_WIDTH_AND_POSITION) or by the new attributes
    // (meLabelFollowedBy, mnListtabPos, mnFirstLineIndent and mnIndentAt)
    // called position and spacing via label alignment.
    // Note 1: Attribute <eNumAdjust> is relevant for both modes.
    // Note 2: The values of the former attributes are treated as 0, if mode
    //         LABEL_ALIGNMENT is active.
    SvxNumPositionAndSpaceMode mePositionAndSpaceMode;
    // <--

	short 				nFirstLineOffset;   //Erstzeileneinzug
	short 				nAbsLSpace;			//Abstand Rand<->Nummer
	short 				nLSpace;			//relative Einrueckung zum Vorgaenger
	short				nCharTextDistance;	//Abstand Nummer<->Text

    // --> OD 2008-01-08 #newlistlevelattrs#
    // specifies what follows the list label before the text of the first line
    // of the list item starts
    SvxNumLabelFollowedBy       meLabelFollowedBy;
    // specifies an additional list tab stop position for meLabelFollowedBy = LISTTAB
    long                        mnListtabPos;
    // specifies the first line indent
    long                        mnFirstLineIndent;
    // specifies the indent before the text, e.g. in L2R-layout the left margin
    long                        mnIndentAt;
    // <--

	SvxBrushItem* 	 	pGraphicBrush; 			//
    sal_Int16           eVertOrient;        // vert. Ausrichtung einer Bitmap

	Size 				aGraphicSize;		// immer! in 1/100 mm
	Font* 				pBulletFont;		// Pointer auf den BulletFont

	String				sCharStyleName;		// Zeichenvorlage

	BitmapEx*			pScaledImageCache;	// Image scaled to aGraphicSize, only cached for WINDOW/VDEV

	DECL_STATIC_LINK( SvxNumberFormat, GraphicArrived, void * );
    virtual void NotifyGraphicArrived();
public:
    // --> OD 2008-01-09 #newlistlevelattrs#
    SvxNumberFormat( sal_Int16 nNumberingType,
                     SvxNumPositionAndSpaceMode ePositionAndSpaceMode = LABEL_WIDTH_AND_POSITION );
    // <--
	SvxNumberFormat(const SvxNumberFormat& rFormat);
    SvxNumberFormat(SvStream &rStream);

	virtual ~SvxNumberFormat();

    SvStream&       Store(SvStream &rStream, FontToSubsFontConverter pConverter);

	SvxNumberFormat& operator=( const SvxNumberFormat&  );
	sal_Bool 			operator==( const SvxNumberFormat&  ) const;
	sal_Bool            operator!=( const SvxNumberFormat& rFmt) const {return !(*this == rFmt);}

	void			SetNumAdjust(SvxAdjust eSet) {eNumAdjust = eSet;}
	SvxAdjust		GetNumAdjust() const {return eNumAdjust;}
	void 			SetPrefix(const String& rSet) { sPrefix = rSet;}
	const String&	GetPrefix() const { return sPrefix;}
	void            SetSuffix(const String& rSet) { sSuffix = rSet;}
	const String&	GetSuffix() const { return sSuffix;}

	void					SetCharFmtName(const String& rSet){ sCharStyleName = rSet; }
	virtual const String&	GetCharFmtName()const;

	void			SetBulletFont(const Font* pFont);
	const Font* 	GetBulletFont() const {return pBulletFont;}
	void			SetBulletChar(sal_Unicode cSet){cBullet = cSet;}
	sal_Unicode 	GetBulletChar()const {return cBullet;}
	void			SetBulletRelSize(sal_uInt16 nSet) {nBulletRelSize = nSet;}
	sal_uInt16			GetBulletRelSize() const { return nBulletRelSize;}
	void			SetBulletColor(Color nSet){nBulletColor = nSet;}
	Color			GetBulletColor()const {return nBulletColor;}

	void			SetIncludeUpperLevels( sal_uInt8 nSet ) { nInclUpperLevels = nSet;}
	sal_uInt8 			GetIncludeUpperLevels()const  { return nInclUpperLevels;}
	void 			SetStart(sal_uInt16 nSet) {nStart = nSet;}
	sal_uInt16			GetStart() const {return nStart;}

    virtual void    SetGraphicBrush( const SvxBrushItem* pBrushItem, const Size* pSize = 0, const sal_Int16* pOrient = 0);
	const SvxBrushItem* 		GetBrush() const {return pGraphicBrush;}
	void 			SetGraphic( const String& rName );
    virtual void        SetVertOrient(sal_Int16 eSet);
    virtual sal_Int16   GetVertOrient() const;
	void			SetGraphicSize(const Size& rSet) {aGraphicSize = rSet;}
	const Size&		GetGraphicSize() const {return aGraphicSize;}

    // --> OD 2008-01-09 #newlistlevelattrs#
    SvxNumPositionAndSpaceMode GetPositionAndSpaceMode() const;
    void SetPositionAndSpaceMode( SvxNumPositionAndSpaceMode ePositionAndSpaceMode );
    // <--

	void			SetLSpace(short nSet) {nLSpace = nSet;}
    // --> OD 2008-01-09 #newlistlevelattrs#
    short           GetLSpace() const;
    // <--
	void			SetAbsLSpace(short nSet) {nAbsLSpace = nSet;}
    // --> OD 2008-01-09 #newlistlevelattrs#
    short           GetAbsLSpace() const;
    // <--
	void 			SetFirstLineOffset(short nSet) { nFirstLineOffset = nSet;}
    // --> OD 2008-01-09 #newlistlevelattrs#
    short           GetFirstLineOffset() const;
    // <--
	void			SetCharTextDistance(short nSet) { nCharTextDistance = nSet; }
    // --> OD 2008-01-09 #newlistlevelattrs#
    short           GetCharTextDistance() const;
    // <--

    // --> OD 2008-01-09 #newlistlevelattrs#
    void SetLabelFollowedBy( const SvxNumLabelFollowedBy eLabelFollowedBy );
    SvxNumLabelFollowedBy GetLabelFollowedBy() const;
    void SetListtabPos( const long nListtabPos );
    long GetListtabPos() const;
    void SetFirstLineIndent( const long nFirstLineIndent );
    long GetFirstLineIndent() const;
    void SetIndentAt( const long nIndentAt );
    long GetIndentAt() const;
    // <--

	static Size		GetGraphicSizeMM100(const Graphic* pGraphic);
	static String 	CreateRomanString( sal_uLong nNo, sal_Bool bUpper );
};

enum SvxNumRuleType
{
	SVX_RULETYPE_NUMBERING,
	SVX_RULETYPE_OUTLINE_NUMBERING,
	SVX_RULETYPE_PRESENTATION_NUMBERING,
	SVX_RULETYPE_END
};

class EDITENG_DLLPUBLIC SvxNumRule
{
	sal_uInt16 				nLevelCount;            // Anzahl der unterstuetzten Levels
	sal_uInt32 				nFeatureFlags;          // was wird unterstuetzt?
	SvxNumRuleType		eNumberingType;         // was fuer eine Numerierung
	sal_Bool 				bContinuousNumbering;	// fortlaufende Numerierung

	SvxNumberFormat* 	aFmts[SVX_MAX_NUM];
	sal_Bool			 	aFmtsSet[SVX_MAX_NUM]; //Flags ueber Gueltigkeit der Ebenen

	static sal_Int32	nRefCount;
	com::sun::star::lang::Locale aLocale;
public:
    // --> OD 2008-02-11 #newlistlevelattrs#
    SvxNumRule( sal_uLong nFeatures,
                sal_uInt16 nLevels,
                sal_Bool bCont,
                SvxNumRuleType eType = SVX_RULETYPE_NUMBERING,
                SvxNumberFormat::SvxNumPositionAndSpaceMode
                        eDefaultNumberFormatPositionAndSpaceMode
                                = SvxNumberFormat::LABEL_WIDTH_AND_POSITION );
    // <--
	SvxNumRule(const SvxNumRule& rCopy);
	SvxNumRule(SvStream &rStream);
	virtual ~SvxNumRule();

	int              		operator==( const SvxNumRule& ) const;
	int              		operator!=( const SvxNumRule& rRule ) const {return !(*this == rRule);}

	SvxNumRule& 			operator=( const SvxNumRule&  );

	SvStream&               Store(SvStream &rStream);

	const SvxNumberFormat* 	Get(sal_uInt16 nLevel)const;
	const SvxNumberFormat& 	GetLevel(sal_uInt16 nLevel)const;
	void					SetLevel(sal_uInt16 nLevel, const SvxNumberFormat& rFmt, sal_Bool bIsValid = sal_True);
	void					SetLevel(sal_uInt16 nLevel, const SvxNumberFormat* pFmt);

	sal_Bool					IsContinuousNumbering()const
											{return bContinuousNumbering;}
	void					SetContinuousNumbering(sal_Bool bSet)
											{bContinuousNumbering = bSet;}

	sal_uInt16 					GetLevelCount() const {return nLevelCount;}
	sal_Bool					IsFeatureSupported(sal_uInt32 nFeature) const
											{return 0 != (nFeatureFlags & nFeature);}
	sal_uInt32 					GetFeatureFlags() const {return nFeatureFlags;}
	void					SetFeatureFlag( sal_uInt32 nFlag, sal_Bool bSet = sal_True ) { if(bSet) nFeatureFlags |= nFlag; else nFeatureFlags &= ~nFlag; }

	String 					MakeNumString( const SvxNodeNum&, sal_Bool bInclStrings = sal_True ) const;

	SvxNumRuleType			GetNumRuleType() const { return eNumberingType; }
	void                    SetNumRuleType( const SvxNumRuleType& rType ) { eNumberingType = rType; }

	sal_Bool					UnLinkGraphics();
};
/* -----------------27.10.98 13:04-------------------
 *
 * --------------------------------------------------*/
class EDITENG_DLLPUBLIC SvxNumBulletItem : public SfxPoolItem
{
	SvxNumRule* 			pNumRule;
public:
	SvxNumBulletItem(SvxNumRule& rRule);
	SvxNumBulletItem(SvxNumRule& rRule, sal_uInt16 nWhich );
	SvxNumBulletItem(const SvxNumBulletItem& rCopy);
	virtual ~SvxNumBulletItem();

	virtual SfxPoolItem*     Clone( SfxItemPool *pPool = 0 ) const;
    virtual SfxPoolItem*     Create(SvStream &, sal_uInt16) const;
	sal_uInt16  GetVersion( sal_uInt16 nFileVersion ) const;
    virtual SvStream&        Store(SvStream &, sal_uInt16 nItemVersion ) const;
	virtual int              operator==( const SfxPoolItem& ) const;

	SvxNumRule* 			GetNumRule() const {return pNumRule;}

	virtual	sal_Bool		QueryValue( com::sun::star::uno::Any& rVal, sal_uInt8 nMemberId = 0 ) const;
	virtual	sal_Bool		PutValue( const com::sun::star::uno::Any& rVal, sal_uInt8 nMemberId = 0 );
};
/* -----------------28.10.98 15:21-------------------
 *
 * --------------------------------------------------*/
class SvxNodeNum
{
	sal_uInt16 nLevelVal[ SVX_MAX_NUM ];	// Nummern aller Levels
	sal_uInt16 nSetValue;					// vorgegebene Nummer
	sal_uInt8 nMyLevel;						// akt. Level
	sal_Bool bStartNum;						// Numerierung neu starten

public:
	inline SvxNodeNum( sal_uInt8 nLevel = SVX_NO_NUM, sal_uInt16 nSetVal = USHRT_MAX );
	inline SvxNodeNum& operator=( const SvxNodeNum& rCpy );

	sal_uInt8 GetLevel() const 					{ return nMyLevel; }
	void SetLevel( sal_uInt8 nVal )  			{ nMyLevel = nVal; }

	sal_Bool IsStart() const					{ return bStartNum; }
	void SetStart( sal_Bool bFlag = sal_True ) 		{ bStartNum = bFlag; }

	sal_uInt16 GetSetValue() const 				{ return nSetValue; }
	void SetSetValue( sal_uInt16 nVal )  		{ nSetValue = nVal; }

	const sal_uInt16* GetLevelVal() const 		{ return nLevelVal; }
		  sal_uInt16* GetLevelVal() 	 		{ return nLevelVal; }
};

SvxNodeNum::SvxNodeNum( sal_uInt8 nLevel, sal_uInt16 nSetVal )
	: nSetValue( nSetVal ), nMyLevel( nLevel ), bStartNum( sal_False )
{
	memset( nLevelVal, 0, sizeof( nLevelVal ) );
}

inline SvxNodeNum& SvxNodeNum::operator=( const SvxNodeNum& rCpy )
{
	nSetValue = rCpy.nSetValue;
	nMyLevel = rCpy.nMyLevel;
	bStartNum = rCpy.bStartNum;

	memcpy( nLevelVal, rCpy.nLevelVal, sizeof( nLevelVal ) );
	return *this;
}

/* --------------------------------------------------
 *
 * --------------------------------------------------*/
SvxNumRule* SvxConvertNumRule( const SvxNumRule* pRule, sal_uInt16 nLevel, SvxNumRuleType eType );

#endif

