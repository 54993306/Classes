#include "MathUtil.h"
void drawSolidCircle(const CCPoint& center,float radius,unsigned int segments,ccColor4F color)  
{  
	const float coef = 2.0f * (float)M_PI/segments;  
	CCPoint *vertices=(CCPoint*)calloc(segments+1,sizeof(CCPoint));  

	for(unsigned int i = 0;i <= segments; i++) {  
		float rads = i*coef;  
		GLfloat j = radius * cosf(rads)  + center.x;  
		GLfloat k = radius * sinf(rads)  + center.y;  
		vertices[i].x=j;  
		vertices[i].y=k;  
	}  
	ccDrawSolidPoly( vertices, segments,color);  
	free(vertices);  
}