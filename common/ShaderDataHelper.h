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
	string strResName;			//��Դ����
	int iPreloadCount;			//Ԥ��������
	//...//
};

const char shaderJsonFile[50] = 
{
	"shaders/shader_data.json"
};

#define SHADER_DEFAULT_VSH "shaders/default.vsh"

enum ShaderType
{
	ShaderBanish,			//����
	ShaderBlur,				//ģ����������ʹ�ã�Ч����̫�ã�
	ShaderFire,				//ȼ��
	ShaderFrozen,			//����
	ShaderGrayScaling,	//�Ҷ�
	ShaderIce,					//����
	ShaderInvisible,		//���ε�
	ShaderMirror,			//���淴��(ƫ��ɫ)
	ShaderPoison,			//�ж�
	ShaderStone,			//ʯ��
	ShaderVanish,			//��ʧ
	ShaderSepia,				//���ɫ
	ShaderOutline,			//��ߣ���Ҫ���ò�����

	ShaderDefault,			//Ĭ��
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