#include <trie.h>

int main(int argc, char**args) {
  int rev = init();
  if (0 != rev) {
    printf("init failed\n");
    return 1;
  }
  
  load_trie_dict(args[1]);
  save_trie();
  print_trie();
  /*char* test = "marquis";
  char key[1024] = {0};
  char val[1024] = {0};
  char line[1024] = {0};
  std::vector<std::string>keys;
  std::vector<std::string>vals;
  FILE* fp = fopen("1.test", "r");
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
    for ( int i = 0; i < keys.size(); ++i ) {
      printf("%s\t%s\n", keys[i].c_str(), vals[i].c_str());
    }
    printf("\n");

  }
*/
  return 0;
}
