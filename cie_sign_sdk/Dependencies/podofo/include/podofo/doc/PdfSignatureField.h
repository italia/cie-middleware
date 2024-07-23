/***************************************************************************
 *   Copyright (C) 2011 by Dominik Seichter                                *
 *   domseichter@web.de                                                    *
 *                      by Petr Pytelka                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef _PDF_SIGNATURE_FIELD_H_
#define _PDF_SIGNATURE_FIELD_H_
#include "base/PdfDefines.h"
#include "PdfField.h"
#include "base/PdfDate.h"
#include "PdfDocument.h"
#include "base/PdfInputStream.h"
#include "doc/PdfImage.h"

#include <string>
using namespace std;

typedef struct _ImageData
{
	string imageData;
	int width;
	int height;
	int bitsPerComponent;
} ImageData;

namespace PoDoFo {


/** Signature field
 */
class PODOFO_DOC_API PdfSignatureField :public PdfField
{
protected:
    PdfObject*     m_pSignatureObj;
	PdfObject*     m_pAppearanceXObj;

    void Init();
public:
    /** Create a new PdfSignatureField
     */
    PdfSignatureField( PdfPage* pPage, const PdfRect & rRect, PdfDocument* pDoc,  const PdfString & rsText, const char* szSubFilter);

	PdfSignatureField( PdfPage* pPage, const PdfRect & rRect, PdfDocument* pDoc,  const PdfString & rsText, const char* szSubFilter, const ImageData* imageData);

	virtual ~PdfSignatureField();

    /** Create space for signature
     *
     * \param signatureData String used to locate reserved space for signature.
     *   This string will be replaiced with signature.
     *
     * Structure of the PDF file - before signing:
     * <</ByteRange[ 0 1234567890 1234567890 1234567890]/Contents<signatureData>
     * Have to be replaiced with the following structure:
     * <</ByteRange[ 0 count pos count]/Contents<real signature ...0-padding>
     */
    void SetSignature(const PdfData &signatureData);

	void SetSignatureSize(const size_t size);
    /** Set reason of the signature
     *
     *  \param rsText the reason of signature
     */
     void SetSignatureReason(const PdfString & rsLabel, const PdfString & rsText);
	
	void SetSignatureName(const PdfString & rsLabel, const PdfString & rsText);
	
	void SetSignatureLocation(const PdfString & rsLabel, const PdfString & rsText);

	/** Date of signature
	 */
	void SetSignatureDate(const PdfDate &sigDate);
	
	void SetAppearance(const char* szImagePath, const char* szDescription, const bool showData = FALSE);

	void SetGraphometricData(const PdfString & rsGraphometricDataName, const PdfString & rsGraphometricData, const PdfString & rsVersion);

private:
	PdfString m_rsText;
	PdfRect m_rRect;
	const char* m_szSubFilter;
	const ImageData* m_pImageData;
	PdfImage* m_pImage;
	PdfDocument* m_pDoc;

	PdfString m_rsName;
	PdfString m_rsLocation;
	PdfString m_rsReason;
	PdfString m_rsDate;
	PdfString m_rsNameLabel;
	PdfString m_rsLocationLabel;
	PdfString m_rsReasonLabel;
	
	
	
	
};

}

#endif
