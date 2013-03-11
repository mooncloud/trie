#include <trie.h>

int TEST_1(int argc, char**args) {
  
  int trie_flag = 1; // 二进制读入
  int m_rev = load_trie_dict(args[1], trie_flag);
  if ( 0 != m_rev ) {
    return -1;
  }
  
  m_rev = save_trie();
  if ( 0 != m_rev ) {
    return -1;
  } 

  print_trie();
  
  return 0;
  
}

int TEST_2(int argc, char**args) {
  load_trie_dict(args[1]);
  char key[1024] = {0};
  char val[1024] = {0};
  char line[1024] = {0};
  std::vector<std::string>keys;
  std::vector<std::string>vals;
  FILE* fp = fopen("test.key", "r");
  while (EOF != fscanf(fp, "%s\n", key)) {
    printf("\n\n******************************\n");
    get_val(key, val);
    if ( 0 != val[0] ) {
      printf("%s\t%s\n", key, val);
    }
    val[0] = 0;
    match_begin(key, line, val);
    printf("******\n");
    if ( 0 != val[0] ) {
      printf("%s\t%s\n", line, val);
    }
    val[0] = 0;
    match_all(key, &keys, &vals);
    printf("******\n");
    for ( size_t i = 0; i < keys.size(); ++i ) {
      printf("%s\t%s\n", keys[i].c_str(), vals[i].c_str());
    }
    printf("\n");
  }
  return 0;
}

int main(int argc, char**args) {
  int rev = init();
  if (0 != rev) {
    printf("init failed\n");
    return 1;
  }

  TEST_1(argc, args);
  TEST_2(argc, args);
  
  return 0;
}
