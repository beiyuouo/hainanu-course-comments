#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <conio.h>
#include <stack>
#include <map>
#include <vector>
#include <unordered_map>
#define digit 1 // 1数字
#define op 2 // +-*/()#
#define Hh 3 // 3Hh
#define AF 4 // 4A-F
#define letter 5 // 5其它字母
using namespace std;
const int N = 8;

typedef struct node {
	char data;
	struct node* before;
	struct node* next;
};

node* temp, * top;

char q; // 指向输入符号串中当前的字符
char word[20]; // 存储当前识别的单词
int state; // 表示所处的状态
int i; // 单词的下标

// 存储算法优先关系表
// 大于关于为1，等于关系为0，小于关系为-1，出错为9
// 顺序：+, -, *, /, i, (, ), #
int table[N][N] = {
	{1, 1, -1, -1, -1, -1, 1, 1},
	{1, 1, -1, -1, -1, -1, 1, 1},
	{1, 1, 1, 1, -1, -1, 1, 1},
	{1, 1, 1, 1, -1, -1, 1, 1},
	{1, 1, 1, 1, 9, 9, 1, 1},
	{-1, -1, -1, -1, -1, -1, 0, 9},
	{1, 1, 1, 1, 9, 9, 1, 1},
	{-1, -1, -1, -1, -1, -1, 9, 0}
};

void push(char c); // 入栈
char pop(); // 出栈
int t2j(char current); // 将字符数字化
int parser(string line);
bool check_terminal(char ch); // 判断是否是终结符
int isDigitOrChar(char ch);
string change_i(string words); // 将含有十进制或十六进制数的表达式转换为用i代替的表达式


int main() {
	top = (node*)malloc(sizeof(node));
	top->before = NULL;
	top->next = NULL;
	top->data = ' ';
	string line = "i+(i+i*i#";
	
	cout << "Start parse string " << line << ":" << endl << endl;
	parser(line);

	return 0;
}

void push(char c) {  // 入栈函数
	temp = (node*)malloc(sizeof(node));
	
	temp->data = c;
	temp->before = top;
	temp->next = NULL;

	top->next = temp;  // 双向链表
	top = temp;
}

char pop() {
	char ch = top->data;

	if (top->before != NULL) {
		temp = top;
		top = temp->before;
		temp->before = NULL;
		top->next = NULL;
		free(temp);
	}
	
	return ch;
}

int t2j(char current) {
	// 顺序：+, -, *, /, i, (, ), #
	int j = 0;

	switch (current) {
	case '+': j = 0; break;
	case '-': j = 1; break;
	case '*': j = 2; break;
	case '/': j = 3; break;
	case 'i': j = 4; break;
	case '(': j = 5; break;
	case ')': j = 6; break;
	case '#': j = 7;
	}

	return j;
}

void showMess(int l, string line) {
	
	node* t = top;
	//cout << t->data;
	vector<char> v;
	while (t) {
		v.push_back(t->data);
		//cout << t->data;
		t = t->before;
		
	}

	string str;
	for (int i = v.size()-1; i >= 0; i--) {
		str += v[i];
	}
	cout << str;

	cout << "\t\t";
	for (int i = l; i < line.size(); i++) {
		cout << line[i];
	}
	cout << "\t\t";

}

int parser(string line) {
	int i, j;
	push('#');
	
	int k = 1;

	bool flag = true;
	// line[l] != '#'
	for (int l = 0; ; l++) {
		showMess(l, line);

		// 退出条件
		if (top->data == 'N' && top->before->data == '#' && line[l] == '#') break;

		node* sktemp = top;
		char sk = top->data;
		char a = line[l];
		//cout << a << endl;
		// 任何两终结符之间最多只有一非终结符，若非终结符往前寻找一位即可
		if (!check_terminal(sk)) {
			sk = top->before->data;
			sktemp = top->before;
		}
		i = t2j(sk); // 获取栈顶终结符
		j = t2j(a); // 获取当前输入符号
		//if (a == '+')
		//cout << i << ' ' << j << endl;
		switch (table[i][j]) {
		case 9:  // 语法错误
			flag = false;
			break;
		case 0: 
			cout << "=" << "\t\t" << "push in" << endl;
			push(a);
			break;
		case -1:  // 小于，移进
			cout << "<" << "\t\t" << "push in" << endl;
			//cout << a << endl;
			push(a);
			break;
		case 1:   // 大于，归约
			cout << ">" << "\t\t" << "reduce" << endl;
			node * Q = sktemp;
			do {
				if (check_terminal(Q->before->data)) Q = Q->before;
				else Q = Q->before->before;
			} while (table[t2j(Q->data)][i] == 0);
			//Q++;
			while (top != Q) {
				char c = pop();
			}
			push('N');
			l--;

			break;
		}
		if (!flag) break;
	}

	if (!flag) {
		cout << endl << line << " is not valid." << endl;
	}
	else {
		cout << endl << line << " is valid." << endl;
	}

	return 1;
}

