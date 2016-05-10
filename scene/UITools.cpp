#include "UITools.h"
#include "common/CGameSound.h"

CCRenderTexture* CaptureScreen(CCNode* target)
{
	CCRenderTexture* texture = CCRenderTexture::create( 
		CCDirector::sharedDirector()->getWinSize().width,
		CCDirector::sharedDirector()->getWinSize().height 
		);
	texture->begin();
	target->visit();
	texture->end();
	return texture;
}

CCSprite* CaptureScreenSprite(CCNode* target)
{
	CCSprite* sprite = CCSprite::createWithTexture( CaptureScreen(target)->getSprite()->getTexture() );
	return sprite;
}

//返回16进制字符串s对应的整数值，遇到任何一个非法字符都返回-1。   
int HexToDec(const char* s)
{   
	const char* p = s; 

	//空串返回0。   
	if(*p == '\0')   
		return 0;   

	//忽略开头的'0'字符   
	while(*p == '0')   
		p++;   

	int dec = 0;   
	char c;   

	//循环直到字符串结束。   
	while( (c = *p++) != 0 )
	{   
		//dec乘16   
		dec <<= 4;   

		//数字字符。   
		if(c >= '0' && c <= '9')   
		{
			dec += c - '0';
			continue;   
		}   

		//小写abcdef。   
		if(c >= 'a' && c <= 'f')   
		{   
			dec += c - 'a' + 10;   
			continue;   
		}   

		//大写ABCDEF。   
		if(c >= 'A' && c <= 'F')   
		{   
			dec += c - 'A' + 10;   
			continue;   
		}   

		//没有从任何一个if语句中结束，说明遇到了非法字符。   
		return -1;   
	}   

	//正常结束循环，返回10进制整数值。   
	return dec;   
}   

