/*
工厂模式（Factory Pattern）:
最常用的设计模式之一，它提供了一种创建对象的方式，使得创建对象的过程与使用对象的过程分离。

工厂模式提供了一种创建对象的方式，而无需指定要创建的具体类。

通过使用工厂模式，可以将对象的创建逻辑封装在一个工厂类中，而不是在客户端代码中直接实例化对象，这样可以提高代码的可维护性和可扩展性。

在工厂方法模式中，核心角色包括：
抽象产品（Product）：定义了产品对象的接口；
具体产品（Concrete Product）：实现抽象产品接口的具体类；
抽象工厂（Creator）：声明了工厂方法，该方法返回一个抽象产品；
具体工厂（Concrete Creator）：重写工厂方法以返回一个具体产品实例。


简单工厂：一个工厂类，根据条件返回不同产品（缺点：违反开闭原则）。
工厂方法：把工厂抽象成接口，每个产品对应一个具体工厂（扩展性更好）
*/

#include <iostream>
#include <memory>
class Logger {
public:
    virtual void log(const std::string &msg) = 0;
    virtual ~Logger() = default;
};

class ConsoleLogger : public Logger {
public:
    virtual void log(const std::string &msg) override { std::cout << "[Console] " << msg << std::endl; }
};

class FileLogger : public Logger {
public:
    virtual void log(const std::string &msg) override { std::cout << "[File] " << msg << std::endl; }
};

// 简单工程模式
class LoggerFactory {
public:
    static Logger*  createLogger(const std::string &type) {
        if (type == "console")
            return new ConsoleLogger();
        else if (type == "file")
            return new FileLogger();
        else
            return nullptr;
    }
};

// 工厂方法
class Factory {
public:
    virtual Logger* createLogger() = 0;
    virtual ~Factory() = default;
};

class ConsoleLoggerFactory : public Factory {
public:
    ConsoleLogger *createLogger() override { return new ConsoleLogger(); }
};

class FileLoggerFactory : public Factory {
public:
    FileLogger* createLogger() override { return new FileLogger(); }
};

int main() {

    // 简单工厂
    Logger* conlog = LoggerFactory::createLogger("console");
    conlog->log("consoleLog.");
    Logger *fileLog = LoggerFactory::createLogger("file");
    fileLog->log("fileLog.");
    delete conlog;
    delete fileLog;

    // 工厂方法
    Factory *factory = new ConsoleLoggerFactory();
    auto logger = factory->createLogger();
    logger->log("consoleLog");
    delete factory;
    delete logger;
}