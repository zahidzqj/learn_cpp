#include <iostream>
#include <cmath>
#include <iomanip>
#include <Windows.h>
using namespace std;

// int main()
// {
//     typedef string my_str;
//     my_str name = "my name is zahid.";
//     size_t a = 255;
//     cout << a <<'\n' ;
//     cout << name << endl;
//     return 0;
// }

// int main()
// {
//     int salary = 255;
//     cout << "my salary is:" <<salary << endl;
//     return 0;
// }


//计算圆柱体积
// int main()
// {
//     const float PI = 3.14; 
//     float radius = 4.5;
//     float hight = 90.0;
//     // double volume = PI*radius*radius*hight;
//     double volume = PI*pow(radius,2)*hight;
//     cout << volume << endl;
//     return 0;
// }

//控制cout显示精度
// int main()
// {
//     //强制以小数形式显示
//     cout << fixed;
//     //显示小数的位数
//     cout << setprecision(2);
//     double dd = 100.0/3;
//     cout << dd << endl;
//     cout << setw(8)<< dd <<endl;
//     cin.get();
//     return 0;
// }

// int main()
// {
//     //设置console title
//     SetConsoleTitle("console name: test");
//     cout << "test" << endl;
//     cin.get();
//     return 0;
// }

int main()
{
    //设置console title
    SetConsoleTitle("console name: test");
    int num,ch,ch2;
    cout << "enter your number" << endl;
    cin >> num;
    cin >> ch>> ch2;
    cout << num << '\t' << ch << '\t' << ch2 << endl;
    cin.get();
    return 0;
}