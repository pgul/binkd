/* crypto/md32_common.h */
/* ====================================================================
 * Copyright (c) 1999 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.OpenSSL.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    licensing@OpenSSL.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.OpenSSL.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com).  This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com).
 *
 */

/*
 * This is a generic 32 bit "collector" for message digest algorithms.
 * Whenever needed it collects input character stream into chunks of
 * 32 bit values and invokes a block function that performs actual hash
 * calculations.
 *
 * Porting guide.
 *
 * Obligatory macros:
 *
 * DATA_ORDER_IS_BIG_ENDIAN or DATA_ORDER_IS_LITTLE_ENDIAN
 *	this macro defines byte order of input stream.
 * HASH_CBLOCK
 *	size of a unit chunk HASH_BLOCK operates on.
 * HASH_LONG
 *	has to be at lest 32 bit wide, if it's wider, then
 *	HASH_LONG_LOG2 *has to* be defined along
 * HASH_CTX
 *	context structure that at least contains following
 *	members:
 *		typedef struct {
 *			...
 *			HASH_LONG	Nl,Nh;
 *			HASH_LONG	data[HASH_LBLOCK];
 *			int		num;
 *			...
 *			} HASH_CTX;
 * HASH_UPDATE
 *	name of "Update" function, implemented here.
 * HASH_TRANSFORM
 *	name of "Transform" function, implemented here.
 * HASH_FINAL
 *	name of "Final" function, implemented here.
 * HASH_BLOCK_HOST_ORDER
 *	name of "block" function treating *aligned* input message
 *	in host byte order, implemented externally.
 * HASH_BLOCK_DATA_ORDER
 *	name of "block" function treating *unaligned* input message
 *	in original (data) byte order, implemented externally (it
 *	actually is optional if data and host are of the same
 *	"endianess").
 * HASH_MAKE_STRING
 *	macro convering context variables to an ASCII hash string.
 *
 * Optional macros:
 *
 * B_ENDIAN or L_ENDIAN
 *	defines host byte-order.
 * HASH_LONG_LOG2
 *	defaults to 2 if not states otherwise.
 * HASH_LBLOCK
 *	assumed to be HASH_CBLOCK/4 if not stated otherwise.
 *
 * MD5 example:
 *
 *	#define DATA_ORDER_IS_LITTLE_ENDIAN
 *
 *	#define HASH_LONG		MD5_LONG
 *	#define HASH_LONG_LOG2		MD5_LONG_LOG2
 *	#define HASH_CTX		MD5_CTX
 *	#define HASH_CBLOCK		MD5_CBLOCK
 *	#define HASH_LBLOCK		MD5_LBLOCK
 *	#define HASH_UPDATE		MD5_Update
 *	#define HASH_TRANSFORM		MD5_Transform
 *	#define HASH_FINAL		MD5_Final
 *	#define HASH_BLOCK_HOST_ORDER	md5_block_host_order
 *	#define HASH_BLOCK_DATA_ORDER	md5_block_data_order
 *
 *					<appro@fy.chalmers.se>
 */

#if !defined(DATA_ORDER_IS_BIG_ENDIAN) && !defined(DATA_ORDER_IS_LITTLE_ENDIAN)
#error "DATA_ORDER must be defined!"
#endif

#ifndef HASH_CBLOCK
#error "HASH_CBLOCK must be defined!"
#endif
#ifndef HASH_LONG
#error "HASH_LONG must be defined!"
#endif
#ifndef HASH_CTX
#error "HASH_CTX must be defined!"
#endif

#ifndef HASH_UPDATE
#error "HASH_UPDATE must be defined!"
#endif
#ifndef HASH_TRANSFORM
#error "HASH_TRANSFORM must be defined!"
#endif
#ifndef HASH_FINAL
#error "HASH_FINAL must be defined!"
#endif

#ifndef HASH_BLOCK_HOST_ORDER
#error "HASH_BLOCK_HOST_ORDER must be defined!"
#endif

#ifndef HASH_BLOCK_DATA_ORDER
#error "HASH_BLOCK_DATA_ORDER must be defined!"
#endif

#ifndef HASH_LBLOCK
#define HASH_LBLOCK	(HASH_CBLOCK/4)
#endif

#ifndef HASH_LONG_LOG2
#define HASH_LONG_LOG2	2
#endif

#ifdef ROTATE
#undef ROTATE
#endif
#define ROTATE(a,n)     (((a)<<(n))|(((a)&0xffffffff)>>(32-(n))))

