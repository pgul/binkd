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

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "Config.h"
#include "readcfg.h"
#include "sys.h"
#include "tools.h"
#include "https.h"
#include "iptools.h"

#ifdef HAVE_THREADS
#include "sem.h"
extern MUTEXSEM hostsem;
#endif

static char b64t[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static char *enbase64(char *s, char *rc)
{	
	int i, j, k;
	if(!s) return NULL;
	k=strlen(s);
	j=(k/3);
	if(k%3) j++;
	j*=4;
	memset(rc, '=', j);
	rc[j]=0;
	for(i=j=0;i<k;i++) {		
		int c;
		rc[j++]=b64t[s[i]>>2];
		c=(s[i]&0x3)<<4;
		if((++i)<k) c|=s[i]>>4;
		rc[j++]=b64t[c];
		if(i>=k) break;
		c=(s[i]&0xF)<<2;
		if((++i)<k) c|=s[i]>>6;
		rc[j++]=b64t[c];
		if(i>=k) break;
		rc[j++]=b64t[s[i]&0x3F];
	}
	return rc;
}
#ifdef WIN32
#define SetTCPError(x) WSASetLastError(x)
#define PR_ERROR WSABASEERR+13
#else
#define SetTCPError(x) errno=x
#define PR_ERROR EACCES
#endif
int h_connect(int *so, struct sockaddr_in *name)
{
	int i, err, connected=0;
	struct sockaddr_in sin;
#ifdef HAVE_THREADS
	struct hostent he;
#endif
	struct hostent *hp;
	unsigned char buf[256];
	char *sp;

	if(proxy[0])
	{
		strcpy(buf, proxy);
		if((sp=strchr(buf, ':'))!=NULL) {
			sp[0]=0;
			sin.sin_port=htons((unsigned short)atoi(sp+1));
			if((sp=strchr(sp+1, '/'))!=NULL) sp++;
		}
		else {
			if((sp=strchr(buf, '/'))!=NULL) {
				sp[0]=0;
				sp++;
			}
			sin.sin_port=3128; /* default port for HTTPS */
		}
		if((isdigit(buf[0]))&&(isdigit(buf[strlen(buf)-1])))
		{
			sin.sin_addr.s_addr=inet_addr(buf);
			sin.sin_family=AF_INET;
			if(!connect(*so, (struct sockaddr*)&sin, sizeof(struct sockaddr_in)))
				connected=1;
			else err=TCPERRNO;
		}
		else 
		{
#ifdef HAVE_THREADS
			LockSem(&hostsem);
#endif
			if((hp=gethostbyname(buf))==NULL) 
			{
#ifdef HAVE_THREADS
				ReleaseSem(&hostsem);
#endif
				Log (1, "%s: unknown proxy host", buf);
				return 1;
			}
#ifdef HAVE_THREADS
			copy_hostent(&he, hp);
			hp = &he;
			ReleaseSem(&hostsem);
#endif
			for(i=0;hp->h_addr_list && hp->h_addr_list[i];i++)
			{
				memcpy(&sin.sin_addr, hp->h_addr_list[i], hp->h_length);
				sin.sin_family=hp->h_addrtype;
				if(!connect(*so, (struct sockaddr*)&sin, sizeof(struct sockaddr_in)))
				{
					connected=1;
					break;
				}
				else
					err=TCPERRNO;
				soclose(*so);
				if((*so=socket(hp->h_addrtype, SOCK_STREAM, 0))==INVALID_SOCKET)
					break;
			}
#ifdef HAVE_THREADS
			if (hp->h_addr_list && hp->h_addr_list[0])
				free(hp->h_addr_list[0]);
			if (hp->h_addr_list)
				free(hp->h_addr_list);
#endif
		}
		if(!connected)
		{
			Log(2, "Unable to connect to proxy server %s:%d", buf, ntohs(sin.sin_port));
			SetTCPError(err);
			return 1;
		}
		Log(4, "connected to proxy %s:%d", buf, ntohs(sin.sin_port));
		if(sp) 
		{
			char *sp1;
			if((sp1=strchr(sp, '/'))!=NULL) sp1[0]=':';
			sp=strdup(sp);
			if(enbase64(sp, buf)) sp1=strdup(buf);
			else sp1=NULL;
			free(sp);
			sp=sp1;
		}
		if(socks[0]) {
			char *sp1;
			strcpy(buf, socks);
			if((sp1=strchr(buf, '/'))!=NULL) sp1[0]=0;
			if((sp1=strchr(buf, ':'))==NULL) strcat(buf, ":1080");
			sp1=strdup(buf);
			i=sprintf(buf, "CONNECT %s HTTP/1.0\r\n", sp1);
			free(sp1);
		}
		else
#ifdef HAVE_THREADS
		LockSem(&hostsem);
#endif
		i=sprintf(buf, "CONNECT %s:%d HTTP/1.0\r\n",
			inet_ntoa(name->sin_addr),
			ntohs(name->sin_port));
#ifdef HAVE_THREADS
		ReleaseSem(&hostsem);
#endif
		if(sp)
		{
			i+=sprintf(buf+i, "Proxy-Authorization: basic %s\r\n", sp);
			free(sp);
		}
		buf[i++]='\r';
		buf[i++]='\n';
		send(*so, buf, i, 0);
		for(i=0;i<sizeof(buf);i++)
		{
			struct timeval tv;
			fd_set fds;
			FD_ZERO(&fds);
			FD_SET(*so, &fds);
			tv.tv_sec=nettimeout;
			tv.tv_usec=0;
			if(select((*so)+1, &fds, NULL, &fds, nettimeout>0?&tv:NULL)<1)
			{
				Log(4, "proxy timeout...");
				SetTCPError(PR_ERROR);
				return 1;
			}
			if(recv(*so, buf+i, 1, 0)<1) {
				Log(2, "Connection closed by proxy...");
				SetTCPError(PR_ERROR);
				return 1;
			}
			buf[i+1]=0;
			if((i+1>=sizeof(buf))||((sp=strstr(buf, "\r\n\r\n"))!=NULL)||((sp=strstr(buf, "\n\n"))!=NULL))
			{
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
	if(socks[0])
	{
		strcpy(buf, socks);
		if((sp=strchr(buf, ':'))!=NULL) {
			sp[0]=0;
			sin.sin_port=htons((unsigned short)atoi(sp+1));
			if((sp=strchr(sp+1, '/'))!=NULL) sp++;
		}
		else {
			if((sp=strchr(buf, '/'))!=NULL) {
				sp[0]=0;
				sp++;
			}
			sin.sin_port=1080; /* default port for SOCKS */
		}
		if(!connected) /* we can use socks proxy outside https... */
		{
			if((isdigit(buf[0]))&&(isdigit(buf[strlen(buf)-1])))
			{
				sin.sin_addr.s_addr=inet_addr(buf);
				sin.sin_family=AF_INET;
				if(!connect(*so, (struct sockaddr*)&sin, sizeof(struct sockaddr_in)))
					connected=1;
				else err=TCPERRNO;
			}
			else 
			{
#ifdef HAVE_THREADS
				LockSem(&hostsem);
#endif
				if((hp=gethostbyname(buf))==NULL)
				{
#ifdef HAVE_THREADS
					ReleaseSem(&hostsem);
#endif
					Log (1, "%s: unknown socks host", buf);
					return 1;
				}
#ifdef HAVE_THREADS
				copy_hostent(&he, hp);
				hp = &he;
				ReleaseSem(&hostsem);
#endif
				for(i=0;hp->h_addr_list[i];i++)
				{
					memcpy(&sin.sin_addr, hp->h_addr_list[i], hp->h_length);
					sin.sin_family=hp->h_addrtype;
					if(!connect(*so, (struct sockaddr*)&sin, sizeof(struct sockaddr_in)))
					{
						connected=1;
						break;
					}
					else
						err=TCPERRNO;
					soclose(*so);
					if((*so=socket(hp->h_addrtype, SOCK_STREAM, 0))==INVALID_SOCKET)
						break;
				}
#ifdef HAVE_THREADS
				if (hp->h_addr_list && hp->h_addr_list[0])
					free(hp->h_addr_list[0]);
				if (hp->h_addr_list)
					free(hp->h_addr_list);
#endif
			}
		}
		if(!connected) {
			Log(2, "Unable to connect to socks server %s:%d", buf, ntohs(sin.sin_port));
			SetTCPError(err);
			return 1;
		}
		Log(4, "connected to socks%c %s:%d", sp?'5':'4', buf, ntohs(sin.sin_port));
		if(!sp)
		{
			buf[0]=4;
			buf[1]=1;
			i=ntohs(name->sin_port);
			buf[2]=(i>>8)&0xFF;
			buf[3]=(i&0xFF);
			memcpy(buf+4, &name->sin_addr, 4);
			buf[8]=0;
			send(*so, buf, 9, 0);
		}
		else 
		{
			char *sp1;
			sp=strdup(sp);
			sp1=strchr(sp, '/');
			buf[0]=5;
			buf[2]=0;
			if(!sp[0]) {
				buf[1]=1;
				send(*so, buf, 3, 0);
			}
			else {
				buf[1]=2;
				if(sp1) buf[3]=2;
				else buf[3]=1;
				send(*so, buf, 4, 0);
			}
			if((recv(*so, buf, 2, 0)!=2)||((buf[1])&&(buf[1]!=1)&&(buf[1]!=2)))
			{
				Log(1, "Auth. method not supported by socks5 server");
				free(sp);
				SetTCPError(PR_ERROR);
				return 1;
			}
			Log(6, "Socks5, Auth=%d", buf[1]);
			if(buf[1]==2) /* username/password method */
			{
				buf[0]=1;
				if(!sp1) i=strlen(sp);
				else i=(sp1-sp);
				buf[1]=i;
				memcpy(buf+2, sp, i);
				i+=2;
				if(!sp1) buf[i++]=0;
				else {
					sp1++;
					buf[i++]=strlen(sp1);
					strcpy(buf+i, sp1);
					i+=strlen(sp1);
				}
				send(*so, buf, i, 0);
				buf[0]=buf[1]=0;
				if((recv(*so, buf, 2, 0)<2)||(buf[1]))
				{
					Log(1, "Authentication failed (socks5 returns %02X%02X)", buf[0], buf[1]);
					free(sp);
					SetTCPError(PR_ERROR);
					return 1;
				}
			}
			buf[0]=5;
			buf[1]=1;
			buf[2]=0;
			buf[3]=1;
			memcpy(buf+4, &name->sin_addr, 4);
			i=ntohs(name->sin_port);
			buf[8]=(i>>8)&0xFF;
			buf[9]=(i&0xFF);
			send(*so, buf, 10, 0);
		}
		for(i=0;i<sizeof(buf);i++)
		{
			struct timeval tv;
			fd_set fds;
			FD_ZERO(&fds);
			FD_SET(*so, &fds);
			tv.tv_sec=nettimeout;
			tv.tv_usec=0;
			if(select((*so)+1, &fds, NULL, &fds, nettimeout>0?&tv:NULL)<1)
			{
				Log(4, "socks timeout...");
				SetTCPError(PR_ERROR);
				return 1;
			}
			if(recv(*so, buf+i, 1, 0)<1) {
				Log(2, "connection closed by socks server...");
				SetTCPError(PR_ERROR);
				return 1;
			}
			if((!sp)&&(i>6)) /* 8th byte received */
			{
				if(buf[1]!=90) {
					Log(2, "connection rejected by socks4 server (%d)", buf[1]);
					SetTCPError(PR_ERROR);
					return 1;					
				}
				else break;
			}
			else if((sp)&&(buf[0]==5)&&(i>5))
			{
				if((buf[3]==1)&&(i<9)) continue;
				if((buf[3]==3)&&(i<(6+buf[4]))) continue;
				if((buf[3]==4)&&(i<21)) continue;
				free(sp);
				if(!buf[1])	break;
				switch(buf[1])
				{
					case 1: Log (2, "general SOCKS5 server failure"); break;
					case 2: Log (2, "connection not allowed by ruleset (socks5)"); break;
					case 3: Log (2, "Network unreachable (socks5)"); break;
					case 4: Log (2, "Host unreachable (socks5)"); break;
					case 5: Log (2, "Connection refused (socks5)"); break;
					case 6: Log (2, "TTL expired (socks5)"); break;
					case 7: Log (2, "Command not supported by socks5"); break;
					case 8: Log (2, "Address type not supported"); break;
					default: Log (2, "Unknown reply (0x%02X) from socks5 server", buf[1]);
				}
				SetTCPError(PR_ERROR);
				return 1;
			}
		}
	}
	if(connected) return 0;
	return connect(*so, (struct sockaddr*)name, sizeof(struct sockaddr_in));
}
