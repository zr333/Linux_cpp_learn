#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>

int main()
{
    // 1. 创建根对象
    cJSON *root = cJSON_CreateObject();

    // 2. 添加键值对
    cJSON_AddItemToObject(root, "ip", cJSON_CreateString("127.0.0.1"));
    cJSON_AddItemToObject(root, "port", cJSON_CreateNumber(3306));
    cJSON_AddItemToObject(root, "userName", cJSON_CreateString("root"));
    cJSON_AddItemToObject(root, "password", cJSON_CreateString("123456789"));
    cJSON_AddItemToObject(root, "dbName", cJSON_CreateString("mydb_test"));

    // 3. 转换成字符串
    char *json_str = cJSON_Print(root); // cJSON_PrintUnformatted(root) 可生成紧凑版
    printf("%s\n", json_str);

   
    // 5. 释放内存
    free(json_str);     // 释放 cJSON_Print 生成的字符串
    cJSON_Delete(root); // 释放 JSON 对象树

    return 0;
}
