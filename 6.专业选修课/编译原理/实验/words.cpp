/*
	忽略空格从字符串中分离出单词
*/ 
#include <iostream>
#include <string.h>
#include <conio.h>
#include <cstdio>
#include <cstring>
using namespace std;

char words[200] = "  Ae35 6638 5392H A10 83A2Eh 65Ha 3G2H 80  ";
char* q; // 指向输入符号串中当前的字符
char word[20]; // 存储当前识别的单词
int state = 0; // 表示所处的状态
int i; // 单词的下标

int main() {
	printf("%s \n", words);

	q = words;
	i = 0;
	memset(word, 0, sizeof word);

	while (*q) {
		if (*q == 32) {
			if (i > 0) {
				word[i] = '\0';
				printf("%s ", word);
			}
			i = 0;
			memset(word, 0, sizeof word);
		} else {
			word[i++] = *q;
		}
		q++;
	}


	return 0;
}