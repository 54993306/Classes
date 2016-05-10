//
//  CProtoType.cpp
//  Proto
//
//  Created by wang liangchen on 13-5-21.
//  Copyright (c) 2013年 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "CProtoType.h"
CProtoType::CProtoType()
{
    add(0,"protos.Disconnect");
	add(1,"protos.Protocol");
	add(2,"protos.LoginResponse");
	add(3,"protos.InGameResponse");
	add(4,"protos.CreateResponse");
	add(5,"protos.RoleListResponse");
}

CProtoType::~CProtoType()
{
    
}

void CProtoType::add(int code, std::string typeName)
{
	/*
    char name[64];
    sprintf(name, "com.mobile.netty.protocal.%s",typeName.c_str());
    std::string strName(name);*/
    typeMap[code] = typeName;
}

std::string CProtoType::getNameByType(int type)
{
    std::map<int, std::string>::const_iterator iter = typeMap.find(type);
    if (iter != typeMap.end()) 
    {
        return typeMap[type];
    }
    return "";
}