/*
 * Make some obvious choices. E.g., HASH_BLOCK_DATA_ORDER_ALIGNED
 * and HASH_BLOCK_HOST_ORDER ought to be the same if input data
 * and host are of the same "endianess". It's possible to mask
 * this with blank #define HASH_BLOCK_DATA_ORDER though...
 *
 *				<appro@fy.chalmers.se>
 */

#if defined(DATA_ORDER_IS_BIG_ENDIAN)

#define HOST_c2l(c,l)	(l =(((unsigned long)(*((c)++)))<<24),		\
			 l|=(((unsigned long)(*((c)++)))<<16),		\
			 l|=(((unsigned long)(*((c)++)))<< 8),		\
			 l|=(((unsigned long)(*((c)++)))    ),		\
			 l)
#define HOST_p_c2l(c,l,n)	{					\
			switch (n) {					\
			case 0: l =((unsigned long)(*((c)++)))<<24;	\
			case 1: l|=((unsigned long)(*((c)++)))<<16;	\
			case 2: l|=((unsigned long)(*((c)++)))<< 8;	\
			case 3: l|=((unsigned long)(*((c)++)));		\
				} }
#define HOST_p_c2l_p(c,l,sc,len) {					\
			switch (sc) {					\
			case 0: l =((unsigned long)(*((c)++)))<<24;	\
				if (--len == 0) break;			\
			case 1: l|=((unsigned long)(*((c)++)))<<16;	\
				if (--len == 0) break;			\
			case 2: l|=((unsigned long)(*((c)++)))<< 8;	\
				} }
/* NOTE the pointer is not incremented at the end of this */
#define HOST_c2l_p(c,l,n)	{					\
			l=0; (c)+=n;					\
			switch (n) {					\
			case 3: l =((unsigned long)(*(--(c))))<< 8;	\
			case 2: l|=((unsigned long)(*(--(c))))<<16;	\
			case 1: l|=((unsigned long)(*(--(c))))<<24;	\
				} }
#define HOST_l2c(l,c)	(*((c)++)=(unsigned char)(((l)>>24)&0xff),	\
			 *((c)++)=(unsigned char)(((l)>>16)&0xff),	\
			 *((c)++)=(unsigned char)(((l)>> 8)&0xff),	\
			 *((c)++)=(unsigned char)(((l)    )&0xff),	\
			 l)

#elif defined(DATA_ORDER_IS_LITTLE_ENDIAN)

#define HOST_c2l(c,l)	(l =(((unsigned long)(*((c)++)))    ),		\
			 l|=(((unsigned long)(*((c)++)))<< 8),		\
			 l|=(((unsigned long)(*((c)++)))<<16),		\
			 l|=(((unsigned long)(*((c)++)))<<24),		\
			 l)
#define HOST_p_c2l(c,l,n)	{					\
			switch (n) {					\
			case 0: l =((unsigned long)(*((c)++)));		\
			case 1: l|=((unsigned long)(*((c)++)))<< 8;	\
			case 2: l|=((unsigned long)(*((c)++)))<<16;	\
			case 3: l|=((unsigned long)(*((c)++)))<<24;	\
				} }
#define HOST_p_c2l_p(c,l,sc,len) {					\
			switch (sc) {					\
			case 0: l =((unsigned long)(*((c)++)));		\
				if (--len == 0) break;			\
			case 1: l|=((unsigned long)(*((c)++)))<< 8;	\
				if (--len == 0) break;			\
			case 2: l|=((unsigned long)(*((c)++)))<<16;	\
				} }
/* NOTE the pointer is not incremented at the end of this */
#define HOST_c2l_p(c,l,n)	{					\
			l=0; (c)+=n;					\
			switch (n) {					\
			case 3: l =((unsigned long)(*(--(c))))<<16;	\
			case 2: l|=((unsigned long)(*(--(c))))<< 8;	\
			case 1: l|=((unsigned long)(*(--(c))));		\
				} }
#define HOST_l2c(l,c)	(*((c)++)=(unsigned char)(((l)    )&0xff),	\
			 *((c)++)=(unsigned char)(((l)>> 8)&0xff),	\
			 *((c)++)=(unsigned char)(((l)>>16)&0xff),	\
			 *((c)++)=(unsigned char)(((l)>>24)&0xff),	\
			 l)

#endif

/*
 * Time for some action:-)
 */

