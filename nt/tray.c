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

#include <stdlib.h>
#include <windows.h>
#include "../sys.h"
#include "../iphdr.h"
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
    char bn[20];
    ATOM wa;
    HWND wnd;
    HICON hi=NULL,loaded_icon=NULL;
    HANDLE in, out;
    int i;

sleep(1); /* Workaround: somewhere (unknown) code is not thread-safe.
             Need to finds this code... */
    i = GetConsoleTitle(buf, sizeof(buf));
    if (i < 0) i = 0;
    buf[i] = 0;
    sprintf(bn, "%x", (unsigned int)GetCurrentThreadId());
    for (i = 0; i < 40; i++)
    {
        SetConsoleTitle(bn);
        if (((mainWindow = FindWindow(NULL, bn)) != NULL) || (isService()>0)) break;
        Sleep(100);
    }
    SetConsoleTitle(buf);
    if ((!IsWindow(mainWindow)) || (!mainWindow))
    {
        if (!AllocConsole())
        {
            Log(-1, "unable to find main window... (%s)", bn);
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

            strcpy(buf, service_name);
            SetConsoleTitle(service_name);
            for (i = 0; i < 40; i++)
            {
                if ((mainWindow = FindWindow(NULL, service_name)) != NULL) break;
                Sleep(100);
            }
            if (!mainWindow) return;
            /*_isService=0; Why this need? (commented out by stas)*/
        }
    }

    hi = (HICON)SendMessage(mainWindow, WM_GETICON, ICON_SMALL, 0);
    if (!hi)
    {
        hi = (HICON)SendMessage(mainWindow, WM_GETICON, ICON_BIG, 0);
    }
    if (!hi)
    {
        loaded_icon = hi = LoadImage( NULL, BINKD_ICON_FILE, IMAGE_ICON, 0, 0,
                        LR_LOADFROMFILE | LR_LOADTRANSPARENT );
        if(loaded_icon)
          Log(12,"Icon for systray is loaded from %s", BINKD_ICON_FILE);
    }
    if (!hi)
    { HMODULE hModule;
      if( (hModule = GetModuleHandle(NULL)) )
        loaded_icon = hi = LoadImage( hModule, MAKEINTRESOURCE(0), IMAGE_ICON,
                                      0, 0, LR_LOADTRANSPARENT);
    }
    if (!hi)
    {
        hi = LoadIcon(NULL, IDI_INFORMATION);
    }

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
    if(DestroyIcon(loaded_icon)==FALSE)
    {
      Log(1,"Error in DestroyIcon(): %s", w32err(GetLastError()));
    }
}
