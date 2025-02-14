/*Mohammed Hamo 1230394 Project 3 Comp 2421*/
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INITIAL_TABLE_SIZE 3
#define LOAD_FACTOR 0.7
#define DISTRICTSFILE "districts.txt "
#define TOWNSFILE "towns.txt"

// create the typedef struct AVLnode Tree; and do the changes in the code
// you have two main menus

typedef struct AVLnode *AVLNode;

// Struct to store town information in the avl node
struct AVLnode {
    char town[30];
    char district[30];
    int population;
    int elevation;
    int hasMunicipality;
    AVLNode Left;
    AVLNode Right;
    int Height; //Balance information
};

//typedef struct AVLNode *AVLTree;

typedef struct {
    char district[30];
    int population;
    int elevation;
    int hasMunicipality; // 1 for Yes, 0 for No
} TownInfo;

// Hash node struct
typedef struct {
    char *key;
    TownInfo value;
    int isDeleted; // 0 for active, 1 for deleted
} HashNode;

// Hash table struct
typedef struct {
    HashNode *table;
    int size;
    int count;
} HashTable;



// Function prototypes
AVLNode MakeEmpty(AVLNode T);
AVLNode Find(char *tempTown, AVLNode T);
AVLNode FindMin(AVLNode T);
AVLNode FindMax(AVLNode T);
int Height(AVLNode P);
int Max(int Lhs, int Rhs);
AVLNode SingleRotateWithLeft(AVLNode K2);
AVLNode SingleRotateWithRight(AVLNode K1);
AVLNode DoubleRotateWithLeft(AVLNode K3);
AVLNode DoubleRotateWithRight(AVLNode K1);
AVLNode InsertNodeToAVL(char *town, char *district, int population, int elevation, int hasMunicipality, AVLNode node);
AVLNode DeleteNodeFromAVL(char *town, AVLNode root);
void PrintInOrder(AVLNode node);
AVLNode CreateTreeFromFile(char *filename);
void InsertNewTownToAVLTree(AVLNode *root);
void UpdateOrDisplayTown(AVLNode root, int choice);
void PrintTownsByPopulation(AVLNode node, int population);
void PrintTownsByMunicipality(AVLNode t);
void WriteToFileInOrderFromAVL(AVLNode root, char *file);
void SaveTownsToFileFromAVL(AVLNode root, char *filename);
HashTable* ReadHashTableTownsFromFile(char *filename);
int hash(char *key, int tableSize);
HashTable *createTable(int size);
int nextPrime(int n);
void rehash(HashTable *hashTable);
void insertToHashTable(HashTable *hashTable, char *key);
void insertFromFileToHashTable(HashTable *hashTable, char *key, TownInfo town_info);
void search(HashTable *hashTable, char key[50]);
void deleteFromHashTable(HashTable *hashTable, char key[50]);
void displayTable(HashTable *hashTable);
void HashTableMenu();
void Menu(AVLNode *root);





// The AVL functions :

AVLNode MakeEmpty(AVLNode T) { // free all elements in the tree after doing post-order traversal
    if (T != NULL) {
        MakeEmpty(T->Left);
        MakeEmpty(T->Right);
        free(T);
    }
    return NULL;
}

AVLNode Find(char *tempTown, AVLNode T) { // Find a node from the AVL Tree
    if (T == NULL)
        return NULL;
    if (strcmp(tempTown, T->town) < 0)
        return Find(tempTown, T->Left);
    else if (strcmp(tempTown, T->town) > 0)
        return Find(tempTown, T->Right);
    else
        return T;
}

AVLNode FindMin(AVLNode T) { // Find min element that is son of a given node
    if (T == NULL)
        return NULL;
    else if (T->Left == NULL)
        return T;
    else
        return FindMin(T->Left);
}

AVLNode FindMax(AVLNode T) { // Find max element that is son of a given node
    if (T != NULL)
        while (T->Right != NULL)
            T = T->Right;

    return T;
}


int Height(AVLNode P) { // returns the height of the node
    if (P == NULL)
        return -1;
    else
        return P->Height;
}

