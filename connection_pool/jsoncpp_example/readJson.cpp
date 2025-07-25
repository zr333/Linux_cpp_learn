#include <iostream>
#include <jsoncpp/json/json.h>
int main()
{
    // JSON字符串
    std::string jsonString = R"({"name":"John Doe","age":30,"isAlive":true,"address":{"city":"New York","state":"NY"}})"; // R表示原始字符串（raw string），不用手动加转义符。

    // 创建一个Json::CharReaderBuilder
    Json::CharReaderBuilder builder;

    // 创建一个Json::Value对象
    Json::Value root;

    // 创建一个错误信息字符串
    std::string errors;

    // 解析JSON字符串
    std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    bool parsingSuccessful = reader->parse(jsonString.c_str(), jsonString.c_str() + jsonString.size(), &root, &errors);

    if (!parsingSuccessful)
    {
        // 打印错误信息并退出
        std::cout << "Error parsing JSON: " << errors << std::endl;
        return 1;
    }

    // 提取并打印数据
    std::cout << "Name: " << root["name"].asString() << std::endl;
    std::cout << "Age: " << root["age"].asInt() << std::endl;
    std::cout << "Is Alive: " << (root["isAlive"].asBool() ? "Yes" : "No") << std::endl;
    std::cout << "City: " << root["address"]["city"].asString() << std::endl;
    std::cout << "State: " << root["address"]["state"].asString() << std::endl;

    return 0;
}