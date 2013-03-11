// Copyright [2013] <Copyright wangye>

#include <trie.h>
#include <string>
#include <vector>

#ifndef DEBUG
// #define DEBUG
#endif

struct trie* p_dict;
const int kMaxValLen   = 32;
const int kMaxKeyLen   = 32;
int g_total_node_num  = 0;
int g_delete_node_num = 0;
int g_save_node_num   = 0;
int g_read_node_num   = 0;

struct trie {
  unsigned char data;        // 字母的ascall码
  unsigned char child_num;   // 存储当前节点的孩子数量，为把链表转换成数组存储，做准备
  bool is_end;               // 当前位置是不是一个key的结束位置
  char key[kMaxKeyLen];
  char val[kMaxValLen];
  trie*p_brother;
  trie*p_child;
};

// 深度优先遍历整个trie树
void dfs_traverse(trie*root);

// 改变存储格式，有链表，改成数组+链表，便于在查找的时候使用
// 二分查找，其实是一种B+树结构
int  shift_storage_format();

// 递归的从二进制文件中读入trie树，要用到文件定位函数fseek
int recursion_read_binary_trie(trie*root, FILE*fp) {
  if ( NULL == root ) {
    return -1;
  }
  int m_child_num = root->child_num;
  root->p_child = new trie[m_child_num];
  memset(root->p_child, 0, sizeof(trie)*m_child_num);
  int m_read_num = fread(root->p_child, sizeof(trie), m_child_num, fp);
  if ( m_read_num != m_child_num ) {
    return -1;
  }
  g_read_node_num += m_read_num;
  int m_seek_suc = fseek(fp, g_read_node_num*sizeof(trie), SEEK_SET);
  if ( 0 != m_seek_suc ) {
    return -1;
  } 
  int m_ret = 0;
  for ( int i = 0; i < m_child_num; ++i ) {
    m_ret =  recursion_read_binary_trie(root->p_child+i, fp);
    if ( 0 != m_ret ) {
      break;
    }
  }
  return m_ret;
}

// 读入二进制保存的trie树
int load_binary_trie_model(const char*binary_trie_file) {
  if ( NULL == binary_trie_file ) {
    return -1;
  }
  FILE*fp = fopen(binary_trie_file, "r");
  if ( NULL == fp ) {
    fprintf(stderr, "[open %s failed]\n", fp);
    fclose(fp);
    return -1;
  }
  int m_read_num = fread(p_dict, sizeof(trie), 1, fp);
  if ( m_read_num != 1 ) {
    fclose(fp);
    return -1;
  }
  g_read_node_num++;
  int m_seek_suc = fseek(fp, g_read_node_num*sizeof(trie), SEEK_SET);
  if ( 0 != m_seek_suc ) {
    return -1;
  } 
  int suc = recursion_read_binary_trie(p_dict, fp);
  fclose(fp);
#ifdef DEBUG
  fprintf(stderr, "g_read_node_num:%d\n", g_read_node_num);
#endif

  return suc;
}

// 递归的删除整个链表，在shift_storage_format之后调用
void unload_trie_list(trie*root) {
  if ( root == NULL ) {
    return;
  }
  unload_trie_list(root->p_child);
  unload_trie_list(root->p_brother);
  delete root;
  g_delete_node_num++;
  root = NULL;
  return;
}

// 以文本形式读入数据，数据格式是key\tval
int load_text_trie_model(const char*text_trie_file) {
  FILE* fp = fopen(text_trie_file, "r");
  if (NULL == fp) {
    fprintf(stderr, "[open %s failed]\n", text_trie_file);
    return -1;
  }
  char m_key[1024] = {0};
  char m_val[1024] = {0};
  char m_line[10240] = {0};
  int m_rev = 0;
  while ( NULL != fgets(m_line, 10240, fp) ) {
    sscanf(m_line, "%s\t%s", m_key, m_val);
    m_rev = insert_trie_dict(m_key, m_val);
#ifdef DEBUG
    printf("key = %s\tval = %s\n", key, val);
#endif
    if (0 != m_rev) {
      fprintf(stderr, "LINE_NUM:%d [error_num:%d]\n", __LINE__, m_rev);
      break;
    }
  }
  fclose(fp);

  return shift_storage_format();
}

