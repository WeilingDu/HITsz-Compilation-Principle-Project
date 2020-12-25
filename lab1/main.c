#include <stdio.h>
#include <string.h>
#define TRUE 1
#define FALSE 0
#define KEY_NUM 14  // 关键字个数
#define KEY_LEN 20  // 关键字长度
#define ID_MAX 1000  // 标识符个数

#define ID 25  // 标识符
#define DECNUM 26 // 十进制无符号整数
#define HEXNUM 27 // 十六进制无符号整数
#define OCTNUM 28 // 八进制无符号整数
#define GT 29  // >
#define GE 30  // >=
#define LS 31  // <
#define LE 32  // <=
#define EQ 33  // ==
#define NE 34  // !=
#define ASSIGN 35  // =
#define NOT 36 // !
#define AND 37  // &
#define DAND 38 // &&
#define OR 39 // |
#define DOR 40 // ||


// 14个(1-14)
char key_set[][KEY_LEN] = {"for",
                      "while",
                      "do",
                      "switch",
                      "case",
                      "break",
                      "continue",
                      "if",
                      "else",
                      "char",
                      "int",
                      "float",
                      "return",
                      "main"};
// 10个(15-24)
char single_set[][KEY_LEN] = {"+",
                              "-",
                              "/",
                              "*",
                              "(",
                              ")",
                              "{",
                              "}",
                              ";",
                              ","};
// 符号表的数据结构
struct ID_NAME{
    char name[KEY_LEN];
    char type[KEY_LEN];
    char address[KEY_LEN];
};
struct ID_LIST{
    struct ID_NAME names[ID_MAX];
    int length;
};

char* INPUT_FILE = "/Users/DWL/Downloads/input_file.txt";  // 输入文件
char* OUTPUT_FILE = "/Users/DWL/Downloads/output_file.txt"; // 输出文件
char strBuffer[1026]; // 字符串缓冲区
char token[KEY_LEN];
int line_num = 0; // 需要统计的程序的行数，表示当前行号，用于定位错误位置
int state = 0; // 当前程序的状态
struct ID_LIST idList;