int Max(int Lhs, int Rhs) {
    return Lhs > Rhs ? Lhs : Rhs;
}

/* This function can be called only if "second" has a left child */
/* Perform a rotate between a node "second" and its left child */
/* Update heights, then return new root */

AVLNode SingleRotateWithLeft(AVLNode second) {
    AVLNode first;

    first = second->Left;
    second->Left = first->Right;
    first->Right = second;

    second->Height = Max(Height(second->Left), Height(second->Right)) + 1;
    first->Height = Max(Height(first->Left), second->Height) + 1;

    return first; /* New root */
}


/* This function can be called only if "first" has a right child */
/* Perform a rotate between a node "first" and its right child */
/* Update heights, then return new root */

AVLNode SingleRotateWithRight(AVLNode first) {
    AVLNode second;

    second = first->Right;
    first->Right = second->Left;
    second->Left = first;

    first->Height = Max(Height(first->Left), Height(first->Right)) + 1;
    second->Height = Max(Height(second->Right), first->Height) + 1;

    return second; /* New root */
}

/* This function can be called only if "third" has a left */
/* child and third's left child has a right child */
/* Do the left-right double rotation */
/* Update heights, then return new root */

AVLNode DoubleRotateWithLeft(AVLNode third) {
    /* Rotate between first and second */
    third->Left = SingleRotateWithRight(third->Left);

    /* Rotate between third and second */
    return SingleRotateWithLeft(third);
}


/* This function can be called only if first has a right */
/* child and first's right child has a left child */
/* Do the right-left double rotation */
/* Update heights, then return new root */

AVLNode DoubleRotateWithRight(AVLNode first) {
    /* Rotate between third and second*/
    first->Right = SingleRotateWithLeft(first->Right);

    /* Rotate between first and second*/
    return SingleRotateWithRight(first);
}


AVLNode InsertNodeToAVL(char *town, char *district, int population, int elevation, int hasMunicipality, AVLNode node) {
    if (node == NULL) { // either empty tree or the right place to insert the node
        node = malloc(sizeof(struct AVLnode)); // finding a space for the new node
        if (node == NULL) {
            // failed memory allocation failed
            printf("Out of space. Insertion Failed.");
            return NULL;
        }
        // copy the inserted info to the new node
        strcpy(node->town, town);
        strcpy(node->district, district);
        node->population = population;
        node->elevation = elevation;
        node->hasMunicipality = hasMunicipality;
        node->Height = 0;
        node->Left = node->Right = NULL;

    } else if (strcmp(town, node->town) < 0) { // if the current town > insertedTown then go to the left child
        node->Left = InsertNodeToAVL(town, district, population, elevation, hasMunicipality, node->Left);
        if (Height(node->Left) - Height(node->Right) == 2) {
            // we have used ==2 because it's a recursive function it will balance the tree level by level, and we don't need to make the condition (>=2)
            if (strcmp(town, node->Left->town) < 0)
                node = SingleRotateWithLeft(node);
            else
                node = DoubleRotateWithLeft(node);
        }
    } else if (strcmp(town, node->town) > 0) { // if the current town < insertedTown then insert to the right child
        node->Right = InsertNodeToAVL(town, district, population, elevation, hasMunicipality, node->Right);
        if (Height(node->Right) - Height(node->Left) == 2) {
            if (strcmp(town, node->Right->town) > 0)
                node = SingleRotateWithRight(node);
            else
                node = DoubleRotateWithRight(node);
        }
    }else {
        // if compare == 0 then the town already exists
        printf("Town already exists.\n\n");
    }

    node->Height = Max(Height(node->Left), Height(node->Right)) + 1; // assign a new height to the node
    return node;
}


