/*
 *  win9x.c -- Windows 95/98/ME support for binkd
 *
 *  win9x.c is a part of binkd project
 *
 *  Copyright (C) 2002 Alexander Reznikov, homebrewer@yandex.ru (Fido 2:4600/220)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

#ifdef BINKDW9X
//
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <process.h>
#include <io.h>
#include "../Config.h"
#include "../tools.h"
#include "win9x.h"

#if !defined(ENDSESSION_LOGOFF)
#define ENDSESSION_LOGOFF    0x80000000
#endif

PHANDLER_ROUTINE phandler;
HWND mainHWND = NULL;
WNDCLASS wc;

int s_console = 0;
FILE stdout_old, stderr_old;

int w9x_service = 0;
int w9x_service_reg = 0;
int s_quiet = 0;
char *srvname = NULL;
DWORD SigType = -1;

char *Win9xWindowClassName = "binkdWin9xHandler";
char *Win9xRegServ = "Software\\Microsoft\\Windows\\CurrentVersion\\RunServices";
char *Win9xServPrefix = "binkd9x-service";
char *Win9xAllSuffix = "--all";

#define WM_BINKD9XCOMMAND  WM_USER+50

int c_argc = 0;
char **c_argv = NULL;
int *c_argv_bool = NULL;
char *s_control = NULL;

extern int checkcfg_flag;
extern int quiet_flag;

int binkd_main(int argc, char **argv, char **envp);
int win9x_service_cmdline(int argc, char **argv, char **envp);
void win9x_service_un_install(int type, int argc, char **argv, char **envp);
void win9x_service_control(int type);

// win9x service support :) :(
typedef DWORD (WINAPI* RSPType)(DWORD, DWORD);
RSPType RegisterServiceProcess;
//DWORD WINAPI RegisterServiceProcess(DWORD dwProcessId,    // process identifier
//                                    DWORD dwServiceType); // type of service
#if !defined(RSP_SIMPLE_SERVICE)
#define RSP_SIMPLE_SERVICE 0x00000001
//  Registers the process as a simple service process.
#endif
#if !defined(RSP_UNREGISTER_SERVICE)
#define RSP_UNREGISTER_SERVICE 0x00000000
//  Unregisters the process as a service process.
#endif


int win9xExec(char *cmdline)
{
  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  memset(&si, 0, sizeof(si));
  si.cb=sizeof(si);

  return CreateProcess(NULL, cmdline, NULL, NULL, 0, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &si, &pi);
}

void win9xAtExit(void)
{
  if (srvname)
  {
        free(srvname);
        srvname = NULL;
  }
  if (c_argv)
        free(c_argv);
  c_argv = NULL;
  c_argc = 0;
  if (s_control)
        free(s_control);
  s_control = NULL;

  FreeTempConsole();
  if (checkcfg_flag==2)
        win9xExec(GetCommandLine());
}

void win9x_service_args(int argc, char **argv, char **envp)
{
        int i, j;

        for (i=0; i<argc; i++)
                if (!c_argv_bool[i])
                        c_argc++;

        c_argv = (char **)malloc(c_argc*sizeof(char *));

        for (i=0, j=0; i<argc; i++)
                if (!c_argv_bool[i])
                        c_argv[j++] = __argv[i];
}  

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
        int r;
        char *sp, *tmp = NULL;
        HINSTANCE hl;

        atexit(win9xAtExit);

        c_argv_bool = (int *)malloc(__argc*sizeof(int));
        memset(c_argv_bool, 0, __argc*sizeof(int));

        r = win9x_service_cmdline(__argc, __argv, environ);

        if (r)  win9x_service_args(__argc, __argv, environ);
        free(c_argv_bool);

        if ((r == 'i')||(r == 'u'))
        {
                win9x_service_un_install(r, c_argc, c_argv, environ);
                return 0;
        }

        if (r == 't')
        {
                win9x_service_control(r);
                return 0;
        }

        if (r == 0)
                return binkd_main(__argc, __argv, environ);

// Running as Win9x service (r == 1)
        for (sp = __argv[0]+strlen(__argv[0])-1;sp>__argv[0];sp--)
                if ((sp[0] == '\\')||(sp[0] == '/'))
                {
                        sp--;
                        if (sp>__argv[0])
                        {
                                tmp = (char *)malloc(sp-__argv[0]+2);
                                memcpy(tmp, __argv[0], sp-__argv[0]+1);
                                tmp[sp-__argv[0]+1] = 0;
                        }
                        break;
                }

        if (tmp)
        {
                SetCurrentDirectory(tmp);
                free(tmp);
        }

        hl = LoadLibrary("KERNEL32.DLL");
        if (hl != NULL)
        {
                RegisterServiceProcess = (RSPType)GetProcAddress(hl, "RegisterServiceProcess");
                if (RegisterServiceProcess!=NULL)
                        if (RegisterServiceProcess(0, RSP_SIMPLE_SERVICE))
                        	w9x_service_reg = 1;
                FreeLibrary(hl);
        }

        quiet_flag = 1;
        return binkd_main(c_argc, c_argv, environ);
}

LRESULT CALLBACK MainWin9xWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
        switch (msg)
        {
        case WM_DESTROY:
                PostQuitMessage(0);
                break;

        case WM_CLOSE:
        	SigType = CTRL_CLOSE_EVENT;
                DestroyWindow(hWnd);
                break;

        case WM_ENDSESSION:
                if (wParam)
                {
                        if (lParam & ENDSESSION_LOGOFF)
                        {
                                if (!(w9x_service && w9x_service_reg))
                                {
                                        SigType = CTRL_LOGOFF_EVENT;
                                        DestroyWindow(hWnd);
                                }
                        }
                        else
                        {
                                SigType = CTRL_SHUTDOWN_EVENT;
                                DestroyWindow(hWnd);
                        }
                }
                break;

        case WM_BINKD9XCOMMAND:
                SigType = wParam;
                DestroyWindow(hWnd);
                break;

        default:
                return DefWindowProc(hWnd, msg, wParam, lParam);
        }

        return 0;
}

static void Win9xWindowThread(void *p)
{
        HINSTANCE hInstance = GetModuleHandle(NULL);
        MSG msg;
        phandler = (PHANDLER_ROUTINE)p;

        memset(&wc, 0, sizeof(WNDCLASS));
        wc.lpfnWndProc = (WNDPROC)MainWin9xWndProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = (w9x_service==0)?Win9xWindowClassName:srvname;

        RegisterClass(&wc);

        mainHWND= CreateWindow(wc.lpszClassName, wc.lpszClassName, 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);

        while(GetMessage(&msg, NULL, 0, 0))
        {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
        }
        mainHWND = NULL;
        win9xAtExit();
       	if (SigType!=-1)
       		phandler(SigType);
}

void CreateWin9xThread(PHANDLER_ROUTINE phandler)
{
        _beginthread(Win9xWindowThread, 0, phandler);
}

/*
void DestroyWin9xThread()
{
  if (mainHWND)
        SendMessage(mainHWND, WM_DESTROY, 0, 0);
  win9xAtExit();
}
*/