int GetIntFromColorName( const char* colorName )
{
	static CCDictionary* ColorDictionary = NULL;
	if( ColorDictionary == NULL )
	{
		ColorDictionary = CCDictionary::create();
		ColorDictionary->retain();
		ColorDictionary->setObject( CCInteger::create( 0xFFF0F8FF ), "AliceBlue" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFAEBD7 ), "AntiqueWhite" );
		ColorDictionary->setObject( CCInteger::create( 0xFF00FFFF ), "Aqua" );
		ColorDictionary->setObject( CCInteger::create( 0xFF7FFFD4 ), "Aquamarine" );
		ColorDictionary->setObject( CCInteger::create( 0xFFF0FFFF ), "Azure" );
		ColorDictionary->setObject( CCInteger::create( 0xFFF5F5DC ), "Beige" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFFE4C4 ), "Bisque" );
		ColorDictionary->setObject( CCInteger::create( 0xFF000000 ), "Black" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFFEBCD ), "BlanchedAlmond" );
		ColorDictionary->setObject( CCInteger::create( 0xFF0000FF ), "Blue" );
		ColorDictionary->setObject( CCInteger::create( 0xFF8A2BE2 ), "BlueViolet" );
		ColorDictionary->setObject( CCInteger::create( 0xFFA52A2A ), "Brown" );
		ColorDictionary->setObject( CCInteger::create( 0xFFDEB887 ), "BurlyWood" );
		ColorDictionary->setObject( CCInteger::create( 0xFF5F9EA0 ), "CadetBlue" );
		ColorDictionary->setObject( CCInteger::create( 0xFF7FFF00 ), "Chartreuse" );
		ColorDictionary->setObject( CCInteger::create( 0xFFD2691E ), "Chocolate" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFF7F50 ), "Coral" );
		ColorDictionary->setObject( CCInteger::create( 0xFF6495ED ), "CornflowerBlue" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFFF8DC ), "Cornsilk" );
		ColorDictionary->setObject( CCInteger::create( 0xFFDC143C ), "Crimson" );
		ColorDictionary->setObject( CCInteger::create( 0xFF00FFFF ), "Cyan" );
		ColorDictionary->setObject( CCInteger::create( 0xFF00008B ), "DarkBlue" );
		ColorDictionary->setObject( CCInteger::create( 0xFF008B8B ), "DarkCyan" );
		ColorDictionary->setObject( CCInteger::create( 0xFFB8860B ), "DarkGoldenrod" );
		ColorDictionary->setObject( CCInteger::create( 0xFFA9A9A9 ), "DarkGray" );
		ColorDictionary->setObject( CCInteger::create( 0xFF006400 ), "DarkGreen" );
		ColorDictionary->setObject( CCInteger::create( 0xFFBDB76B ), "DarkKhaki" );
		ColorDictionary->setObject( CCInteger::create( 0xFF8B008B ), "DarkMagenta" );
		ColorDictionary->setObject( CCInteger::create( 0xFF556B2F ), "DarkOliveGreen" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFF8C00 ), "DarkOrange" );
		ColorDictionary->setObject( CCInteger::create( 0xFF9932CC ), "DarkOrchid" );
		ColorDictionary->setObject( CCInteger::create( 0xFF8B0000 ), "DarkRed" );
		ColorDictionary->setObject( CCInteger::create( 0xFFE9967A ), "DarkSalmon" );
		ColorDictionary->setObject( CCInteger::create( 0xFF8FBC8F ), "DarkSeaGreen" );
		ColorDictionary->setObject( CCInteger::create( 0xFF483D8B ), "DarkSlateBlue" );
		ColorDictionary->setObject( CCInteger::create( 0xFF2F4F4F ), "DarkSlateGray" );
		ColorDictionary->setObject( CCInteger::create( 0xFF00CED1 ), "DarkTurquoise" );
		ColorDictionary->setObject( CCInteger::create( 0xFF9400D3 ), "DarkViolet" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFF1493 ), "DeepPink" );
		ColorDictionary->setObject( CCInteger::create( 0xFF00BFFF ), "DeepSkyBlue" );
		ColorDictionary->setObject( CCInteger::create( 0xFF696969 ), "DimGray" );
		ColorDictionary->setObject( CCInteger::create( 0xFF1E90FF ), "DodgerBlue" );
		ColorDictionary->setObject( CCInteger::create( 0xFFB22222 ), "Firebrick" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFFFAF0 ), "FloralWhite" );
		ColorDictionary->setObject( CCInteger::create( 0xFF228B22 ), "ForestGreen" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFF00FF ), "Fuchsia" );
		ColorDictionary->setObject( CCInteger::create( 0xFFDCDCDC ), "Gainsboro" );
		ColorDictionary->setObject( CCInteger::create( 0xFFF8F8FF ), "GhostWhite" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFFD700 ), "Gold" );
		ColorDictionary->setObject( CCInteger::create( 0xFFDAA520 ), "Goldenrod" );
		ColorDictionary->setObject( CCInteger::create( 0xFF808080 ), "Gray" );
		ColorDictionary->setObject( CCInteger::create( 0xFF008000 ), "Green" );
		ColorDictionary->setObject( CCInteger::create( 0xFFADFF2F ), "GreenYellow" );
		ColorDictionary->setObject( CCInteger::create( 0xFFF0FFF0 ), "Honeydew" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFF69B4 ), "HotPink" );
		ColorDictionary->setObject( CCInteger::create( 0xFFCD5C5C ), "IndianRed" );
		ColorDictionary->setObject( CCInteger::create( 0xFF4B0082 ), "Indigo" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFFFFF0 ), "Ivory" );
		ColorDictionary->setObject( CCInteger::create( 0xFFF0E68C ), "Khaki" );
		ColorDictionary->setObject( CCInteger::create( 0xFFE6E6FA ), "Lavender" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFFF0F5 ), "LavenderBlush" );
		ColorDictionary->setObject( CCInteger::create( 0xFF7CFC00 ), "LawnGreen" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFFFACD ), "LemonChiffon" );
		ColorDictionary->setObject( CCInteger::create( 0xFFADD8E6 ), "LightBlue" );
		ColorDictionary->setObject( CCInteger::create( 0xFFF08080 ), "LightCoral" );
		ColorDictionary->setObject( CCInteger::create( 0xFFE0FFFF ), "LightCyan" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFAFAD2 ), "LightGoldenrodYellow" );
		ColorDictionary->setObject( CCInteger::create( 0xFFD3D3D3 ), "LightGray" );
		ColorDictionary->setObject( CCInteger::create( 0xFF90EE90 ), "LightGreen" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFFB6C1 ), "LightPink" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFFA07A ), "LightSalmon" );
		ColorDictionary->setObject( CCInteger::create( 0xFF20B2AA ), "LightSeaGreen" );
		ColorDictionary->setObject( CCInteger::create( 0xFF87CEFA ), "LightSkyBlue" );
		ColorDictionary->setObject( CCInteger::create( 0xFF778899 ), "LightSlateGray" );
		ColorDictionary->setObject( CCInteger::create( 0xFFB0C4DE ), "LightSteelBlue" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFFFFE0 ), "LightYellow" );
		ColorDictionary->setObject( CCInteger::create( 0xFF00FF00 ), "Lime" );
		ColorDictionary->setObject( CCInteger::create( 0xFF32CD32 ), "LimeGreen" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFAF0E6 ), "Linen" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFF00FF ), "Magenta" );
		ColorDictionary->setObject( CCInteger::create( 0xFF800000 ), "Maroon" );
		ColorDictionary->setObject( CCInteger::create( 0xFF66CDAA ), "MediumAquamarine" );
		ColorDictionary->setObject( CCInteger::create( 0xFF0000CD ), "MediumBlue" );
		ColorDictionary->setObject( CCInteger::create( 0xFFBA55D3 ), "MediumOrchid" );
		ColorDictionary->setObject( CCInteger::create( 0xFF9370DB ), "MediumPurple" );
		ColorDictionary->setObject( CCInteger::create( 0xFF3CB371 ), "MediumSeaGreen" );
		ColorDictionary->setObject( CCInteger::create( 0xFF7B68EE ), "MediumSlateBlue" );
		ColorDictionary->setObject( CCInteger::create( 0xFF00FA9A ), "MediumSpringGreen" );
		ColorDictionary->setObject( CCInteger::create( 0xFF48D1CC ), "MediumTurquoise" );
		ColorDictionary->setObject( CCInteger::create( 0xFFC71585 ), "MediumVioletRed" );
		ColorDictionary->setObject( CCInteger::create( 0xFF191970 ), "MidnightBlue" );
		ColorDictionary->setObject( CCInteger::create( 0xFFF5FFFA ), "MintCream" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFFE4E1 ), "MistyRose" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFFE4B5 ), "Moccasin" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFFDEAD ), "NavajoWhite" );
		ColorDictionary->setObject( CCInteger::create( 0xFF000080 ), "Navy" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFDF5E6 ), "OldLace" );
		ColorDictionary->setObject( CCInteger::create( 0xFF808000 ), "Olive" );
		ColorDictionary->setObject( CCInteger::create( 0xFF6B8E23 ), "OliveDrab" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFFA500 ), "Orange" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFF4500 ), "OrangeRed" );
		ColorDictionary->setObject( CCInteger::create( 0xFFDA70D6 ), "Orchid" );
		ColorDictionary->setObject( CCInteger::create( 0xFFEEE8AA ), "PaleGoldenrod" );
		ColorDictionary->setObject( CCInteger::create( 0xFF98FB98 ), "PaleGreen" );
		ColorDictionary->setObject( CCInteger::create( 0xFFAFEEEE ), "PaleTurquoise" );
		ColorDictionary->setObject( CCInteger::create( 0xFFDB7093 ), "PaleVioletRed" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFFEFD5 ), "PapayaWhip" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFFDAB9 ), "PeachPuff" );
		ColorDictionary->setObject( CCInteger::create( 0xFFCD853F ), "Peru" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFFC0CB ), "Pink" );
		ColorDictionary->setObject( CCInteger::create( 0xFFDDA0DD ), "Plum" );
		ColorDictionary->setObject( CCInteger::create( 0xFFB0E0E6 ), "PowderBlue" );
		ColorDictionary->setObject( CCInteger::create( 0xFF800080 ), "Purple" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFF0000 ), "Red" );
		ColorDictionary->setObject( CCInteger::create( 0xFFBC8F8F ), "RosyBrown" );
		ColorDictionary->setObject( CCInteger::create( 0xFF4169E1 ), "RoyalBlue" );
		ColorDictionary->setObject( CCInteger::create( 0xFF8B4513 ), "SaddleBrown" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFA8072 ), "Salmon" );
		ColorDictionary->setObject( CCInteger::create( 0xFFF4A460 ), "SandyBrown" );
		ColorDictionary->setObject( CCInteger::create( 0xFF2E8B57 ), "SeaGreen" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFFF5EE ), "SeaShell" );
		ColorDictionary->setObject( CCInteger::create( 0xFFA0522D ), "Sienna" );
		ColorDictionary->setObject( CCInteger::create( 0xFFC0C0C0 ), "Silver" );
		ColorDictionary->setObject( CCInteger::create( 0xFF87CEEB ), "SkyBlue" );
		ColorDictionary->setObject( CCInteger::create( 0xFF6A5ACD ), "SlateBlue" );
		ColorDictionary->setObject( CCInteger::create( 0xFF708090 ), "SlateGray" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFFFAFA ), "Snow" );
		ColorDictionary->setObject( CCInteger::create( 0xFF00FF7F ), "SpringGreen" );
		ColorDictionary->setObject( CCInteger::create( 0xFF4682B4 ), "SteelBlue" );
		ColorDictionary->setObject( CCInteger::create( 0xFFD2B48C ), "Tan" );
		ColorDictionary->setObject( CCInteger::create( 0xFF008080 ), "Teal" );
		ColorDictionary->setObject( CCInteger::create( 0xFFD8BFD8 ), "Thistle" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFF6347 ), "Tomato" );
		ColorDictionary->setObject( CCInteger::create( 0x00FFFFFF ), "Transparent" );
		ColorDictionary->setObject( CCInteger::create( 0xFF40E0D0 ), "Turquoise" );
		ColorDictionary->setObject( CCInteger::create( 0xFFEE82EE ), "Violet" );
		ColorDictionary->setObject( CCInteger::create( 0xFFF5DEB3 ), "Wheat" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFFFFFF ), "White" );
		ColorDictionary->setObject( CCInteger::create( 0xFFF5F5F5 ), "WhiteSmoke" );
		ColorDictionary->setObject( CCInteger::create( 0xFFFFFF00 ), "Yellow" );
		ColorDictionary->setObject( CCInteger::create( 0xFF9ACD32 ), "YellowGreen" );
	}
	CCInteger* ccint = (CCInteger*)ColorDictionary->objectForKey( colorName );
	int intv = ccint->getValue();
	return intv;
}

