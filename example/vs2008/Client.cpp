/****************************** Module Header ******************************\
Module Name:  Client.cpp
Project:      Client
Copyright (c) Microsoft Corporation.

Inter-process Communication (IPC) based on the Windows message WM_COPYDATA is
a mechanism for exchanging data among Windows applications in the local
machine. The receiving application must be a Windows application. The data
being passed must not contain pointers or other references to objects not
accessible to the application receiving the data. While WM_COPYDATA is being
sent, the referenced data must not be changed by another thread of the
sending process. The receiving application should consider the data read-only.
If the receiving application must access the data after SendMessage returns,
it needs to copy the data into a local buffer.

This code sample demonstrates sending a custom data structure (MY_STRUCT) to
the receiving Windows application (CppReceiveWM_COPYDATA) by using
SendMessage(WM_COPYDATA). If the data structure fails to be passed, the
application displays the error code for diagnostics. A typical error code is
0x5 (Access is denied) caused by User Interface Privilege Isolation (UIPI).
UIPI prevents processes from sending selected window messages and other USER
APIs to processes running with higher integrity. When the receiving
application (CppReceiveWM_COPYDATA) runs at an integrity level higher than
this sending application, you will see the "SendMessage(WM_COPYDATA) failed
w/err 0x00000005" error message.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma region Includes and Manifest Dependencies
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include "Resource.h"

// Enable Visual Style
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#pragma endregion


#include "FixedLength.h"
#include "MixedLength.h"

#include "Packet.h"
using namespace IPC_WM_COPYDATA;

#include <ctime>


//
//   FUNCTION: ReportError(LPWSTR, DWORD)
//
//   PURPOSE: Display an error dialog for the failure of a certain function.
//
//   PARAMETERS:
//   * pszFunction - the name of the function that failed.
//   * dwError - the Win32 error code. Its default value is the calling 
//   thread's last-error code value.
//
//   NOTE: The failing function must be immediately followed by the call of 
//   ReportError if you do not explicitly specify the dwError parameter of 
//   ReportError. This is to ensure that the calling thread's last-error code 
//   value is not overwritten by any calls of API between the failing 
//   function and ReportError.
//
void ReportError(LPCWSTR pszFunction, DWORD dwError = GetLastError())
{
	wchar_t szMessage[200];
	if (-1 != swprintf_s(szMessage, ARRAYSIZE(szMessage),
		L"%s failed w/err 0x%08lx", pszFunction, dwError))
	{
		MessageBox(NULL, szMessage, L"Error", MB_ICONERROR);
	}
}


BOOL OnCopyData(HWND hWnd, HWND hwndFrom, PCOPYDATASTRUCT pcds)
{
	/*
	string str = toJsonString(pcds);
	Json::Value json;
	Json::Reader reader;
	if (!reader.parse(str, json)) {
		return false;
	}

	if (!json.isArray()) {
		Json::Value result = toCmd(hWnd, hwndFrom, json)->exec();
		return true;
	}
	for (unsigned int i = 0; i < json.size(); i++) {
		Json::Value result = toCmd(hWnd, hwndFrom, json[i])->exec();
	}
	//*/
	return true;
}


//
//   FUNCTION: OnCommand(HWND, int, HWND, UINT)
//
//   PURPOSE: Process the WM_COMMAND message
//
void OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify)
{
	if (id == IDC_SENDMSG_BUTTON)
	{
		// Find the target window handle.
		HWND hTargetWnd = FindWindow(NULL, L"IpcProxy");
		if (hTargetWnd == NULL)
		{
			MessageBox(hWnd, L"Unable to find the \"IpcProxy\" window",
				L"Error", MB_ICONERROR);
			return;
		}

		//*
		time_t ltime0;
		time(&ltime0);

		time_t ltime02;
		int i0 = 0;

		MixedLength::PacketSender sender(hTargetWnd, hWnd);
		MixedLength::PacketQuery* query = new MixedLength::PacketQuery();
		query->from = 12347654;
		query->query = "§A¬O½Ö¡H";
		sender.add(*query);
		sender.send();

		FixedLength::PacketSender<FixedLength::Position> sender2(hTargetWnd, hWnd);

		int i1 = 0;
		FixedLength::Position position;
		do
		{
			i1++;
			position.x = i1;
			position.y = i1;
			position.time = i1;
			sender2.add(position);
			//if ((i1 % 2) == 0) {
			sender2.send();
			return;
			//}
		} while (i1 < 1000); //(logs.add(log));

		do
		{
			//cmd->send();

			//int i1 = 0;
			FixedLength::Position position;

			do
			{
				i1++;
				position.x = i1;
				position.y = i1;
				position.time = i1;
				sender2.add(position);
			} while (sender2.add(position));

			sender2.send();
			i1--;
			time(&ltime02);
			i0++;
		} while ((ltime02 - ltime0) < 1);

		return;

	}
}


//
//   FUNCTION: OnClose(HWND)
//
//   PURPOSE: Process the WM_CLOSE message
//
void OnClose(HWND hWnd)
{
	EndDialog(hWnd, 0);
}


//
//  FUNCTION: DialogProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main dialog.
//
INT_PTR CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		// Handle the WM_COPYDATA message in OnCopyData
		HANDLE_MSG(hWnd, WM_COPYDATA, OnCopyData);

		// Handle the WM_COMMAND message in OnCommand
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);

		// Handle the WM_CLOSE message in OnClose
		HANDLE_MSG(hWnd, WM_CLOSE, OnClose);

	default:
		return FALSE;
	}
	return 0;
}


//
//  FUNCTION: wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
//
//  PURPOSE:  The entry point of the application.
//
int APIENTRY wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPWSTR    lpCmdLine,
	int       nCmdShow)
{
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, DialogProc);
}