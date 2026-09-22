#include <iostream>
#include <vector>
#include <queue>

using namespace std;

vector<int> node[10001];
bool visited[10001];

void DFS(int v)
{
    visited[v] = true;
    for(int i = 0; i < node[v].size(); i++)
        if(!visited[node[v][i]])
            DFS(node[v][i]);
}

void forDFSinsert()
{
    int nV, nE;
    cout << "Number of vertex: ";
    cin >> nV;
    cout << "Number of edge: ";
    cin >> nE;


    for(int i = 0; i < nV; i++)
        visited[i] = false;

    // Store path / edge between v and u;
    for(int i = 0; i < nE; i++)
    {
        int u, v;
        cout << "\nFirst vertex: ";
        cin >> u;
        cout << "\nSecond vertex: ";
        cin >> v;

        node[u].push_back(v);
        node[v].push_back(u);
    }

    cout << "Start point: ";
    int start;
    cin >> start;
    // ...
}

void BFS(int v)
{
    queue<int>  q;

    q.push(v);
    visited[v] = true;

    while(!q.empty())
    {
        int w = q.front();
        q.pop();
        for(int i = 0; i < node[w].size(); i++)
        {
            if(!visited[node[w][i]])
            {
                visited[node[w][i]] = true;
                q.push(node[w][i]);
            }
        }
    }
}

int main()
{
    return 0;
}