// Function to delete a node from avl
AVLNode DeleteNodeFromAVL(char *town, AVLNode root) {
    if (root == NULL || Find(town,root) == NULL ) {
        printf("Town not found\n");
        return root;
    }


    if (strcmp(town, root->town) < 0) // Go to the left subtree
        root->Left = DeleteNodeFromAVL(town, root->Left);
    else if (strcmp(town, root->town) > 0) // Go to the right subtree
        root->Right = DeleteNodeFromAVL(town, root->Right);
    else // Node to be deleted is found
    {
        if (root->Left && root->Right) // Node has two children
        {
            AVLNode Temp = FindMin(root->Right); // Find the minimum in the right subtree
            strcpy(root->town, Temp->town); // Copy the data to the current node
            strcpy(root->district, Temp->district);
            root->population = Temp->population;
            root->elevation = Temp->elevation;
            root->hasMunicipality = Temp->hasMunicipality;
            root->Right = DeleteNodeFromAVL(Temp->town, root->Right); // Delete the duplicate node
        } else // Node with one or zero children
        {
            AVLNode Temp = root;
            if (root->Left == NULL)
                root = root->Right;
            else if (root->Right == NULL)
                root = root->Left;

            free(Temp);
        }
    }

    // Update the height and balance the tree
    if (root == NULL)
        return root;

    root->Height = Max(Height(root->Left), Height(root->Right)) + 1;

    // Balance the tree
    if (Height(root->Left) - Height(root->Right) == 2) {
        if (Height(root->Left->Left) >= Height(root->Left->Right))
            root = SingleRotateWithLeft(root);
        else
            root = DoubleRotateWithLeft(root);
    } else if (Height(root->Right) - Height(root->Left) == 2) {
        if (Height(root->Right->Right) >= Height(root->Right->Left))
            root = SingleRotateWithRight(root);
        else
            root = DoubleRotateWithRight(root);
    }

    return root;
}

// Print in-order traversal
void PrintInOrder(AVLNode node) {
    if (node != NULL) {
        PrintInOrder(node->Left);
        printf("Town: %s, District: %s, Population: %d, Elevation: %d, HasMunicipality: %d\n",
               node->town, node->district, node->population, node->elevation, node->hasMunicipality);
        PrintInOrder(node->Right);
    }
}

AVLNode CreateTreeFromFile(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Unable to open file %s\n", filename);
        return NULL;
    }

    AVLNode root = NULL;
    char line[256];
    char town[50], district[50], hasMunicipalityStr[5];
    int population, elevation, hasMunicipality;

    // Read file line by line
    while (fgets(line, sizeof(line), file)) {
        // Remove newline character at the end of the line
        line[strcspn(line, "\n")] = 0;

        // Parse the line (format: District:Town:Population:Elevation:HasMunicipality)
        if (sscanf(line, "%99[^:]:%99[^:]:%d:%d:%99s", district, town, &population, &elevation,
                   hasMunicipalityStr) == 5) {
            // Convert "yes" or "no" to 1 or 0
            hasMunicipality = (strcasecmp(hasMunicipalityStr, "yes") == 0) ? 1 : 0;

            // Insert into the AVL tree
            root = InsertNodeToAVL(town, district, population, elevation, hasMunicipality, root);
        } else {
            // Print error for invalid lines
            printf("Invalid line format: %s\n", line);
        }
    }

    fclose(file);
    printf("Reading from districts.txt was done successfully.\n\n");
    return root;
}

void InsertNewTownToAVLTree(AVLNode *root) {
    char town[50], district[50], hasMunicipalityStr[5];
    int population, elevation;
    // reading town info from the user
    printf("Enter town name: ");
    scanf("%s", town);
    printf("Enter district name: ");
    scanf("%s", district);
    printf("Enter population: ");
    scanf("%d", &population);
    printf("Enter elevation: ");
    scanf("%d", &elevation);
    printf("Does it have a municipality (yes/no): ");
    scanf("%s", hasMunicipalityStr);

    int hasMunicipality = (strcasecmp(hasMunicipalityStr, "yes") == 0) ? 1 : 0;
    *root = InsertNodeToAVL(town, district, population, elevation, hasMunicipality, *root);

    printf("Town inserted successfully.\n");
}

