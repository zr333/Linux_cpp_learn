#include <iostream>
#include <fstream>
#include <jsoncpp/json/json.h> // Ubuntu包是 <jsoncpp/json/json.h>，源码版是 <json/json.h>
using namespace std;

int main()
{
    // 1. 打开文件
    ifstream ifs("dbconf.json");
    if (!ifs.is_open())
    {
        cerr << "Failed to open config file!" << endl;
        return -1;
    }

    // 2. 解析
    Json::CharReaderBuilder reader;
    /*
    CharReaderBuilder 是 JsonCpp 提供的类，用来生成一个 CharReader（字符流 JSON 解析器）。
    它会读取字符流（如文件、字符串）并解析为 JsonCpp 的数据结构。
    */
    Json::Value root;
    string errs;
    if (!Json::parseFromStream(reader, ifs, &root, &errs))
    /*
    作用：从文件流 ifs 中读取 JSON，并解析到 root 变量中。
    参数：
        reader：解析器配置。
        ifs：输入流（上面打开的文件）。
        &root：解析结果存放到这里。
        &errs：如果解析失败，错误信息存储在这里。
    返回值：true 表示解析成功，false 表示解析失败。
    如果解析失败，打印错误信息并返回 -1 退出。
    */
    {
        cerr << "Failed to parse JSON: " << errs << endl;
        return -1;
    }

    // 3. 访问数据
    string ip = root["ip"].asString();
    int port = root["port"].asInt();
    string userName = root["userName"].asString();
    string password = root["password"].asString();
    string dbName = root["dbName"].asString();

    cout << "IP: " << ip << endl;
    cout << "Port: " << port << endl;
    cout << "User: " << userName << endl;
    cout << "Password: " << password << endl;
    cout << "DB: " << dbName << endl;

    return 0;
}
