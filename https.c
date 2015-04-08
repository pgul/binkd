/*
 *  https.c -- connect procedure for https & socks4/5 proxy
 *
 *  https.c is an addition part of binkd project
 *
 *  Copyright (C) 1996-1998  Dima Maloff, 5047/13
 *  Copyright (C) 1998-2000  Dima Afanasiev, 5020/463
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#include <time.h>
#endif

#include "sys.h"
#include "readcfg.h"
#include "https.h"
#include "tools.h"
#include "iphdr.h"
#include "iptools.h"
#include "sem.h"
#ifdef NTLM
#include "ntlm/helpers.h"
#endif
#include "rfc2553.h"
#include "srv_gai.h"

static char b64t[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static int enbase64(char *data, int size, char *p)
{	
  int i;
  int c;
  unsigned char *q;
  char *s = p;
  q = (unsigned char*)data;
  i=0;
  for(i = 0; i < size;){
    c=q[i++];
    c*=256;
    if(i < size)
      c+=q[i];
    i++;
    c*=256;
    if(i < size)
      c+=q[i];
    i++;
    p[0]=b64t[(c&0x00fc0000) >> 18];
    p[1]=b64t[(c&0x0003f000) >> 12];
    p[2]=b64t[(c&0x00000fc0) >> 6];
    p[3]=b64t[(c&0x0000003f) >> 0];
    if(i > size)
      p[3]='=';
    if(i > size+1)
      p[2]='=';
    p+=4;
  }
  return p - s;
}

#ifdef WIN32
#define SetTCPError(x) WSASetLastError(x)
#define PR_ERROR WSABASEERR+13
#else
#define SetTCPError(x) errno=x
#define PR_ERROR EACCES
#endif
int h_connect(int so, const char *host, const char *port, BINKD_CONFIG *config, const char *proxy, const char *socks)
{
	int ntlm = 0;
#ifdef NTLM
	char *ntlmsp = NULL;
#endif
	int i, n;
	struct addrinfo *ai, *aiHead, hints;
	int aiErr;
	char buf[1024], *pbuf;
	char *sp, *sauth;
	struct in_addr defaddr;

	/* setup hints for getaddrinfo */
	memset((void *)&hints, 0, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	if (proxy[0])
	{
		strncpy(buf, proxy, sizeof(buf));
		buf[sizeof(buf)-1] = '\0';
		if ((sp=strchr(buf, '/')) != NULL)
			*sp++ = '\0';
		Log(4, "connected to proxy %s", buf);
		if(sp) 
		{
			char *sp1;
#ifdef NTLM
			if(strchr(sp, '/') != strrchr(sp, '/')) 
			{
				ntlm = 1;
				ntlmsp = sp = strdup(sp);
			}
			else
#endif
			{
				if((sp1=strchr(sp, '/'))!=NULL) sp1[0]=':';
				sp1=malloc(strlen(sp)*4/3 + 4);
				sp1[enbase64(sp, strlen(sp), sp1)] = 0;
				sp = sp1;
			}
		}
		memset(buf, 0, sizeof(buf));
		if (socks[0]) {
			char *sp1;
			strncpy(buf, socks, sizeof(buf)-6);
			if((sp1=strchr(buf, '/'))!=NULL) sp1[0]=0;
			if((sp1=strchr(buf, ':'))==NULL) strcat(buf, ":1080");
			sp1=strdup(buf);
			i=snprintf(buf, sizeof(buf), "CONNECT %s HTTP/1.%d\r\n", sp1, ntlm);
			free(sp1);
		}
		else
			i=snprintf(buf, sizeof(buf), "CONNECT %s:%s HTTP/1.%d\r\n", host, port, ntlm);
		if (sp)
		{
#ifdef NTLM
			if (ntlm)
			{
				if (i<sizeof(buf)) i+=snprintf(buf+i, sizeof(buf)-i, "Connection: keep-alive\r\n");
				if (i<sizeof(buf)) i+=snprintf(buf+i, sizeof(buf)-i, "Proxy-Authorization: NTLM ");
				if (i<sizeof(buf)) getNTLM1(sp, buf+i, sizeof(buf)-i);
				if (sizeof(buf)>strlen(buf)+3) strcat(buf, "\r\n");
				i = strlen(buf);
			}
			else
#endif
			{
				if (i<sizeof(buf)) i+=snprintf(buf+i, sizeof(buf)-i, "Proxy-Authorization: basic %s\r\n", sp);
				free(sp);
			}
		}
		if (sizeof(buf) > i+2)
		{
			buf[i++]='\r';
			buf[i++]='\n';
		}
		if (send(so, buf, i, 0) < 0)
		{
			Log(4, "Send to proxy error: %s", TCPERR());
			SetTCPError(PR_ERROR);
			return 1;
		}
		Log(10, "sent proxy sockfd %d request: %s", so, buf);
		for(i=0; i<sizeof(buf)-1; i++)
		{
			struct timeval tv;
			fd_set fds;
			FD_ZERO(&fds);
			FD_SET(so, &fds);
			tv.tv_sec=config->nettimeout;
			tv.tv_usec=0;
			if ((n=select(so+1, &fds, NULL, NULL, config->nettimeout > 0 ? &tv : NULL)) < 1)
			{
				if (n<0)
					Log(4, "proxy error: %s", TCPERR());
				else
					Log(4, "proxy timeout...");
				SetTCPError(PR_ERROR);
				return 1;
			}
			if ((n=recv(so, buf+i, 1, 0)) < 1)
			{
				if (n<0)
					Log(2, "Proxy error: %s", TCPERR());
				else
					Log(2, "Connection closed by proxy...");
				SetTCPError(PR_ERROR);
				return 1;
			}
			buf[i+1]=0;
			if((i+2>=sizeof(buf))||((sp=strstr(buf, "\r\n\r\n"))!=NULL)||((sp=strstr(buf, "\n\n"))!=NULL))
			{
#ifdef NTLM
				if ((ntlm) && ((sp=strstr(buf, "uthenticate: NTLM "))!=NULL))
				{
					char *sp1 = strstr(buf, "Content-Length: ");
					sp = strdup(sp+18);
					if (sp1)
					{
						int j=atoi(sp1+16);
						for(;j>0; j--)
						{
							if(recv(so, buf+i, 1, 0)<1) break;
						}
					}
					memset(buf, 0, sizeof(buf));
					i=snprintf(buf, sizeof(buf), "CONNECT %s:%s HTTP/1.%d\r\nProxy-Authorization: NTLM ",
						 host, port, ntlm);
					i = getNTLM2(ntlmsp, sp, buf + i, sizeof(buf) - i);
					free(sp);
					if (i) Log(2, "Invalid username/password/host/domain string (%s) %d", ntlmsp, i);
					free(ntlmsp);

					if(!i)
					{
						ntlm = 0;
						if (strlen(buf)+3<sizeof(buf)) strcat(buf, "\r\n\r\n");
						send(so, buf, strlen(buf), 0);
						i=0;
						continue;
					}
					
				}
#endif
				if((sp=strchr(buf, '\n'))!=NULL)
				{
					sp[0]=0;
					sp--;
					if(sp[0]=='\r') sp[0]=0;
				}
				if(strstr(buf, " 200 ")) break;
				Log(2, "Connection rejected by proxy (%s)", buf);
				SetTCPError(PR_ERROR);
				return 1;
			}
		}
	}
	if (socks[0])
	{
		strncpy(buf, socks, sizeof(buf));
		buf[sizeof(buf)-1] = '\0';
		if ((sauth=strchr(buf, '/')) != NULL)
			*sauth++ = '\0';
		Log(4, "connected to socks%c %s", sauth ? '5' : '4', buf);
		if (!sauth) /* SOCKS4 */
		{
			/* SOCKS4 only support IPv4 and we need the IP address */
			if ((aiErr=srv_getaddrinfo(host, port, &hints, &aiHead)) != 0)
			{
				Log(2, "getaddrinfo failed: %s (%d)", gai_strerror(aiErr), aiErr);
				SetTCPError(PR_ERROR);
				return 1;
			}
		}
		else	    /* SOCKS5 */
		{
			if ((aiErr=getaddrinfo(NULL, port, &hints, &aiHead)) != 0)
			{
				Log(2, "getaddrinfo failed: %s (%d)", gai_strerror(aiErr), aiErr);
				return 1;
			}
			sauth=strdup(sauth);
			sp=strchr(sauth, '/');
			buf[0]=5;
			buf[2]=0;
			if(!sauth[0]) {
				buf[1]=1;
				send(so, buf, 3, 0);
			}
			else {
				buf[1]=2;
				if(sp) buf[3]=2;
				else buf[3]=1;
				send(so, buf, 4, 0);
			}
			if ((recv(so, buf, 2, 0)!=2)||((buf[1])&&(buf[1]!=1)&&(buf[1]!=2)))
			{
				Log(1, "Auth. method not supported by socks5 server");
				free(sauth);
				freeaddrinfo(aiHead);
				SetTCPError(PR_ERROR);
				return 1;
			}
			Log(6, "Socks5, Auth=%d", buf[1]);
			if (buf[1]==2) /* username/password method */
			{
				buf[0]=1;
				if (!sp) i=strlen(sauth);
				else i=(sp-sauth);
				buf[1]=i;
				memcpy(buf+2, sauth, i);
				i+=2;
				if (!sp) buf[i++]=0;
				else {
					sp++;
					buf[i++]=strlen(sp);
					strcpy(buf+i, sp);
					i+=strlen(sp);
				}
				send(so, buf, i, 0);
				buf[0]=buf[1]=0;
				if ((recv(so, buf, 2, 0)<2)||(buf[1]))
				{
					Log(1, "Authentication failed (socks5 returns %02X%02X)", (unsigned char)buf[0], (unsigned char)buf[1]);
					free(sauth);
					freeaddrinfo(aiHead);
					SetTCPError(PR_ERROR);
					return 1;
				}
			}
		}

		for (ai = aiHead; ai != NULL; ai = ai->ai_next)
		{
			unsigned short portnum = ntohs(((struct sockaddr_in*)(ai->ai_addr))->sin_port);
			if (!sauth) /* SOCKS4 */
			{
				buf[0]=4;
				buf[1]=1;
				lockhostsem();
				Log (4, strcmp(port, config->oport) == 0 ? "trying %s..." : "trying %s:%u...",
				     inet_ntoa(((struct sockaddr_in*)(ai->ai_addr))->sin_addr), portnum);
				releasehostsem();
				buf[2]=(unsigned char)((portnum>>8)&0xFF);
				buf[3]=(unsigned char)(portnum&0xFF);
				memcpy(buf+4, &(((struct sockaddr_in*)(ai->ai_addr))->sin_addr), 4);
				buf[8]=0;
				send(so, buf, 9, 0);
			}
			else	    /* SOCKS5 */
			{
				buf[0]=5;
				buf[1]=1;
				buf[2]=0;
				if (isdigit(host[0]) &&
				    (defaddr.s_addr = inet_addr (host)) != INADDR_NONE)
				{	
					buf[3]=1;
					memcpy(buf+4, &defaddr, 4);
					pbuf = buf+8;
				} else
				{
					buf[3]=3;
					i = strlen(host);
					buf[4]=(unsigned char)i;
					memcpy(buf+5, host, i);
					pbuf = buf+5+i;
				}
				*pbuf++=(unsigned char)((portnum>>8)&0xFF);
				*pbuf++=(unsigned char)(portnum&0xFF);
				send(so, buf, pbuf-buf, 0);
			}
			for (i=0; i<sizeof(buf); i++)
			{
				struct timeval tv;
				fd_set fds;
				FD_ZERO(&fds);
				FD_SET(so, &fds);
				tv.tv_sec=config->nettimeout;
				tv.tv_usec=0;
				if ((n=select(so+1, &fds, NULL, NULL, config->nettimeout > 0 ? &tv : NULL)) < 1)
				{
					if (n<0)
						Log(4, "socks error: %s", TCPERR());
					else
						Log(4, "socks timeout...");
					if (sauth) free(sauth);
					freeaddrinfo(aiHead);
					SetTCPError(PR_ERROR);
					return 1;
				}
				if ((n=recv(so, buf+i, 1, 0))<1) {
					if (n<0)
						Log(2, "socks error: %s", TCPERR());
						Log(2, "connection closed by socks server...");
					if (sauth) free(sauth);
					freeaddrinfo(aiHead);
					SetTCPError(PR_ERROR);
					return 1;
				}
				if (!sauth && i>6) /* 8th byte received */
				{
					if (buf[0]!=0) {
						Log(2, "Bad reply from socks server");
						freeaddrinfo(aiHead);
						SetTCPError(PR_ERROR);
						return 1;
					}
					if (buf[1]!=90) {
						Log(2, "connection rejected by socks4 server (%d)", (unsigned char)buf[1]);
						SetTCPError(PR_ERROR);
						break; /* try next IP */
					}
					else {
						freeaddrinfo(aiHead);
						return 0;
					}
				}
				else if (sauth && i>5)
				{
					if (buf[0]!=5) {
						Log(2, "Bad reply from socks server");
						free(sauth);
						freeaddrinfo(aiHead);
						SetTCPError(PR_ERROR);
						return 1;
					}
					if ((buf[3]==1) && (i<9)) continue;
					if ((buf[3]==3) && (i<(6+(unsigned char)buf[4]))) continue;
					if ((buf[3]==4) && (i<21)) continue;
					free(sauth);
					freeaddrinfo(aiHead);
					if (!buf[1])	return 0;
					switch (buf[1])
					{
						case 1: Log (2, "general SOCKS5 server failure"); break;
						case 2: Log (2, "connection not allowed by ruleset (socks5)"); break;
						case 3: Log (2, "Network unreachable (socks5)"); break;
						case 4: Log (2, "Host unreachable (socks5)"); break;
						case 5: Log (2, "Connection refused (socks5)"); break;
						case 6: Log (2, "TTL expired (socks5)"); break;
						case 7: Log (2, "Command not supported by socks5"); break;
						case 8: Log (2, "Address type not supported"); break;
						default: Log (2, "Unknown reply (0x%02X) from socks5 server", (unsigned char)buf[1]);
					}
					SetTCPError(PR_ERROR);
					return 1;
				}
			}
		}
		freeaddrinfo(aiHead);
	}
	return 0;
}
