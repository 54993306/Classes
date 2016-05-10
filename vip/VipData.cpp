#include "VipData.h"
#include "support/tinyxml2/tinyxml2.h"

CVipData::CVipData():level(0)	
,target(0)
,buyAction(0)
,buyCoin(0)
,actionLimit(0)
{

}

void CVipData::read(const protos::VipData& vd)
{
	this->level = vd.level();
	this->target = vd.target();
	this->buyAction  = vd.buyaction();
	this->buyCoin = vd.buycoin();
	this->actionLimit = vd.actionlimit();
	for (int i = 0; i < vd.prizelist_size(); i++)
	{
		CPrize prize;
		prize.read(vd.prizelist(i));
		this->prizeList.push_back(prize);
	}
}

CVipInfoRes::CVipInfoRes():level(0)	
	,exp(0)
	,nextExp(0)
{

}


void CVipInfoRes::read(const protos::VipInfoRes& vr)
{
	this->level = vr.level();
	this->exp = vr.exp();
	this->nextExp = vr.nextexp();
	this->data.read(vr.data());
}

CVipDesc::CVipDesc()
{
	parseXml("csv/vip.xml");
}

void CVipDesc::parseXml(const char* xml)
{
	tinyxml2::XMLDocument doc;
#if ( CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID )
	std::string filePath = CCFileUtils::sharedFileUtils()->fullPathForFilename(xml);
	CCString* xmlContent = CCString::createWithContentsOfFile(filePath.c_str());
	tinyxml2::XMLError error = doc.Parse(xmlContent->getCString());
	if( error != tinyxml2::XML_SUCCESS ) return;
#else
	tinyxml2::XMLError error = doc.LoadFile( CCFileUtils::sharedFileUtils()->fullPathForFilename(xml).c_str() );
	if( error != tinyxml2::XML_SUCCESS ) return;
#endif
	tinyxml2::XMLNode* node = doc.FirstChildElement();
	tinyxml2::XMLElement* viplv = node->FirstChildElement("viplv");
	while (viplv)
	{
		vector<VipPriv> privVec;
		tinyxml2::XMLElement* xmlChild = viplv->FirstChildElement();
		while( xmlChild != NULL )
		{
			VipPriv priv;
			priv.priv1 = xmlChild->Attribute("desc1");
			priv.priv2 = xmlChild->Attribute("number");
			priv.priv3 = xmlChild->Attribute("desc2");
			privVec.push_back(priv);
			xmlChild = xmlChild->NextSiblingElement();
		}
		m_privMap[viplv->IntAttribute("lv")] = privVec;
		viplv = viplv->NextSiblingElement();
	}
}

const VipPrivMap& CVipDesc::getPriv()
{
	return m_privMap;
}

const vector<VipPriv>& CVipDesc::getPrivByLevel(int level)
{
	return m_privMap[level];
}


CGoldCard::CGoldCard():id(0)
,price(0)
,recommend(false)
,canBy(false)
{
}

void CGoldCard::read(const GoldCard& gc)
{
	this->id = gc.id();
	this->name = gc.name();
	this->note = gc.note();
	this->price = gc.price();
	this->canBy = gc.canby();
	this->recommend = gc.recommend();
}
