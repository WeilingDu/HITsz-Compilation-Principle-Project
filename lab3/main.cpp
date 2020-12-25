#include <cstdio>
#include <vector>
#include <string>
#include <iostream>
# include <fstream>
#include <map>
#include <set>
#include <stack>
#include <queue>
#include <sstream>
using namespace std;

# define INIT_LEN 100

string ACTION_TABLE_PATH = "/Users/DWL/Downloads/action_table.txt";
string GOTO_TABLE_PATH = "/Users/DWL/Downloads/goto_table.txt";
string INPUT_FILE_PATH = "/Users/DWL/Downloads/input_file.txt";
string OUTPUT_FILE_PATH = "/Users/DWL/Downloads/output_file.txt";

int skip = 0; // 跳过第一行


/* action表和goto表 */
pair<int, int> action[INIT_LEN][INIT_LEN]; // first表示分析动作，1->Shift 2->Reduce, 3->ACC; second表示转移状态或者产生式序号
int goton[INIT_LEN][INIT_LEN];


/* 产生式结构体，左部符号和右部符号串 */
struct Production {
    char left;
    vector<char> rights;
    /* 重载== */
    bool operator==(Production& rhs) const {
        if (left != rhs.left)
            return false;
        for (int i = 0; i < rights.size(); i++) {
            if (i >= rhs.rights.size())
                return false;
            if (rights[i] != rhs.rights[i])\
                return false;
        }
        return true;
    }
};

/* 文法结构体 */
struct Grammar {
    int num;  // 产生式数量
    vector<char> T;   // 终结符
    vector<char> N;   // 非终结符
    vector<Production> prods;  //产生式
}grammar;


/* 待分析串 */
string str;

/* 分析栈 */
stack<pair<int, char>> ST; // first是状态，second是符号

/* 三元组符号栈 */
vector<string> tri;


/* 变量扩展符号表 */
vector<string> vartab;


/* 常量扩展符号表 */
vector<string> contab;

/* 判断是否符合变量命名规则 */
int isvar(char ch) {
    int flag = 1;
    if ('a' <= ch && ch <= 'z') {

    }
    else
    {
        flag = 0;
    }

    return flag;
}
/* 判断是否符合数字形式 */
int isnum(char ch) {
    int flag = 1;

    if (isdigit(ch)) {}
    else
    {
        flag = 0;
    }
    return flag;
}


/* 判断ch是否是终结符 */
int isInT(char ch)
{
    for (int i = 0; i < grammar.T.size(); i++) {
        if (grammar.T[i] == ch) {
            return i + 1;
        }
    }
    return 0;
}

/* 判断ch是否是非终结符 */
int isInN(char ch)
{
    for (int i = 0; i < grammar.N.size(); i++) {
        if (grammar.N[i] == ch) {
            return i + 1;
        }
    }
    return 0;
}

/* 读入ActionTable */
void readAction(string file_path, pair<int, int> action[INIT_LEN][INIT_LEN]) {
    ifstream file(file_path);
    string temp;
    if (file.is_open() == false) {
        cout << "ERROR: failed to open the file" << endl;
        return;
    }
    while (getline(file, temp)) {
        if (skip == 0) {
            skip++;
            continue;
        }
        int row_num_pos = temp.find_first_of('\t');
        int row_num = atoi(temp.substr(0, row_num_pos).c_str());
        int j = 0;
        for (int i = row_num_pos + 1; i < temp.size();) {
            if (temp[i] == '\t') {
                action[row_num][++j] = pair<int, int>();
                i++;
            } else {
                if (temp[i] == 'A') {
                    action[row_num][j] = pair<int, int>(3, 0);
                } else if (temp[i] == 'S') {
                    action[row_num][j] = pair<int, int>(1, 0);
                } else {
                    action[row_num][j] = pair<int, int>(2, 0);
                }
                int k = i + 1;
                while (temp[k] != '\t') {
                    k++;
                }
                if (temp[i] != 'A') {
                    action[row_num][j].second = atoi(temp.substr(i + 1, k).c_str());
                }
                i = k;
            }
        }
    }
    file.close();
    skip = 0;
}

/* 读入GotoTable*/
void readGoto(string file_path, int goton[INIT_LEN][INIT_LEN]) {
    ifstream file(file_path);
    string temp;
    if (file.is_open() == false) {
        cout << "ERROR: failed to open the file" << endl;
        return;
    }
    while (getline(file, temp)) {
        if (skip == 0) {
            skip++;
            continue;
        }
        int row_num_pos = temp.find_first_of('\t');
        int row_num = atoi(temp.substr(0, row_num_pos).c_str());
        int j = 0;
        for (int i = row_num_pos + 1; i < temp.size();) {
            if (temp[i] == '\t') {
                goton[row_num][(++j)+1] = 0;
                i++;
            } else {
                int k = i + 1;
                while (temp[k] != '\t') {
                    k++;
                }
                goton[row_num][j+1] = atoi(temp.substr(i, k).c_str());
                i = k;
            }
        }
    }
    file.close();
    skip = 0;
    for (int k=0; k<36; k++){
        goton[k][0] = 0;
    }
}

