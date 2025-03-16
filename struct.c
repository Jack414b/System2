#include <stdio.h>

struct A {
    char a; //1字节
    int b; //4字节
};

#pragma pack(push, 1) //设置对齐为1字节
struct B {
    char a; //1字节
    int b; //4字节
};
#pragma pack(pop) //恢复默认对齐


int main() {
    struct A x;
    printf("%d\n",sizeof(x)); //输出为8字节
    printf("%d\n",sizeof(x.a)); //输出为1字节
    printf("%d\n",sizeof(x.b)); //输出为4字节
    

/*
假设结构体的起始地址位0x00：
对齐前：a在0x00位置，占用1字节；b在0x01位置，占用4字节。
对齐后：a在0x00位置，占用1字节；接下来是3个填充字节（0x01，0x02，0x03）b在0x04位置，占用4字节。

假设32位cpu，所以每次访问数据我们都同时读取32位（8字节）。
假设需要访问x.b；
对齐前我们需要先访问0x00-0x03得到b的前三个字节，然后再访问0x04-0x07得到b的第四个字节，进行了两次访问；
对齐后我们只需要直接访问0x04-0x07四个字节，只需要进行一次数据访问。

显然使整个结构体大小为4字节的倍数，以部分空间为代价得到了访问效率的提升。
*/

    struct B y;
    printf("%d\n",sizeof(y));//输出为5字节
    printf("%d\n",sizeof(y.a));//输出为1字节
    printf("%d\n",sizeof(y.b));//输出为4字节

/*
我们使用#pragma pack指令来设置结构体的对齐方式。
*/

    return 0;
}



