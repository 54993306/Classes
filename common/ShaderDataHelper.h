#pragma once

#include "AppUI.h"

USING_NS_CC;

using namespace std;

//provide easy access
#define ShaderDataMgr ShaderDataHelper::getInstance()

//Shader_Struct
struct ShaderStruct
{
	int iId;									//ID
	string strResName;			//资源名称
	int iPreloadCount;			//预加载数量
	//...//
};

const char shaderJsonFile[50] = 
{
	"shaders/shader_data.json"
};

#define SHADER_DEFAULT_VSH "shaders/default.vsh"

enum ShaderType
{
	ShaderBanish,			//放逐
	ShaderBlur,				//模糊（不建议使用，效果不太好）
	ShaderFire,				//燃烧
	ShaderFrozen,			//冻结
	ShaderGrayScaling,	//灰度
	ShaderIce,					//冰封
	ShaderInvisible,		//无形的
	ShaderMirror,			//镜面反射(偏蓝色)
	ShaderPoison,			//中毒
	ShaderStone,			//石化
	ShaderVanish,			//消失
	ShaderSepia,				//深褐色
	ShaderOutline,			//描边（需要设置参数）

	ShaderDefault,			//默认
	ShaderMax
};

class ShaderDataHelper
{
public:
	ShaderDataHelper();
	~ShaderDataHelper();
	ShaderStruct& getShaderDataByType(ShaderType iType);
	std::vector<ShaderStruct>& getShaderDataVector();
	CCGLProgram* getShaderByType(ShaderType iType);
	static ShaderDataHelper* getInstance();
	static void destroy();
	void reloadAllShaders();
public:
	void loadShaderFile();
private:
	void loadJsonFile();
private:
	std::vector<ShaderStruct> vecShaderData;
	static ShaderDataHelper* m_instance;
	CCDictionary* m_pPrograms;
};