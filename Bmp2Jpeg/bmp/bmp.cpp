// bmp.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Bmp2Jpeg.h"
#include <iostream>
#include <ctime>
#include <string> 

using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
	cout << "-------OPEM  BMP NAME == " << argc<< ".bmp-------------" << endl;
	for (int i = 1; i <= 100;i++)
	{
		string str_saveBmp =  to_string(argc);
		string str_openBmp=to_string(argc);
		clock_t start, finish;
		CBmp2Jpeg bmp;

		str_openBmp += ".bmp";
	    str_saveBmp += "-"+to_string(i);
		str_saveBmp = str_saveBmp + ".jpg";
		cout << "------------------------------" << endl;
		cout << "---------------BEGIN----------------" << endl;
		cout << "-------------NO." << i<<"---------------"<<endl;
		start = clock();

		bmp.Bmp2Jpeg(str_openBmp.c_str(), str_saveBmp.c_str());
	
		finish = clock();

		double time = (double)(finish - start) / CLK_TCK;
		cout << "The used time is:" << time << endl;
		cout << "---------------END----------------\n" << endl;
	}
	system("pause");
	return 0;
}

