#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

using namespace std;

const string LINE = "============================================================";
const string DASH = "------------------------------------------------------------";


// ============================================================
// NODE
// ============================================================

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


// ============================================================
// PRIORITY QUEUE (sorted by frequency, lowest first)
// ============================================================

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


// ============================================================
// READ TEXT FILE + CHARACTER FREQUENCY ANALYSIS
// ============================================================

bool readFrequencies(string filename, int frequency[])
{
    ifstream file(filename.c_str(), ios::binary);
    if (!file)
    {
        return false;
    }

    char ch;
    while (file.get(ch))
    {
        frequency[(unsigned char)ch]++;
    }

    file.close();
    return true;
}


string charLabel(int i)
{
    if (i == ' ')  return "[SPACE]";
    if (i == '\n') return "[NEWLINE]";
    if (i == '\r') return "[CR]";
    if (i == '\t') return "[TAB]";

    if (i < 32 || i > 126)
    {
        return "[ASCII " + to_string(i) + "]";
    }

    return string("'") + char(i) + "'";
}

void displayFrequencies(int frequency[])
{
    cout << "\n" << LINE << "\n";
    cout << "                 CHARACTER FREQUENCIES\n";
    cout << LINE << "\n";

    cout << left << setw(20) << "Character"
         << right << setw(15) << "Frequency" << endl;
    cout << DASH << "\n";

    for (int i = 0; i < 256; i++)
    {
        if (frequency[i] > 0)
        {
            cout << left << setw(20) << charLabel(i)
                 << right << setw(15) << frequency[i] << endl;
        }
    }

    cout << LINE << "\n";
}


// ============================================================
// HUFFMAN TREE CONSTRUCTION
// ============================================================

HuffmanNode* buildHuffmanTree(int frequency[])
{
    PriorityQueue queue;

    for (int i = 0; i < 256; i++)
    {
        if (frequency[i] > 0)
        {
            queue.enqueue(new HuffmanNode(char(i), frequency[i]));
        }
    }

    if (queue.size == 0)
    {
        return NULL;
    }

    while (queue.size > 1)
    {
        HuffmanNode* first = queue.dequeue();
        HuffmanNode* second = queue.dequeue();

        HuffmanNode* newNode =
            new HuffmanNode('\0', first->frequency + second->frequency);

        newNode->left = first;
        newNode->right = second;

        queue.enqueue(newNode);
    }

    return queue.dequeue();
}

void deleteTree(HuffmanNode* root)
{
    if (root == NULL)
    {
        return;
    }

    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}


// ============================================================
// HUFFMAN CODE GENERATION
// ============================================================

void createBinaryCodes(HuffmanNode* root, string code, string codes[])
{
    if (root == NULL)
    {
        return;
    }

    // Leaf node
    if (root->left == NULL && root->right == NULL)
    {
        codes[(unsigned char)root->data] = code.empty() ? "0" : code;
        return;
    }

    createBinaryCodes(root->left, code + "0", codes);
    createBinaryCodes(root->right, code + "1", codes);
}

void displayBinaryCodes(int frequency[], string codes[])
{
    cout << "\n" << LINE << "\n";
    cout << "                    HUFFMAN CODES\n";
    cout << LINE << "\n";

    cout << left << setw(20) << "Character"
         << right << setw(15) << "Frequency"
         << setw(20) << "Huffman Code" << endl;
    cout << DASH << "\n";

    for (int i = 0; i < 256; i++)
    {
        if (frequency[i] > 0)
        {
            cout << left << setw(20) << charLabel(i)
                 << right << setw(15) << frequency[i]
                 << setw(20) << codes[i] << endl;
        }
    }

    cout << LINE << "\n";
}


// ============================================================
// FILE COMPRESSION
// ============================================================