ccColor3B GetColorFromXML( const char* xml )
{
	if( xml[0] == '#' )
	{
		int intv = HexToDec( (char*)xml+1 );
		return ccc3( (intv>>16)&0xFF, (intv>>8)&0xFF, intv&0xFF );
	}
	else
	{
		int intv = GetIntFromColorName( xml );
		return ccc3( (intv>>16)&0xFF, (intv>>8)&0xFF, intv&0xFF );
	}
}

//使用窗口坐标定位
void SetNodePosition( CCNode* node, float x, float y )
{
	//static const int SCREEN_WIDTH  = 960;
	static const int SCREEN_HEIGHT = 720;
	float w = node->boundingBox().size.width;
	float h = node->boundingBox().size.height;
	float ax = node->getAnchorPoint().x;
	float ay = node->getAnchorPoint().y;
	node->setPosition( ccp( x + w * ax, (SCREEN_HEIGHT-y) - h*(1.0f-ay) ) );	//上下颠倒，所以锚点也要颠倒
}
void SetNodePosition( CCNode* node, CCPoint pos )
{
	SetNodePosition( node, pos.x, pos.y );
}

CCPoint UI2GL( float x, float y, float w, float h, float ax, float ay )
{
	static const int SCREEN_HEIGHT = 720;
	return ccp( x + w * ax, (SCREEN_HEIGHT-y) - h*(1.0f-ay) );
}
CCPoint CCSizeCenter(CCSize size)
{
	return ccp(size.width/2, size.height/2);
}

