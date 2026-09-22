#include <iostream>
#include <fstream>
#include <iomanip>
#define M 1000

using namespace std;

struct footballPlayer
{   
    string pos; // DF FW GK MF
    string name;
    int birthYear;
    string nationalTeam;
    string club;
};

footballPlayer player [M];
int num = 0;
int n94 = 0;
int nMU = 0;
int nFW = 0;

//////////////////////////////////////////////////////////////////////////////////////////////

void readFile()
{

    ifstream fin;
    fin.open( "Players.txt" );

    if ( !fin.is_open() )
    {
        cout << "Can't read file! Please try again!";
        return;
    }

    //fin.ignore();

    while ( !fin.eof() )
    {
        
        string Ignore = "";
        getline (fin, player[num].pos, '/');
        getline (fin, player[num].name, '/');
        fin >> player[num].birthYear;
        getline(fin, Ignore, '/');
        getline (fin, player[num].nationalTeam, '/');
        getline (fin, player[num].club, '\n');

        num++;
    }

    fin.close();

}

//////////////////////////////////////////////////////////////////////////////////////////////

void write1994File( footballPlayer player_94[M] )
{
    ofstream COUT;
    COUT.open("Player_1994.txt");
    if ( !COUT.is_open() )
    {
        cout << "Can't open file!";
        return;
    }

    COUT << setw(10) << "Position"
         << setw(20) << "Name" 
         << setw(20) << "Year Of Birth"
         << setw(20) << "National Team" 
         << setw(26) << "Club\n";

    for ( int i = 0; i < n94; i++ )
    {
        COUT << setw(10) << player_94[i].pos
             << setw(20) << player_94[i].name
             << setw(20) << player_94[i].birthYear
             << setw(20) << player_94[i].nationalTeam
             << setw(25) << player_94[i].club
             << '\n';
    }

    COUT.close();
}

void isBornIn1994( footballPlayer player_94[M] )
{
    for ( int i = 0; i < num; i++ )
    {
        if ( player[i].birthYear == 1994 )
        {

            player_94[n94].pos = player[i].pos;
            player_94[n94].name = player[i].name;
            player_94[n94].birthYear = player[i].birthYear;
            player_94[n94].nationalTeam = player[i].nationalTeam;
            player_94[n94].club = player[i].club;

            n94++;
        }
    }

    write1994File( player_94 );
}

//////////////////////////////////////////////////////////////////////////////////////////////

void writeMUFile( footballPlayer MUplayer[M] )
{
    ofstream COUT;
    COUT.open("MU_players.txt");
    if ( !COUT.is_open() )
    {
        cout << "Can't open file!";
        return;
    }

    COUT << setw(10) << "Position"
         << setw(20) << "Name" 
         << setw(20) << "Year Of Birth"
         << setw(20) << "National Team" 
         << setw(26) << "Club\n";

    for ( int i = 0; i < nMU; i++ )
    {
        COUT << setw(10) << MUplayer[i].pos
             << setw(20) << MUplayer[i].name
             << setw(20) << MUplayer[i].birthYear
             << setw(20) << MUplayer[i].nationalTeam
             << setw(25) << MUplayer[i].club
             << '\n';
    }

    COUT.close();
}

void playForMUClub( footballPlayer MUplayer[M])
{
    for ( int i = 0; i < num; i++ )
    {
        if ( player[i].club == "Manchester United" )
        {

            MUplayer[nMU].pos = player[i].pos;
            MUplayer[nMU].name = player[i].name;
            MUplayer[nMU].birthYear = player[i].birthYear;
            MUplayer[nMU].nationalTeam = player[i].nationalTeam;
            MUplayer[nMU].club = player[i].club;

            nMU++;
        }
    }

     writeMUFile( MUplayer );
}

//////////////////////////////////////////////////////////////////////////////////////////////

void writeFWFile( footballPlayer FWplayer[M] )
{
    ofstream COUT;
    COUT.open("FW_ManchesterCity.txt");
    if ( !COUT.is_open() )
    {
        cout << "Can't open file!";
        return;
    }

    COUT << setw(10) << "Position"
         << setw(20) << "Name" 
         << setw(20) << "Year Of Birth"
         << setw(20) << "National Team" 
         << setw(26) << "Club\n";

    for ( int i = 0; i < nFW; i++ )
    {
        COUT << setw(10) << FWplayer[i].pos
             << setw(20) << FWplayer[i].name
             << setw(20)<< FWplayer[i].birthYear
             << setw(20) << FWplayer[i].nationalTeam
             << setw(25) << FWplayer[i].club
             << '\n';
    }

    COUT.close();
}