// Win9x service

int win9x_checkservice(char *srvname)
{
  HKEY hk=0;
  int r=0;

  if (RegOpenKey(HKEY_LOCAL_MACHINE, Win9xRegServ, &hk)!=ERROR_SUCCESS)
        return 0;

  if (RegQueryValueEx(hk, srvname, NULL, NULL, NULL, NULL)==ERROR_SUCCESS)
        r=1;

  RegCloseKey(hk);
  return r;
}

int win9x_service_cmdline(int argc, char **argv, char **envp)
{
  int i, j, type = 0, len = 0;
  int prefixlen;

  prefixlen = strlen(Win9xServPrefix);

  for(i=1; i<argc; i++)
  {
        if (strlen(argv[i])<2)
                continue;

        if (argv[i][0]=='-')
        {
                type = argv[i][1];
                if ((argv[i][1]=='(')&&(strlen(argv[i])>=4)&&(argv[i][strlen(argv[i])-1]=='t')&&(argv[i][strlen(argv[i])-2]==')'))
                {
                        char *sp2 = strchr(argv[i], ')');

                        c_argv_bool[i] = 1;
                        
                        len = sp2-(argv[i])-2;

                        srvname = (char *)malloc((len?len:prefixlen)+1);
                        if (!len)
                        {
                                len = prefixlen;
                                memcpy(srvname, Win9xServPrefix, len);
                        }
                        else
                                memcpy(srvname, argv[i]+2, len);

                        srvname[len]=0;
                        w9x_service = 1;

                        return 1;
                }

                if ((type=='i')||(type=='u')||(type=='t'))
                {
                        char *sp1 = strchr(argv[i], '(');
                        char *sp2 = strchr(argv[i], ')');

                        c_argv_bool[i] = 1;

                        if ((argv[i][strlen(argv[i])-1]=='q')||(argv[i][strlen(argv[i])-1]=='Q'))
                                s_quiet = 1;

                        if (type == 't')
                        {
                                for (j=2; ((unsigned int)j<strlen(argv[i]))&&(argv[i][j]!='(')&&(argv[i][j]!='q')&&(argv[i][j]!='Q'); j++);
                                if (j>2)
                                {
                                        s_control = (char *)malloc(j-1);
                                        memcpy(s_control, argv[i]+2, j-2);
                                        s_control[j-2] = 0;
                                }
                        }

                        if ((sp1) && (sp2 > (sp1 + 1)))
                                len = sp2-sp1-1;
                        if (len)  len++;

                        srvname = (char *)malloc(prefixlen+len+1);

                        memcpy(srvname, Win9xServPrefix, prefixlen);
                        if (len)
                        {
                                srvname[prefixlen] = '-';
                                memcpy(srvname+prefixlen+1, sp1+1, len-1);
                        }
                        srvname[prefixlen+len]=0;
                        break;
                }
        }
  }

  if (!srvname)
        return 0;

  return type;
}

