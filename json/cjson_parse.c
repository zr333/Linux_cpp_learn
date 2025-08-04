#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>

/*
cJSON的基本使用
*/

// 解释json：先把文件内容读到字符串，然后用 cJSON_Parse 解析
char *read_file(const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp)
        return NULL;

    fseek(fp, 0, SEEK_END); // 把文件指针移动到文件的末尾
    long size = ftell(fp);  // 返回当前文件指针相对于文件开头的偏移量（以字节为单位），即文件大小
    rewind(fp);             // 把文件指针重新移动到文件开头

    char *buffer = (char *)malloc(size + 1);
    fread(buffer, 1, size, fp);
    buffer[size] = '\0';

    fclose(fp);
    return buffer;
}
int main()
{
    // 1. 读取文件
    char *json_data = read_file("dbconf.json");
    if (!json_data)
    {
        printf("Failed to read file!\n");
        return -1;
    }
    // 2. 解析 JSON
    cJSON *root = cJSON_Parse(json_data);
    if (!root)
    {
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
        free(json_data);
        return -1;
    }

    // 3. 获取字段
    cJSON *ip = cJSON_GetObjectItem(root, "ip");
    cJSON *port = cJSON_GetObjectItem(root, "port");
    cJSON *user = cJSON_GetObjectItem(root, "userName");
    cJSON *pwd = cJSON_GetObjectItem(root, "password");
    cJSON *db = cJSON_GetObjectItem(root, "dbName");

    // 4. 打印
    printf("IP: %s\n", ip->valuestring);
    printf("Port: %d\n", port->valueint);
    printf("User: %s\n", user->valuestring);
    printf("Password: %s\n", pwd->valuestring);
    printf("Database: %s\n", db->valuestring);

    // 5. 释放内存
    cJSON_Delete(root);
    free(json_data);

    return 0;
}
