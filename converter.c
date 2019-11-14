#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define OR 0
#define AND 1
#define NOT 2
#define INTEGER 3

// 괄호의 개수를 먼저 만든 다음에 list에 malloc을 해주고 거기에 집어 넣어주면 될 듯..
// and, or 안에 있는 것들 중에서 괄호가 나온다면 무시하고 다음 닫힘 괄호가 나올 때까지 확인해 준다음 개수를 구하면 될 것 같다.
struct Node{
  int integer;
  int type;
  int size;
  struct Node** list;
};
typedef struct Node node;

node* getTree (char string[], node* fir_node, int* start, int block_size);
void getString (char string[], char word[]);
void printTree (node* fir_node, int not);
node* NNFconverter (node* fir_node, int not_true);
node* CNFconverter (node* fir_node);
int checkSize (char string[]);
void printTree1(node* fir_node);

int main () {
  char string[10000];
  printf("Input: ");
  //gets(string);
  fgets(string, sizeof(string)/sizeof(char), stdin);
  node* fir_node = (node*)malloc(sizeof(node));
  int start = 0;

  fir_node = getTree (string, fir_node, &start, 0);
  fir_node = NNFconverter (fir_node, 0);
  fir_node = CNFconverter (fir_node);
  printTree(fir_node, 0);
  printf("\n");

  return 0;
}

node* getTree (char string[], node* fir_node, int* start, int block_size){
  if (*start >= strlen(string))
    return NULL;
  char word[100];
  getString(string+*start, word);
  *start += string[*start+strlen(word)]==' '? strlen(word)+1: strlen(word);
  if (word[1]=='o' && word[2]=='r'){
    fir_node->integer=-1;
    fir_node->type=OR;
    fir_node->size = checkSize (string+*start);
  }else if(word[1]=='a' && word[2]=='n' && word[3]=='d'){
    fir_node->integer=-1;
    fir_node->type=AND;
    fir_node->size = checkSize (string+*start);
  }else if(word[1]=='n' && word[2]=='o' && word[3]=='t'){
    fir_node->integer=-1;
    fir_node->type=NOT;
    fir_node->size=1;
  }else{
    int answer = 0;
    fir_node->integer=0;
    fir_node->size=0;
    for (int i = 1; i < strlen(word); i++)
      if (word[i]>='0' && word[i]<='9')
        fir_node->integer=fir_node->integer*10+word[i]-'0';
      else if (word[i] == ')')
        answer++;
    fir_node->type=INTEGER;
    fir_node->list=NULL;
    if (answer != block_size || fir_node->integer==0){
      printf("ERROR : the given input does not follow the syntax\n");
      exit(-1);
    }
    return fir_node;
  }
  if (fir_node->size == 1){
    printf("ERROR : the given input does not follow the syntax\n");
    exit(-1);
  }
  node** list = (node**)malloc(sizeof(node*)*fir_node->size);
  for (int i = 0; i < fir_node->size-1; i++){
    *(list+i)=(node*)malloc(sizeof(node));
    *(list+i) = getTree (string, *(list+i), start, 0);
  }
  *(list+fir_node->size-1)=(node*)malloc(sizeof(node));
  *(list+fir_node->size-1)=getTree (string, *(list+fir_node->size-1), start, block_size+1);
  fir_node->list = list;
  return fir_node;
}

void getString(char string[], char word[]){
  for (int i = 0; i < strlen(string); i++){
    if (string[i] == ' ' && i!=0)
      return;
    word[i] = string[i];
  }
}

void printTree(node* fir_node, int not){

  if (fir_node == NULL)
    return;
  if (fir_node->type==OR){
    for (int i = 0; i < fir_node->size; i++){
      printTree (*(fir_node->list+i), 0);
      printf(" ");
    }
  }else if(fir_node->type==AND){
    for (int i = 0; i < fir_node->size; i++){
      printTree (*(fir_node->list+i), 0);
      printf("\n");
    }
  }
  else if(fir_node->type==NOT){
    printTree(*(fir_node->list), 1);
  }else{
    if (not)
      printf("%d", (-1)*fir_node->integer);
    else
      printf("%d", fir_node->integer);
  }
}

void printTree1(node* fir_node){
  if(fir_node==NULL)
    return;
  if (fir_node->type==OR)
    printf("(OR ");
  else if (fir_node->type==AND)
    printf("(AND ");
  else if (fir_node->type==NOT)
    printf("(NOT ");
  else{
    printf("a%d ", fir_node->integer);
    return;
  }
  for (int i = 0; i < fir_node->size; i++){
    printTree1 (*(fir_node->list+i));
  }
  printf(")");
}
//*(list+fir_node->size-1)=getTree (string, *(list+fir_node->size-1), start, block_size+1);
node* NNFconverter (node* fir_node, int not_true){
  node* not_node = (node*)malloc(sizeof(node));
  if (fir_node==NULL)
    return NULL;
  if(not_true){ //not 이후의 문
    if (fir_node->type==OR){
      fir_node->type=AND;
      for (int i = 0; i < fir_node->size; i++)
        *(fir_node->list+i) = NNFconverter (*(fir_node->list+i), 1);
    }else if (fir_node->type==AND){
      fir_node->type=OR;
      for (int i = 0; i < fir_node->size; i++)
        *(fir_node->list+i) = NNFconverter (*(fir_node->list+i), 1);
    }else if (fir_node->type==NOT){
      node* not = *(fir_node->list);
      free(fir_node);
      return NNFconverter(not, 0);
    }else{
      not_node->integer = -1;
      not_node->type = NOT;
      not_node->size = 1;
      not_node->list = (node**)malloc(sizeof(node*));
      *(not_node->list) = fir_node;
      return not_node;
    }
  }else{
    if (fir_node->type==NOT){
      node* not = *(fir_node->list);
      free(fir_node);
      return NNFconverter(not, 1);
    }
    for (int i = 0; i < fir_node->size; i++)
      *(fir_node->list+i) = NNFconverter (*(fir_node->list+i), 0);
  }
  return fir_node;
}

node* CNFconverter (node* fir_node){
  if (fir_node==NULL)
    return NULL;
  int size = fir_node->size;
  for (int i = 0; i < size; i++){
    node* check = *(fir_node->list+i);
    int l = 0;
    if (fir_node->type==OR && check->type==AND){
      fir_node->size = check->size;
      fir_node->type = AND;
      node** input = (node**)malloc(sizeof(node*)*check->size);
      for(int j = 0; j < check->size; j++){
        node* add_node = (node*)malloc(sizeof(node));
        add_node->size = size;
        add_node->list = (node**)malloc(sizeof(node*)*size);
        add_node->type = OR;
        add_node->integer = -1;
        for (int k = 0; k < size; k++)
          if (k == i)
            *(add_node->list+k) = *(check->list+(l++));
          else
            *(add_node->list+k) = *(fir_node->list+k);
        *(input+j) = add_node;
      }
      free(fir_node->list);
      fir_node->list = input;
      break;
    }
  }
  for (int i = 0; i < fir_node->size; i++)
    *(fir_node->list+i) = CNFconverter (*(fir_node->list+i));
  return fir_node;
}

int checkSize (char string[]){
  int i = 0, size = 0, block = 1;
  while (1){
    if (string[i] == '\0')
      return size;
    else if (block == 0)
      break;
    if (block == 1 && string[i] == ' '){
      size++;
    }else if (string[i] == '('){
      block++;
    }else if (string[i] == ')'){
      block--;
    }
    i++;
  }
  return size+1;
}
// (or a1 (not (or (not (or a2 a3)) a4)))
// (and (and a2 a3) (or (not a1) a4))