void UpdateOrDisplayTown(AVLNode root, int choice) {
    if(choice == 0) {
        printf("Exiting...\n");
        return;
    }
    char town[50];
    char hasMunicipalityStr[5];
    printf("Enter the town name to find: ");
    scanf("%s", town);

    AVLNode node = Find(town, root);
    if (node == NULL) {
        printf("Town not found.\n");
    } else {
        printf("Town found: %s, District: %s, Population: %d, Elevation: %d, HasMunicipality: %d\n",
               node->town, node->district, node->population, node->elevation, node->hasMunicipality);
                // here we end the display function

        if(choice == 1) return;
        // else Update the information the choice will be 2

        char yesNoStr[5];
        printf("Do you want to update population ? (yes/no) \n");
        scanf("%s", yesNoStr);
        if(strcasecmp(yesNoStr, "yes") == 0){
            printf("Enter new population: ");
            scanf("%d", &node->population);
        }
        printf("Do you want to update elevation ? (yes/no) \n");
        scanf("%s", yesNoStr);
        if(strcasecmp(yesNoStr, "yes") == 0){
            printf("Enter new elevation: ");
            scanf("%d", &node->elevation);
        }
        printf("Do you want to update municipality status ? (yes/no) \n");
        scanf("%s", yesNoStr);
        if(strcasecmp(yesNoStr, "yes") == 0){
            printf("Does it have a municipality (yes/no): ");
            scanf("%s", hasMunicipalityStr);
            node->hasMunicipality = (strcmp(hasMunicipalityStr, "yes") == 0) ? 1 : 0;
        }
        printf("Town updated successfully.\n");
    }
}

void PrintTownsByPopulation(AVLNode node, int population) {// in-order traversal and condition to check the population number
    if (node != NULL) {
        PrintTownsByPopulation(node->Left, population);
        if (node->population >= population) {
            printf("Town: %s, Population: %d, Elevation: %d, HasMunicipality: %d\n",
                   node->town, node->population, node->elevation, node->hasMunicipality);
        }
        PrintTownsByPopulation(node->Right, population);
    }
}

void PrintTownsByMunicipality(AVLNode t) { // in-order traversal and checking if it has municipality
    if (t != NULL) {
        PrintTownsByMunicipality(t->Left);
        if (t->hasMunicipality == 1) {
            printf("Town: %s, Population: %d, Elevation: %d, HasMunicipality: %d\n",
                   t->town, t->population, t->elevation, t->hasMunicipality);
        }
        PrintTownsByMunicipality(t->Right);
    }
}


void WriteToFileInOrderFromAVL(AVLNode root, char *file) { // in order traversal
    if (root != NULL) {
        WriteToFileInOrderFromAVL(root->Left, file); // Write left subtree

        char *hasMunicipalityStr;
        if (root->hasMunicipality) hasMunicipalityStr = "yes";
        else
            hasMunicipalityStr = "No";
        fprintf(file, "%s:%s:%d:%d:%s\n",
                root->district, root->town, root->population, root->elevation, hasMunicipalityStr);
        WriteToFileInOrderFromAVL(root->Right, file); // Write right subtree
    }
}


void SaveTownsToFileFromAVL(AVLNode root, char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Unable to open file %s for writing.\n", filename);
        return;
    }
    WriteToFileInOrderFromAVL(root, file);
    printf("All town information has been saved to %s.\n", filename);
    fclose(file);
}


void SaveTownsToFileFromTable(HashTable *hashTable,char *filename){
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Unable to open file %s for writing.\n", filename);
        return;
    }

    for (int i = 0; i < hashTable->size; i++) {
        char *hasMunicipalityStr;
        if (hashTable->table[i].value.hasMunicipality)
            hasMunicipalityStr = "yes";
        else
            hasMunicipalityStr = "No";

        if (hashTable->table[i].key != NULL && !hashTable->table[i].isDeleted) {
            fprintf(file, "%s:%s:%d:%d:%s\n",
                hashTable->table[i].key,
                hashTable->table[i].value.district,
                hashTable->table[i].value.population,
                hashTable->table[i].value.elevation,
                hasMunicipalityStr);
        }
    }

    printf("All town information has been saved to %s.\n", filename);
    fclose(file);

}