int win9x_service_start(char *name)
{
  HKEY hk;
  int k;
  char *cmdline;
  DWORD size, reg_type;

  if (RegOpenKey(HKEY_LOCAL_MACHINE, Win9xRegServ, &hk)!=ERROR_SUCCESS)
        return 0;

  size = 0;
  k = RegQueryValueEx(hk, name, NULL, &reg_type, NULL, &size) == ERROR_SUCCESS;

  k = k&&(reg_type == REG_SZ);

  if(k)
  {
        cmdline = (char *)malloc(size);
        k = RegQueryValueEx(hk, name, NULL, &reg_type, cmdline, &size) == ERROR_SUCCESS;

        if(k)  k = win9xExec(cmdline);
        free(cmdline);
  }

  RegCloseKey(hk);

  return k!=0;
}

int win9x_check_name_all() // Maniac :)
{
  int prefixlen = strlen(Win9xServPrefix);
  int suffixlen = strlen(Win9xAllSuffix);

  if (!srvname)  return 0;
  if (strlen(srvname)!=(unsigned int)(prefixlen+1+suffixlen))  return 0;

  if (strncmp(srvname, Win9xServPrefix, prefixlen)!=0) return 0;
  if (srvname[prefixlen]!='-') return 0;
  if (stricmp(srvname+prefixlen+1, Win9xAllSuffix)!=0) return 0;
  return 1;
}

HWND win9x_service_find(char *name)
{
  return FindWindow(name, NULL);
}

