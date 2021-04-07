CK_RV CK_ENTRY __stdcall firmaConCIE(const char* inFilePath, const char* type, const char* pin, const char* pan, int page, float x, float y, float w, float h, const char* imagePathFile, const char* outFilePath, PROGRESS_CALLBACK progressCallBack, SIGN_COMPLETED_CALLBACK completedCallBack)
{

	printf("page: %d, x: %f, y: %f, w: %f, h: %f", page, x, y, w, h);

	char* readers = NULL;
	char* ATR = NULL;
	try
	{
		std::map<uint8_t, ByteDynArray> hashSet;

		DWORD len = 0;
		ByteDynArray CertCIE;
		ByteDynArray SOD;

		SCARDCONTEXT hSC;

		progressCallBack(1, "");

		long nRet = SCardEstablishContext(SCARD_SCOPE_USER, nullptr, nullptr, &hSC);
		if (nRet != SCARD_S_SUCCESS)
			return CKR_DEVICE_ERROR;

		Log.write("Establish Context ok\n");

		if (SCardListReaders(hSC, nullptr, NULL, &len) != SCARD_S_SUCCESS) {
			Log.write("List readers ko\n");
			return CKR_TOKEN_NOT_PRESENT;
		}

		if (len == 1)
			return CKR_TOKEN_NOT_PRESENT;

		readers = (char*)malloc(len);

		if (SCardListReaders(hSC, nullptr, (char*)readers, &len) != SCARD_S_SUCCESS) {
			free(readers);
			return CKR_TOKEN_NOT_PRESENT;
		}

		char *curreader = readers;
		bool foundCIE = false;
		for (; curreader[0] != 0; curreader += strnlen(curreader, len) + 1)
		{

			safeConnection conn(hSC, curreader, SCARD_SHARE_SHARED);
			if (!conn.hCard)
				continue;

			LONG res = 0;

			res = SCardBeginTransaction(conn.hCard);

			while (res != SCARD_S_SUCCESS)
			{
				DWORD protocol = 0;
				SCardReconnect(conn.hCard, SCARD_SHARE_SHARED, SCARD_PROTOCOL_Tx, SCARD_UNPOWER_CARD, &protocol);
				Log.write("errore\n");
				res = SCardBeginTransaction(conn.hCard);
			}

			DWORD atrLen = 40;
			res = SCardGetAttrib(conn.hCard, SCARD_ATTR_ATR_STRING, (uint8_t*)ATR, &atrLen);
			if (res != SCARD_S_SUCCESS) {
				free(readers);
				Log.write("GetAttrib ko 1, %d\n", res);
				return CKR_DEVICE_ERROR;
			}

			ATR = (char*)malloc(atrLen);

			if (SCardGetAttrib(conn.hCard, SCARD_ATTR_ATR_STRING, (uint8_t*)ATR, &atrLen) != SCARD_S_SUCCESS) {
				free(readers);
				free(ATR);
				return CKR_DEVICE_ERROR;
			}

			ByteArray atrBa((BYTE*)ATR, atrLen);

			progressCallBack(20, "");

			IAS* ias = new IAS((CToken::TokenTransmitCallback)TokenTransmitCallback, atrBa);
			ias->SetCardContext(&conn);
			foundCIE = false;

			ias->token.Reset();
			ias->SelectAID_IAS();
			ias->ReadPAN();

			ByteDynArray IntAuth;
			ias->SelectAID_CIE();
			ias->ReadDappPubKey(IntAuth);
			ias->SelectAID_CIE();
			ias->InitEncKey();

			ByteDynArray IdServizi;
			ias->ReadIdServizi(IdServizi);
			ByteArray baPan = ByteArray((uint8_t*)pan, strlen(pan));

			if (memcmp(baPan.data(), IdServizi.data(), IdServizi.size()) != 0)
			{
				return CARD_PAN_MISMATCH;
			}

			foundCIE = true;
			ByteDynArray FullPIN;
			ByteArray LastPIN = ByteArray((uint8_t*)pin, strlen(pin));
			ias->GetFirstPIN(FullPIN);
			FullPIN.append(LastPIN);
			ias->token.Reset();

			progressCallBack(40, "");

			char fullPinCStr[9];
			memcpy(fullPinCStr, FullPIN.data(), 8);
			fullPinCStr[8] = 0;

			CIESign* cieSign = new CIESign(ias);

			uint16_t ret = cieSign->sign(inFilePath, type, fullPinCStr, page, x, y, w, h, imagePathFile, outFilePath);

			if ((ret & (0x63C0)) == 0x63C0)
			{
				return CKR_PIN_INCORRECT;
			}
			else if (ret == 0x6983)
			{
				return CKR_PIN_LOCKED;
			}
			else if (ret != 0)
			{
				return CKR_GENERAL_ERROR;
			}

			progressCallBack(100, "");

			Log.write("CieSign ret: %d", ret);

			free(ias);
			free(cieSign);

			completedCallBack(ret);
		}

		if (!foundCIE) {
			free(ATR);
			free(readers);
			return CKR_TOKEN_NOT_RECOGNIZED;

		}
	}
	catch (std::exception &ex) {
		OutputDebugString(ex.what());
		if (ATR)
			free(ATR);
		Log.write("Eccezione: %s", ex.what());
		if (readers)
			free(readers);

		Log.write("General error\n");
		return CKR_GENERAL_ERROR;
	}

	if (ATR)
		free(ATR);

	free(readers);

	return SCARD_S_SUCCESS;
}