//从字符串中获取指针当前所指位置的字符
char getChar(char* str)
{
    return *str;
}
// 判断是否是数字0-9
int isDigit09(char c)
{
    if (c>='0' && c<='9')
    {
        return TRUE;
    } else {
        return FALSE;
    }
}
// 判断是否是数字1-9
int isDigit19(char c)
{
    if (c>='1' && c<='9')
    {
        return TRUE;
    } else {
        return FALSE;
    }
}
// 判断是否是数字0-7
int isDigit07(char c)
{
    if (c>='0' && c<='7')
    {
        return TRUE;
    } else {
        return FALSE;
    }
}
// 判断是否是0-9或a-f
int isDigit09Letteraf(char c)
{
    if ((c>='0' && c<='9') || (c>='a' && c<='f') )
    {
        return TRUE;
    } else {
        return FALSE;
    }
}
// 判断是否是字母
int isLetter(char c)
{
    if ((c>='a'&&c<='z') || (c>='A'&& c<='Z'))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
//判断是否是标点符号
int isPunctuation(char c)
{
    for(int i=0; i<10; i++)
    {
        if(c == single_set[i][0])
        {
            return (i+15);
        }
    }
    return FALSE;
}
// 判断是否是关键字
int isKey(char* str)
{
    for(int i=0; i<KEY_NUM; i++)
    {
        if(strcmp(str, key_set[i]) ==0)
        {
            return i+1;
        }
    }
    return FALSE;
}
void error()
{
    printf("the %dth line error\n",line_num);
}
// 将token存入符号表
void install_id(char id[])
{
    struct ID_NAME tmp;
    strcpy(tmp.name, id);
    strcpy(tmp.type, "-1");
    strcpy(tmp.address, "?");
    int flag = 0;
    if(idList.length)
    {
        for(int i=0; i<idList.length; i++)
        {
            if(strcmp(idList.names[i].name, tmp.name) == 0)
            {
                flag = 1;
                break;
            }
        }
        if(flag == 0)
        {
            idList.names[idList.length] = tmp;
            idList.length++;
        }
    }
    else
    {
        idList.names[0] = tmp;
        idList.length++;
    }

}

//对一行字符串的内容进行词法分析
void process_string(char* buf)
{
    char C = getChar(buf);  // 从but中读取一个字符
    while(C!='\n' && C!='\0') // 不是换行符，不是结束符
    {
        switch(state)
        { //对当前状态进行分析
            case 0: //处于代读取字符串状态，还没有进入任何自动机
                if(isLetter(C) || C=='_' )
                {
                    state = 1; //进入状态1
                    char tmp[2];
                    tmp[0] = C;
                    tmp[1] = '\0';
                    strcpy(token, tmp);
                    buf++;
                    C = getChar(buf);
                }
                else if(C == '0')
                {
                    state = 2;
                    char tmp[2];
                    tmp[0] = C;
                    tmp[1] = '\0';
                    strcpy(token, tmp);
                    buf++;
                    C = getChar(buf);
                }
                else if(isDigit19(C))
                {
                    state = 12;
                    char tmp[2];
                    tmp[0] = C;
                    tmp[1] = '\0';
                    strcpy(token, tmp);
                    buf++;
                    C = getChar(buf);
                }
                else if(isPunctuation(C))
                {
                    state = 0;
                    printf("(%d, %c)\n", isPunctuation(C), C);
                    buf++;
                    C = getChar(buf);
                }
                else if(C == ' ') //空格
                {
                    state = 0;
                    buf++;
                    C = getChar(buf);
                }
                else if(C == '<')
                {
                    state = 3;
                    char tmp[2];
                    tmp[0] = C;
                    tmp[1] = '\0';
                    strcpy(token, tmp);
                    buf++;
                    C = getChar(buf);
                }
                else if(C=='>')
                {
                    state = 4;
                    char tmp[2];
                    tmp[0] = C;
                    tmp[1] = '\0';
                    strcpy(token, tmp);
                    buf++;
                    C = getChar(buf);
                }
                else if(C == '=')
                {
                    state = 5;
                    char tmp[2];
                    tmp[0] = C;
                    tmp[1] = '\0';
                    strcpy(token, tmp);
                    buf++;
                    C = getChar(buf);
                }
                else if(C == '!')
                {
                    state = 6;
                    char tmp[2];
                    tmp[0] = C;
                    tmp[1] = '\0';
                    strcpy(token, tmp);
                    buf++;
                    C = getChar(buf);
                }
                else if(C == '&')
                {
                    state = 7;
                    char tmp[2];
                    tmp[0] = C;
                    tmp[1] = '\0';
                    strcpy(token, tmp);
                    buf++;
                    C = getChar(buf);
                }
                else if(C == '|')
                {
                    state = 8;
                    char tmp[2];
                    tmp[0] = C;
                    tmp[1] = '\0';
                    strcpy(token, tmp);
                    buf++;
                    C = getChar(buf);
                }
                else
                {
                    error();
                    //跳过这个字符
                    buf++;
                    C = getChar(buf);
                }
                break;
            case 1:
                if(isLetter(C) || C=='_' || isDigit09(C))
                {
                    state=1;
                    char tmp[2];
                    tmp[0] = C;
                    tmp[1] = '\0';
                    strcat(token, tmp);
                    buf++;
                    C = getChar(buf);
                }
                else
                { //到了分隔符
                    if(isKey(token))
                    {
                        printf("(%d, %s)\n", isKey(token), token);
                    }
                    else
                    {
                        install_id(token);
                        printf("(%d, %s)\n", ID, token);
                    }
                    state = 0;
                }
                break;
            case 2:
                if(isDigit07(C))
                {
                    state = 25;
                    char tmp[2];
                    tmp[0] = C;
                    tmp[1] = '\0';
                    strcat(token, tmp);
                    buf++;
                    C = getChar(buf);
                }
                else if(C == 'x')
                {
                    state = 26;
                    char tmp[2];
                    tmp[0] = C;
                    tmp[1] = '\0';
                    strcat(token, tmp);
                    buf++;
                    C = getChar(buf);
                }
                else
                {
                    printf("(%d, %s)\n", DECNUM, token);
                    state = 0;
                }
                break;
            case 3:
                if(C == '=')
                {
                    state = 0;
                    printf("(%d, <=)\n", LE);
                    buf++;
                    C = getChar(buf);
                }
                else
                {
                    state = 0;
                    printf("(%d, <)\n", LS);
                }
                break;
            case 4:
                if(C == '=')
                {
                    state = 0;
                    printf("(%d, >=)\n", GE);
                    buf++;
                    C = getChar(buf);
                }
                else
                {
                    state = 0;
                    printf("(%d, >)\n", GT);
                }
                break;
            case 5:
                if(C == '=')
                {
                    state = 0;
                    printf("(%d, ==)\n", EQ);
                    buf++;
                    C = getChar(buf);
                }
                else
                {
                    state = 0;
                    printf("(%d, =)\n", ASSIGN);
                }
                break;
            case 6:
                if(C == '=')
                {
                    state = 0;
                    printf("(%d, !=)\n", NE);
                    buf++;
                    C = getChar(buf);
                }
                else
                {
                    printf("(%d, !)\n", NOT);
                    state = 0;
                }
                break;
            case 7:
                if(C == '&')
                {
                    printf("(%d, &&)\n", DAND);
                    state = 0;
                    buf++;
                    C = getChar(buf);
                }
                else
                {
                    printf("(%d, &)\n", AND);
                    state = 0;
                }
                break;
            case 8:
                if(C == '|')
                {
                    printf("(%d, ||)\n", DOR);
                    state = 0;
                    buf++;
                    C = getChar(buf);
                }
                else
                {
                    printf("(%d, |)\n", OR);
                    state = 0;
                }
                break;
            case 12:
                if ((isDigit09(C)))
                {
                    state = 12;
                    char tmp[2];
                    tmp[0] = C;
                    tmp[1] = '\0';
                    strcat(token, tmp);
                    buf++;
                    C = getChar(buf);
                }
                else
                {
                    printf("(%d, %s)\n", DECNUM, token);
                    state = 0;
                }
                break;
            case 25:
                if ((isDigit07(C)))
                {
                    state = 25;
                    char tmp[2];
                    tmp[0] = C;
                    tmp[1] = '\0';
                    strcat(token, tmp);
                    buf++;
                    C = getChar(buf);
                }
                else
                {
                    printf("(%d, %s)\n", OCTNUM, token);
                    state = 0;
                }
                break;
            case 26:
                if ((isDigit09Letteraf(C)))
                {
                    state = 28;
                    char tmp[2];
                    tmp[0] = C;
                    tmp[1] = '\0';
                    strcat(token, tmp);
                    buf++;
                    C = getChar(buf);
                }
                else
                {
                    error();
                    state = 0;
                    buf++;
                    C = getChar(buf);
                }
                break;
            case 28:
                if ((isDigit09Letteraf(C)))
                {
                    state = 28;
                    char tmp[2];
                    tmp[0] = C;
                    tmp[1] = '\0';
                    strcat(token, tmp);
                    buf++;
                    C = getChar(buf);
                }
                else
                {
                    printf("(%d, %s)\n", HEXNUM, token);
                    state = 0;
                }
                break;
            default:
                error();
                state = 0;
                buf++;
                C = getChar(buf);
        }
    }
}

int main() {
    FILE* file = fopen(INPUT_FILE,"rw"); //只读的方式打开要扫描的文件
    freopen(OUTPUT_FILE,"w",stdout); //输出重定向
    if(file == NULL)
    {
        printf("Error! opening file");
        return 0;
    }
    printf("*****输出单词*****\n");
    idList.length = 0;
    //实现从文件中逐行读取
    while(!feof(file))
    {
        line_num++;
        fgets(strBuffer,1024,file);
        process_string(strBuffer); //处理字符串
    }
    fclose(file);

    // 输出符号表
    printf("*****符号表*****\n");
    for(int i=0; i<idList.length; i++){
        printf("(%s, %s, %s)\n", idList.names[i].name, idList.names[i].type, idList.names[i].address);
    }
    return 0;
}