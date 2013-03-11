#include <stdio.h>
#include <string.h>
#include "stdlib.h"
#include <string>
#include <vector>


// 从头开始匹配
extern void match_begin(const char*content, char*key, char*val);

// 匹配所有的串
extern void match_all(const char*content, std::vector<std::string>*keys, std::vector<std::string>*vals);

// 初始化，在加载词典之前请务必调用此函数
extern int init();

// 加载词典 flag = 0:text形式读入  flag = 1:二进制形式读入
extern int  load_trie_dict(const char*dict_file, int flag = 0);

// 查找key的值，如果存在，返回写到val里
extern void get_val(const char*key, char*val);


// 打印整个trie树
extern void print_trie();

// 保存trie到二进制格式
extern int save_trie();

