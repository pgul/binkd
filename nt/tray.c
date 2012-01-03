/*
 *  tray.c -- Functions used to implement "minimize to tray" feature
 *
 *  This file is a part of binkd project
 *
 *  Copyright (C) 2003 Stas Degteff 2:5080/102@fidonet, g@grumbler.org
 *  Portions copyright (C) Dima Afanasiev da@4u.net (Fido 2:5020/463)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

#include "../iphdr.h"
#include <stdlib.h>
#include <windows.h>
#include "../sys.h"
#include "../tools.h"
#include "w32tools.h"
#include "service.h"
#include "tray.h"

static HWND mainWindow;
static NOTIFYICONDATA nd;
static int wstate = 0;

/* Window Handler function
 */
static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,  LPARAM lParam)
{
    if ((lParam == WM_LBUTTONDBLCLK) || (lParam == WM_RBUTTONUP))
    {
    	ShowWindow(mainWindow, SW_RESTORE);
        SetForegroundWindow(mainWindow);
        Shell_NotifyIcon(NIM_DELETE, &nd);
        wstate = 2;
    }
    return 1;
}

/* handler function on close wndthread()
 */
static void closeProcess(void)
{
    Shell_NotifyIcon(NIM_DELETE, &nd);
    ShowWindow(mainWindow, SW_RESTORE);
}

/* Keyboard event handler function
 */
static void processKeyCode(WORD kc, DWORD cs, HANDLE out)
{
    if ((cs & SHIFT_PRESSED) &&
        ((kc == VK_UP) || (kc == VK_DOWN) || (kc == VK_LEFT) || (kc == VK_RIGHT)))
    {
        CONSOLE_SCREEN_BUFFER_INFO cb;
        if(!GetConsoleScreenBufferInfo(out, &cb)) return;
        switch(kc)
        {
            case VK_UP: cb.dwSize.Y-=10; break;
            case VK_DOWN: cb.dwSize.Y+=10; break;
            case VK_LEFT: cb.dwSize.X-=10; break;
            case VK_RIGHT: cb.dwSize.X+=10; break;
        }
        SetConsoleScreenBufferSize(out, cb.dwSize);
    }
}

/* Function used for "minimize to tray" feature
 * Can be used only as parameter of the _beginthread()!
 */
void wndthread(void *par)
{
    WINDOWPLACEMENT wp;
    WNDCLASS rc;
    char *cn = "testclass";
    char buf[256];
    ATOM wa;
    HWND wnd;
    HICON hi=NULL;
    HANDLE in, out;
    int i;

    if( IsNT() && isService() )
    {
        if (!AllocConsole())
        {
            Log(-1, "unable to allocate console");
            return;
        }
        else
        {
            HANDLE ha = GetStdHandle(STD_OUTPUT_HANDLE);
            int hCrt = _open_osfhandle((long) ha, 0x4000);
            FILE *hf = _fdopen( hCrt, "w" );
            *stdout = *hf;

            ha = GetStdHandle(STD_ERROR_HANDLE);
            hCrt = _open_osfhandle((long) ha, 0x4000);
            hf = _fdopen( hCrt, "w" );
            *stderr = *hf;
            setvbuf( stdout, NULL, _IONBF, 0 );
            setvbuf( stderr, NULL, _IONBF, 0 );

        }
    }
    mainWindow = GetMainWindow();
    hi = LoadBinkdIcon();

    if (isService()) SetConsoleTitle(service_name);

    memset(&rc, 0, sizeof(rc));
    rc.lpszClassName = cn;
    rc.lpfnWndProc = WindowProc;
    wa = RegisterClass(&rc);
    if (!wa)
    {
        Log(-1, "unable to register class...");
        return;
    }
    wnd = CreateWindow(cn, "", 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
    if (!wnd)
    {
        Log(-1, "Unable to create message window...");
        return;
    }
    memset(&nd, 0, sizeof(nd));
    nd.cbSize = sizeof(nd);
    nd.hWnd = wnd;
    nd.uID = 1111;
    nd.uFlags = NIF_TIP|NIF_MESSAGE|NIF_ICON;
    nd.uCallbackMessage = 1111;
    nd.hIcon = hi;
    atexit(closeProcess);
    strncpy(nd.szTip, buf, 63);
    i = 1000;
    in = GetStdHandle(STD_INPUT_HANDLE);
    out = GetStdHandle(STD_OUTPUT_HANDLE);
    for (;;)
    {
        MSG msg;
        INPUT_RECORD cb;
        DWORD dw;
        if (in)
        {
            while(PeekConsoleInput(in, &cb, 1, &dw))
            {
                if (!dw)
                {
                    break;
                }
                ReadConsoleInput(in, &cb, 1, &dw);
                if ((cb.EventType == KEY_EVENT) && (cb.Event.KeyEvent.bKeyDown))
                {
                    processKeyCode(cb.Event.KeyEvent.wVirtualKeyCode,
                        cb.Event.KeyEvent.dwControlKeyState, out);
                }
            }
	    }

        while(PeekMessage( &msg, wnd, 0, 0, PM_REMOVE))
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        if ((i++) < 10)
        {
            Sleep(50);
            continue;
        }
        i = 0;
        if (wstate != 1)
        {
            memset(&wp, 0, sizeof(wp));
            wp.length = sizeof(wp);
            if ((GetWindowPlacement(mainWindow, &wp)) &&
                (wp.showCmd == SW_SHOWMINIMIZED))
            {
    	        wstate = 1;
                Shell_NotifyIcon(NIM_ADD, &nd);
                ShowWindow(mainWindow, SW_HIDE);
            }
    	}
    }
}