void isFWMCity ( footballPlayer FWplayer[M] )
{
    for ( int i = 0; i < num; i++ )
    {
        if ( player[i].pos == "FW")
        {
            if ( player[i].club == "Manchester City" )
            {

                FWplayer[nFW].pos = player[i].pos;
                FWplayer[nFW].name = player[i].name;
                FWplayer[nFW].birthYear = player[i].birthYear;
                FWplayer[nFW].nationalTeam = player[i].nationalTeam;
                FWplayer[nFW].club = player[i].club;

                nFW++;
            }
        }
    }

    writeFWFile( FWplayer );
}

//////////////////////////////////////////////////////////////////////////////////////////////

void isAscendingYear()
{
    for ( int i = 0; i < num; i++ )
        for( int j = 0; j < num - i - 1; j++ )
            if ( player[j].birthYear < player[j + 1].birthYear )
                swap(player[j], player[j + 1]);
}

bool isAlphabet(char s)
{
    if ( ( ( 'a' <= s ) && ( s <= 'z' ) ) || ( ( 'A' <= s ) && ( s <= 'Z' ) ) )
        return true;

    return false;
}

int countWord (string s)
{
    int i = 0;
    while ( s[i] == ' ' )
        i++;

    int len = s.length();
    int words = 1;

    while ( i < len - 1 )
    {
        if ( ( s[i] == ' ' ) && ( s[i + 1] != ' ' ) )
            words++;

        i++;
    }

    return words;
}

string lastName( string name )
{
    string last = "";
    int len = name.length();
    int i = 0;

    if ( countWord (name) == 1 )
        return name;

    else
    {
        int count = 0;
        while ( count < countWord(name) - 1 )
        {
            if ( name[i] == ' ' )
                count++;
            
            i++;
        }

        while ( i < len )
        {
            last += name[i];
            i++;
        }
    }
    
    return last;
}

int chooseLength (int len1, int len2)
{
    if ( len1 > len2 )
        return len2;

    else
        return len1;
}


void sortName(int start, int end)
{
    int e = end;
    for ( int k = start; k < end; k++ )
    {
        for ( int i = start; i < e - 1; i++ )
        {
                
            string last1, last2;
            last1 = lastName(player[i].name);
            last2 = lastName(player[i + 1].name);

            
            int len1 = last1.length();
            int len2 = last2.length();
            int len = chooseLength(len1, len2);
            
            
            
            int j = 0;
            if ( last1[j] > last2[j] )
                swap(player[i], player[i + 1]);
        
            else if ( last1[j] == last2[j] )
            {
                while ( last1[j] <= last2[j] && j < len )
                    j++;

                if ( last1[j] > last2[j] )
                    swap(player[i], player[i + 1]);

            }

        }
        --e;
    }                                                                      
}

void isAscendingLastName()
{
    for ( int i = 0; i < num; i++ )
    {
        int year = player[i].birthYear;
        int j = i + 1;
        while ( ( player[j].birthYear == year ) && ( j < num ) )
            j++;

        sortName(i, j);
        j--;
        i = j;
    }
}

void writeSortedList()
{
    isAscendingYear();
    isAscendingLastName();

    ofstream COUT;
    COUT.open("SortedPlayers.txt");
    if ( !COUT.is_open() )
    {
        cout << "Can't open file!";
        return;
    }

    COUT << setw(15) << "Position"
         << setw(25) << "Name" 
         << setw(25) << "Year Of Birth"
         << setw(25) << "National Team" 
         << setw(31) << "Club\n";

    for ( int i = 0; i < num; i++ )
    {
        COUT << setw(15) << player[i].pos
             << setw(25) << player[i].name
             << setw(25) << player[i].birthYear
             << setw(25) << player[i].nationalTeam
             << setw(30) << player[i].club
             << '\n';
    }

    COUT.close();
}

//////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    footballPlayer player_94[M];
    footballPlayer MUplayer[M];
    footballPlayer FWplayer[M];

    readFile();
    isBornIn1994(player_94);
    playForMUClub(MUplayer);
    isFWMCity(FWplayer);
    writeSortedList();

    return 0;
}