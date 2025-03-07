#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "c_blue.tab.h"
#define MAXLENGTH   200
#define DX 3*sizeof(int)          /*活动记录控制信息需要的单元数，这个根据实际系统调整*/
//以下语法树结点类型、三地址结点类型等定义仅供参考，实验时一定要根据自己的理解来定义
int LEV;      //层号

struct Node {
//以下对结点属性定义没有考虑存储效率，只是简单地列出要用到的一些属性
    int kind;
    union {
        int type_int;
        float type_float;
        char type_char[32];
        char type_id[32];
    };
    struct Node *ptr[4];         //由kind确定有多少棵子树
    int place;                     //存放（临时）变量在符号表的位置序号
    char Etrue[15],Efalse[15];       //对布尔表达式的翻译时，真假转移目标的标号
    char Snext[15];               //结点对应语句S执行后的下一条语句位置标号
    struct codenode *code;          //该结点中间代码链表头指针
    int type;                      //用以标识表达式结点的类型
    int pos;                       //语法单位所在位置行号
    int offset;                     //偏移量
    int width;                     //占数据字节数
    int num;                      //计数器，可以用来统计形参个数
};


struct Node * mknode(int num,int kind,int pos,...);
void semantic_error(int line, char *msg1, char *msg2);