void win9x_service_control_exec(char *tmp, int mode)
{
  HWND hwnd;

  hwnd = win9x_service_find(tmp);
  switch (mode)
  {
          case 0:
            if (!s_quiet)  Log(-1, "%s: %s\n", tmp, hwnd?"started":"stopped");
            break;
          case 1:
            if (!hwnd)
            { 
                if (win9x_service_start(tmp))
                {
                        if (!s_quiet)   Log(-1, "%s: started\n", tmp);
                }
                else
                {
                        if (!s_quiet)  Log(-1, "%s: starting failed!\n", tmp);
                }
            }
            else
            {
                if (!s_quiet)  Log(-1, "%s: already started\n", tmp);
            }
            break;
          case 2:
            if (hwnd) SendMessage(hwnd, WM_BINKD9XCOMMAND, 254, 0);
            if (!s_quiet)  Log(-1, "%s: %s\n", tmp, hwnd?"stopped":"already stopped");
            break;
          case 3:
            if (hwnd)
            {
                SendMessage(hwnd, WM_BINKD9XCOMMAND, 255, 0);
                if (win9x_service_start(tmp))
                {
                        if (!s_quiet)  Log(-1, "%s: restarted\n", tmp);
                }
                else
                {
                        if (!s_quiet)  Log(-1, "%s: restarting failed!\n", tmp);
                }
            } 
            else
                if (!s_quiet)  Log(-1, "%s: already stopped\n", tmp);
            break;
  }
}

void win9x_service_control(int type)
{
  HKEY hk;
  int i, len, prefixlen, mode;
  DWORD size, reg_type;
  char *tmp;

  prefixlen = strlen(Win9xServPrefix);

//  if (!s_control) // View status of service(s)
//  {
  if (!s_quiet)  AllocTempConsole();

  if (!s_control)
  {
        mode = 0;
        if (!s_quiet)  Log(-1, "Status of binkd9x service(s):\n");
  }
  else
        if (stricmp(s_control, "start")==0)
        {
                mode = 1;
                if (!s_quiet)  Log(-1, "Starting service(s):\n");
        }
        else
                if (stricmp(s_control, "stop")==0)
                {
                        mode = 2;
                        if (!s_quiet)  Log(-1, "Stopping service(s):\n");
                }
                else
                        if (stricmp(s_control, "restart")==0)
                        {
                                mode = 3;
                                if (!s_quiet)  Log(-1, "Restarting service(s):\n");
                        }
                        else
                        {
                                Log((s_quiet?0:-1), "Unknown control token %s, only \'start\', \'stop\' and \'restart\' allowed!%s", s_control, s_quiet?"":"\n");
                                return;
                        }               
     
  if (win9x_check_name_all())
  {
        tmp = (char *)malloc(256);

        if (RegOpenKey(HKEY_LOCAL_MACHINE, Win9xRegServ, &hk)==ERROR_SUCCESS)
        {
                for (i=0, size = 256; RegEnumValue(hk, i, tmp, &size, NULL, &reg_type, NULL, NULL)==ERROR_SUCCESS; i++, size = 256)
                {
                        if (reg_type != REG_SZ) continue;

                        len = strlen(tmp);
                        if ((len<prefixlen)||(strncmp(tmp, Win9xServPrefix, prefixlen)!=0)||((len>prefixlen)&&(tmp[prefixlen]!='-')))
                                continue;

                        win9x_service_control_exec(tmp, mode);
                }

                RegCloseKey(hk);
        }
        free(tmp);
  }
  else
        win9x_service_control_exec(srvname, mode);
}

