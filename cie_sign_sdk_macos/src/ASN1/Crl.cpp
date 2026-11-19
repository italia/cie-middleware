/*
 *  CCrl.cpp
 *  iDigitalSApp
 *
 *  Created by svp on 15/07/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "Crl.h"
#include "ASN1Octetstring.h"
#include "ASN1UTCTime.h"
#include "UUCLogger.h"

USE_LOG;

CCrl::CCrl(UUCBufferedReader& reader)
: CASN1Sequence(reader)
{
}

CCrl::CCrl(const CASN1Object& contentInfo)
: CASN1Sequence(contentInfo)
{
}

bool CCrl::isRevoked(const CASN1Integer& serialNumber, const char* szDateTime, int* pReason, REVOCATION_INFO* pRevocationInfo) 
{
    LOG_MSG((0, "CCrl::isRevoked", "enter"));
    
	CASN1Sequence tbsCertList(elementAt(0));
	
	if(pRevocationInfo)
	{
		CASN1UTCTime thisUpdate = tbsCertList.elementAt(3);
		thisUpdate.getUTCTime(pRevocationInfo->szThisUpdate);
	}

	CASN1Sequence revokedCertificates(tbsCertList.elementAt(5));
	int count = revokedCertificates.size();
	if(count > 0)
	{
		for(int i = 0; i < count; i++)
		{
			CASN1Sequence revokedCertificate = revokedCertificates.elementAt(i);

			CASN1Integer sn(revokedCertificate.elementAt(0));

			if(serialNumber == sn)
			{
				CASN1Object revocationDate(revokedCertificate.elementAt(1));
				
				BYTE* btRevocationDate;
				
				if(revocationDate.getValue()->getLength() > 13)
				{
					btRevocationDate = (BYTE*)revocationDate.getValue()->getContent() + revocationDate.getValue()->getLength() - 13;
				}
				else 
				{
					btRevocationDate = (BYTE*)revocationDate.getValue()->getContent();
				}
				
				if(pRevocationInfo)
				{
					pRevocationInfo->nType = TYPE_CRL;
					memcpy(pRevocationInfo->szRevocationDate, btRevocationDate, 13);
					pRevocationInfo->szRevocationDate[13] = 0;
				}

				if(szDateTime != NULL)
				{
					
					if(memcmp(szDateTime, btRevocationDate, 13) < 0)
					{
						if(pRevocationInfo)
							pRevocationInfo->nRevocationStatus = REVOCATION_STATUS_GOOD;
						*pReason = REVOCATION_STATUS_GOOD;                        
                        return false;
					}
				}
				
				if(revokedCertificate.size() > 2)
				{
					CASN1Sequence extension(revokedCertificate.elementAt(2));
					
					CASN1Sequence crlReason(extension.elementAt(0));
					
					CASN1OctetString reasonCode(crlReason.elementAt(1));
					const UUCByteArray *pVal = reasonCode.getValue();
					
					BYTE reason = pVal->getContent()[2];//reasonCode.getTag() & 0x0F;
					if(reason == 6) //Certificate HOLD
						*pReason = REVOCATION_STATUS_SUSPENDED;
					else 
						*pReason = REVOCATION_STATUS_REVOKED;
				}
				else 
				{
					// reason non presente
					*pReason = REVOCATION_STATUS_REVOKED;
				}

				if(pRevocationInfo)
					pRevocationInfo->nRevocationStatus = *pReason;
	
                LOG_MSG((0, "CCrl::isRevoked", "YES: %d", *pReason));
                
				return true;
			}
		}
	}
	
    *pReason = REVOCATION_STATUS_GOOD;
    LOG_MSG((0, "CCrl::isRevoked", "NO: %d", *pReason));
    if(pRevocationInfo)
        pRevocationInfo->nRevocationStatus = *pReason;
    
    LOG_MSG((0, "CCrl::isRevoked", "exit with false"));
    
	return false;
}