bool compressFile(string inputFile, string outputFile,
                  string codes[], int frequency[])
{
    ifstream file(inputFile.c_str(), ios::binary);

    if (!file)
    {
        cout << "Error: Could not open input file." << endl;
        return false;
    }

    ofstream compressed(outputFile.c_str(), ios::binary);

    if (!compressed)
    {
        cout << "Error: Could not create compressed file." << endl;
        return false;
    }

    // Header: frequency table 
    int uniqueCharacters = 0;

    for (int i = 0; i < 256; i++)
    {
        if (frequency[i] > 0)
        {
            uniqueCharacters++;
        }
    }

    compressed.write((char*)&uniqueCharacters, sizeof(uniqueCharacters));

    for (int i = 0; i < 256; i++)
    {
        if (frequency[i] > 0)
        {
            unsigned char character = (unsigned char)i;

            compressed.write((char*)&character, sizeof(character));
            compressed.write((char*)&frequency[i], sizeof(frequency[i]));
        }
    }

    //Convert the text into Huffman bits 
    string bits = "";
    char ch;

    while (file.get(ch))
    {
        bits += codes[(unsigned char)ch];
    }

    file.close();

    long long validBits = (long long)bits.length();

    compressed.write((char*)&validBits, sizeof(validBits));

    // ---- Pack every 8 bits into one byte ----
    for (size_t i = 0; i < bits.length(); i += 8)
    {
        unsigned char byte = 0;

        for (size_t j = 0; j < 8; j++)
        {
            if (i + j < bits.length() && bits[i + j] == '1')
            {
                byte |= (1 << (7 - j));
            }
        }

        compressed.write((char*)&byte, sizeof(byte));
    }

    compressed.close();

    cout << "\nTotal Encoded Bits : " << validBits << endl;
    return true;
}


// ============================================================
// FILE DECOMPRESSION
// ============================================================

bool decompressFile(string inputFile, string outputFile)
{
    ifstream compressed(inputFile.c_str(), ios::binary);

    if (!compressed)
    {
        cout << "Error: Could not open compressed file." << endl;
        return false;
    }

    // ---- Read the frequency table ----
    int uniqueCharacters = 0;

    compressed.read((char*)&uniqueCharacters, sizeof(uniqueCharacters));

    if (!compressed || uniqueCharacters < 1 || uniqueCharacters > 256)
    {
        cout << "Error: Not a valid compressed file." << endl;
        return false;
    }

    int frequency[256] = {0};

    for (int i = 0; i < uniqueCharacters; i++)
    {
        unsigned char character;
        int count;

        compressed.read((char*)&character, sizeof(character));
        compressed.read((char*)&count, sizeof(count));

        frequency[character] = count;
    }

    long long validBits = 0;

    compressed.read((char*)&validBits, sizeof(validBits));

    if (!compressed)
    {
        cout << "Error: Compressed file is damaged." << endl;
        return false;
    }

    // ---- Rebuild the Huffman tree ----
    HuffmanNode* root = buildHuffmanTree(frequency);

    ofstream output(outputFile.c_str(), ios::binary);

    if (!output)
    {
        cout << "Error: Could not create decompressed file." << endl;
        deleteTree(root);
        return false;
    }

    // ---- Decode the bits ----
    if (root->left == NULL && root->right == NULL)
    {
        // Only one unique character in the whole file
        for (long long i = 0; i < validBits; i++)
        {
            output.put(root->data);
        }
    }
    else
    {
        HuffmanNode* current = root;
        long long bitsRead = 0;
        unsigned char byte;

        while (bitsRead < validBits &&
               compressed.read((char*)&byte, sizeof(byte)))
        {
            for (int i = 7; i >= 0 && bitsRead < validBits; i--)
            {
                current = (byte & (1 << i)) ? current->right : current->left;
                bitsRead++;

                if (current->left == NULL && current->right == NULL)
                {
                    output.put(current->data);
                    current = root;
                }
            }
        }
    }

    compressed.close();
    output.close();
    deleteTree(root);

    return true;
}


// ============================================================
// FILE SIZE + FILE INTEGRITY
// ============================================================

long long getFileSize(string filename)
{
    ifstream file(filename.c_str(), ios::binary | ios::ate);

    if (!file)
    {
        return -1;
    }

    return (long long)file.tellg();
}

bool checkIntegrity(string originalFile, string decompressedFile)
{
    ifstream original(originalFile.c_str(), ios::binary);
    ifstream decompressed(decompressedFile.c_str(), ios::binary);

    if (!original || !decompressed)
    {
        return false;
    }

    if (getFileSize(originalFile) != getFileSize(decompressedFile))
    {
        return false;
    }

    char a, b;

    while (original.get(a) && decompressed.get(b))
    {
        if (a != b)
        {
            return false;
        }
    }

    return true;
}


// ============================================================
// FILE COMPRESSION
// ============================================================

