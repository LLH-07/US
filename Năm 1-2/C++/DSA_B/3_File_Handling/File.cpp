#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

struct Examinee
{
    string id;
    float math, literature, physic, chemistry, biology, history, geography, civic_education, natural_science, social_science, foreign_language;
    string note;
    string province;
};

Examinee readExaminee(string line_info)
{
    Examinee e;
    stringstream ss;
    char ignoreChar;

    for (int i = 0; i < line_info.length(); i++)
    {
        if (line_info[i] != ',')
            ss << line_info[i];

        else if (line_info[i] == ',' && line_info[i + 1] == ',')
            ss << " 0 ";

        else
            ss << " ";
    }

    getline(ss, e.id, ' ');
    ss >> ignoreChar;
    ss >> e.math >> e.literature >> e.physic >> e.chemistry >> e.biology >> e.history >> e.geography >> e.civic_education >> e.natural_science >> e.social_science >> e.foreign_language;
    ss >> e.note;
    ss.ignore();
    ss >> e.province;

    return e;
}

vector<Examinee> readExamineeList(string file_name)
{
    ifstream in(file_name);
    if (!in.is_open())
    {
        cout << "File could not be opened!\n";
        exit(1);
    }

    vector<Examinee> e;
    string ignoreLine;

    getline(in, ignoreLine);

    while (!in.eof())
    {
        Examinee tmp;
        string line;

        getline(in, line);
        tmp = readExaminee(line);
        e.push_back(tmp);
    }

    in.close();

    return e;
}

float NaturalScienceSum(Examinee e)
{
    return e.physic + e.chemistry + e.biology;
}

float SocialScienceSum(Examinee e)
{
    return e.history + e.geography + e.civic_education;
}

void writeTotal(vector<Examinee> examinee_list, string out_file_name)
{
    ofstream out(out_file_name);

    for(int i = 0; i < examinee_list.size(); i++)
    {
        float total;
        total = NaturalScienceSum(examinee_list[i]) + SocialScienceSum(examinee_list[i]) + examinee_list[i].math + examinee_list[i].literature + examinee_list[i].foreign_language;
        out << examinee_list[i].id << ' ' << total << endl;
    }

    out.close();
}

int main()
{
    vector<Examinee> e;

    e = readExamineeList("data.txt");

    cout << e[1].id << endl
         << e[1].math << endl
         << e[1].literature << endl
         << e[1].physic << endl
         << e[1].chemistry << endl
         << e[1].biology << endl
         << e[1].history << endl
         << e[1].geography << endl
         << e[1].civic_education << endl
         << e[1].natural_science << endl
         << e[1].social_science << endl
         << e[1].foreign_language << endl
         << e[1].note << endl
         << e[1].province;
}