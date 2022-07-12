#include <iostream>
#include <string>
#include <cstring>
#define digit 1 // 1数字
#define op 2 // +-*/()#
#define Hh 3 // 3Hh
#define AF 4 // 4A-F
#define letter 5 // 5其它字母
using namespace std;

typedef struct node {
	char data;
	struct node* next;
};
node* p, * h, * temp, * top, * base;
char cmpchar;
string line;
// 定义分析表结构
int table[5][8] = {
	{0, 0, 0, 0, 1, 0, 1, 0},
	{1, 1, 0, 0, 0, 1, 0, 1},
	{0, 0, 0, 0, 1, 0, 1, 0},
	{1 ,1, 1, 1, 0, 1, 0, 1},
	{0, 0, 0, 0, 1, 0, 1, 0}
};

char q; // 指向输入符号串中当前的字符
char word[20]; // 存储当前识别的单词
int state; // 表示所处的状态
int i; // 单词的下标

char read(string line, int k);
void push(char c);
void pop();
int i2d(char cmpchar); // EATBF
int j2d(char current); // +-*/()i#
void dopush(int t);
int isDigitOrChar(char ch);
bool check_terminal(char ch); // 判断是否是终结符
string change_i(string words); // 将含有十进制或十六进制数的表达式转换为用i代替的表达式


int main() {
	cout << "Please input the string which is at the end of #:" << endl;
	getline(cin, line);
	//cout << line << endl;

	int i, j, t;

	push('#');
	push('E'); // 初始化
	int cur = 0;
	char current; 

	while (cur < line.size()) {
		current = read(line, cur);
		cmpchar = top->data;
		pop();
		printf("Top: %c --- Cur: %c", cmpchar, current);
		// 栈顶是终结符或#，但输入串中不是终结符或#时，判定为出错
		if (check_terminal(cmpchar) && cmpchar != current) {
			cur--; // 便于判定为非法
			break;
		}

		if (current == cmpchar) {
			if (current == '#') break;
			printf("	match success of %c\n\n", current);
			cur++;
			continue;
		}
		cout << endl;
		i = i2d(cmpchar);
		j = j2d(current);
		if (table[i][j] == 1) {
			t = 10 * i + j;
			dopush(t);
		}
		else {
			cur--; // 便于判定为非法
			break;
		}
	}

	if (cur + 1 == line.size()) {
		cout << endl << "Your input is valid!" << endl;
	}
	else {
		cout << endl << "Sorry, your input is invalid!" << endl;
	}

	return 0;
}

char read(string line, int k) {
	return line[k];
}

// 压栈
void push(char c) {
	temp = (node*)malloc(sizeof node);
	temp->data = c;
	temp->next = top;
	top = temp;
}

// 弹栈
void pop() {
	cmpchar = top->data;

	if (top->next != NULL)
		top = top->next;
}

// 将i字符数字化
int i2d(char cmpchar) {
	int i = 0;

	switch (cmpchar) {
	case 'E': i = 0; break;
	case 'A': i = 1; break;
	case 'T': i = 2; break;
	case 'B': i = 3; break;
	case 'F': i = 4;
	}

	return i;
}

// 将j字符数字化
int j2d(char current) {
	int j = 0;

	switch (current) {
	case '+': j = 0; break;
	case '-': j = 1; break;
	case '*': j = 2; break;
	case '/': j = 3; break;
	case '(': j = 4; break;
	case ')': j = 5; break;
	case 'i': j = 6; break;
	case '#': j = 7; 
	}

	return j;
}

void dopush(int t) {
	switch (t) {
	case 4: push('A'); push('T'); break;
	case 6: push('A'); push('T'); break;
	case 10: push('A'); push('T'); push('+'); break;
	case 11: push('A'); push('T'); push('-'); break;
	case 15: break;
	case 17: break;
	case 24: push('B'); push('F'); break;
	case 26: push('B'); push('F'); break;
	case 30: break;
	case 31: break;
	case 32: push('B'); push('F'); push('*'); break;
	case 33: push('B'); push('F'); push('/'); break;
	case 35: break;
	case 37: break;
	case 44: push(')'); push('E'); push('('); break;
	case 46: push('i'); break;
	}
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