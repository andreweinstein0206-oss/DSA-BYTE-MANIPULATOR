#include <iostream>
#include <fstream>
using namespace std;





struct HuffmanNode
{
    char data;
    int frequency;

    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(char x, int f)
    {
        data = x;
        frequency = f;
        left = NULL;
        right = NULL;
    }
};




struct PriorityQueue
{
    HuffmanNode* nodes[256];
    int size;

    PriorityQueue()
    {
        size = 0;
    }

    void enqueue(HuffmanNode* node)
    {
        nodes[size] = node;
        size++;

        int i = size - 1;

        while (i > 0 && nodes[i]->frequency < nodes[i - 1]->frequency)
        {
            HuffmanNode* temp = nodes[i];
            nodes[i] = nodes[i - 1];
            nodes[i - 1] = temp;

            i--;
        }
    }







    HuffmanNode* dequeue()
    {
        if (size == 0)
        {
            return NULL;
        }

        HuffmanNode* first = nodes[0];

        for (int i = 0; i < size - 1; i++)
        {
            nodes[i] = nodes[i + 1];
        }

        size--;

        return first;
    }
};








void displayFrequencies(int frequency[])
{
    cout << "\nCharacter Frequencies\n";
    cout << "---------------------\n";

    for (int i = 0; i < 256; i++)
    {
        if (frequency[i] > 0)
        {
            if (i == ' ')
            {
                cout << "[SPACE]";
            }
            else if (i == '\n')
            {
                cout << "[NEWLINE]";
            }
            else
            {
                cout << "'" << char(i) << "'";
            }

            cout << " : " << frequency[i] << endl;
        }
    }
}








void displayTree(HuffmanNode* root, string code)
{
    if (root == NULL)
    {
        return;
    }

    if (root->left == NULL && root->right == NULL)
    {
        if (root->data == ' ')
        {
            cout << "[SPACE]";
        }
        else if (root->data == '\n')
        {
            cout << "[NEWLINE]";
        }
        else
        {
            cout << root->data;
        }

        cout << " : " << root->frequency << endl;

        return;
    }








    displayTree(root->left, code + "0");
    displayTree(root->right, code + "1");
}

HuffmanNode* buildHuffmanTree(PriorityQueue& queue)
{
    while (queue.size > 1)
    {
        HuffmanNode* first = queue.dequeue();
        HuffmanNode* second = queue.dequeue();

        HuffmanNode* newNode = new HuffmanNode(
            '\0',
            first->frequency + second->frequency
        );

        newNode->left = first;
        newNode->right = second;

        queue.enqueue(newNode);
    }

    return queue.dequeue();
}








int main()
{
    ifstream file("Ratfiles.txt");
    if (!file)
    {
        cout << "Error: Ratfiles.txt not found." << endl;
        return 1;
    }

    int frequency[256] = {0};
    char ch;
    while (file.get(ch))
    {
        frequency[(unsigned char)ch]++;
    }
    file.close();


    cout << "____________________________" << endl;
    cout << "=== HUFFMAN CODING PROJECT ===" << endl;
    cout << "____________________________" << endl;

    cout << "\nFile read successfully." << endl;

    displayFrequencies(frequency);

    PriorityQueue queue;

    for (int i = 0; i < 256; i++)
    {
        if (frequency[i] > 0)
        {
            HuffmanNode* newNode =
                new HuffmanNode(char(i), frequency[i]);

            queue.enqueue(newNode);
        }
    }

    if (queue.size == 0)
    {
        cout << "\nThe file is empty." << endl;
        return 0;
    }

    HuffmanNode* root = buildHuffmanTree(queue);

    cout << "\nHuffman Tree created successfully." << endl;

    cout << "\nTree Characters:" << endl;
    cout << "----------------" << endl;

    displayTree(root, "");



































    return 0;
}