void AddDebugPoint( CCNode* parent, CCPoint pos )
{
	CCLayerColor* redPoint = CCLayerColor::create(ccc4(0xFF,0,0,0xFF));
	redPoint->setContentSize(CCSize(2,2));
	redPoint->setPosition(pos);
	redPoint->setAnchorPoint(CCPointCenter);
	parent->addChild(redPoint);
}

char ToStringBuffer[1024];
char* ToString(int i)
{
	sprintf( ToStringBuffer, "%d", i );
	return ToStringBuffer;
}
char* ToString(unsigned int i)
{
	sprintf( ToStringBuffer, "%d", i );
	return ToStringBuffer;
}
char* ToString(float f)
{
	sprintf( ToStringBuffer, "%f", f );
	return ToStringBuffer;
}
char* ToString(long long ll)
{
	sprintf( ToStringBuffer, "%lld", ll );
	return ToStringBuffer;
}

std::string& toStrString(int i)
{
	sprintf(ToStringBuffer,"%d",i);
	std::string str(ToStringBuffer);
	return str;
}

const char* FormatChip(long long ll, bool dollar)
{
	if( ll <= 9999 )
	{
		if( dollar == true )
			sprintf( ToStringBuffer, "$%.0f",(float)ll );
		else 
			sprintf( ToStringBuffer, "%.0f", (float)ll );
	}
	if( ll >= 10000 && ll <= 99999 )
	{
		if( dollar == true )
			sprintf( ToStringBuffer, "$%.2f万",(float)ll/10000.0f );
		else 
			sprintf( ToStringBuffer, "%.2f万", (float)ll/10000.0f );
	}
	else if( ll >= 100000 && ll <= 999999 )
	{
		if( dollar == true )
			sprintf( ToStringBuffer, "$%.1f万",(float)ll/10000.0f );
		else 
			sprintf( ToStringBuffer, "%.1f万", (float)ll/10000.0f );
	}
	else if( ll >= 1000000 )
	{
		if( dollar == true )
			sprintf( ToStringBuffer, "$%.0f万",(float)ll/10000.0f );
		else 
			sprintf( ToStringBuffer, "%.0f万", (float)ll/10000.0f );
	}
	return U8(ToStringBuffer);
}
const char* FormatChipAtlas(long long ll)
{
	if( ll <= 9999 )						{ sprintf( ToStringBuffer, "%.0f", (float)ll ); }
	if( ll >= 10000 && ll <= 99999 )		{ sprintf( ToStringBuffer, "%.1f:", (float)ll/10000.0f ); }
	else if( ll >= 100000 && ll <= 999999 ) { sprintf( ToStringBuffer, "%.1f:", (float)ll/10000.0f ); }
	else if( ll >= 1000000 )				{ sprintf( ToStringBuffer, "%.0f:", (float)ll/10000.0f ); }
	int len = strlen(ToStringBuffer);
	for (int i=0; i<len; i++)
	{
		if( ToStringBuffer[i] == '.' )
			ToStringBuffer[i] = ';';
	}
	return U8(ToStringBuffer);
}

