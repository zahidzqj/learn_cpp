#include <iostream>		//预处理命令
using namespace std;	//使用命名空间std
int main()				//主函数首部
{						//函数体开始
	int a,b,sum;			//定义变量
	cin>>a>>b;				//输入语句
	sum=a+b;				//赋值语句
	cout <<"a+b="<<sum<<endl;	//输出语句（endl是end line的缩写，表示行结束，与“\n”作用相同）
	system("pause");		
	return 0;				//如程序正常结束，向操作系统返回一个零值
}