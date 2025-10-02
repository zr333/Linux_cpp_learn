#include <stdio.h>
#include <string.h>

int main() {
    char str[50] = "Hello";
    strcat(str, " World");
    printf("%s\n", str);
}