// EsempioPCSC.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winscard.h>
#include <vector>
#include <iostream>
#include <string>


int _tmain(int argc, _TCHAR* argv[])
{

	// stibilisco la connessione al sottosistema di gestione delle smart card
	SCARDCONTEXT Context;
	SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &Context);

	// ottiengo la lista dei lettori installati
	char *ReaderList;
	DWORD ReaderListLen = SCARD_AUTOALLOCATE;
	SCardListReaders(Context, NULL, (char*)&ReaderList, &ReaderListLen);
	
	// inserisco i lettori in un vettore
	char* Reader = ReaderList;
	std::vector<char*> Readers;
	while (Reader[0] != NULL) {
		Readers.push_back(Reader);
		Reader += strlen(Reader) + 1;
	}

	// richiedo all'utente quale lettore utilizzare
	for (std::size_t i = 0; i < Readers.size(); i++) {
		std::cout << (i + 1) << ") " << Readers[i] << "\n";
	}
	std::cout << "Selezionare il lettore su cui è appoggiata la CIE\n";

	std::size_t ReaderNum;
	std::cin >> ReaderNum;
	if (ReaderNum < 1 || ReaderNum>Readers.size()) {
		std::cout << "Lettore inesistente\n";
		system("pause");
		return 0;
	}

	// apre la connessione al lettore selezionato, specificando l'accesso esclusivo e il protocollo T=1
	SCARDHANDLE Card;
	DWORD Protocol;
	LONG result = SCardConnect(Context, Readers[ReaderNum - 1], SCARD_SHARE_EXCLUSIVE, SCARD_PROTOCOL_T1, &Card, &Protocol);

	if (result != SCARD_S_SUCCESS) {
		std::cout << "Connessione al lettore fallita\n";
		system("pause");
		return 0;
	}

	// prepara la prima APDU: Seleziona il DF dell'applicazione IAS
	BYTE selectIAS[] = { 0x00, // CLA
		0xa4, // INS = SELECT FILE
		0x04, // P1 = Select By AID
		0x0c, // P2 = Return No Data
		0x0d, // LC = lenght of AID
		0xA0, 0x00, 0x00, 0x00, 0x30, 0x80, 0x00, 0x00, 0x00, 0x09, 0x81, 0x60, 0x01 // AID
		};

	// invia la prima APDU
	BYTE response[300];
	DWORD responseLen = 300;
	SCardTransmit(Card, SCARD_PCI_T1, selectIAS, sizeof(selectIAS), NULL, response, &responseLen);

	// verifica che la Status Word sia 9000 (OK)
	if (response[responseLen - 2] != 0x90 || response[responseLen - 1] != 0x00) {
		std::cout << "Errore nella selezione del DF_IAS\n";
	}

	else {

		// prepara la seconda APDU: Seleziona il DF degli oggetti CIE

		BYTE selectCIE[] = { 0x00, // CLA
			0xa4, // INS = SELECT FILE
			0x04, // P1 = Select By AID
			0x0c, // P2 = Return No Data
			0x06, // LC = lenght of AID
			0xA0, 0x00, 0x00, 0x00, 0x00, 0x39 // AID
		};

		// invia la seconda APDU
		responseLen = 300;
		SCardTransmit(Card, SCARD_PCI_T1, selectCIE, sizeof(selectCIE), NULL, response, &responseLen);
		if (response[responseLen - 2] != 0x90 || response[responseLen - 1] != 0x00) {
			std::cout << "Errore nella selezione del DF_CIE\n";
		}
		else {

			// prepara la terza APDU: Lettura del file dell'ID_Servizi selezionato contestualmente tramite Short Identifier (SFI = 1)

			BYTE readNIS[] = { 0x00, // CLA
				0xb0, // INS = READ BINARY
				0x81, // P1 = Read by SFI & SFI = 1
				0x00, // P2 = Offset = 0
				0x0c // LE = lenght of NIS
			};

			// invia la terza APDU
			responseLen = 300;
			SCardTransmit(Card, SCARD_PCI_T1, readNIS, sizeof(readNIS), NULL, response, &responseLen);
			if (response[responseLen - 2] != 0x90 || response[responseLen - 1] != 0x00) {
				std::cout << "Errore nella lettura dell'Id_Servizi\n";
			}
			else {

				std::cout << "NIS:" << std::string((char*)response, responseLen - 2) << "\n";
			}
		}
	}
	SCardFreeMemory(Context, ReaderList);
	SCardDisconnect(Card, SCARD_RESET_CARD);
	system("pause");
	return 0;
}

