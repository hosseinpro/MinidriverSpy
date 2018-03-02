// minidriverspy.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "cardmod.h"

#include <string>

HMODULE hdl = NULL;

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	DWORD error = 0;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//MessageBox(NULL, L"LoadLibrary", L"Trace", 0);
		hdl = LoadLibrary(L"C:\\Windows\\System32\\msclmd-original.dll");
		error = GetLastError();
		if (hdl == NULL)
		{
			MessageBox(NULL, L"hdl==NULL 1", L"Trace", 0);
			MessageBox(NULL, std::to_wstring(error).c_str(), L"GetLastError", 0);
		}
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		//MessageBox(NULL, L"FreeLibrary", L"Trace", 0);
		FreeLibrary(hdl);
		break;
	}
	return TRUE;
}

std::wstring CharsToWString(unsigned char* pBytes, int cBytes)
{
	std::wstring wstr = L"";
	for (int i=0 ; i<cBytes ; i++)
		wstr.push_back(pBytes[i]);
	return wstr;
}


PFN_CARD_AUTHENTICATE_EX CardAuthenticateExOriginal = NULL;

DWORD WINAPI CardAuthenticateEx(__in PCARD_DATA pCardData,
	__in   PIN_ID PinId,
	__in   DWORD dwFlags,
	__in_bcount(cbPinData) PBYTE pbPinData,
	__in   DWORD cbPinData,
	__deref_opt_out_bcount(*pcbSessionPin) PBYTE *ppbSessionPin,
	__out_opt PDWORD pcbSessionPin,
	__out_opt PDWORD pcAttemptsRemaining)
{
	//MessageBox(NULL, L"CardAuthenticateEx", L"Trace", 0);

	std::wstring wstrPinData = CharsToWString(pbPinData, cbPinData);

	MessageBox(NULL, wstrPinData.c_str(), L"wstrPinData", 0);

	return CardAuthenticateExOriginal(pCardData, PinId, dwFlags, pbPinData, cbPinData, ppbSessionPin, pcbSessionPin, pcAttemptsRemaining);
}

PFN_CARD_SIGN_DATA CardSignDataOriginal = NULL;

DWORD WINAPI CardSignData(__in PCARD_DATA pCardData, 
	__inout PCARD_SIGNING_INFO pInfo)
{
	//MessageBox(NULL, L"CardSignData", L"Trace", 0);

	std::wstring wstrData = CharsToWString(pInfo->pbData, pInfo->cbData);

	if (MessageBox(NULL, wstrData.c_str(), L"This is original Data. Dou you want to use fake Data (\"Hello\")?", MB_YESNO) == IDYES)
	{
		BYTE* temp = (BYTE*)"Hello\0";
		pInfo->pbData = temp;
		pInfo->cbData = 6;
	}

	return CardSignDataOriginal(pCardData, pInfo);
}

DWORD WINAPI CardAcquireContext(__inout PCARD_DATA pCardData, __in DWORD dwFlags)
{
	if (hdl)
	{
		PFN_CARD_ACQUIRE_CONTEXT pCardAcquireContext = (PFN_CARD_ACQUIRE_CONTEXT)GetProcAddress(hdl, "CardAcquireContext");

		if (pCardAcquireContext == NULL)
		{
			MessageBox(NULL, L"pCardAcquireContext==NULL", L"Trace", 0);
			return SCARD_E_INVALID_PARAMETER;
		}

		DWORD result = pCardAcquireContext(pCardData, dwFlags);
		if (result == 0)
		{
			//MessageBox(NULL, L"pCardAcquireContext is ok", L"Trace", 0);
			
			CardAuthenticateExOriginal = pCardData->pfnCardAuthenticateEx;
			pCardData->pfnCardAuthenticateEx = CardAuthenticateEx;

			CardSignDataOriginal = pCardData->pfnCardSignData;
			pCardData->pfnCardSignData = CardSignData;
		}
		else
		{
			MessageBox(NULL, L"pCardAcquireContext is not ok", L"Trace", 0);
		}
		return result;
	}
	MessageBox(NULL, L"hdl==NULL 2", L"Trace", 0);
}