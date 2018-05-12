#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <string>
using namespace std;

int main(int argc, char *argv[])
{
	//if (argc == 1)
	//{
	//	cout << "Please input filename you want to handle.\n";
	//	return -1;
	//}

	vector<string> forbidden_string{ "cout","cerr","iostream","fstream","exit" };
	for (int i = 0; i != argc; ++i)
	{
		vector<string> vecContent;
		string strLine;
		ifstream inFile("test.cpp");
		if (!inFile)
		{
			cout << "Not find the file.\n";
		}
		while (inFile)
		{
			getline(inFile, strLine);
			vecContent.push_back(strLine);
		}
		inFile.close();

		// ±éÀú²¢É¾³ýÎ¥½ûÐÐ
		for (auto it = vecContent.begin(); it != vecContent.end(); ++it)
		{
			for(const string& fs:forbidden_string)
				if (it->find(fs) != string::npos)
				{
					vecContent.erase(it);
					break;
				}
		}

		ofstream outFile("test.cpp");
		vector<string>::const_iterator iter = vecContent.begin();
		for (; vecContent.end() != iter; ++iter)
		{
			outFile.write((*iter).c_str(), (*iter).size());
			outFile << '\n';
		}

		outFile.close();
	}
	return 0;
}