void shift_list_to_array(trie*p_dest, trie*psrc) {
  if ( NULL == p_dest || NULL == psrc ) {
    return;
  }
  int m_child_num = psrc->child_num;
  trie*p_child   = psrc->p_child;
  p_dest->p_child = new trie[m_child_num];
  for ( int i = 0; i < m_child_num; ++i ) {
    memcpy(p_dest->p_child+i, p_child, sizeof(trie));
    shift_list_to_array(p_dest->p_child+i, p_child);
    (p_dest->p_child+i)->p_brother = NULL;
    p_child = p_child->p_brother;
  }
  return;
}

int shift_storage_format() {
  if (NULL == p_dict) {
    return -1;
  }
  trie*p_dest = new trie;
  memcpy(p_dest, p_dict, sizeof(trie));
  shift_list_to_array(p_dest, p_dict);
  unload_trie_list(p_dict);
#ifdef DEBUG
  fprintf(stderr, "g_delete_node_num:%d\n", g_delete_node_num); 
#endif
  p_dict = p_dest;
  return 0;
}

int save_to_binary(trie*p_child, int m_child_num, FILE*fp) {
  if (NULL == p_child || 0 >= m_child_num) {
    return 0;
  }
  g_save_node_num += m_child_num;
  int m_write_num = fwrite(p_child, sizeof(trie), m_child_num, fp);
  if ( m_write_num != m_child_num ) {
    return -1;
  }
  int m_rev = 0;
  for (int i = 0; i < m_child_num; ++i) {
    m_rev = save_to_binary(p_child[i].p_child, p_child[i].child_num, fp);
    if ( 0 != m_rev ) {
      break;
    }
  }
  return m_rev;
}

int save_trie() {
  if (NULL == p_dict) {
    return -1;
  }
  FILE* fp = fopen("_trie_model", "w");
  fwrite(p_dict, sizeof(trie), 1, fp);
  g_save_node_num++;
  int m_rev = save_to_binary(p_dict->p_child, p_dict->child_num, fp);
  fclose(fp);
#ifdef DEBUG
  fprintf(stderr, "g_save_node_num:%d\n", g_save_node_num);
#endif
  return m_rev; 
}

int binary_search(trie*cur, int size, const unsigned char data) {
  if (NULL == cur) {
    return -1;
  }
  int m_start = 0;
  int m_end = size;
  int m_mid = m_start + (m_end-m_start)/2;
  while (m_start <= m_end) {
    if (data == cur[m_mid].data) {
      return m_mid;
    }
    if (data < cur[m_mid].data) {
      m_end = m_mid-1;
    } else {
      m_start = m_mid+1;
    }
    m_mid = m_start + (m_end-m_start)/2;
  }
  return -1;
}

void match_all(const char*content, std::vector<std::string>*keys, std::vector<std::string>*vals) {
  if (NULL == content) {
    return;
  }
  keys->clear();
  vals->clear();
  int m_size = strlen(content);
  int i = 0;
  char m_key[1024] = {0};
  char m_val[1024] = {0};
  for (i = 0; i < m_size;) {
    match_begin(content+i, m_key, m_val);
    if (0 == m_key[0]) {
      i++;
      continue;
    }
    keys->push_back(m_key);
    vals->push_back(m_val);
    i += strlen(m_key);
  }
  return;
}

// 从头匹配
void match_begin(const char*content, char*key, char*val) {
  if (NULL == content) {
    return;
  } 
  key[0] = 0;
  val[0] = 0;
  trie* cur = p_dict;
  int size = strlen(content);
  int i = 0;
  unsigned char data = 0;
  int id = -1;
  for ( i = 0; i < size; ++i ) {
    data = (unsigned char)content[i];
    id = binary_search(cur->p_child, cur->child_num, data);
    if (-1 == id) {
      break;
    }
    cur = cur->p_child+id;
  }
  if (cur->is_end) {
    snprintf(val, sizeof(cur->val), "%s", cur->val);
    snprintf(key, sizeof(cur->key), "%s", cur->key);
  }
  return;
}


