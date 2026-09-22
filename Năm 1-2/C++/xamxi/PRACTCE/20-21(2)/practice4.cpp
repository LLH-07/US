#include <iostream>
#include <cmath>

using namespace std;

void convertToBinary(string& binary, long long n)
{
    string temp = "";
    binary = "";
    while(n != 0)
    {
        if(n % 2 == 0)
            temp += '0';
            
        else temp += '1';
        
        n /= 2;
    }
    
    for(int i = temp.size() - 1; i >= 0; i--)
        binary.push_back(temp[i]);
}

void equal2Binary(string& binaryX, string& binaryY, long long x, long long y)
{
    string temp = "";
    if(binaryX.size() < binaryY.size())
    {
        int lim = binaryY.size() - binaryX.size();
        binaryX = "";
        for(int i = 0; i < lim; i++)
            binaryX += '0';
            
        convertToBinary(temp, x);
        binaryX += temp;
    }
    
    else if(binaryY.size() < binaryX.size())
    {
        int lim = binaryX.size() - binaryY.size();
        binaryY = "";
        for(int i = 0; i < lim; i++)
            binaryY += '0';
            
        convertToBinary(temp, y);
        binaryY += temp;
    }
}

string XOR(string binaryX, string binaryY)
{
    string res = "";
    int len = binaryX.size();
    for(int i = 0; i < len; i++)
    {
        if(binaryX[i] == binaryY[i])
            res += '0';
        else
            res += '1';
    }
    
    return res;
}

void convertToDecimal(string binary, long long& decimal)
{
    decimal = 0;
    int len = binary.size();
    for(int i = len - 1; i >= 0; i--)
    {
        int temp;
        if(binary[i] == '1') temp = 1;
        else temp = 0;
        
        decimal += temp * pow(2, len - i - 1);
    }
}

bool pairC(long long x, long long y)
{
    string binaryX, binaryY;
    string res;
    long long decimal, mod;
    
    convertToBinary(binaryX, x);
    convertToBinary(binaryY, y);
    equal2Binary(binaryX, binaryY, x, y);
    res = XOR(binaryY, binaryX);
    convertToDecimal(res, decimal);
    mod = y % x;
    
    if(decimal == mod) 
       return true;
    
   return false;
}

int main()
{
    long long l, r;
    cin >> l >> r;
    
    long long cnt = 0;
    for(long long i = l; i <= r; i++)
    {
       for(long long j = i; j <= r; j++)
           if(pairC(i, j))
               cnt++;
    }
    
    cout << cnt;
    return 0;
}