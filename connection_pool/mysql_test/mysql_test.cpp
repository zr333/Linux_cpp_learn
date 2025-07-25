#include <iostream>
#include <mysql/mysql.h> //mysql提供的函数接口头文件

using namespace std;

int main()
{
    const char *host = "localhost"; // 主机名
    const char *user = "root";      // 用户名
    const char *pwd = "123456789"; // 密码
    const char *dbName = "mydb_test";     // 数据库名称
    int port = 3306;                // 端口号

    // 创建mysql对象
    MYSQL *sql = nullptr;
    sql = mysql_init(sql);
    if (!sql)
    {
        cout << "MySql init error!" << endl;
    }

    // 连接mysql
    sql = mysql_real_connect(sql, host, user, pwd, dbName, port, nullptr, 0);
    if (!sql)
    {
        cout << "MySql Connect error!" << endl;
    }else {
        cout << "MySql Connect success!" << endl;
    }

    // 执行命令
  
    // 关闭mysql
    mysql_close(sql);
    return 0;
}