int isDigitOrChar(char ch) {
	if (ch >= 48 && ch <= 57) // 数字
		return digit;
	else if (ch == 72 || ch == 104) // H or h
		return Hh;
	else if ((ch >= 65 && ch <= 70) || (ch >= 97 && ch <= 102)) // 字母A,B,C,D,E,F
		return AF;
	else if ((ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122)) // 除A~F外的其它字母
		return letter;
	else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '(' || ch == ')' || ch == '#')
		return op;
}

// 将含有十进制或十六进制数的表达式转换为用i代替的表达式
string change_i(string words) {
	memset(word, 0, sizeof word);
	state = 0;
	i = 0;
	cout << "Input string is: " << words << endl;

	string result = "";
	int cnt = 0;
	q = words[cnt++];

	while (cnt <= words.size()) {
		// 先判断状态，再判断字符
		switch (state) {
		case 0: // 0状态
			switch (isDigitOrChar(q)) {
			case digit: // 数字
				word[i++] = q;
				state = 2; // 转移到2状态
				break;
			case Hh: // H or h
			case AF: // 字母A,B,C,D,E,F or a,b,c,d,e,f
			case letter: // 字母
				word[i++] = q;
				state = 1;
				break;
			case op: // 操作符
				result += q;
				state = 0;
				break;
			default: // 其它（非法字符 ）
				word[i++] = q;
				state = 5;
			}
			break;
		case 1: // 1状态
			switch (isDigitOrChar(q)) {
			case Hh: // 当前状态遇到字母、数字往下读入
			case AF:
			case digit:
			case letter:
				word[i++] = q;
				state = 1;
				break;
			case op: // 读入完毕，识别为标识符
				word[i] = '\0';
				printf("%s is an identifier.\n", word);
				//result += "i";
				memset(word, 0, sizeof word);
				i = 0;
				state = 0;
				result = "-1";
				return result;
				break;
			default:
				word[i++] = q;
				state = 5;
			}
			break;
		case 2: // 2状态
			switch (isDigitOrChar(q)) {
			case digit: // 若为数字，不改变状态往下读入
				word[i++] = q;
				state = 2;
				break;
			case Hh: // 若为Hh，转移至状态3
				word[i++] = q;
				state = 3;
				break;
			case AF: // 若为AF，则有可能是16进制，转移至状态4
				word[i++] = q;
				state = 4;
				break;
			case op: // 成功识别为整数
				word[i] = '\0';
				printf("%s is an Integer.\n", word);
				result += "i";
				result += q;
				//cout << result << endl;
				memset(word, 0, sizeof word);
				i = 0;
				state = 0;
				break;
			default:
				word[i++] = q;
				state = 5;
			}
			break;
		case 3: // 3状态
			switch (isDigitOrChar(q)) {
			case op: // 识别为16进制数
				word[i] = '\0';
				printf("%s is a Hex digit.\n", word);
				result += "i";
				result += q;
				//cout << result << endl;
				memset(word, 0, sizeof word);
				i = 0;
				state = 0;
				break;
			default:
				word[i++] = q;
				state = 5;
			}
			break;
		case 4: // 4状态
			switch (isDigitOrChar(q)) {

			case digit: // 若为数字或A~F，仍为状态4，往下读入
			case AF:
				word[i++] = q;
				state = 4;
				break;
			case Hh:
				word[i++] = q;
				state = 3;
				break;
			case op: // 如果16进制没有以h或H结尾，转移至错误状态
				state = 5;
				cnt--;
				break;
			default:
				word[i++] = q;
				state = 5;
			}
			break;
		case 5: // 出错状态
			if (isDigitOrChar(q) == op) { // 若为空格，则识别为非标识符
				word[i] = '\0';
				printf("%s is not an identifier.\n", word);
				memset(word, 0, sizeof word);
				i = 0;
				state = 0;
				result = "-1";
				return result;
			}
			else { // 出错序列还未读取完毕，往下读入
				word[i++] = q;
				q = words[cnt++];
				continue;
			}
			break;
		}
		q = words[cnt++]; // 指针下移（指向输入符号串中的下一个字符）
	}

	return result;
}

// 判断是否是终结符
bool check_terminal(char ch) {
	if (isDigitOrChar(ch) == op || ch == 'i') return true;
	else return false;
}