void dfs_traverse(trie*root) {
  if (NULL == root) {
    return;
  }
  int child_num = root->child_num;
  trie* p_child = root->p_child;
  for (int i = 0; i < child_num; ++i) {
      dfs_traverse(p_child+i);
      if (p_child[i].is_end) {
        printf("%s\t%s\n", p_child[i].key, p_child[i].val);
    }
  }
  g_total_node_num += child_num;
  return;
}

void print_trie() {
  if (NULL == p_dict) {
    return;
  }
  dfs_traverse(p_dict);
  g_total_node_num++;
#ifdef DEBUG
  fprintf(stderr, "g_total_node_num:%d\n", g_total_node_num);
#endif
}

void get_val(const char*key, char*val) {
  if (NULL == key) {
    val[0] = 0;
    return;
  }
  char m_key[1024] = {0};
  char m_val[1024] = {0};
  match_begin(key, m_key, m_val);
  if (0 != strcmp(key, m_key)) {
    val[0] = 0;
    return;
  }
  snprintf(val, sizeof(m_val), "%s", m_val);
  
  return;
}


int insert_trie_dict(char* key, char*val) {
  if (NULL == key || NULL == val) {
    return -1;
  }
  trie*cur = p_dict;
  int size = strlen(key);
  unsigned char data = 0;
  for ( int i = 0; i < size; ++i ) {
    data = (unsigned char)key[i];
    trie* p_brother_pre = cur;
    trie* p_brother_cur = cur->p_child;
    if (NULL == cur->p_child) {
      trie* m_trie = new trie;
      m_trie->data = data;
      cur->p_child = m_trie;
      cur->child_num++;
      cur = m_trie;
      continue;
    }
    for ( int j = 0; j < cur->child_num; ++j ) {
      if (j < cur->child_num && p_brother_cur == NULL) {
        printf("line_num:%d\t %d\t %d\n", __LINE__, j, cur->child_num);
        return -1;
      }
      if (p_brother_cur-> data == data) {
        cur = p_brother_cur;
        break;
      } else if ( p_brother_cur->data < data ) {
        p_brother_pre = p_brother_cur; 
        p_brother_cur = p_brother_cur->p_brother;
      } else if ( p_brother_cur-> data > data ) {
        trie* m_trie = new trie;
        memset(m_trie, 0, sizeof(trie));
        m_trie->data = data;
        if (cur != p_brother_pre) {
          p_brother_pre->p_brother = m_trie;
          m_trie->p_brother = p_brother_cur;
          p_brother_pre = p_brother_pre->p_brother;
        } else {
          cur->p_child = m_trie;
          m_trie->p_brother = p_brother_cur;
          p_brother_pre = m_trie;
        }
        cur->child_num++;
        cur = m_trie;
        break;        
      }
    }
    if (NULL == p_brother_cur) {
      trie* m_trie = new trie;
      m_trie->data = data;
      p_brother_pre->p_brother = m_trie;
      cur->child_num++;
      cur = m_trie;
    }
  }
  
  if (NULL != cur) {
    if (cur->is_end) {
      fprintf(stderr, "[ %s already in ]\n", key);
      return 0;
    }
    snprintf(cur->val, sizeof(cur->val), "%s", val);
    snprintf(cur->key, sizeof(cur->key), "%s", key);
    cur->is_end = true;
  } 
  return 0;
}

// 默认情况下是以文本的格式读入词典
int load_trie_dict(const char*dict_file, int flag) {
  if (NULL == dict_file) {
    return -1;
  }
  int m_ret = 0;
  switch (flag) {
    case 0: {
            m_ret = load_text_trie_model(dict_file);
            break;
            }
    case 1: {
            m_ret = load_binary_trie_model(dict_file);
            break;
            }
    default:
           break;
  }
  return m_ret;
}


int init() {
  p_dict = new trie;
  if (NULL == p_dict) {
    return -1;
  }
  memset(p_dict, 0, sizeof(p_dict[0]));
  return 0;
}