void win9x_service_un_install(int type, int argc, char **argv, char **envp)
{
  int i, j, k, len = 0;
  char *sp, *tmp, *path;
  HKEY hk=0;

  if (!s_quiet)  AllocTempConsole();

  if (win9x_check_name_all())
  {
        Log((s_quiet?0:-1), "Invalid service name!%s", s_quiet?"":"\n");
        return;
  }

  j = win9x_checkservice(srvname);
  if (type == 'u')
          j = !j;

  if (j)
  {
          if (!s_quiet)  Log(-1, "Service already %sinstalled...\n", type=='i'?"":"UN");
          return;
  }

  if (type == 'u')
  {
   if (RegOpenKey(HKEY_LOCAL_MACHINE, Win9xRegServ, &hk)==ERROR_SUCCESS)
   {
        RegDeleteValue(hk, srvname);
        RegCloseKey(hk);
   }
   if (!s_quiet)  Log(-1, "%s uninstalled...\n", srvname);
   win9x_service_control_exec(srvname, 2);

   return;
  }

// type == 'i'

  k = 1;
  if (RegOpenKey(HKEY_LOCAL_MACHINE, Win9xRegServ, &hk)!=ERROR_SUCCESS)
        if (RegCreateKey(HKEY_LOCAL_MACHINE, Win9xRegServ, &hk)!=ERROR_SUCCESS)
                k = 0;
  if (k)
  {
        len = 0;
        for (i=1;i<argc;i++)
                len+=strlen(argv[i])+1;

        j = GetCurrentDirectory(0, NULL);
        len+=j;

        sp = argv[0]+strlen(argv[0])-1;
        for (;sp>argv[0];sp--)
                if ((sp[0] == '\\')||(sp[0] == '/'))
                {
                        sp++;
                        break;
                }
        len+=strlen(sp)+1+strlen(srvname)+4;

        path = (char *)malloc(len+1);
        GetCurrentDirectory(j, path);
        path[j-1] = '\\';
        memcpy(path+j, sp, strlen(sp));
        tmp = path+j+strlen(sp);
        tmp[0] = ' ';
        tmp[1] = '-';
        tmp[2] = '(';
        memcpy(tmp+3, srvname, strlen(srvname));
        tmp+=strlen(srvname)+3;
        tmp[0] = ')';
        tmp[1] = 't';
        tmp+=2;

        path[len] = 0;

        j = 0;
        for (i=1; i<argc; i++)
        {
                tmp[j++] = ' ';
                memcpy(tmp+j, argv[i], strlen(argv[i]));
                j+=strlen(argv[i]);
        }


        k = RegSetValueEx(hk, srvname, 0, REG_SZ, path, strlen(path)) == ERROR_SUCCESS;
   
        free(path);
        RegCloseKey(hk);
  }

  if (!k)
        Log((s_quiet?0:-1), "Unable to store data in registry...%s", s_quiet?"":"\n");
  else
  {
        if (win9x_service_start(srvname))
        {
                if (!s_quiet)  Log(-1, "%s installed and started...\n", srvname);
        }
        else
        {
                if (!s_quiet)
                {
                        Log(-1, "%s installed...\n", srvname);
                        Log(-1, "Unable to start service!\n");
                }
        }
  }      

  return;
}

// TempConsole
void AllocTempConsole()
{
        if (s_console)
                return;
        
        if (AllocConsole())
        {
                HANDLE ha = GetStdHandle(STD_OUTPUT_HANDLE);
                int hCrt = _open_osfhandle((long) ha, 0x4000);
                FILE *hf = _fdopen( hCrt, "w" );
                stdout_old = *stdout;
                *stdout = *hf;

                ha = GetStdHandle(STD_ERROR_HANDLE);
                hCrt = _open_osfhandle((long) ha, 0x4000);
                hf = _fdopen( hCrt, "w" );
                stderr_old = *stderr;
                *stderr = *hf;
                setvbuf( stdout, NULL, _IONBF, 0 );
                setvbuf( stderr, NULL, _IONBF, 0 );

                s_console = 1;
        }
}

void FreeTempConsole()
{
        HANDLE in;
        int hCrt;
        FILE stdin_old, *hf;

        if (!s_console)
                return;

        printf("Press any key...\n");

        in = GetStdHandle(STD_OUTPUT_HANDLE);
        FlushConsoleInputBuffer(in);
        hCrt = _open_osfhandle((long) in, 0x4000);
        hf = _fdopen( hCrt, "r" );
        stdin_old = *stdin;
        *stdin = *hf;
        setvbuf(stdin, NULL, _IONBF, 0 );

        getch();

        FlushConsoleInputBuffer(in);

        *stdin  = stdin_old;
        *stdout = stdout_old;
        *stderr = stderr_old;

        FreeConsole();

        s_console = 0;
}

//
#endif        
