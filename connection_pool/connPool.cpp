#include "connPool.h"
#include <jsoncpp/json/json.h>
#include <fstream>
#include <thread>
#include <iostream>
using namespace Json;

// 使用静态局部变量的方式实现单例模式：静态局部变量初始化一次
// 线程安全：C++11保证函数内部的局部静态变量初始化在多线程环境下也是安全的。
ConnPool *ConnPool::getConnPool()
{
    static ConnPool pool;
    return &pool;
}

// 从连接池中取出一个连接
shared_ptr<MysqlConn> ConnPool::getConn()
{
    unique_lock<mutex> locker(m_mutexQ);
    while (m_connQ.empty())
    {
        // 如果没有空闲连接，但总数小于最大值，则扩容
        if (m_connQ.size() + 1 <= m_maxSize)
        {
            addConn();
        }
        else
        {
            // 如果连接池中的连接已经用完，就等待一段时间，直到有其他线程把连接放回来。
            // 如果超时，返回cv_status::timeout并再次判断队列是否为空
            // 如果队列仍为空（m_connQ.empty()），则继续循环等待（continue）。
            // 如果队列非空（其他线程放入了资源），则退出循环
            if (cv_status::timeout == m_cond.wait_for(locker, chrono::milliseconds(m_timeout)))
            {
                if (m_connQ.empty())
                {
                    //return nullptr;
                    continue;
                }
            }
        }
    }

    /*
    获取连接并在处理完后返回该连接到连接池队列

    当对应的线程把连接conn拿走之后，操作完了。数据库需要把这个连接还回来，如何实现？
    可以使用智能指针实现：由于当共享指针对象析构的时候，不需要把智能指针管理的连接（地址）析构，而是回收。因此可以手动去指定这个共享的智能指针它的删除器对应的处理动作。
    实现机理：
        自定义删除器 [this](MysqlConn *conn) { ... }
            - shared_ptr 可以指定自定义删除器，在 shared_ptr 销毁时执行。
            - 这里传入的是一个 lambda，它捕获 this（即当前连接池对象），确保在回收时可以访问 m_connQ 和 m_mutexQ。
        执行时机： 当 connptr 离开作用域（或引用计数为 0）时，这个 lambda 会被调用。

    在匿名函数里边，并不是真正的要销毁这个指针指向的那块内存，而是要把它放到数据库连接池对应的队列里边。
    */
    shared_ptr<MysqlConn> connptr(m_connQ.front(), [this](MysqlConn *conn)
                                  {
                                      lock_guard<mutex> locker(m_mutexQ); // 自动管理加锁和解锁
                                      conn->refreshAliveTime();           // 更新连接的起始的空闲时间点
                                      this->m_connQ.push(conn);                 // 回收数据库连接，此时它再次处于空闲状态
                                  });                                     // 智能指针
    m_connQ.pop();
    
    m_cond.notify_one(); // 本意是唤醒生产者
    return connptr;
}

ConnPool::~ConnPool()
{
    while (!m_connQ.empty())
    {
        MysqlConn *conn = m_connQ.front();
        m_connQ.pop();
        delete conn;
    }
}

ConnPool::ConnPool()
{
    // 加载配置文件
    if (!parseJsonFile())
    {
        std::cout << "加载配置文件失败！！！" << std::endl;
        return;
    }
    for (int i = 0; i < m_minSize; ++i)
    {
        addConn();
    }
    thread producer(&ConnPool::produceConn, this); // 生产连接线程
    thread recycler(&ConnPool::recycleConn, this); // 销毁连接线程
    producer.detach();
    recycler.detach();
}

bool ConnPool::parseJsonFile()
{
    ifstream ifs("dbconf.json");
    Reader rd;
    Value root;
    rd.parse(ifs, root);
    if (root.isObject())
    {
        std::cout << "开始解析配置文件..." << std::endl;
        m_ip = root["ip"].asString();
        m_port = root["port"].asInt();
        m_user = root["userName"].asString();
        m_passwd = root["password"].asString();
        m_dbName = root["dbName"].asString();
        m_minSize = root["minSize"].asInt();
        m_maxSize = root["maxSize"].asInt();
        m_maxIdleTime = root["maxIdleTime"].asInt();
        m_timeout = root["timeout"].asInt();
        std::cout << "配置文件解析成功！" << std::endl;
        return true; // 解析成功返回true，否则返回false。
    }

    return false;
}

void ConnPool::produceConn()
{
    while (true)
    {                                        // 生产者线程不断生产连接，直到连接池达到最大值
        unique_lock<mutex> locker(m_mutexQ); // 加锁，保证线程安全
        while (m_connQ.size() >= m_minSize)
        {
            m_cond.wait(locker); // 等待消费者通知
        }
        addConn();           // 生产连接
        m_cond.notify_all(); // 通知消费者(唤醒)
    }
}

// 回收数据库连接
void ConnPool::recycleConn()
{
    while (true)
    {
        this_thread::sleep_for(chrono::milliseconds(500)); // 每隔半秒钟检测一次
        lock_guard<mutex> locker(m_mutexQ);                // 加锁，保证线程安全
        while (m_connQ.size() > m_minSize)
        {                                      // 如果连接池中的连接数大于最小连接数，则回收连接
            MysqlConn *conn = m_connQ.front(); // 取出连接池中的连接
            if (conn->getAliveTime() >= m_maxIdleTime)
            {
                m_connQ.pop(); // 回收连接
                delete conn;   // 释放连接资源
            }
            else
            {
                break; // 如果连接的空闲时间小于最大空闲时间，则跳出循环
            }
        }
    }
}

// 添加连接到连接池
void ConnPool::addConn()
{
    MysqlConn *conn = new MysqlConn;
    conn->connect(m_user, m_passwd, m_dbName, m_ip, m_port);
    conn->refreshAliveTime(); // 记录建立连接的时候的对应的时间戳
    m_connQ.push(conn);
}