void compressOption()
{
    string inputFile;

    cout << "\nEnter file to compress (e.g. Ratfiles.txt): ";
    getline(cin, inputFile);

    string compressedFile = inputFile + ".huf";

    //Read file
    int frequency[256] = {0};

    if (!readFrequencies(inputFile, frequency))
    {
        cout << "Error: " << inputFile << " not found." << endl;
        return;
    }

    cout << "\nFile read successfully." << endl;

    displayFrequencies(frequency);

    // Huffman tree
    HuffmanNode* root = buildHuffmanTree(frequency);

    if (root == NULL)
    {
        cout << "\nThe file is empty. Nothing to compress." << endl;
        return;
    }

    string codes[256];

    createBinaryCodes(root, "", codes);
    displayBinaryCodes(frequency, codes);

    //Compression 
    cout << "\n" << LINE << "\n";
    cout << "                    COMPRESSION\n";
    cout << LINE << "\n";

    if (compressFile(inputFile, compressedFile, codes, frequency))
    {
        cout << "Compressed file    : " << compressedFile << endl;

        //Compression ratio 
        long long originalSize = getFileSize(inputFile);
        long long compressedSize = getFileSize(compressedFile);

        double saved =
            ((double)(originalSize - compressedSize) / originalSize) * 100;

        cout << "\n" << DASH << "\n";
        cout << left << setw(25) << "Original File Size"
             << ": " << originalSize << " bytes" << endl;
        cout << left << setw(25) << "Compressed File Size"
             << ": " << compressedSize << " bytes" << endl;
        cout << left << setw(25) << "Space Saved"
             << ": " << fixed << setprecision(2) << saved << "%" << endl;

        if (saved < 0)
        {
            cout << "\nNote: this file is too small. The header is bigger "
                 << "than the savings." << endl;
        }
    }

    cout << LINE << "\n";

    deleteTree(root);
}


// ============================================================
// DECOMPRESS
// ============================================================

void decompressOption()
{
    string compressedFile;

    cout << "\nEnter file to decompress (e.g. Ratfiles.txt.huf): ";
    getline(cin, compressedFile);

    if (getFileSize(compressedFile) < 0)
    {
        cout << "Error: " << compressedFile << " not found." << endl;
        return;
    }

    
    string originalFile = "";

    if (compressedFile.length() > 4 &&
        compressedFile.substr(compressedFile.length() - 4) == ".huf")
    {
        originalFile = compressedFile.substr(0, compressedFile.length() - 4);
    }

   
    // so the original file is never overwritten
    string outputFile;

    if (originalFile.empty())
    {
        outputFile = compressedFile + "_decoded.txt";
    }
    else
    {
        size_t dot = originalFile.rfind('.');

        if (dot == string::npos)
        {
            outputFile = originalFile + "_decoded";
        }
        else
        {
            outputFile = originalFile.substr(0, dot) + "_decoded" +
                         originalFile.substr(dot);
        }
    }

    cout << "\n" << LINE << "\n";
    cout << "                   DECOMPRESSION\n";
    cout << LINE << "\n";

    if (!decompressFile(compressedFile, outputFile))
    {
        cout << LINE << "\n";
        return;
    }

    cout << "Decompressed file  : " << outputFile << endl;
    cout << LINE << "\n";

    //File integrity
    if (getFileSize(originalFile) < 0)
    {
        cout << "\nEnter the original file to compare with: ";
        getline(cin, originalFile);
    }

    cout << "\n" << LINE << "\n";
    cout << "                    FILE INTEGRITY\n";
    cout << LINE << "\n";

    if (getFileSize(originalFile) < 0)
    {
        cout << "Original file not found. Integrity check skipped." << endl;
    }
    else if (checkIntegrity(originalFile, outputFile))
    {
        cout << "Original and decompressed files are identical.\n\n";
        cout << "              FILE INTEGRITY: VERIFIED" << endl;
    }
    else
    {
        cout << "Original and decompressed files are different.\n\n";
        cout << "              FILE INTEGRITY: FAILED" << endl;
    }

    cout << LINE << "\n";
}


// ============================================================
// MAIN MENU
// ============================================================

int main()
{
    string choice;

    do
    {
        cout << "\n" << LINE << "\n";
        cout << "                 HUFFMAN CODING PROJECT\n";
        cout << LINE << "\n";
        cout << "  [1] Compress a file\n";
        cout << "  [2] Decompress a file\n";
        cout << "  [3] Exit\n";
        cout << DASH << "\n";
        cout << "Enter choice: ";

        getline(cin, choice);

        if (choice == "1")
        {
            compressOption();
        }
        else if (choice == "2")
        {
            decompressOption();
        }
        else if (choice == "3")
        {
            cout << "\nGoodbye!" << endl;
        }
        else
        {
            cout << "\nInvalid choice. Please enter 1, 2, or 3." << endl;
        }

    } while (choice != "3" && cin);

    return 0;
}