void HASH_UPDATE (HASH_CTX *c, const void *data_, unsigned long len)
	{
	const unsigned char *data=(unsigned char *)data_;
	register HASH_LONG * p;
	register unsigned long l;
	int sw,sc,ew,ec;

	if (len==0) return;

	l=(c->Nl+(len<<3))&0xffffffffL;
	/* 95-05-24 eay Fixed a bug with the overflow handling, thanks to
	 * Wei Dai <weidai@eskimo.com> for pointing it out. */
	if (l < c->Nl) /* overflow */
		c->Nh++;
	c->Nh+=(len>>29);
	c->Nl=l;

	if (c->num != 0)
		{
		p=c->data;
		sw=c->num>>2;
		sc=c->num&0x03;

		if ((c->num+len) >= HASH_CBLOCK)
			{
			l=p[sw]; HOST_p_c2l(data,l,sc); p[sw++]=l;
			for (; sw<HASH_LBLOCK; sw++)
				{
				HOST_c2l(data,l); p[sw]=l;
				}
			HASH_BLOCK_HOST_ORDER (c,p,1);
			len-=(HASH_CBLOCK-c->num);
			c->num=0;
			/* drop through and do the rest */
			}
		else
			{
			c->num+=len;
			if ((sc+len) < 4) /* ugly, add char's to a word */
				{
				l=p[sw]; HOST_p_c2l_p(data,l,sc,len); p[sw]=l;
				}
			else
				{
				ew=(c->num>>2);
				ec=(c->num&0x03);
				l=p[sw]; HOST_p_c2l(data,l,sc); p[sw++]=l;
				for (; sw < ew; sw++)
					{
					HOST_c2l(data,l); p[sw]=l;
					}
				if (ec)
					{
					HOST_c2l_p(data,l,ec); p[sw]=l;
					}
				}
			return;
			}
		}

	sw=len/HASH_CBLOCK;
	if (sw > 0)
		{
#if defined(HASH_BLOCK_DATA_ORDER)
			{
			HASH_BLOCK_DATA_ORDER(c,data,sw);
			sw*=HASH_CBLOCK;
			data+=sw;
			len-=sw;
			}
#endif
		}

	if (len!=0)
		{
		p = c->data;
		c->num = len;
		ew=len>>2;	/* words to copy */
		ec=len&0x03;
		for (; ew; ew--,p++)
			{
			HOST_c2l(data,l); *p=l;
			}
		HOST_c2l_p(data,l,ec);
		*p=l;
		}
	}


void HASH_TRANSFORM (HASH_CTX *c, const unsigned char *data)
	{
#if defined(HASH_BLOCK_DATA_ORDER)
	HASH_BLOCK_DATA_ORDER (c,data,1);
#endif
	}


void HASH_FINAL (unsigned char *md, HASH_CTX *c)
	{
	register HASH_LONG *p;
	register unsigned long l;
	register int i,j;
	static const unsigned char end[4]={0x80,0x00,0x00,0x00};
	const unsigned char *cp=end;

	/* c->num should definitly have room for at least one more byte. */
	p=c->data;
	i=c->num>>2;
	j=c->num&0x03;

#if 0
	/* purify often complains about the following line as an
	 * Uninitialized Memory Read.  While this can be true, the
	 * following p_c2l macro will reset l when that case is true.
	 * This is because j&0x03 contains the number of 'valid' bytes
	 * already in p[i].  If and only if j&0x03 == 0, the UMR will
	 * occur but this is also the only time p_c2l will do
	 * l= *(cp++) instead of l|= *(cp++)
	 * Many thanks to Alex Tang <altitude@cic.net> for pickup this
	 * 'potential bug' */
#ifdef PURIFY
	if (j==0) p[i]=0; /* Yeah, but that's not the way to fix it:-) */
#endif
	l=p[i];
#else
	l = (j==0) ? 0 : p[i];
#endif
	HOST_p_c2l(cp,l,j); p[i++]=l; /* i is the next 'undefined word' */

	if (i>(HASH_LBLOCK-2)) /* save room for Nl and Nh */
		{
		if (i<HASH_LBLOCK) p[i]=0;
		HASH_BLOCK_HOST_ORDER (c,p,1);
		i=0;
		}
	for (; i<(HASH_LBLOCK-2); i++)
		p[i]=0;

#if   defined(DATA_ORDER_IS_BIG_ENDIAN)
	p[HASH_LBLOCK-2]=c->Nh;
	p[HASH_LBLOCK-1]=c->Nl;
#elif defined(DATA_ORDER_IS_LITTLE_ENDIAN)
	p[HASH_LBLOCK-2]=c->Nl;
	p[HASH_LBLOCK-1]=c->Nh;
#endif
	HASH_BLOCK_HOST_ORDER (c,p,1);

#ifndef HASH_MAKE_STRING
#error "HASH_MAKE_STRING must be defined!"
#else
	HASH_MAKE_STRING(c,md);
#endif

	c->num=0;
	/* clear stuff, HASH_BLOCK may be leaving some stuff on the stack
	 * but I'm not worried :-)
	memset((void *)c,0,sizeof(HASH_CTX));
	 */
	}
