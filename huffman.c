#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//Global Variables
char Characters[95]; // Keeps all characters used
char *Codes[95]; // Stores all the codes
int count = 0; // Global counter for how many global characters
float OriginalSize; // Original size of the input file
float CompressedSize; // Compressed file size
float CompressionRatio; // Ratio

struct HuffmanNode{
    char c; //character
    int f; // frequency
    // Two child nodes
    struct HuffmanNode *left;
    struct HuffmanNode *right;
};

struct HuffmanTree{
    unsigned size; //Size of Tree
    unsigned capacity; // Capacity
    struct HuffmanNode **array; // Pointer to the array of pointers
};

// Adding a new Node
struct HuffmanNode *newNode(char c, int f){
    struct HuffmanNode *New = (struct HuffmanNode*)malloc(sizeof (struct HuffmanNode)); // ALlocate Memory

    // Set the children pointers to NULL
    New->left = NULL;
    New->right = NULL;

    // Set current Value
    New->c = c;
    New->f = f;

    // Return the node
    return New;
}

struct HuffmanTree *initialiseTree(unsigned capacity){
    struct HuffmanTree *ht = (struct HuffmanTree *) malloc(sizeof (struct HuffmanTree)); // Allocate Memory

    // initial size and capacity
    ht->size = 0;
    ht->capacity = capacity;
    ht->array = (struct HuffmanNode **)malloc(ht->capacity * sizeof (struct HuffmanNode));

    return ht;
}

// When swapping two nodes
void swap(struct HuffmanNode **x, struct HuffmanNode **y){
    struct HuffmanNode *temp = *x;
    *x = *y;
    *y = temp;
}

// Inserting a new node
void insert (struct HuffmanTree *ht, int i){
    int low = i;
    int left = 2 * i + 1;
    int right = 2 * i;

    if(left < ht->size && ht->array[left]->f < ht->array[low]->f){
        low = left;
    }
    if (right < ht->size && ht->array[right]->f < ht->array[low]->f){
        low = right;
    }
    if (low != i) {
        swap(&ht->array[low], &ht->array[i]);
        insert(ht, low);
    }
}

// Breaker used later if the current size is 1
int breakOne(struct HuffmanTree* ht){
    return (ht->size == 1);
}

struct HuffmanNode *getMin(struct HuffmanTree *ht){
    struct HuffmanNode *temp = ht->array[0];
    ht->array[0] = ht->array[ht->size - 1];
    --ht->size;
    insert(ht, 0);

    return temp;
}

void insertTree(struct HuffmanTree *ht, struct HuffmanNode *hn){
    ++ht->size;
    int i = ht->size - 1;
    while (i && hn->f < ht->array[(i - 1) / 2]->f) {
        ht->array[i] = ht->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }

    ht->array[i] = hn;
}

// Building the tree
void buildTree(struct HuffmanTree *ht){
    int count = ht->size -1;

    for(int i = (count - 1) / 2; i >= 0; --i){
        insert(ht, i);
    }
}

// Creating the tree
struct HuffmanTree *createTree(char c[], int f[], int size){
    struct HuffmanTree *ht = initialiseTree(size);
    for(int i = 0; i < size; ++i){
        ht->array[i] = newNode(c[i], f[i]);
    }
    ht->size = size;
    buildTree(ht);

    return ht;
}

// Creating each node
struct HuffmanNode *createNodes(char c[], int f[], int size) {
    struct HuffmanNode *left, *top, *right;
    struct HuffmanTree *ht = createTree(c, f, size); //create the tree first

    while (!breakOne(ht)) {
        left = getMin(ht);
        right = getMin(ht);
        top = newNode('$', left->f + right->f);

        top->left = left;
        top->right = right;

        insertTree(ht, top);
    }
    return getMin(ht);
}

// Function goes throuhg all nodes
void getHuffmanCodes(struct HuffmanNode *node, int array[], int top, FILE *codetable){
    if ((node->right == NULL) && (node->left) == NULL) {
        fprintf(codetable,"%c|", node->c); //write the character to codeTable
        for (int i = 0; i < top; ++i){
            fprintf(codetable,"%d", array[i]); // Write code to codeTable
        }
        fprintf(codetable,"|%d", node->f); // Write frequency
        fprintf(codetable,"\n");
    }

    // Go all the way left
    if (node->left != NULL) {
        array[top] = 0;
        getHuffmanCodes(node->left, array, top + 1, codetable);
    }

    // Go all the way right
    if (node->right) {
        array[top] = 1;
        getHuffmanCodes(node->right, array, top + 1, codetable);
    }
}

// Gets the code from the codeTable file and saves it in array
void GetCode(char *codetable_path){
    char lineInfo[256];
    char *word;
    char delimiter[]= "|\n";
    FILE *codetable;       // codeTable.txt file pointer
    codetable = fopen(codetable_path, "r"); // Open for writing
    if(codetable == NULL) {
        printf("File Reading Error\n");
    }
    while (fgets(lineInfo, 255, codetable) != NULL) {
        word = strtok(lineInfo, delimiter);
        while(word!=NULL){
            Characters[count] = word[0];
            word = strtok(NULL,delimiter);
            Codes[count] = malloc(10000);
            strcpy(Codes[count], word);
            word = strtok(NULL,delimiter);
            word = strtok(NULL,delimiter);
            count++;
        }
    }
    fclose(codetable);
}