HashTable *ReadHashTableTownsFromFile(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Unable to open file %s\n", filename);
        return NULL;
    }
    HashTable *hashTable = createTable(10);

    if(hashTable == NULL)
        return NULL;

    char line[256];
    char town[50], district[50], hasMunicipalityStr[5];
    int population, elevation;
    TownInfo town_info;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;

        // Parse the line (format: Town:District:Population:Elevation:HasMunicipality)
        if (sscanf(line, "%99[^:]:%99[^:]:%d:%d:%99s", district, town, &population, &elevation,
                   hasMunicipalityStr) == 5) {
            // Convert "yes" or "no" to 1 or 0
            int hasMunicipality = (strcasecmp(hasMunicipalityStr, "yes") == 0) ? 1 : 0;

            // Insert into the AVL tree
            strncpy(town_info.district, district, sizeof(town_info.district) - 1);
            town_info.population = population;
            town_info.elevation = elevation;
            town_info.hasMunicipality = hasMunicipality;
            // Use the first 5 characters of the town as the key
            char key[6] = {0};
            strncpy(key, town, 5);

            // Insert into the hash table
            insertFromFileToHashTable(hashTable, key, town_info);


           }else{
            printf("Invalid line format: %s\n", line);
        }
    }
        fclose(file);
           return hashTable; // Return the hash table


}

// Hash function
int hash(char *key, int tableSize) {
    int hashValue = 0;
    int count = 0;
    while (*key != '\0' && count < 5) {
        // Use only the first 5 characters
        hashValue += *key++;
        count++;
    }
    return hashValue % tableSize;
}

// Create a new hash table
HashTable *createTable(int size) {
    HashTable *hashTable = (HashTable *)malloc(sizeof(HashTable));
    if(hashTable == NULL) {
        printf("Failed to allocate memory for hash table.\n");
        return NULL;
    }
    hashTable->size = size;
    hashTable->count = 0;
    hashTable->table = (HashNode *) malloc(sizeof(HashNode)*size);
    for (int i = 0; i < size; i++) {
        hashTable->table[i].key = NULL;
        hashTable->table[i].isDeleted = 0;
    }
    return hashTable;
}



// Function to find the next prime number
int nextPrime(int n) {
    // If n is less than 2, the next prime is 2
    if (n < 2) return 2;

    // Loop until we find a prime number
    while (1) {
        int is_prime = 1; // Flag to indicate if n is prime

        // Check if n is prime
        if (n <= 1) {
            is_prime = 0;
        } else if (n <= 3) {
            is_prime = 1;
        } else if (n % 2 == 0 || n % 3 == 0) {
            is_prime = 0;
        } else {
            for (int i = 5; i * i <= n; i += 6) {
                if (n % i == 0 || n % (i + 2) == 0) {
                    is_prime = 0; // Found a divisor, not prime
                    break;
                }
            }
        }

        // If n is prime, return it
        if (is_prime) {
            return n;
        }

        // Increment n to check the next number
        n++;
    }
}


// Resize and rehash the hash table
void rehash(HashTable *hashTable) {
    int oldSize = hashTable->size;
    HashNode *oldTable = hashTable->table;

    hashTable->size = nextPrime(oldSize * 2); // assign it to the first next prime after we double the size
    hashTable->table = (HashNode *) malloc(hashTable->size*sizeof(HashNode));
    if(hashTable->table == NULL) {
        printf("Failed to allocate memory for hash table.\n");
        return;
    }
    for (int i = 0; i < hashTable->size; i++) { // make sure its empty
        hashTable->table[i].key = NULL;
        hashTable->table[i].isDeleted = 0;
    }

    for (int i = 0; i < oldSize; i++) {
        if (oldTable[i].key != NULL && !oldTable[i].isDeleted) {

            int index = hash(oldTable[i].key, hashTable->size);
            while (hashTable->table[index].key != NULL) {
                index = (index + 1) % hashTable->size;
            }
            hashTable->table[index] = oldTable[i];
        }
    }
    free(oldTable);
}