void Url2Filename(char* url)
{
	int count = strlen(url);
	for (int i = 0; i < count; i++)
	{
		if( url[i] == ':' || url[i] == '/' )
			url[i] = '_';
	}
}



static wchar_t* make_unicode_string(const unsigned char* utf8)
{
	int size = 0, index = 0, out_index = 0;
	wchar_t* out;
	unsigned char c;

	/* first calculate the size of the target string */
	c = utf8[index++];
	while (c)
	{
		if ((c & 0x80) == 0)
		{
			index += 0;
		}
		else if((c & 0xe0) == 0xe0)
		{
			index += 2;
		}
		else
		{
			index += 1;
		}
		size += 1;
		c = utf8[index++];
	}

	out = (wchar_t*)malloc((size + 1) * sizeof(wchar_t));
	if (out == NULL)
		return NULL;
	index = 0;

	c = utf8[index++];
	while (c)
	{
		if ((c & 0x80) == 0)
		{
			out[out_index++] = c;
		}
		else if ((c & 0xe0) == 0xe0)
		{
			out[out_index] = (c & 0x1F) << 12;
			c = utf8[index++];
			out[out_index] |= (c & 0x3F) << 6;
			c = utf8[index++];
			out[out_index++] |= (c & 0x3F);
		}
		else
		{
			out[out_index] = (c & 0x3F) << 6;
			c = utf8[index++];
			out[out_index++] |= (c & 0x3F);
		}
		c = utf8[index++];
	}
	out[out_index] = 0;
	return out;
}
static unsigned char* make_utf8_string(const wchar_t* unicode)
{
	int size = 0, index = 0, out_index = 0;
	unsigned char* out;
	unsigned short c;

	/* first calculate the size of the target string */
	c = unicode[index++];
	while (c) 
	{
		if (c < 0x0080) 
		{
			size += 1;
		}
		else if (c < 0x0800)
		{
			size += 2;
		}
		else
		{
			size += 3;
		}
		c = unicode[index++];
	}

	//out = (unsigned char*)malloc(size + 1);
	out = (unsigned char*)ToStringBuffer;
	if (out == NULL)
		return NULL;
	index = 0;

	c = unicode[index++];
	while (c)
	{
		if (c < 0x080)
		{
			out[out_index++] = (unsigned char)c;
		}
		else if (c < 0x800)
		{
			out[out_index++] = 0xc0 | (c >> 6);
			out[out_index++] = 0x80 | (c & 0x3f);
		}
		else
		{
			out[out_index++] = 0xe0 | (c >> 12);
			out[out_index++] = 0x80 | ((c >> 6) & 0x3f);
			out[out_index++] = 0x80 | (c & 0x3f);
		}
		c = unicode[index++];
	}
	out[out_index] = 0x00;
	return out;
}