// Write function to the encode.txt file
void writeToEncode(char *plaintext_path, FILE *encodetxt){
    int c;
    FILE *plaintext = fopen(plaintext_path, "r"); // Open for reading
    if(plaintext == NULL) {
        printf("File Reading Error\n");
    }

    while( (c = fgetc(plaintext)) != EOF && c!='\n') {
        for(int i = 0; i < count; i++){
            if(c == Characters[i]){
                fprintf(encodetxt, "%s", Codes[i]);
            }
        }
    }
    fclose(plaintext);
    fclose(encodetxt);
}

// Counts the number of bits in the output file
void numberOfBits(char *encodetxt_path){
    int c;
    int bits = 0;
    FILE *encodetxt = fopen(encodetxt_path, "r"); // Open for writing
    while( (c = fgetc(encodetxt)) != EOF && c!='\n'){
        bits++;
    }
    CompressedSize = bits;
}

// Funciton that handles all encoding
void encode(char *plaintext_path, char *codetable_path, char *encodetxt_path){
    int c;
    int letters[128]={-1};
    int f[128]={0};
    int counter = 0;
    int counterChar = 0;
    int array[100];
    int top = 0;

    FILE *plaintext = fopen(plaintext_path, "r"); // Open for reading
    FILE *encodetxt = fopen(encodetxt_path, "w"); // Open for writing
    FILE *codetable = fopen(codetable_path, "w"); // Open for writing
    if(plaintext == NULL || encodetxt == NULL || codetable == NULL) {
        printf("File Reading Error\n");
    }

    while( (c = fgetc(plaintext)) != EOF && c!='\n') {
        letters[c] = c;
        counterChar++;
        if(f[c] == 0){
            counter++;
        }
        f[c] = f[c] + 1;

    }
    fclose(plaintext);

    char finalChar[counter];
    int finalFreq[counter];
    int j = 0;

    for(int i = 30; i < 128; i++){
        if(f[i] > 0){
            finalChar[j] = letters[i];
            finalFreq[j] = f[i];
            j++;
        }
    }

    struct HuffmanNode *node = createNodes(finalChar, finalFreq, counter);
    getHuffmanCodes(node, array, top, codetable);
    fclose(codetable);

    GetCode(codetable_path);
    writeToEncode(plaintext_path, encodetxt);
    numberOfBits(encodetxt_path);
    OriginalSize = counterChar * 8;
    CompressionRatio = (CompressedSize / OriginalSize) * 100;
    printf("Original: %0.0f bits\nCompressed: %0.0f bits\nCompression Ratio: %0.2f%%\n", OriginalSize, CompressedSize, CompressionRatio);
}

// Determines whether a code is a prefix to the encode file
int prefix(const char *pre, const char *str){
    if (strncmp(pre, str, strlen(pre)) == 0){
        return 1;
    } else{
        return -1;
    }
}

// Removes the desired number of initial letters
char *removeFirst(char *str, int i){
    char *p;
    p = str;
    p = p + i;
    return p;
}

// Function that handles all decoding
void decode(char *codetable_path, char *encodetxt_path, char *decodetxt_path){
    GetCode(codetable_path);
    FILE *encodetxt = fopen(encodetxt_path, "r");
    FILE *decoded = fopen(decodetxt_path, "w");
    char lineInfo[10000];
    char *word;
    char delimiter[]= "\n";

    while (fgets(lineInfo, 1000, encodetxt) != NULL) {
        word = strtok(lineInfo, delimiter);
    }

    while(strlen(word) != 0){
        for(int i = 0; i < count; i++){
            if(prefix(Codes[i], word) == 1){
                fprintf(decoded, "%c", Characters[i]);
                word = removeFirst(word, strlen(Codes[i]));
            }
        }
    }
    fclose(encodetxt);
    fclose(decoded);
}

int main( int argc, char **argv) {
    char *mode;            // Encode/Decode argument
    char *plaintext_path;  // plainText.txt file path argument - this is the input file (MESSAGE)
    char *codetable_path;  // codeTable.txt file path argument - This is the file that stores the codes
    char *encodetxt_path;  // encodedText.txt file path argument - This is the coded Text
    char *decodetxt_path;  // decodedText.txt file path argument - This is the decoded text

    if( argc < 5 ) {
        printf("Not Enough Arguments Supplied\n");
        return -1;
    }
    mode = argv[1];
    if( strcmp(mode, "encode") == 0 ) {
        plaintext_path = argv[2];
        codetable_path = argv[3];
        encodetxt_path = argv[4];
        encode(plaintext_path, codetable_path, encodetxt_path);
    }
    else if( strcmp(mode, "decode") == 0 ) {
        codetable_path = argv[2];
        encodetxt_path = argv[3];
        decodetxt_path = argv[4];
        decode(codetable_path, encodetxt_path, decodetxt_path);
    }
    else {
        printf("Not A Valid Mode\n");
        return -1;
    }
}
