#include "convert.h"
string inttostring(int tempint){
    char t[256];
    string s;
    sprintf(t, "%d", tempint);
    s = t;
    return s;
}

string doubletostring(double d)
{
    char t[256];
    string s;
    sprintf(t, "%lf", d);
    s = t;
    return s;
}

string floattostring(float f)
{
    char t[256];
    string s;
    sprintf(t, "%f", f);
    s = t;
    return s;
}

string qstringtostring(QString qs)
{
    return qs.toStdString();
}

string booltostring(bool b)
{
    if(b) return "T";
    else return "F";
}

int stringtoint(string s)
{
    return stoi(s);
}

float stringtofloat(string s)
{
    return stof(s);
}

double stringtodouble(string s)
{
    return stod(s);
}

QString stringtoqstring(string s)
{
    return QString::fromStdString(s);
}

bool stringtobool(string s)
{
    if(s=="T") return true;
    else return false;
}

QString inttoqstring(int i)
{
    return stringtoqstring(inttostring(i));
}

QString doubletoqstring(double d)
{
    return stringtoqstring(doubletostring(d));
}

QString floattoqstring(float f)
{
    return stringtoqstring(floattostring(f));
}

QString booltoqstring(bool b)
{
    return stringtoqstring(booltostring(b));
}

int qstringtoint(QString qs)
{
    return stringtoint(qstringtostring(qs));
}

float qstringtofloat(QString qs)
{
    return stringtofloat(qstringtostring(qs));
}

double qstringtodouble(QString qs)
{
    return stringtodouble(qstringtostring(qs));
}

bool qstringtobool(QString qs)
{
    return stringtobool((qstringtostring(qs)));
}

void split(const string& src, const string& separator, vector<string>& dest)
{
    string str = src;
    string substring;
    string::size_type start = 0, index;
    do
    {
        index = str.find_first_of(separator,start);
        if (index != string::npos)
        {
            substring = str.substr(start,index-start);
            dest.push_back(substring);
            start = str.find_first_not_of(separator,index);
            if (start == string::npos) return;
        }
    }while(index != string::npos);

    //the last token
    substring = str.substr(start);
    dest.push_back(substring);
}