// Insert a new record into the hash table
// the difference between this function and the insertFromFileToHashTable is the town_info
void insertToHashTable(HashTable *hashTable,  char *key) {
    if ((float) (hashTable->count + 1) / (float) hashTable->size > LOAD_FACTOR) {
        rehash(hashTable);
    }

    int index = hash(key, hashTable->size);
    while (hashTable->table[index].key != NULL && !hashTable->table[index].isDeleted) {
        if (strcmp(hashTable->table[index].key, key) == 0) {
            printf("Town Already exist.\n");
            return;
        }
        index = (index + 1) % hashTable->size;
    }
    char district[50], hasMunicipalityStr[5];
    int population, elevation;
    TownInfo town_info;
    printf("Enter the district.\n");
    scanf("%s", district);
    printf("Enter the population.\n");
    scanf("%d", &population);
    printf("Enter the elevation.\n");
    scanf("%d", &elevation);
    printf("Enter the has municipality. ( yes or No )\n");
    scanf("%s", &hasMunicipalityStr);
    int hasMunicipality = (strcasecmp(hasMunicipalityStr, "yes") == 0) ? 1 : 0;


    strncpy(town_info.district, district, sizeof(town_info.district) - 1);
    town_info.population = population;
    town_info.elevation = elevation;
    town_info.hasMunicipality = hasMunicipality;
    // Use the first 5 characters of the town as the key


    hashTable->table[index].key = malloc(strlen(key) + 1); // +1 for the null terminator
    if (hashTable->table[index].key != NULL) {
        strcpy(hashTable->table[index].key, key);
    } else {
        printf("Memory allocation failed for key: %s\n", key);
        return;
    }
    hashTable->table[index].value = town_info;
    hashTable->table[index].isDeleted = 0;
    hashTable->count++;
}

void insertFromFileToHashTable(HashTable *hashTable, char *key,TownInfo town_info) {
    if ((float) (hashTable->count + 1) / (float) hashTable->size > LOAD_FACTOR) {
        rehash(hashTable);
    }

    int index = hash(key, hashTable->size);
    while (hashTable->table[index].key != NULL && !hashTable->table[index].isDeleted) {
        if (strcmp(hashTable->table[index].key, key) == 0) {
            // Update existing record
            hashTable->table[index].value = town_info;
            return;
        }
        index = (index + 1) % hashTable->size;
    }

    hashTable->table[index].key = malloc(strlen(key) + 1); // +1 for the null terminator
    if (hashTable->table[index].key != NULL) {
        strcpy(hashTable->table[index].key, key);
    } else {
        printf("Memory allocation failed for key: %s\n", key);
        return;
    }
    hashTable->table[index].value = town_info;
    hashTable->table[index].isDeleted = 0;
    hashTable->count++;

}

// Search for a town in the hash table
void search(HashTable *hashTable, char key[50]) {
    int index = hash(key, hashTable->size);
    int collisions = 0;


    while(hashTable->table[index].key != NULL ) {
        if (strcmp(hashTable->table[index].key, key) == 0) {
            printf("Number of collisions to reach %s : %d\n", hashTable->table[index].key, collisions);
            printf("Key: %s, District: %s, Population: %d, Elevation: %d, Has Municipality: %d\n",
                   hashTable->table[index].key,
                   hashTable->table[index].value.district,
                   hashTable->table[index].value.population,
                   hashTable->table[index].value.elevation,
                   hashTable->table[index].value.hasMunicipality);

            return;
        }
        if(!hashTable->table[index].isDeleted)
            collisions++;
        index = (index + 1) % hashTable->size;
    }
}

// Delete a specific record from the hash table
void deleteFromHashTable(HashTable *hashTable, char key[50]) {
    int index = hash(key, hashTable->size);

    while (hashTable->table[index].key != NULL) {
        if (strcmp(hashTable->table[index].key, key) == 0) {
            hashTable-> table[index].isDeleted = 1;
            hashTable->count--;
            return;
        }
        index = (index + 1) % hashTable->size;
    }
    printf("Town not found\n");
}


// Display the hash table (for debugging)
void displayTable(HashTable *hashTable) {
    for (int i = 0; i < hashTable->size; i++) {
        if (hashTable->table[i].key != NULL && !hashTable->table[i].isDeleted) {
            printf("Key: %s, District: %s, Population: %d, Elevation: %d, Has Municipality: %d\n",
                hashTable->table[i].key,
                hashTable->table[i].value.district,
                hashTable->table[i].value.population,
                hashTable->table[i].value.elevation,
                hashTable->table[i].value.hasMunicipality);
        }else {
            printf("NULL\n");
        }
    }
}


