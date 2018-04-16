#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

//定义保留字，运算符，限界符的最大个数
#define MAX_RESERVED 18   //保留字的最大个数
#define MAX_OPERATOR 13     //运算符的最大个数
#define MAX_DELIMITER 6     //限界符的最大个数

//定义DFA的7种状态
#define START_STATE 0
#define RESERVED_STATE 1
#define OPRERATOR_STATE  2
#define DELIMITER_STATE  3
#define CONSTANT_STATE  4
#define IDENTIFIER_STATE  5
#define OTHER_STATE 7
#define END_STATE 6

//定义无符号整型的DFA状态
#define DIGIT 4
#define POINT 10
#define OTHER 11
#define POWER 12
#define PLUS 13
#define MINUS 14

//定义字符串类型
#define TYPE_RESERVED 1
#define TYPE_OPRERATOR 2
#define TYPE_DELIMITER 3
#define TYPE_CONSTANT 4
#define TYPE_IDENTIFIER 5
#define TYPE_ENDFILE 6
#define TYPE_UNKNOWN 7



char *reserveds[MAX_RESERVED] = {
    "unsigned","break","return","void","case","float","char","for",
    "while","continue","if","default","do","int","switch","double",
    "long","else"
  };


char *operators[MAX_OPERATOR] = {
    "+","-","*","/",">",">=","<","<=","==","!=","&&","||","!"
};

char *delimiters[MAX_DELIMITER] = {
    "{","}","(",")",";",","
};

// 对比字符串是否是保留字
int isReserved(char *str){
    int i;
    for (i = 0; i < MAX_RESERVED; i++) {
      if (strcmp(reserveds[i],str) == 0)
        return 1;
    }
    return 0;
}

//对比是否是运算符
int isOperator(char str){
    int i;
    for (i = 0; i < MAX_OPERATOR; i++) {
      if (operators[i][0] == str)
        return 1;
    }
    return 0;
}

//对比是否是限界符
int isDelimiter(char str){
    int i;
    for (i = 0; i < MAX_DELIMITER; i++) {
      if (delimiters[i][0]== str)
        return 1;
    }
    return 0;
}


int handleError(){
  printf("Error\n");
  return 0;
}

//执行
int lex(FILE *filein){
  int i  = -1,m = 0;
  int c;
  char str[20];
  char strl[1000][20];
  int state = START_STATE,type = 0,digitState = DIGIT;
  while ((c = fgetc(filein)) != EOF) {
    switch (state) {
      case START_STATE:
        if ((c <= 'z' && c >= 'a') || (c <= 'Z' && c >= 'A') || (c == '_')) {
          str[++i] = c;
          state = IDENTIFIER_STATE;
          type = TYPE_IDENTIFIER;
        }else if ((c <= '9' && c >= '0') || (c == '.')) {
          str[++i] = c;
          state = CONSTANT_STATE;
          type = TYPE_CONSTANT;
        }else if (isOperator(c)) {
          str[++i] = c;
          state = OPRERATOR_STATE;
          type = TYPE_OPRERATOR;
        }else if (isDelimiter(c)) {
          str[++i] = c;
          state = END_STATE;
          type = TYPE_DELIMITER;
        }else {
          state = END_STATE;
        }
        break;

      case IDENTIFIER_STATE:
        if ((c <= 'z' && c >= 'a') || (c <= 'Z' && c >= 'A') || (c <= '9' && c >= '0') ) {
          str[++i] = c;
          state = IDENTIFIER_STATE;
        }else
        {
          fseek(filein,-1,SEEK_CUR);
          state = END_STATE;
        }
        break;

      case OPRERATOR_STATE:
        if ((c == '&' && str[i] == '&') || (c == '=' && (str[i] == '<' || str[i] =='>' || str[i] == '=')) || (c == '|' && str[i] == '|')) {
          str[++i] = c;
          state = END_STATE;
        }else{
          fseek(filein,-1,SEEK_CUR);      //文件流前移一个字节
          state = END_STATE;     //将自动机状态置为终止态
        }
        break;

      case CONSTANT_STATE:
        switch (digitState) {
          case DIGIT:
            if ((c <= '9' && c >= '0')) {
              str[++i] = c;
              digitState = DIGIT;
              /* code */
            }else if (c == 'E' || c == 'e') {
              str[++i] = c;
              digitState = POWER;
            }else if (c == '.') {
              str[++i] = c;
              digitState = POINT;
            }else{
              fseek(filein,-1,SEEK_CUR);
              state = END_STATE;
            }
            break;
          case POINT:
            if ((c <= '9' && c >= '0')) {
              str[++i] = c;
            }else if (c == 'E' || c == 'e') {
              str[++i] = c;
              digitState = POWER;
            }else {
              fseek(filein,-1,SEEK_CUR);
              state = END_STATE;
            }
            break;
          case OTHER:
            if (c <= '9' && c >= '0') {
              str[++i] = c;
              digitState = POINT;
            }else{
              fseek(filein,-1,SEEK_CUR);
              state = END_STATE;
            }
            break;
          case POWER:
            if (c == '+' || c == '-') {
              str[++i] = c;
              digitState = PLUS;
            }else if (c <= '9' && c>= '0') {
              str[++i] = c;
              digitState = MINUS;
            }else {
              fseek(filein,-1,SEEK_CUR);
              state = END_STATE;
            }
            break;
          case PLUS:
            if (c <= '9' && c >= '0') {
                str[++i] = c;
                digitState = MINUS;
            }else {
              fseek(filein,-1,SEEK_CUR);
              state = END_STATE;
            }
            break;
          case MINUS:
            if (c <= '9' && c >= '0') {
              str[++i] = c;
            }else{
              fseek(filein,-1,SEEK_CUR);
              state = END_STATE;
            }
            break;
          default:
            handleError();
            break;
        }
        break;

      case END_STATE:
        state = START_STATE;    //重新置为START_STATE状态
        digitState = DIGIT;     //将digitState重新置为DIGIT状态
        int n;
        for (n = 0; n <= i; n++) {
          strl[m][n] = str[n];
        }
        if (type == TYPE_IDENTIFIER) {
          if (isReserved(strl[m])) {
            type = TYPE_RESERVED;
          }
        }
        if (i>-1) {
            printf("%s,%d\n",strl[m],type);
        }
        fseek(filein,-1,SEEK_CUR);
        i = -1;
        ++m;
        break;

      default:
        handleError();
        break;
    }
  }
}


int main(int argc, char const *argv[]) {

  int h6h = 12;
  FILE *fp = fopen("/home/salorum/桌面/123.txt","r");
  lex(fp);
  printf("%d\n",h6h );

}