size_t strlen_utf8(const char* s)
{
	size_t i = 0, j = 0;
	while (s[i]) {
		if ((s[i] & 0xc0) != 0x80)
			j++;
		i++;
	}
	return (j);
}


#if( CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 )
#include "iconv/iconv.h"
int code_convert(const char *from_charset, const char *to_charset, const char *inbuf, size_t inlen, char *outbuf, size_t outlen)  
{  
	iconv_t cd;  
	const char* temp = inbuf;  
	const char** pin = &temp;  
	char** pout = &outbuf;  
	memset(outbuf,0,outlen);  
	cd = iconv_open(to_charset,from_charset);  
	if(cd==0) return -1;  
	if(iconv(cd,pin,&inlen,pout,&outlen)==-1) return -1;  
	iconv_close(cd);  
	return 0;  
}
char iconvBuffer[1024];
//UTF8 -> GB2312
char* u2a(const char *inbuf)  
{  
	size_t inlen = strlen(inbuf);  
	if(code_convert("utf-8", "gb2312", inbuf, inlen, iconvBuffer, sizeof(iconvBuffer)) == 0)  
		return iconvBuffer;
	return NULL;
}  

//big5 ->utf8  big5
char* b5tu(const char *inbuf)  
{  
	size_t inlen = strlen(inbuf);  
	if(code_convert("big5", "utf-8", inbuf, inlen, iconvBuffer, sizeof(iconvBuffer)) == 0)  
		return iconvBuffer;
	return NULL;
}  


//GB2312 -> UTF8
char* a2u(const char* inbuf)  
{  
	size_t inlen = strlen(inbuf);  
	if(code_convert("gb2312", "utf-8", inbuf, inlen, iconvBuffer, sizeof(iconvBuffer)) == 0)  
		return iconvBuffer;
	return NULL;
}
#endif	// WIN32

#if( CC_TARGET_PLATFORM == CC_PLATFORM_WP8 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT )
#include "Encoder.h"
#endif

//只能处理1024个字符
const char* U8(const char* inbuf)
{
#if( CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 )
	return a2u(inbuf);
#endif
#if( CC_TARGET_PLATFORM == CC_PLATFORM_WP8 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT )
	GB2312StrToUtf8(ToStringBuffer, inbuf, strlen(inbuf));
	return ToStringBuffer;
#endif
	return inbuf;
}

const char* ShortName(const char* src)
{
	wchar_t* unicode = make_unicode_string((const unsigned char*)src);
	size_t ulen = wcslen(unicode);	//todo 安卓修改
	wchar_t buffer[16];
	memset(buffer,0,sizeof(buffer));
	int max = 0;
	for (size_t	i = 0; i < ulen; i++)
	{
		if( unicode[i] < 0xFF )
		{
			max+=1;
			buffer[i] = unicode[i];
		}
		else
		{
			max+=2;
			buffer[i] = unicode[i];
		}
		if( max >= 8 )
		{
			if( i+1 < ulen )
			{
				buffer[i+1] = '.';	
				buffer[i+2] = '.';
				buffer[i+3] = '.';
				break;
			}
		}
	}
	free(unicode);
	unsigned char* utf8 = make_utf8_string(buffer);
	return (const char*)utf8;
}
const char* SubString(const char* s, int offset, const char* startString, const char* endString )
{
	const char* start = strstr(s+offset, startString);
	if( start == NULL ) return NULL;
	size_t startLen = strlen(startString);

	const char* end = strstr(start+startLen, endString);
	if( end == NULL ) return NULL;
	size_t resultLen = end - start - startLen;

	assert( resultLen+1 < 1024 );	//ToStringBuffer 1024
	memcpy( ToStringBuffer, start + startLen, resultLen );
	ToStringBuffer[resultLen] = 0;
	return ToStringBuffer;
}

void SetSpirteTexture(const char* image, CCSprite* sprite, float width, float height)
{
	//todo check 这里内存会不会用尽？
	CCTexture2D* texture = CCTextureCache::sharedTextureCache()->textureForKey(image);	
	if( texture == NULL )
		texture = CCTextureCache::sharedTextureCache()->addImage(image);
	if( texture != NULL )
	{
		CCSpriteFrame* frame = CCSpriteFrame::createWithTexture( texture, CCRectMake(0,0,texture->getPixelsWide(),texture->getPixelsHigh()) );
		sprite->setScaleX( width/texture->getPixelsWide() );
		sprite->setScaleY( height/texture->getPixelsHigh() );
		sprite->setDisplayFrame(frame);
	}
}