int main() {

    AVLNode root = CreateTreeFromFile(DISTRICTSFILE);
    //AVLTree avl_tree = (struct AVLNode *)malloc(sizeof(AVLTree));

    Menu(&root);
    return 0;
}


void Menu(AVLNode *root) {
    int choice;
    do {
        printf("1. Insert New Town\n");
        printf("2. Display or Update Town Information\n");
        printf("3. List All Towns in alphabetical order with their associated information. \n");
        printf("4. List all towns that have population greater than an input number.\n");
        printf("5. List all towns that have municipalities or not.\n");
        printf("6. Delete a town from the tree.\n");
        printf("7. Save all information to towns file...\n");
        printf("8. Save then Switch to HashTable Menu...\n");
        printf("9. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        printf("\n");
        int minPopulationNumber;
        char town[50];


        switch (choice) {
            case 1:
                InsertNewTownToAVLTree(root);
                break;
            case 2:
                do {
                    printf("Enter Your choice : \n");
                    printf("0. Exit and go back to menu\n");
                    printf("1. Display Town Information\n");
                    printf("2. Update Town Information\n");
                    scanf("%d", &choice);
                    printf("Choice : %d \n",choice);
                    if (choice!= 1 && choice != 2 && choice != 0) {
                        printf("Invalid Choice\n");
                    }else
                        break;

                }while (choice!= 1 || choice != 2 || choice != 0);
                UpdateOrDisplayTown(*root,choice);

                break;
            case 3:
                PrintInOrder(*root);
                printf("\n");
                break;
            case 4:
                printf("Enter minimum population number: ");
                scanf("%d", &minPopulationNumber);
                PrintTownsByPopulation(*root, minPopulationNumber);
                break;
            case 5:
                PrintTownsByMunicipality(*root);
                break;
            case 6:
                printf("Enter the town name to delete: ");
                scanf("%s", town);
                *root = DeleteNodeFromAVL(town, *root);
                break;
            case 7:
                SaveTownsToFileFromAVL(*root, TOWNSFILE);
                break;
            case 8:
                printf("Saving Towns file...");
                SaveTownsToFileFromAVL(*root, TOWNSFILE);
                HashTableMenu();
            case 9:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 9);
}


void HashTableMenu() {
    HashTable *hashTable = ReadHashTableTownsFromFile(TOWNSFILE);

    if (hashTable == NULL) {
        printf("Failed to create hash table from file.\n");
        return;
    }

    int choice;
    do {

        printf("\n1. Print hashed table\n");
        printf("2. Print the size of hashtable and load factor \n");
        printf("3. Insert a new record into the hashtable. \n");
        printf("4. Search for a specific town and print the number of collisions to reach the town.\n");
        printf("5. Delete a specific record.\n");
        printf("6. Save the data back to the file towns.txt .\n");
        printf("7. Exit\n");
        printf("Enter your choice:\n ");
        scanf("%d", &choice);
        char town[50];
        TownInfo town_info;


        switch (choice) {
            case 1:
                displayTable(hashTable);
                break;
            case 2:
                printf("Hash table size : %d\nHashTable current loadFactor : %f\n", hashTable->size, (float)hashTable->count/(float)hashTable->size);
                break;
            case 3:  // insert new record
            printf("Enter the town name.\n");
                scanf("%s", town);

                char key[6] = {0};
                strncpy(key, town, 5);

                // Insert into the hash table
                insertToHashTable(hashTable, key);

                break;
            case 4: // search
                printf("Enter the town name.\n");
                scanf("%s", town);
                search(hashTable, town);
                break;

            case 5:
                printf("Enter the town name to delete: \n");
                scanf("%s", town);
                deleteFromHashTable(hashTable, town);
            break;
            case 6:
            SaveTownsToFileFromTable(hashTable,TOWNSFILE);
                break;

            case 7:
                return;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 7);
}
