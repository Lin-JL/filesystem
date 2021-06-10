#include "FileManager.h"
#include <iostream>
#include <sstream>
#include<stdlib.h>


bool checkCommandLength(vector<string>& input, const int expected_length)
{
	const int input_length = input.size();
	if (input_length != expected_length)
	{
		cout << input[0] << ": Expected " << expected_length - 1 << " parameters, but given " << input_length - 1 << endl;
		return false;
	}
	return true;
}

int main() {
	srand(time(0));
	FileManager fm;

	vector<string> str;
	string input;
	bool running = true;
	string currentDir = "/";
	cout << endl;
	while (running) {
		//shell prompt
		stack<string> path = fm.getCurrentPath();
		currentDir = path.empty() ? "/" : path.top();
		cout << "[root@Filesystem] " << currentDir << " $ ";
		getline(cin, input);  //enter the command
		istringstream input_stream(input);
		string item;
		while (input_stream >> item) {
			str.push_back(item);
		}
		if (str.empty()) {
			continue;
		}

		if (str[0] == "exit") {
			running = false;
		}
		else if (str[0] == "pwd") {
			stack<string> currentPath = fm.getCurrentPath();
			string curDir;
			if (currentPath.empty())
			{
				cout << "/";
			}
			while (!currentPath.empty())
			{
				curDir = currentPath.top();
				currentPath.pop();
				cout << "/" << curDir;
			}
			cout << endl;
		}
		else if (str[0] == "dir") {
			fm.listAll();
		}
		else if (str[0] == "changeDir") {
			if (checkCommandLength(str, 2))
			{
				fm.changeDirectory(str[1].data());
				stack<string> currentPath = fm.getCurrentPath();
				string curDir;
				if (currentPath.empty())
				{
					curDir = "/";
				}
				else
				{
					while (!currentPath.empty())
					{
						curDir = currentPath.top();
						currentPath.pop();
					}
				}
				currentDir = curDir;
			}
		}
		else if (str[0] == "createFile") {
			if (checkCommandLength(str, 3))
			{
				fm.createFile(str[1].data(), atoi(str[2].data()));
			}
		}
		else if (str[0] == "deleteFile") {
			if (checkCommandLength(str, 2)) {
				fm.deleteFile(str[1].data());
			}
		}
		else if (str[0] == "createDir") {
			if (checkCommandLength(str, 2))
				fm.createDirectory(str[1].data());
		}
		else if (str[0] == "deleteDir") {
			if (checkCommandLength(str, 2))
				fm.deleteDirectory(str[1].data());
		}
		else if (str[0] == "cp") {
			if (checkCommandLength(str, 3))
				fm.copyFile(str[1].data(), str[2].data());
		}
		else if (str[0] == "sum") {
			fm.displayUsage();
		}
		else if (str[0] == "cat") {
			if (checkCommandLength(str, 2))
				fm.printFileContents(str[1].data());
		}
		else if (str[0] == "help") {
			fm.getDisk()->welcomeMessage();
		}
		else if (str[0] == "end") {
			cout << "Thank you, Professor Zhong!    �� ���\n";
			cout << "           ��������������..������ �� ��� .... /\n";
			cout << "           �����|�|�����|�|�����|�|�|�|�|��������\n";
			cout << "           ������������������������������������\n";
			cout << "       ����������������������������������������������\n" << endl;
			cout << "�j �j��������  �Щ�����  �X�T�[  �X�T�[���������������Щ�  �X�j�[�������� ��  ��\n";
			cout << "�d�T�g�����ȩ�����������   �d�T�g  �U �j�� ���� �� ����   �U�U�����ȩ��Щ�  ��\n";
			cout << "�m �m�� �� ���� ������  �m �m  �^�T�a���������������ة�  �T�m�a�� �� ��   o" << endl;
		}
		else {
			cout << "-bash: " << str[0] << ": Command not found" << endl;
		}
		str.clear();
		cout << endl;
	}
	return 0;
}
