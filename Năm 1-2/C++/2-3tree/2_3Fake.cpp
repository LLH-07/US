#include <iostream>
#include<vector>
#include<algorithm>

using namespace std;

struct Node 
{
	int valLeft, valRight;
	Node* pLeft, * pMid, * pRight;
	Node();
	Node(int x);
	Node(int x, Node* left, Node* right);
};

pair<int, pair<Node*, Node*>> insertNode(Node*& pCur, int x);
Node* searchTree(Node* pCur, int x);

Node::Node()
{
	valLeft = valRight = -1;
	pLeft = pMid = pRight = nullptr;
}
Node::Node(int x)
{
	valLeft = x;
	valRight = -1;
	pLeft = pMid = pRight = nullptr;
}
Node::Node(int x, Node* left, Node* right)
{
	valLeft = x;
	pLeft = left;
	pRight = right;
	pMid = nullptr;
}
pair<int, pair<Node*, Node*>> insertNode(Node*& pCur, int x)
{
	if (!(pCur->pLeft || pCur->pMid || pCur->pRight))
	{
		if (pCur->valRight == -1)
		{
			pCur->valRight = x;
			if (pCur->valLeft > pCur->valRight) swap(pCur->valLeft, pCur->valRight);
			return { -1, {nullptr, nullptr} };
		}
		else
		{
			vector<int> z;
			z.push_back(pCur->valLeft); z.push_back(pCur->valRight); z.push_back(x);
			sort(z.begin(), z.end());
			Node* newLeft = new Node();
			Node* newRight = new Node();
			newLeft->valLeft = z[0]; newRight->valLeft = z[2];
			return { z[1], {newLeft, newRight} };
		}
	}

	pair<int, pair<Node*, Node*>> data;

	if (x < pCur->valLeft)
		data = insertNode(pCur->pLeft, x);
	else if ((pCur->valRight != -1 && x > pCur->valRight) || (pCur->valRight == -1 && x > pCur->valLeft))
		data = insertNode(pCur->pRight, x);
	else
		data = insertNode(pCur->pMid, x);

	if (data.first == -1) return { -1, {nullptr, nullptr} };

	if (pCur->valRight == -1)
	{
		pCur->valRight = data.first;
		if (pCur->valLeft > pCur->valRight) swap(pCur->valLeft, pCur->valRight);

		if (x < pCur->valLeft)
		{
			pCur->pLeft = data.second.first;
			pCur->pMid = data.second.second;
		}
		else 
		{
			pCur->pMid = data.second.first;
			pCur->pRight = data.second.second;
		}
		return { -1, {nullptr, nullptr} };
	}
	vector<int> z;
	z.push_back(pCur->valLeft); z.push_back(pCur->valRight); z.push_back(data.first);
	sort(z.begin(), z.end());
	Node* newLeft = new Node();
	Node* newRight = new Node();
	if (x < pCur->valLeft)
	{
		newLeft = new Node(z[0], data.second.first, data.second.second);
		newRight = new Node(z[2], pCur->pMid, pCur->pRight);
	}
	else if ((pCur->valRight != -1 && x > pCur->valRight) || (pCur->valRight == -1 && x > pCur->valLeft))
	{
		newLeft = new Node(z[0], pCur->pLeft, pCur->pMid);
		newRight = new Node(z[2], data.second.first,data.second.second);
	}
	else
	{
		newLeft = new Node(z[0], pCur->pLeft, data.second.first);
		newRight = new Node(z[2], data.second.second, pCur->pLeft);
	}
	return { z[1], {newLeft, newRight} };
}

Node* searchTree(Node* pCur, int x)
{
	if (!pCur) return nullptr;
	if (pCur->valLeft == x || pCur->valRight == x) return pCur;

	if (x < pCur->valLeft) return searchTree(pCur->pLeft, x);
	if ((pCur->valRight != -1 && x > pCur->valRight) || (pCur->valRight == -1 && x > pCur->valLeft)) return searchTree(pCur->pRight, x);

	return searchTree(pCur->pMid, x);
}

void displayTree(Node* pCur)
{
	if (!pCur) return;
	cout << pCur->valLeft << ' ' << pCur->valRight << ": ";
	if (pCur->pLeft) cout << "|L " << pCur->pLeft->valLeft << ' ' << pCur->pLeft->valRight << "| ";
	if (pCur->pMid) cout << "|M " << pCur->pMid->valLeft << ' ' << pCur->pMid->valRight << "| ";
	if (pCur->pRight) cout << "|R " << pCur->pRight->valLeft << ' ' << pCur->pRight->valRight << "| ";
	cout << '\n';

	displayTree(pCur->pLeft);
	displayTree(pCur->pMid);
	displayTree(pCur->pRight);
	return;
}


int main(int argc, char** argv)
{
	ios::sync_with_stdio(false);
	cin.tie(0);
	cout.tie(0);
	Node* pRoot = nullptr;
	int x = 0;
	while (x < 20) 
	{
		if (searchTree(pRoot, x) != nullptr) continue;

		if (pRoot == nullptr) 
		{
			pRoot = new Node(x);
		}
		else 
		{
			pair<int, pair<Node*, Node*>> data = insertNode(pRoot, x);
			if (data.first == -1) {} 
			else 
			{
				pRoot->valLeft = data.first; pRoot->valRight = -1;
				pRoot->pLeft = data.second.first;
				pRoot->pMid = nullptr;
				pRoot->pRight = data.second.second;
			}
		}
		x += 1;
	}
	displayTree(pRoot);
}