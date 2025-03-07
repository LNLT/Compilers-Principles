#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "c_blue.tab.h"
#define MAXLENGTH   200
#define DX 3*sizeof(int)          /*活动记录控制信息需要的单元数，这个根据实际系统调整*/
//以下语法树结点类型、三地址结点类型等定义仅供参考，实验时一定要根据自己的理解来定义

struct Node {
        //以下对结点属性定义没有考虑存储效率，只是简单地列出要用到的一些属性
	int kind;
	union {
		char type_id[33];             //由标识符生成的叶结点
        char case_id[33];
		int type_int;                 //由整常数生成的叶结点
		float type_float;               //由浮点常数生成的叶结点
        char type_char[5];
        char type_string[32];
        char struct_name[32];
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

struct symbol {       //这里只列出了一个符号表项的部分属性，没考虑属性间的互斥
    char name[33];   //变量或函数名
    int level;        //层号
    int type;         //变量类型或函数返回值类型
    int  paramnum;  //对函数适用，记录形式参数个数
    char alias[10];   //别名，为解决嵌套层次使用
    char flag;       //符号标记，函数：'F'  变量：'V'   参数：'P'  临时变量：'T'
    char offset;      //外部变量和局部变量在其静态数据区或活动记录中的偏移量，
    char struct_name[33];
    int array[30];
    int const_int;
        //或记录函数活动记录大小，目标代码生成时使用
    //函数入口等实验可能会用到的属性...
};
//符号表
struct symboltable{
    struct symbol symbols[MAXLENGTH];
    int index;
};

struct symbol_scope_begin {
    //当前作用域的符号在符号表的起始位置序号,这是一个栈结构,当使用顺序表作为符号表时，进入、退出一个作用域时需要对其操作，以完成符号表的管理。对其它形式的符号表，不一定需要此数据结构
    int TX[30];
    int top;
};

struct opn
{
    int kind; //标识操作的类型
    int type; //标识操作数的类型
    union {
        int const_int;     //整常数值，立即数
        float const_float; //浮点常数值，立即数
        char const_char;   //字符常数值，立即数
        char *const_string;
        char id[33]; //变量或临时变量的别名或标号字符串
        struct Array *type_array;
        struct Struct *type_struct;
    };
    int level;  //变量的层号，0表示是全局变量，数据保存在静态数据区
    int offset; //变量单元偏移量，或函数在符号表的定义位置序号，目标代码生成时用
};

struct codenode
{                                  //三地址TAC代码结点,采用双向循环链表存放中间语言代码
    int op;                        //TAC代码的运算符种类
    struct opn opn1, opn2, result; //2个操作数和运算结果
    struct codenode *next, *prior;
};

union Value {
    char type_id[33]; //由标识符生成的叶结点
    int type_int;     //由整常数生成的叶结点
    float type_float; //由浮点常数生成的叶结点
    char type_char;
    char type_string[32];
    struct Array *arrayPtr;
};

// 使用链表存储多个变量
struct Array
{
    int kind;
    union Value value;
    int index;
    struct Array *next;
};

// 使用链表存储多个变量
struct Struct
{
    int kind;
    char *name; // 字段名字
    union Value value;
    struct Struct *next;
};


struct Node * mknode(int num,int kind,int pos,...);


// semantic_Analysic
void semantic_Analysis(struct Node *T);
void semantic_Analysis0(struct Node *T);
int searchSymbolTable(char *name);
void semantic_error(int line,char *msg1,char *msg2);
void prn_symbol();
void DisplaySymbolTable();
int fillSymbolTable(char *name, char *alias, int level, int type, char flag, int offset);
int fill_Temp(char *name, int level, int type, char flag, int offset);
int match_param(int i, struct Node *T);
int compute_width(struct Node *T);
int compute_arraywidth(int *array,int index);
int compute_width0(struct Node *T,int *array,int index);
int array_out(struct Node *T,int *a,int index);

// semantic_case
void ext_var_list(struct Node *T);
void ext_def_list(struct Node *T);
void ext_var_def(struct Node *T);
void func_def(struct Node *T);
void func_dec(struct Node *T);
void ext_struct_def(struct Node *T);
void struct_def(struct Node *T);
void struct_dec(struct Node *T);
void array_dec(struct Node *T);
void param_list(struct Node *T);
void param_dec(struct Node *T);
void comp_stm(struct Node *T);
void def_list(struct Node *T);
void var_def(struct Node *T);
void stm_list(struct Node *T);
void if_then(struct Node *T);
void if_then_else(struct Node *T);
void while_dec(struct Node *T);
void for_stmt(struct Node *T);
void break_dec(struct Node *T);
void continue_dec(struct Node *T);
void exp_stmt(struct Node *T);
void return_dec(struct Node *T);
void default_stmt(struct Node *T);

// exp
void Exp(struct Node *T);
void boolExp(struct Node *T);
void id_exp(struct Node *T);
void int_exp(struct Node *T);
void float_exp(struct Node *T);
void char_exp(struct Node *T);
void string_exp(struct Node *T);
void assignop_exp(struct Node *T);
void other_assignop_exp(struct Node *T);
void auto_op_exp(struct Node *T);
void relop_exp(struct Node *T);
void args_exp(struct Node *T);
void op_exp(struct Node *T);
void func_call_exp(struct Node *T);
void not_exp(struct Node *T);
void ext_var_list(struct Node *T);
void ext_def_list(struct Node *T);
void ext_var_def(struct Node *T);
void func_def(struct Node *T);
void func_dec(struct Node *T);
void param_list(struct Node *T);
void param_dec(struct Node *T);
void comp_stm(struct Node *T);
void def_list(struct Node *T);
void var_def(struct Node *T);
void stmt_list(struct Node *T);
void if_then(struct Node *T);
void if_then_else(struct Node *T);
void while_dec(struct Node *T);
void exp_stmt(struct Node *T);
void return_dec(struct Node *T);
void exp_array(struct Node *T);
void exp_ele(struct Node *T);

// code
char *str_catch(char *s1, char *s2);
char *newAlias();
char *newLabel();
char *newTemp();
struct codenode *genIR(int op, struct opn opn1, struct opn opn2, struct opn result);
struct codenode *genLabel(char *label);
struct codenode *genGoto(char *label);
struct codenode *merge(int num, ...);
void prnIR(struct codenode *head);