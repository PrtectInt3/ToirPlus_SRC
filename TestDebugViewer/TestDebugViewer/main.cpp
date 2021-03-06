#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

int* ConvertBinaryString(string inputString);
string ConvertInt2String(int * inputArr);
void PrintArray(int *inputArr, string inputStr);
void PrintArray2SourceFile(int *inputArr, string inputStr, std::ofstream &fileOut);
void Print2HeaderFile(string inputStr, std::ofstream &fileOut);
vector<string> LoadFile2ProccessData(char* szInputFile);
void ProcessData2File(char* szOutputFile, char* szInputFile);
string NormalizeStr(string strInput);

int* ConvertBinaryString(string inputString)
{
	int key[3] = {13, 26 , 39};
	//char key[3] = {'K', 'C', 'Q'};
	int x = inputString.size();
	int *output = new int[x + 1];
	for (int i = 0; i < x; ++i)
	{
		*(output + i) = int(inputString.at(i));
		//*(output + i) = (((int(inputString.at(i)) + *output)+13)*7)^key[i%(sizeof(key)/sizeof(int))];
		//*(output + i) = ((int(inputString.at(i))+13)*7)^3 + *output;
		//cout << inputString.at(i) << " ---> " << int(inputString.at(i)) << "--->" << *(output + i) << endl;
	}
	*(output + x) = -1;
	return output;
}
string ConvertInt2String(int * inputArr)
{
	//int key[3] = {13, 26 , 39};
	////char key[3] = {'K', 'C', 'Q'};
	//string result = "";
	//result.insert(0, 1, *inputArr);
	//for (int i = 1;inputArr[i] != -1; ++i)
	//{
	//	char ch =  char( ((inputArr[i])^key[i%(sizeof(key)/sizeof(int))])/7-13-(*inputArr) );
	//	//char ch =  char( ( ( (inputArr[i])-(*inputArr) )^3 )/7-13);
	//	result.insert(i, 1, ch);
	//}
	//return result;
	int key[3] = {13, 26 , 39};
	//char key[3] = {'K', 'C', 'Q'};
	std::string ketQua = "";
	ketQua.insert(0, 1, *inputArr);
	for (int i = 1;inputArr[i] != -1; ++i)
	{
		char ch = char (inputArr[i]);
		//char ch =  char( ((inputArr[i])^key[i%(sizeof(key)/sizeof(int))])/7-13-(*inputArr) );
		//char ch =  char( ( ( (inputArr[i])-(*inputArr) )^3 )/7-13);
		ketQua.insert(i, 1, ch);
	}
	char * tmp = new char[ketQua.size() + 1];
	int xxx = ketQua.size();
	strcpy(tmp, ketQua.c_str());
	ketQua.clear();
	return tmp;
}
void PrintArray(int *inputArr, string inputStr)
{
	int i = 0;
	inputStr = string("str__").append(inputStr.c_str());
	cout << "char " << inputStr.c_str() << "[] = {";
	do 
	{
		cout << inputArr[i] << ", ";
		++i;
	} while (inputArr[i] != -1);
	cout << "0};";
	cout << endl;
}
void PrintArray2SourceFile(int *inputArr, string inputStr, std::ofstream &fileOut)
{
	int i = 0;
	inputStr = string("sz__").append(NormalizeStr(inputStr).c_str());
	fileOut << "char " << inputStr.c_str() << "[] = {";
	do 
	{
		fileOut << inputArr[i] << ", ";
		++i;
	} while (inputArr[i] != -1);
	fileOut << "0};";
	fileOut << "\n";
}
void Print2HeaderFile(string inputStr, std::ofstream &fileOut)
{
	inputStr = string("sz__").append(NormalizeStr(inputStr).c_str());
	fileOut << "extern char " << inputStr.c_str() << "[];";
	fileOut << "\n";
}
vector<string> LoadFile2ProccessData(char* szInputFile)
{
	std::ifstream file2Read(szInputFile, ios::in);
	std::string strLine;
	vector<std::string> lines;
	if (file2Read.is_open())
	{
		while ( getline (file2Read,strLine) )
		{
			cout << strLine.size() << strLine.c_str() << '\n';
			if (strLine.size())
				lines.push_back(strLine);
		}
		file2Read.close();
	}
	else cout << "Unable to open file";
	return lines;
}
void ProcessData2File(char* szOutputFile, char* szInputFile)
{
	std::ofstream fileSource(string(szOutputFile).append(".cpp").c_str(), ios::out);
	std::ofstream fileHeader(string(szOutputFile).append(".h").c_str(), ios::out);
	vector<string> lines = LoadFile2ProccessData(szInputFile);
	int *arrInt;
	for (int i = 0; i < lines.size(); ++i)
	{
		arrInt = ConvertBinaryString(lines.at(i));
		PrintArray2SourceFile(arrInt, lines.at(i), fileSource);
		Print2HeaderFile( lines.at(i), fileHeader);
		delete []arrInt;
	}
	lines.clear();
	fileSource.close();
	fileHeader.close();
}
string NormalizeStr(string strInput)
{
	int x = strInput.size();
	for (int i = 0; i < x; ++i)
	{
		if (strInput.compare(i, 1, "=") == 0) // =
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, "+") == 0) // +
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, "-") == 0) // -
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, "*") == 0)// *
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, "/") == 0) // /
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, "\\") == 0) // 
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, ">") == 0) // >
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, "<") == 0) // <
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, ":") == 0) // :
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, "?") == 0) // ?
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, ".") == 0) // .
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, ",") == 0) // ,
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, "~") == 0) // ~
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, "!") == 0) // !
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, "@") == 0) // @
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, "#") == 0) //#
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, "$") == 0) // $
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, "%") == 0) // %
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, "^") == 0) // ^
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, "&") == 0) // &
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, " ") == 0) // dau cach
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, "(") == 0) // (
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, ")") == 0) // )
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, "[") == 0) // [
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, "]") == 0) // ]
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, "{") == 0) // {
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, "}") == 0) // }
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, "'") == 0) // dau nhay don
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, "\"") == 0) // dau nhay kep
			strInput.replace(i, 1, 1, '_');
		else if (strInput.compare(i, 1, "	") == 0) // dau tab
			strInput.replace(i, 1, 1, '_');
	}
	return strInput;
}
void main(int n, char* args[])
{
	char* szInputFileName = "DataString.txt";
	char* szOutputFileName = "DataString";
	if (n == 2) szInputFileName = args[1];
	if (n >= 3)
	{
		szInputFileName = args[1];
		szOutputFileName = args[2];
	}
	ProcessData2File(szOutputFileName, szInputFileName);
	char * strRaw = "abcdef";
	int * a = ConvertBinaryString(strRaw);
	PrintArray(a, strRaw);
	cout << "-----------------------" << endl;
	string blll = ConvertInt2String(a);
	cout << "xxxx:" << blll.c_str() << endl;
	//cin.ignore();
}