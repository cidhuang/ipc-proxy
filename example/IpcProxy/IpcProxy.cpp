/****************************** Module Header ******************************\
Module Name:  IpcProcy.cpp
Project:      IpcProcy
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

This code sample demonstrates receiving a custom data structure (MY_STRUCT) 
from the sending application (CppSendWM_COPYDATA) by handling WM_COPYDATA 
messages.

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


#include "FixedLengthPacketQueue.h"
#include "MixedLengthPacketQueue.h"

#include "Packet.h"
using namespace IPC_WM_COPYDATA;


#include <ctime>
#include <string>
#include <thread>
using namespace std;


//
//   FUNCTION: OnCopyData(HWND, HWND, PCOPYDATASTRUCT)
//
//   PURPOSE: Process the WM_COPYDATA message
//
//   PARAMETERS:
//   * hWnd - Handle to the current window.
//   * hwndFrom - Handle to the window passing the data. 
//   * pcds - Pointer to a COPYDATASTRUCT structure that contains the data 
//     that was passed. 
//
int logsCount = 0;
BOOL OnCopyData(HWND hWnd, HWND hwndFrom, PCOPYDATASTRUCT pcds)
{
    eCOPYDATA_TYPE type = getCopyDataType(pcds);
    if (type == eCOPYDATA_TYPE_MIXED_LENGTH) {
        return MixedLength::PacketQueue::GetInstance().add(hWnd, hwndFrom, pcds);
    }
    if (type == eCOPYDATA_TYPE_FIXED_LENGTH) {
        return FixedLength::PacketQueue<FixedLength::Position>::GetInstance().add(hWnd, hwndFrom, pcds);
    }
    return true;
}


//
//   FUNCTION: OnClose(HWND)
//
//   PURPOSE: Process the WM_CLOSE message
//
void OnClose(HWND hWnd)
{
    MixedLength::PacketQueue::GetInstance().stop();
    FixedLength::PacketQueue<FixedLength::Position>::GetInstance().stop();
    EndDialog(hWnd, 0);
}


//
//  FUNCTION: DialogProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main dialog.
//
time_t ltime0;
INT_PTR CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    time_t ltime1;
    time(&ltime1);

    if ((ltime1 - ltime0) > 1) {
        ltime0 = ltime1;
        SetDlgItemText(hWnd, IDC_NUMBER_STATIC, std::to_wstring(MixedLength::PacketQueue::GetInstance().queueSize()).c_str());
        SetDlgItemText(hWnd, IDC_MESSAGE_STATIC, std::to_wstring(FixedLength::PacketQueue<FixedLength::Position>::GetInstance().queueSize()).c_str());
    }

    switch (message)
    {
        // Handle the WM_COPYDATA message in OnCopyData
        HANDLE_MSG (hWnd, WM_COPYDATA, OnCopyData);

        // Handle the WM_CLOSE message in OnClose
        HANDLE_MSG (hWnd, WM_CLOSE, OnClose);

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
    time(&ltime0);

    thread t2(&MixedLength::PacketQueue::start, &MixedLength::PacketQueue::GetInstance());
    thread t(&FixedLength::PacketQueue<FixedLength::Position>::start, &FixedLength::PacketQueue<FixedLength::Position>::GetInstance());

    auto r = DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, DialogProc);

    t2.join();
    t.join();

    return r;
}