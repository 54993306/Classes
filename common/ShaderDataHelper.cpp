#include "common/ShaderDataHelper.h"

using namespace std;  

ShaderDataHelper* ShaderDataHelper::m_instance = nullptr;

ShaderDataHelper::ShaderDataHelper()
{
	m_pPrograms = new CCDictionary();
}

ShaderDataHelper::~ShaderDataHelper()
{
	CC_SAFE_RELEASE(m_pPrograms);
}

void ShaderDataHelper::loadShaderFile()
{
	loadJsonFile();
}

CCGLProgram* ShaderDataHelper::getShaderByType( ShaderType iType )
{
	if(iType==ShaderDefault)
	{
		return CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTextureColor);
	}

	ShaderStruct& structShader = getShaderDataByType(iType);

	CCGLProgram* program;
	program = CCShaderCache::sharedShaderCache()->programForKey(structShader.strResName.c_str());
	if(program==nullptr)
	{
		program = new CCGLProgram;
		program->initWithVertexShaderFilename(SHADER_DEFAULT_VSH, structShader.strResName.c_str());
		program->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);  
		program->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);  
		program->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
		program->link();
		program->updateUniforms();
		CCShaderCache::sharedShaderCache()->addProgram(program, structShader.strResName.c_str());
		m_pPrograms->setObject(program, structShader.strResName);
		program->release();
	}
	return program;
}

ShaderStruct& ShaderDataHelper::getShaderDataByType(ShaderType iType )
{
	return vecShaderData.at(iType);
}

std::vector<ShaderStruct>& ShaderDataHelper::getShaderDataVector()
{
	return vecShaderData;
}

void ShaderDataHelper::loadJsonFile()
{
	unsigned long size = 0;
	unsigned char *buff = CCFileUtils::sharedFileUtils()->getFileData(shaderJsonFile,"r",&size);
	string contentStr ((const char*)buff, size);
	rapidjson::Document d;
	d.Parse<0>(contentStr.c_str());

	if (d.HasParseError())
	{    
		//解析错误  
		CCLOG("GetParseError %s\n",d.GetParseError());    
	}   
	else if (d.IsObject()&&d.HasMember("shader"))//这个时候要保证d.IsObject类型  
	{    
		const rapidjson::Value &a=d["shader"];  //读取中文的数组  
		if (a.IsArray())  //判断是不是数组  
		{    
			for(unsigned int i=0;i<a.Size();++i) //如果不是数组，这一行会报错  
			{  
				const rapidjson::Value &val = a[i]; //得到单个对象  

				ShaderStruct effectStruct;

				//标记ID 
				effectStruct.iId = i;

				//获取ID 
				if(val.HasMember("id"))
				{  
					const rapidjson::Value &val_id = val["id"]; 
					if(val_id.IsInt())
					{
						int iId = val_id.GetInt();
						effectStruct.iId = iId;
					}  
				}

				//获取pic_name
				if(val.HasMember("file_name"))
				{
					const rapidjson::Value &val_id = val["file_name"];
					if(val_id.IsString())
					{
						string strName = val_id.GetString();
						effectStruct.strResName = strName;
					}
				}

				//获取preload
				if(val.HasMember("preload"))
				{
					const rapidjson::Value &val_id = val["preload"];
					if(val_id.IsInt())
					{
						int iPreload = val_id.GetInt();
						effectStruct.iPreloadCount = iPreload;
					}
				}
				vecShaderData.push_back(effectStruct);
			}  
		}    
	}    
	else
	{
		CCLOG("!----------Ｎo_Effect_Find----------!"); //没查找到任何对象则返回空字符串  
	}
}

void ShaderDataHelper::destroy()
{
	CC_SAFE_DELETE(m_instance);
}

ShaderDataHelper* ShaderDataHelper::getInstance()
{
	if(m_instance==nullptr)
	{
		m_instance = new ShaderDataHelper;
	}
	return m_instance;
}

void ShaderDataHelper::reloadAllShaders()
{
	CCDictElement * pElement;

	CCDICT_FOREACH(m_pPrograms, pElement)
	{
		const char * key = pElement->getStrKey();
		CCGLProgram* program = (CCGLProgram *)pElement->getObject();
		program->reset();
		program->initWithVertexShaderFilename(SHADER_DEFAULT_VSH, key);
		program->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);  
		program->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);  
		program->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
		program->link();
		program->updateUniforms();
		CCLOG("ShaderDataHelper::reloadAllShaders->%s", key);
	}
}