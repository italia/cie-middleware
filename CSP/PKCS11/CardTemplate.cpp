#include "..\StdAfx.h"
#include ".\cardtemplate.h"
#include "..\util.h"
#include "..\moduleinfo.h"
#include ".\CIEP11Template.h"

static char *szCompiledFile=__FILE__;

const char szTemplatesQry[]="./TEMPLATES";
const char szTemplateNode[]="TEMPLATE";
const char szLibPathQry[]="./DLLMANAGER";
const char szNameQry[]="./NAME";

extern CModuleInfo moduleInfo;

namespace p11 {

static const char *szTemplateFuncListName = "TemplateGetFunctionList";
TemplateVector CCardTemplate::g_mCardTemplates;

CCardTemplate::CCardTemplate(void)
{
	hLibrary=NULL;
}

CCardTemplate::~CCardTemplate(void)
{
	if (hLibrary)
		FreeLibrary(hLibrary);
}

RESULT CCardTemplate::AddTemplate(CCardTemplate *pTemplate) {
	init_func
	g_mCardTemplates.push_back(pTemplate);
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CCardTemplate::DeleteTemplateList() {
	init_func
	for (DWORD i=0;i<g_mCardTemplates.size();i++)
		delete(g_mCardTemplates[i]);
	
	g_mCardTemplates.clear();
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CCardTemplate::InitTemplateList()
{
	init_func

		Allocator<CCardTemplate> pTemplate;
	pTemplate->szName = "Carta d'Identità Elettronica";
	pTemplate->szManifacturer = "Gemalto";
	pTemplate->FunctionList.templateInitLibrary = CIEtemplateInitLibrary;
	pTemplate->FunctionList.templateInitCard = CIEtemplateInitCard;
	pTemplate->FunctionList.templateFinalCard = CIEtemplateFinalCard;
	pTemplate->FunctionList.templateInitSession = CIEtemplateInitSession;
	pTemplate->FunctionList.templateFinalSession = CIEtemplateFinalSession;
	pTemplate->FunctionList.templateMatchCard = CIEtemplateMatchCard;
	pTemplate->FunctionList.templateGetSerial = CIEtemplateGetSerial;
	pTemplate->FunctionList.templateGetModel = CIEtemplateGetModel;
	pTemplate->FunctionList.templateLogin = CIEtemplateLogin;
	pTemplate->FunctionList.templateLogout = CIEtemplateLogout;
	pTemplate->FunctionList.templateReadObjectAttributes = CIEtemplateReadObjectAttributes;
	pTemplate->FunctionList.templateSign = CIEtemplateSign;
	pTemplate->FunctionList.templateSignRecover = CIEtemplateSignRecover;
	pTemplate->FunctionList.templateDecrypt = CIEtemplateDecrypt;
	pTemplate->FunctionList.templateGenerateRandom = CIEtemplateGenerateRandom;
	pTemplate->FunctionList.templateInitPIN = CIEtemplateInitPIN;
	pTemplate->FunctionList.templateSetPIN = CIEtemplateSetPIN;
	pTemplate->FunctionList.templateGetObjectSize = CIEtemplateGetObjectSize;
	pTemplate->FunctionList.templateChangeSecAuthPIN = CIEtemplateChangeSecAuthPIN;
	pTemplate->FunctionList.templateUnblockSecAuthPIN = CIEtemplateUnblockSecAuthPIN;
	pTemplate->FunctionList.templateSetKeyPIN = CIEtemplateSetKeyPIN;
	pTemplate->FunctionList.templateSetAttribute = CIEtemplateSetAttribute;
	pTemplate->FunctionList.templateCreateObject = CIEtemplateCreateObject;
	pTemplate->FunctionList.templateDestroyObject = CIEtemplateDestroyObject;
	pTemplate->FunctionList.templateGetTokenFlags = CIEtemplateGetTokenFlags;
	pTemplate->FunctionList.templateGenerateKey = CIEtemplateGenerateKey;
	pTemplate->FunctionList.templateGenerateKeyPair = CIEtemplateGenerateKeyPair;

	if (AddTemplate(pTemplate.detach())) {
		throw CStringException(ERR_CANT_ADD_SLOT);
	}

	_return(OK)
		exit_func
		_return(FAIL)
}

RESULT CCardTemplate::GetTemplate(CSlot &pSlot,CCardTemplate *&pTemplate)
{
	init_func
	for (DWORD i=0;i<g_mCardTemplates.size();i++) {
		bool bMatched;
		if (g_mCardTemplates[i]->FunctionList.templateMatchCard(bMatched,pSlot)) {
			continue;
		}
		if (bMatched) {
			pTemplate=g_mCardTemplates[i];
			_return(OK)
		}
	}
	pTemplate=NULL;
	_return(OK)
	exit_func
	_return(FAIL)
}

//RESULT CCardTemplate::InitLibrary(const char *szPath,void *templateData)
//{
//	init_func
//	hLibrary=LoadLibrary(szPath);
//	if (hLibrary==NULL) {
//		throw CStringException(CWinException(), ERR_CANT_LOAD_LIBRARY);
//	}
//
//	templateFuncListFunc funcList;
//	funcList=(templateFuncListFunc)GetProcAddress(hLibrary,szTemplateFuncListName);
//	if (!funcList) {
//		throw CStringException(ERR_GET_LIBRARY_FUNCTION_LIST);
//	}
//	
//	if (funcList(&FunctionList)) {
//		throw CStringException(ERR_CALL_LIBRARY_FUNCTION_LIST);
//	}
//	
//	if (FunctionList.templateInitLibrary(*this,templateData)) {
//		throw CStringException(ERR_INIT_LIBRARY);
//	}
//
//	_return(OK)
//	exit_func
//	_return(FAIL)
//}

};