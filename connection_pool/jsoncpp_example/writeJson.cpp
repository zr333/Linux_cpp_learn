#include <jsoncpp/json/json.h>
#include <iostream>

/*
JsonCpp 主要包含三种类型的 class： (老式API讲解理论核心并不过时，这里按照老式的来讲，新式api只是为了安全而已)

Json::Value：可以表示所有支持的类型，如：int , double ,string , object, array等。其包含节点的类型判断(isNull,isBool,isInt,isArray,isMember,isValidIndex等),类型获取(type),类型转换(asInt,asString等),节点获取(get,[]),节点比较(重载<,<=,>,>=,==,!=),节点操作(compare,swap,removeMember,removeindex,append等)等函数。

Json::Reader：将文件流或字符串创解析到Json::Value中，主要使用parse函数。Json::Reader的构造函数还允许用户使用特性Features来自定义Json的严格等级。

Json::Writer：与JsonReader相反，将Json::Value转换成字符串流等，Writer类是一个纯虚类，并不能直接使用。在此我们使用 Json::Writer 的子类：Json::FastWriter(将数据写入一行,没有格式),Json::StyledWriter(按json格式化输出,易于阅读)
*/

int main()
{
    // 创建一个Json::Value对象
    Json::Value root;

    // 向对象中添加数据
    root["name"] = "John Doe";
    root["age"] = 30;
    root["isAlive"] = true;
    root["address"]["city"] = "New York";
    root["address"]["state"] = "NY";

    // 创建一个Json::StreamWriterBuilder配置器,用于控制输出格式
    Json::StreamWriterBuilder writer;
    //writer["indentation"] = ""; // 每层缩进 4 个空格
    writer["emitUTF8"] = true;      // 输出 UTF-8

    // 将Json::Value对象转换为字符串
    std::string output = Json::writeString(writer, root);

    // 打印输出
    std::cout << output << std::endl;
    return 0;
}