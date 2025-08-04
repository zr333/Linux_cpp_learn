#include <iostream>
#include <memory>
#include <thread>
#include "MysqlConn.h"
#include "connPool.h"
using namespace std;
// 1.单线程：使用/不适用连接池
// 2.多线程：使用/不适用连接池

void op1(int begin, int end)
{
    for (int i = begin; i < end; ++i)
    {
        MysqlConn conn;
        conn.connect("root", "123456789", "mydb_test", "127.0.0.1", 3306);
        char sql[1024] = {0};
        sprintf(sql, "insert into person (id,age,sex,name) values(%d,%d,'%s','%s')",
                i, 6, "女", "胧月大魔王");
        conn.update(sql);
    }
}

void op2(ConnPool *pool, int begin, int end)
{
    for (int i = begin; i < end; ++i)
    {
        shared_ptr<MysqlConn> conn = pool->getConn();
        char sql[1024] = {0};
        sprintf(sql, "insert into person (id,age,sex,name) values(%d,%d,'%s','%s')",
                i, 19, "女", "安陵容");
        conn->update(sql);
    }
}

// 压力测试
void test1()
{
#if 1
    // 非连接池，单线程，用时：26276567700 纳秒, 26276 毫秒
    steady_clock::time_point begin = steady_clock::now();
    op1(0, 5000);
    steady_clock::time_point end = steady_clock::now();
    auto length = end - begin; // 计算时间差，得到操作耗时
    cout << "非连接池，单线程，用时：" << length.count() << " 纳秒,"
         << length.count() / 1000000 << " 毫秒" << endl;
#else
    // 连接池，单线程，用时：6100671100 纳秒, 6100 毫秒
    ConnPool *pool = ConnPool::getConnPool();
    steady_clock::time_point begin = steady_clock::now();
    op2(pool, 0, 5000);
    steady_clock::time_point end = steady_clock::now();
    auto length = end - begin; // 计算时间差，得到操作耗时
    cout << "连接池，单线程，用时：" << length.count() << " 纳秒,"
         << length.count() / 1000000 << " 毫秒" << endl;
#endif
}

void test2()
{
#if 0
    // 非连接池，多线程，用时：110655335417 纳秒,110655 毫秒
    MysqlConn conn;
    if (!conn.connect("root", "123456789", "mydb_test", "localhost", 3306))
    {
        cout << "连接错误，检查你的账号密码" << endl;
        exit(-1);
    }
    steady_clock::time_point begin = steady_clock::now();
    thread t1(op1, 0, 1000);
    thread t2(op1, 1000, 2000);
    thread t3(op1, 2000, 3000);
    thread t4(op1, 3000, 4000);
    thread t5(op1, 4000, 5000);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    steady_clock::time_point end = steady_clock::now();
    auto length = end - begin; // 计算时间差，得到操作耗时
    cout << "非连接池，多线程，用时：" << length.count() << " 纳秒,"
        << length.count() / 1000000 << " 毫秒" << endl;
#else
    // 连接池，多线程，用时：3644675505 纳秒,3644 毫秒
    ConnPool *pool = ConnPool::getConnPool();
    steady_clock::time_point begin = steady_clock::now();
    // 多线程模拟多个客户端，实现多并发操作
    thread t1(op2, pool, 0, 1000);
    thread t2(op2, pool, 1000, 2000);
    thread t3(op2, pool, 2000, 3000);
    thread t4(op2, pool, 3000, 4000);
    thread t5(op2, pool, 4000, 5000);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    steady_clock::time_point end = steady_clock::now();
    auto length = end - begin; // 计算时间差，得到操作耗时
    cout << "连接池，多线程，用时：" << length.count() << " 纳秒,"
         << length.count() / 1000000 << " 毫秒" << endl;
#endif
}

int query()
{
    MysqlConn conn;
    //conn.connect("root", "123456789", "mydb_test", "localhost", 3306);
    if (!conn.connect("root", "123456789", "mydb_test", "localhost", 3306))
    {
        cout << "连接错误，检查你的账号密码" << endl;
        return -1;
    }
    string sql = "insert into person (id, age, sex, name) values(6,35,'女','乌拉那拉皇后')";
    bool flag = conn.update(sql);
    if (flag)
        cout << "插入数据成功了！！！" << endl;
    else
        cout << "插入数据失败,耐心检查一下哪里有出差错喔！" << endl;

    sql = "select * from person";
    conn.query(sql);
    while (conn.next())
    {
        cout << conn.value(0) << ", "
             << conn.value(1) << ", "
             << conn.value(2) << ", "
             << conn.value(3) << endl;
    }
    return 0;
}

int main()
{
    // SetConsoleOutputCP(CP_UTF8);
    // query();
    // test1();
    // 测试前，删除之前插入的脏数据
    MysqlConn conn;
    conn.connect("root", "123456789", "mydb_test", "localhost", 3306);
    conn.update("TRUNCATE TABLE person");

    test2();
    
    
    return 0;
}