void readInput(string file_path){
    // 备份cin和cout的默认buf
    streambuf *cin_backup;
    cin_backup = cin.rdbuf();

    // 打开要参与重定向的文件
    fstream in;
    in.open(INPUT_FILE_PATH, ios::in);
    if (in.fail())
        return;

    // 将input_file.txt内容重定向到cin
    cin.rdbuf(in.rdbuf());

    // 读入产生式个数
    cin >> grammar.num;

    // 读入产生式
    string s;
    for (int i = 0; i < grammar.num; i++) {
        cin >> s;
        Production tmp;
        tmp.left = s[0];
        for (int j = 3; j < s.size(); j++) {
            tmp.rights.push_back(s[j]);
        }
        grammar.prods.push_back(tmp);
    }

    char ch;
    // 读入终结字符
    cin >> ch;
    while (ch != '#') {
        grammar.T.push_back(ch);
        cin >> ch;
    }
    // 把$当作终结符
    grammar.T.push_back('$');

    // 读入非终结字符
    cin >> ch;
    while (ch != '#') {
        grammar.N.push_back(ch);
        cin >> ch;
    }

    // 入待分析串并初始化分析栈
    cin >> str;
    str += '$';
    ST.push(pair<int, char>(0, '-'));

    cin.rdbuf(cin_backup);

}

/* 根据读入文件初始化数据结构 */
void initGrammar()
{
    readInput(INPUT_FILE_PATH);
    readAction(ACTION_TABLE_PATH, action);
    readGoto(GOTO_TABLE_PATH, goton);
}


/* 语法分析程序 */
void grammaticalAnalysis()
{
    string strout = "";
    int tnum = 0;

    // 备份cout的默认buf
    streambuf *cout_backup;
    cout_backup = cout.rdbuf();

    // 打开要参与重定向的文件
    fstream out;
    out.open(OUTPUT_FILE_PATH, ios::out);
    if (out.fail())
        return;

    // 将cout重定向到output_file.txt
    cout.rdbuf(out.rdbuf());

    // 语法分析算法
    int ip = 0;
    cout << "************* 三地址码 **********" << endl;
    do {
        int s = ST.top().first;  // 当前状态
        char a = str[ip];   // 缓冲区读入的字符

        string ch;
        char s1[2] = {a, 0}; // 定义一个字符数组，即传统的字符串，使其值为单个字符加上字符串结束符\0。
        ch = s1; // 将字符赋值给string对象。

        // 如果是变量
        if (isvar(a)) {
            if (std::find(vartab.begin(), vartab.end(), ch) == vartab.end()){
                vartab.push_back(ch);
            }
            tri.push_back(ch);
            a = 'a';
        }
        // 如果是数字
        else if (isnum(a))
        {
            tri.push_back(ch);
            contab.push_back(ch);
            a = 'a';
        }


//        if (a == 'a') {
//            tri.push_back("a");
//        }


        int j = isInT(a) - 1;  // 若是终结字符，返回

        if (action[s][j].first == 1) {  // 移进
            ST.push(pair<int, char>(action[s][j].second, a));
            ip = ip + 1;
        } else if (action[s][j].first == 2) { // 规约

            // 中间代码生成
            // 三地址码的生成
            int p_num = action[s][j].second;

            if (p_num == 1){
                string fir = tri.back();
                tri.pop_back();
                string sec = tri.back();
                tri.pop_back();
                cout << sec + " = " + fir << endl;
            }
            else if (p_num == 2) {
                string fir = tri.back();
                tri.pop_back();
                string sec = tri.back();
                tri.pop_back();
                tnum++;
                string tmpn;
                tmpn = to_string(tnum);
                tmpn = "t" + tmpn;
                tri.push_back(tmpn);
                cout << tmpn << " = " + sec + " + " + fir << endl;
            } else if (p_num == 3){
                string fir = tri.back();
                tri.pop_back();
                string sec = tri.back();
                tri.pop_back();
                tnum++;
                string tmpn;
                tmpn = to_string(tnum);
                tmpn = "t" + tmpn;
                tri.push_back(tmpn);
                cout << tmpn << " = " + sec + " - " + fir << endl;
            } else if (p_num == 5) {
                string fir = tri.back();
                tri.pop_back();
                string sec = tri.back();
                tri.pop_back();
                tnum++;
                string tmpn;
                tmpn = to_string(tnum);
                tmpn = "t" + tmpn;
                tri.push_back(tmpn);
                cout << tmpn << " = " + sec + " * " + fir << endl;
            } else if (p_num == 6){
                string fir = tri.back();
                tri.pop_back();
                string sec = tri.back();
                tri.pop_back();
                tnum++;
                string tmpn;
                tmpn = to_string(tnum);
                tmpn = "t" + tmpn;
                tri.push_back(tmpn);
                cout << tmpn << " = " + sec + " / " + fir << endl;
            }




            Production &P = grammar.prods[action[s][j].second];
            // 弹出并输出产生式
//            cout << P.left << "->";
            for (int i = 0; i < P.rights.size(); i++) {
                ST.pop();
//                cout << P.rights[i];
            }
//            cout << endl;
            s = ST.top().first;
            char A = P.left;
            j = isInN(A) - 1;
            ST.push(pair<int, char>(goton[s][j], A));
        } else if (action[s][j].first == 3) {   //接受
            Production &P = grammar.prods[action[s][j].second];
            // 输出产生式
//            cout << P.left << "->";
            for (int i = 0; i < P.rights.size(); i++) {
                ST.pop();
//                cout << P.rights[i];
            }
//            cout << endl;
            break;
        } else {
//            cout << "error" << endl;
        }

    } while(1);

    //输出变量表
    int size = vartab.size();
    cout << "************* 符号表 **********" << endl;
    cout << "名字\t" << "符号种类\t" << "类型\t" << "地址\t" << "扩展属性指针\n";
    for (int i = 0; i < size; i++) {
        string sv = vartab.back();
        cout << sv << "\t变量\t\tint\t" << i*4 << "\tNULL" << endl;
        vartab.pop_back();
    }

    cout.rdbuf(cout_backup);

}

int main()
{
    initGrammar();
    grammaticalAnalysis();

    return 0;
}