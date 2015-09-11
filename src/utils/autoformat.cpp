#include "autoformat.h"
#include <string>
#include <sstream>
#include <vector>


using namespace std;
AutoFormat::AutoFormat()
{
}

QString AutoFormat::formatString(QString code)
{
    int pos = 0, count =0, j=0;
    string ch;

    stringstream inText;
    inText << code.toStdString();

    while(inText.good())
    {
        pos++;//return value at end of code, number of lines
        getline(inText,ch);
    }

    vector<string> hold(pos+1);//holds the actual content of the line.
    vector<int> bracketcount(pos+1);//keeps track of number of nested brackets
    inText.clear();
    inText.seekg(0);
    while(getline(inText,hold[count]))
    {
        count++;
    }

    int endline = count;
    int p=0;
    while(p<=endline)
    {
        bracketcount[p]=0;
        p++;
    }
    count = 0;
    int len=0,i = 0, current=0;
    while(count<endline)
    {
        len = hold[count].length();
        i=0;
        while(i<=len)
        {
            if(hold[count][i] == '{')
                {
                    current=count;
                    while(current <endline)
                    {
                        current++;
                        bracketcount[current]++;
                    }
                }
            if(hold[count][i] == '}')
            {
                current=count;
                while(current <endline)
                {
                    bracketcount[current]--;
                    current++;
                }
            }
            i++;
        }
        count++;
    }

    stringstream outText;// consider this a safety net, change to fileName if you trust the code.
                                                  // I'm probably just being overly cautious.
    vector<string> indents(pos+1);
     p=0;
    while(p<endline)
    {
        count=0;
        while(hold[p][count]=='\t')//check if there were already tabs in the file
        {
            count++;
        }
        while(bracketcount[p]>0)//tells indents[p] how many indents should be in a line
        {
            bracketcount[p]--;
            indents[p] +="\t";
        }
        if(count==0)//if the line wasn't previously indented in the code
        {
            outText << indents[p] << hold[p] << endl;
        }
        else if(count>0)//If there were already indents in the code then set them to the right amount
        {
            outText << indents[p];
            len = hold[p].length();
            i=0;
            while(i<len-count)
            {
                outText << hold[p][i+count];
                i++;
            }
            outText << endl;
        }
        p++;
    }

    return QString::fromStdString(outText.str());
}
