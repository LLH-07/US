#include <iostream>

using namespace std;

struct Player
{
    string name;
    int numAssists;
    int numGoals;
};

//void inputPlayer(int& num, Player cauthu[])

void outputAvailable(Player cauthu[])
{
    cout << "Name                 numGoals     numAssits" << endl;
    cout << "-------------------------------------------" << endl;
    cout << "Lionel Messi             -            -    " << endl;
    cout << "Antonie Griezmann        -            -    " << endl;

    cauthu[0].name = "Lionel Messi";
    //cauthu[0].numGoals = 28;
    //cauthu[0].numAssists = 8;

    cauthu[1].name = "Antonie Griezmann";
    //cauthu[1].numGoals = 12;
    //cauthu[1].numAssists = 6;

    cout << endl;
}

void addPlayer(int& num, Player cauthu[])
{
    cout << "NEW PLAYER: \n";
    cout << "Name: ";
    //cin.ignore();
    getline(cin, cauthu[num].name);
    num++;
    cout << endl;
}

int findPos(int num, Player cauthu[], string namePlayer)
{
    int pos = -1;
    for(int i = 0; i < num; i++)
    {
        if(namePlayer == cauthu[i].name)
        {
            pos = i;
        }
    }
    return pos;
}

void deletePlayer(int& num, Player cauthu[])
{
    string namePlayer;
    cout << "Delete player: \n";
    cout << "Name: ";
    //cin.ignore();
    getline(cin, namePlayer);
    //cout << "delete " << namePlayer << endl;
    int pos = findPos(num, cauthu, namePlayer);
    if(pos == -1) cout << "There is no player!\n";
    else
    {
        for(int i = pos; i < num - 1; i++)
        {
            cauthu[i].name = cauthu[i + 1].name;
            cauthu[i].numGoals = cauthu[i + 1].numGoals;
            cauthu[i].numAssists = cauthu[i + 1].numAssists;
        }
        num--;
    }
    cout << endl;
}

void inputGoalAssit(int num, Player cauthu[])
{
    for(int i = 0; i < num; i++)
    {
        cout << "Input Goals & Assists: \n";
        cout << cauthu[i].name << endl;
        cout << "Number of Goals: ";
        cin >> cauthu[i].numGoals;
        cout << "Number of Assits: ";
        cin >> cauthu[i].numAssists;
        cout << endl;
    }
}
void caculateScore (int num, Player cauthu[], float score[])
{
    for(int i = 0; i < num; i++)
    {
        score[i] = cauthu[i].numGoals * 0.6 + cauthu[i].numAssists * 0.4;
    }
}

int isBestPlayer(int num, Player cauthu[], float score[])
{
    float mx = score[0];
    int pos;
    for(int i = 1; i < num; i++)
    {
        if(mx < score[i])
        {
            mx = score[i];
            pos = i;
        }
    }
    return pos;
}

void Output(int num, Player cauthu[], float score[])
{
    string s = "Name                 numGoals     numAssits";
    cout << s << endl;
    cout << "-------------------------------------------" << endl;
    for(int i = 0; i < num; i++)
    {
        int j = cauthu[i].name.length();
        cout << cauthu[i].name;

        while(s[j] == ' ')
        {
            cout << ' ';
            j++;
            if(s[j] != ' ')
            {
                j += 7;
                //if(cauthu[i].numGoals / 10 == 0) j++;
                break;
            }
        }
        cout << cauthu[i].numGoals;
        for(int k = 0; k < 6; k++) cout << ' ';
        if(cauthu[i].numGoals / 10 == 0) cout << ' ';

        while(s[j] == ' ')
        {
            cout << ' ';
            j++;
        }
        for(int k = 0; k < 5; k++) cout << ' ';
        cout << cauthu[i].numAssists;
        cout << endl;
    }

    int pos = isBestPlayer(num, cauthu, score);
    cout << cauthu[pos].name << " is the BEST PLAYER!" << endl;
}

int main()
{
    Player cauthu[200];
    int num = 2;
    float score[200] = {};
    outputAvailable(cauthu);
    addPlayer(num, cauthu);
    deletePlayer(num, cauthu);
    inputGoalAssit(num, cauthu);
    caculateScore(num, cauthu, score);
    isBestPlayer(num, cauthu, score);
    Output(num, cauthu, score);
}
