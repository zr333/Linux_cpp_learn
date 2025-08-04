#include <iostream>
#include <fstream>
#include <jsoncpp/json/json.h>
using namespace std;

int main()
{
    // 1. 构造 JSON 对象
    Json::Value root;
    root["ip"] = "127.0.0.1";
    root["port"] = 3306;
    root["userName"] = "root";
    root["password"] = "123456789";
    root["dbName"] = "mydb_test";

    // 2. 转换为字符串（带缩进，便于阅读）
    Json::StreamWriterBuilder writer;
    writer["indentation"] = "    "; // 设置4空格缩进
    string jsonStr = Json::writeString(writer, root);

    // 3. 输出到控制台
    cout << jsonStr << endl;

    // 4. （可选）保存到文件
    // ofstream ofs("db_config.json");
    // ofs << jsonStr;
    // ofs.close();

    return 0;
}
