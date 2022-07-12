#include <iostream>
#include <cstring>
#include <string>
#include <conio.h>
#include <cstdio>
#define digit 1 // 1数字
#define space 2 // 2空格
#define Hh 3 // 3Hh
#define AF 4 // 4A-F
#define letter 5 //5其它字母
using namespace std;

string words;
char q; // 指向输入符号串中当前的字符
char word[20]; // 存储当前识别的单词
int state = 0; // 表示所处的状态
int i; // 单词的下标

int isDigitOrChar(char ch) {
	if (ch >= 48 && ch <= 57) // 数字
		return digit;
	else if (ch == 32) // 空格 
		return space;
	else if (ch == 72 || ch == 104) // H or h
		return Hh;
	else if ((ch >= 65 && ch <= 70) || (ch >= 97 && ch <= 102)) // 字母A,B,C,D,E,F
		return AF;
	else if ((ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122)) // 除A~F外的其它字母
		return letter;
}

int main() {
	// = "  Ae35 6638 5392H A10 83A2Eh 65Ha 3G2H 80  #"
	cout << "请输入标识符或数字以空格分隔(#结尾)：" << endl;
	getline(cin, words);

	cout << "Input string is: " << words << endl;

	int cnt = 0;
	q = words[cnt++];

	while (q != '#') {
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
			case space: // 空格
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
			case space: // 读入完毕，识别为标识符
				word[i] = '\0';
				printf("%s is an identifier.\n", word);
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
			case space: // 成功识别为整数
				word[i] = '\0';
				printf("%s is an Integer.\n", word);
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
			case space: // 识别为16进制数
				word[i] = '\0';
				printf("%s is a Hex digit.\n", word);
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
			default:
				word[i++] = q;
				state = 5;
			}
			break;
		case 5: // 出错状态
			if (q == 32) { // 若为空格，则识别为非标识符
				word[i] = '\0';
				printf("%s is not an identifier.\n", word);
				memset(word, 0, sizeof word);
				i = 0;
				state = 0;
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

	return 0;
}