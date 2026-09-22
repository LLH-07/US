#include <iostream>

using namespace std;

struct DoB
{
    int day;
    int month;
    int year;
};

struct Student
{
    char *ID;
    char *name;
    DoB dob;
    float score;
};

// Problem 1
struct NodeStu
{
    Student data;
    NodeStu *next;
};

struct List
{
    NodeStu *head;
    NodeStu *tail;
};

// Problem 2
int countStudent(List *LoS)
{
    if (LoS->head == NULL)
        return 0;

    int count = 0;
    NodeStu *curr = LoS->head;
    while (curr != NULL)
    {
        ++count;
        curr = curr->next;
    }

    return count;
}

// Problem 3
void addStudentID(List *&LoS, Student stu)
{
    NodeStu *newStudent = new NodeStu;
    newStudent->data = stu;
    newStudent->next = NULL;

    if (LoS->head == NULL)
    {
        LoS->head = newStudent;
        LoS->tail = newStudent;
        return;
    }

    LoS->tail->next = newStudent;
    LoS->tail = newStudent;
}

// Problem 4
float averageScore(List *LoS)
{
    if (LoS->head == NULL)
        return 0;

    float ave = 0;
    NodeStu *curr = LoS->head;

    while (curr != NULL)
    {
        ave += curr->data.score;
        curr = curr->next;
    }

    ave /= countStudent(LoS);

    return ave;
}

// Problem 5 (GPT)
void XepLoaiSv(List *&LoS)
{
    if (LoS->head == NULL)
        return;

    NodeStu *curr = LoS->head;
    while (curr != NULL)
    {
        if (curr->data.score >= 8)
            cout << "EXCELLENT";

        else if (curr->data.score >= 6.5)
            cout << "GOOD";

        else if (curr->data.score >= 5)
            cout << "BAD";

        else
            cout << "WORSE";

        curr = curr->next;
    }
}

// Problem 6
void saveUnderAveStudent(List *LoS, Student *&under, int &numUnder)
{
    if (LoS->head == NULL)
        return;

    int numStu = countStudent(LoS);
    float ave = averageScore(LoS);
    NodeStu *curr = LoS->head;

    while (curr != NULL)
    {
        if (curr->data.score < ave)
        {
            under[numUnder] = curr->data;
            numUnder++;
        }
        curr = curr->next;
    }
}

// Problem 7
void deleteIDStudent(List *&LoS, char *ID)
{
    if (LoS->head == NULL)
        return;

    if (strcmp(LoS->head->data.ID, ID) == 0)
    {
        NodeStu *del = LoS->head;
        LoS->head = LoS->head->next;
        delete del;
    }

    NodeStu *curr = LoS->head->next;
    NodeStu *prev = LoS->head;

    while (curr != NULL)
    {
        if (strcmp(curr->data.ID, ID) == 0)
        {
            if (curr == LoS->tail)
                LoS->tail = prev;

            else
                prev->next = curr->next;

            delete curr;
            return;
        }
        curr = curr->next;
    }
}

// Problem 8
void sortByID(List *&LoS)
{
    if (LoS->head == NULL)
        return;

    for (NodeStu *i = LoS->head; i != NULL; i = i->next)
        for (NodeStu *j = i->next; j != NULL; j = j->next)
            if (strcmp(i->data.ID, j->data.ID) > 0)
                swap(i->data, j->data);
}

// Problem 9
void sortYear(List *&LoS)
{
    for (NodeStu *i = LoS->head; i != NULL; i = i->next)
        for (NodeStu *j = i->next; j != NULL; j = j->next)
            if (i->data.dob.year > j->data.dob.year)
                swap(i->data, j->data);
}

void sortMonth(List *&LoS)
{
    for (NodeStu *i = LoS->head; i != NULL; i = i->next)
        for (NodeStu *j = i->next; j != NULL; j = j->next)
            if (i->data.dob.year == j->data.dob.year)
                if (i->data.dob.month > j->data.dob.month)
                    swap(i->data, j->data);
}

void sortDay(List *&LoS)
{
    for (NodeStu *i = LoS->head; i != NULL; i = i->next)
        for (NodeStu *j = i->next; j != NULL; j = j->next)
            if (i->data.dob.year == j->data.dob.year && i->data.dob.month == j->data.dob.month)
                if (i->data.dob.day > j->data.dob.day)
                    swap(i->data, j->data);
}

bool compareDob(Student s1, Student s2)
{
    return (s1.dob.day == s2.dob.day && s1.dob.month == s2.dob.month && s1.dob.year == s2.dob.year);
}

void listStudentSameDoB(List *&LoS)
{
    if (LoS->head == NULL)
        return;

    sortYear(LoS);
    sortMonth(LoS);
    sortDay(LoS);

    NodeStu *curr = LoS->head;
    int count;

    while (curr != NULL)
    {
        count = 1;
        while (curr->next != NULL && compareDob(curr->data, curr->next->data))
        {
            ++count;
            if(curr == LoS->head)
                cout << curr->data.name << endl;

            cout << curr->next->data.name << endl;

            curr = curr->next;
        }

        if(curr->next != NULL && !compareDob(curr->data, curr->next->data) && count > 1)
            cout << curr->data.name << endl;

        curr = curr->next;
    }
}
int main()
{
    return 0;
}