<@setoutput file=newFileName?default("")/>
${bannerComment}

<#if namespaceName!="">namespace ${namespaceName};</#if>

<#if doBasic>
//
// TODO generated message class
//
${msgKeyword} ${msgTypeName} {
    int someField;
    string anotherField;
    double arrayField1[];
    double arrayField2[10];
}
<#elseif doSubclassing>
cplusplus {{
#include "SomeMessage_m.h"
}}
${msgKeyword} SomeMessage;

//
// TODO generated message class
//
${msgKeyword} ${msgTypeName} extends SomeMessage {
    int someField;
}
<#elseif doUseCppType>
cplusplus {{
#include "SomeType.h"
}}
class noncobject SomeType;

cplusplus {{
#include <map>
typedef OtherType std::map<int,int>;
}}
class noncobject OtherType;

//
// TODO generated message class
//
${msgKeyword} ${msgTypeName} {
    SomeType field1;
    OtherType field2;
}
<#elseif doCustomize>
//
// TODO generated message class
//
${msgKeyword} ${msgTypeName} {
    @customize(true);  // see the generated C++ header for more info
    int someField;
    abstract int anotherField;
}
</#if>

