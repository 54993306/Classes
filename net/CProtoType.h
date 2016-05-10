//
//  CProtoType.h
//  Proto
//
//  Created by wang liangchen on 13-5-21.
//  Copyright (c) 2013年 __MyCompanyName__. All rights reserved.
//

#ifndef Proto_CProtoType_h
#define Proto_CProtoType_h
#include <map>

class CProtoType {
    public:
    CProtoType();
    ~CProtoType();
    std::string getNameByType(int type);
    private:
	void add(int code, std::string typeName);
    std::map<int,std::string> typeMap;
};


#endif
