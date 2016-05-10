#ifndef __ENCODER_H__
#define __ENCODER_H__

void GB2312StrToUtf8(
	char *utf8Str,		/* Output Utf-8 chars */
	const char* gbStr,	/* Input GB2312 chars */
	int nBytes			/* size of input GB2312 chars */
	);

#endif  // __ENCODER_H__
