#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
/* Mohammad Hamo
 * 1230394
 * Project 4
 * 12/1/2025
 * Section NO.3
 */

#define MAX_NODES 62
// Nodes are letter and digits characters A-Za-z0-9
// Adj lists node ( considered as a list )
typedef struct AdjListNode {
    char dest;
    int weight;
    struct AdjListNode *next;
} AdjListNode;

// Adjacency list structure ( we will link this struct to be the first node of the list for nodes in the graph )
typedef struct {
    AdjListNode *head;
} AdjList;

// Graph structure ( source nodes are the index, destinations for each node are the AdjList linked saved in each index )
typedef struct {
    int numNodes;
    // can create capacity and do linear search
    int parallelArray[MAX_NODES];
    AdjList array[MAX_NODES];
    // A-Za-z0-9 62 node:  starts with A ->Z:a->z:0->9
} Graph;

// Queue structure used in BFS
typedef struct QueueNode {
    char data;
    struct QueueNode *next;
} QueueNode;

typedef struct {
    QueueNode *front, *rear;
} Queue;

// minHeap used in the dijkstra
typedef struct MinHeapNode {
    int element;
    int distance;
} MinHeapNode;

typedef struct MinHeap {
    int size;
    int capacity;
    int *position;
    MinHeapNode **array;
} MinHeap;


//prototypes
void initializeQueue(Queue *q);

void enqueue(Queue *q, char value);

char dequeue(Queue *q);

int isEmptyQueue(Queue *q);

MinHeapNode *createMinHeapNode(int element, int distance);

MinHeap *createMinHeap(int capacity);

void swapMinHeapNode(MinHeapNode **a, MinHeapNode **b);

void minHeapify(MinHeap *minHeap, int idx);

MinHeapNode *extractMin(MinHeap *minHeap);

void decreaseKey(MinHeap *minHeap, int element, int distance);

int isInMinHeap(MinHeap *minHeap, int vertex);

int getIndexForCharacterInTheGraphArray(char ch);

char getCharFromIndex(int i);

Graph *createGraph(int numNodes);

AdjListNode *createNode(char dest, int weight);

void addEdge(Graph *graph, char src, char dest, int weight);

int graphContains(Graph *graph, char node);

void bfs(Graph *graph, char source, char destination, int writeToFileFlag, FILE *file);

void dijkstra(Graph *graph, char source, char destination, int writeToFileFlag, FILE *file);

int readRoutersFromFile(char *filename, Graph *graph) ;

void printGraph(Graph *graph);


void initializeQueue(Queue *q) {
    q->front = q->rear = NULL;
}

// isEmpty queue function
int isEmptyQueue(Queue *q) {
    return q->front == NULL;
}

void enqueue(Queue *q, char value) {
    QueueNode *newNode = (QueueNode *) malloc(sizeof(QueueNode));
    if (!newNode) {
        printf("Queue overflow\n");
        return;
    }
    newNode->data = value;
    newNode->next = NULL;
    if (q->rear != NULL) {
        q->rear->next = newNode;
    }
    q->rear = newNode;
    if (q->front == NULL) {
        q->front = newNode;
    }
}

char dequeue(Queue *q) {
    if (isEmptyQueue(q)) {
        printf("Queue underflow\n");
        return -1;
    }
    QueueNode *temp = q->front;
    char value = temp->data;
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    free(temp);
    return value;
}


// MinHeap implementation functions
MinHeapNode *createMinHeapNode(int element, int distance) {
    MinHeapNode *newNode = (MinHeapNode *) malloc(sizeof(MinHeapNode));
    if (newNode == NULL) {
        printf("Failed to create minHeapNode \n");
        return NULL;
    }
    newNode->element = element;
    // used the character index as the vertex instead of the character itself , will not change anything both are unique but this is easier implementation
    newNode->distance = distance; // this is the value we will be comparing in the heap
    return newNode;
}

MinHeap *createMinHeap(int capacity) {
    MinHeap *minHeap = (MinHeap *) malloc(sizeof(MinHeap));
    if (minHeap == NULL) {
        printf("Failed to create minHeap");
        return NULL;
    }
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->position = (int *) malloc(capacity * sizeof(int));
    minHeap->array = (MinHeapNode **) malloc(capacity * sizeof(MinHeapNode *));
    return minHeap;
}

void swapMinHeapNode(MinHeapNode **a, MinHeapNode **b) {
    MinHeapNode *temp = *a;
    *a = *b;
    *b = temp;
}

void minHeapify(MinHeap *minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size &&
        minHeap->array[left]->distance < minHeap->array[smallest]->distance)
        smallest = left;

    if (right < minHeap->size &&
        minHeap->array[right]->distance < minHeap->array[smallest]->distance)
        smallest = right;

    if (smallest != idx) {
        MinHeapNode *smallestNode = minHeap->array[smallest];
        MinHeapNode *idxNode = minHeap->array[idx];

        minHeap->position[smallestNode->element] = idx;
        minHeap->position[idxNode->element] = smallest;

        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

MinHeapNode *extractMin(MinHeap *minHeap) {
    if (minHeap->size == 0)
        return NULL;

    MinHeapNode *root = minHeap->array[0];

    MinHeapNode *lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;

    minHeap->position[root->element] = minHeap->size - 1;
    minHeap->position[lastNode->element] = 0;

    --minHeap->size;
    minHeapify(minHeap, 0);

    return root;
}

void decreaseKey(MinHeap *minHeap, int element, int distance) {
    int i = minHeap->position[element];
    minHeap->array[i]->distance = distance;

    while (i && minHeap->array[i]->distance < minHeap->array[(i - 1) / 2]->distance) {
        minHeap->position[minHeap->array[i]->element] = (i - 1) / 2;
        minHeap->position[minHeap->array[(i - 1) / 2]->element] = i;
        swapMinHeapNode(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

int isInMinHeap(MinHeap *minHeap, int vertex) {
    if (minHeap->position[vertex] < minHeap->size)
        return 1;
    return 0;
}


// the next two functions are like hashing the character to its index in the used arrays in the graph, we can convert from char to its index and visa verse
int getIndexForCharacterInTheGraphArray(char ch) {
    if ('A' <= ch && ch <= 'Z')
        return ch - 'A';
    if ('a' <= ch && ch <= 'z')
        return ch - 'a' + 26;
    return ch - '0' + 52;
}

char getCharFromIndex(int i) {
    if (0 <= i && i < 26)
        return 'A' + i;
    if (26 <= i && i < 52)
        return 'a' + i - 26;
    return '0' + i - 52;
}

// graph implementation functions
AdjListNode *createNode(char dest, int weight) {
    AdjListNode *newNode = (AdjListNode *) malloc(sizeof(AdjListNode));
    newNode->dest = dest;
    newNode->weight = weight;
    newNode->next = NULL;
    return newNode;
}

Graph *createGraph(int numNodes) {
    Graph *graph = (Graph *) malloc(sizeof(Graph));
    graph->numNodes = numNodes;
    for (int i = 0; i < numNodes; i++) {
        // make sure its empty
        graph->array[i].head = NULL;
        graph->parallelArray[i] = 0;
    }
    return graph;
}

void addEdge(Graph *graph, char src, char dest, int weight) {
    int srcIndex = getIndexForCharacterInTheGraphArray(src);
    int destIndex = getIndexForCharacterInTheGraphArray(dest);

    AdjListNode *newNode = createNode(dest, weight);
    if (newNode == NULL) {
        printf("Failed to create new Node for destination\n");
        return;
    }

    // add the dest node to the src next nodes
    newNode->next = graph->array[srcIndex].head;
    graph->array[srcIndex].head = newNode;


    newNode = createNode(src, weight);
    if (newNode == NULL) {
        printf("Failed to create new Node for source\n");
        return;
    }

    // add the src node to the dest next nodes
    newNode->next = graph->array[destIndex].head;
    graph->array[destIndex].head = newNode;
}

// check if the graph contains the node
int graphContains(Graph *graph, char node) {
    return graph->parallelArray[getIndexForCharacterInTheGraphArray(node)];
}


void bfs(Graph *graph, char source, char destination, int writeToFileFlag, FILE *file) {
    char color[MAX_NODES]; // W : for not visited, G: for currently visiting, B: for visited
    int distance[MAX_NODES]; // array to store the min cost to reach a certain node
    char parent[MAX_NODES];
    // parallel array for distance array, used to keep track for parent of the current node that has the least cost
    for (int i = 0; i < graph->numNodes; i++) {
        // initialize the bfs arrays
        color[i] = 'W';
        distance[i] = INT_MAX;
        parent[i] = -1;
    }

    // we deal with source Index, destIndex
    int sourceIndex = getIndexForCharacterInTheGraphArray(source);
    int destIndex = getIndexForCharacterInTheGraphArray(destination);
    color[sourceIndex] = 'G'; // we are visiting this node which is the source
    distance[sourceIndex] = 0; // we don't need any cost to reach this node
    parent[sourceIndex] = -1; // it doesn't have any parents since we will start from it

    Queue q; // q for the bfs technique where we will save the nodes we want to visit
    initializeQueue(&q); //
    enqueue(&q, source);

    while (!isEmptyQueue(&q)) {
        char u = dequeue(&q);
        int uIndex = getIndexForCharacterInTheGraphArray(u);
        AdjListNode *temp = graph->array[uIndex].head;
        while (temp != NULL) {
            char v = temp->dest;
            int vIndex = getIndexForCharacterInTheGraphArray(v);
            if (color[vIndex] == 'W') {
                color[vIndex] = 'G'; // currently visiting
                distance[vIndex] = distance[uIndex] + temp->weight;
                parent[vIndex] = u;
                enqueue(&q, v);
            }
            temp = temp->next;
        }
        color[uIndex] = 'B'; // change state to visited
    }

    if (distance[destIndex] == INT_MAX) {
        if (writeToFileFlag == 0)
            printf("No path from %c to %c\n", source, destination);
        if (writeToFileFlag == 1)
            fprintf(file, "BFS: No path from %c to %c\n", source, destination);
    } else {
        if (writeToFileFlag == 0)
            printf("BFS Shortest path from %c to %c: ", source, destination);
        if (writeToFileFlag == 1)
            fprintf(file, "BFS Shortest path from %c to %c:\n", source, destination);
        char path[MAX_NODES];
        int count = 0;
        char temp = destination;

        while (temp != -1) {
            path[count++] = temp;
            temp = parent[getIndexForCharacterInTheGraphArray(temp)];
        }

        for (int i = count - 1; i >= 0; i--) {
            if (writeToFileFlag == 0) {
                printf("%c", path[i]);
                if (i > 0) printf(" -> ");
            }

            if (writeToFileFlag == 1) {
                fprintf(file, "%c ", path[i]);
                if (i > 0)
                    fprintf(file, " -> ");
            }
        }

        if (writeToFileFlag == 0)
            printf("\nTotal cost: %d\n", distance[destIndex]);
        if (writeToFileFlag == 1)
            fprintf(file, "\nTotal cost: %d\n\n\n", distance[destIndex]);
    }
}

void dijkstra(Graph *graph, char source, char destination, int writeToFileFlag, FILE *file) {
    int sourceIndex = getIndexForCharacterInTheGraphArray(source);
    int destIndex = getIndexForCharacterInTheGraphArray(destination);

    int dist[MAX_NODES]; // Distance array
    int parent[MAX_NODES]; // Array to store the path
    MinHeap *minHeap = createMinHeap(graph->numNodes);
    if (minHeap == NULL) {
        printf("Failed to create MinHeap.\n");
        return;
    }

    // Initialize distances and parent array
    for (int i = 0; i < graph->numNodes; i++) {
        dist[i] = INT_MAX;
        parent[i] = -1;
        minHeap->array[i] = createMinHeapNode(i, dist[i]);
        minHeap->size++;
        minHeap->position[i] = i;
    }
    dist[sourceIndex] = 0;
    minHeap->array[sourceIndex] = createMinHeapNode(sourceIndex, dist[sourceIndex]);

    // Heapify the source node to have the smallest distance at the root
    decreaseKey(minHeap, sourceIndex, dist[sourceIndex]);

    while (minHeap->size != 0) {
        MinHeapNode *minNode = extractMin(minHeap);
        int u = minNode->element;

        AdjListNode *temp = graph->array[u].head;
        while (temp != NULL) {
            int v = getIndexForCharacterInTheGraphArray(temp->dest);
            if (isInMinHeap(minHeap, v) && dist[u] != INT_MAX && dist[u] + temp->weight < dist[v]) {
                dist[v] = dist[u] + temp->weight;
                parent[v] = u;
                decreaseKey(minHeap, v, dist[v]);
            }
            temp = temp->next;
        }
    }

    // Print the shortest path and the total cost
    if (dist[destIndex] == INT_MAX) {
        if (writeToFileFlag == 0) {
            printf("No path from %c to %c\n", source, destination);
        } else {
            fprintf(file, "Dijkstra: No path from %c to %c\n", source, destination);
        }
    } else {
        // Reconstruct the path from destination to source
        int path[MAX_NODES];
        int count = 0;
        int temp = destIndex;

        // If the parent of a node is -1, it means there's no valid path
        while (temp != -1) {
            path[count++] = temp;
            temp = parent[temp];
        }
        if (writeToFileFlag == 0) {
            printf("Dijkstra Shortest path from %c to %c: ", source, destination);
        } else {
            fprintf(file, "Dijkstra Shortest path from %c to %c: ", source, destination);
        }
        for (int i = count - 1; i >= 0; i--) {
            char nodeChar = getCharFromIndex(path[i]);
            if (writeToFileFlag == 0) {
                printf("%c", nodeChar);
                if (i > 0) printf(" -> ");
            } else {
                fprintf(file, "%c", nodeChar);
                if (i > 0) fprintf(file, " -> ");
            }
        }

        // Print and store the total cost
        if (writeToFileFlag == 0) {
            printf("\nTotal cost: %d\n", dist[destIndex]);
        } else {
            fprintf(file, "\nTotal cost: %d\n", dist[destIndex]);
        }
    }
}


int readRoutersFromFile(char *filename, Graph *graph) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return -1;
    }

    int maxNode = 0;
    char from, to;
    int latency;

    while (fscanf(file, " %c-%c-%d", &from, &to, &latency) == 3) {
        int fromIndex = getIndexForCharacterInTheGraphArray(from);
        int toIndex = getIndexForCharacterInTheGraphArray(to);
        addEdge(graph, from, to, latency);
        printf("From: %c, TO : %c, latency :%d\n", from, to, latency);
        if (graph->parallelArray[fromIndex] == 0) {
            maxNode++;
            graph->parallelArray[fromIndex] = 1;
        }
        if (graph->parallelArray[toIndex] == 0) {
            maxNode++;
            graph->parallelArray[toIndex] = 1;
        }
    }

    fclose(file);
    return maxNode;
}

void printGraph(Graph *graph) {
    for (int i = 0; i < graph->numNodes + 0; i++) {
        if(graph->parallelArray[i] == 0) {
            continue;
        }
        AdjListNode *current = graph->array[i].head;
        printf("Node %c: ", getCharFromIndex(i));
        while (current != NULL ) {
            printf("-> %c(%d) ", current->dest, current->weight);
            current = current->next;
        }
        printf("\n");
    }
}


int main() {
    Graph *graph = createGraph(MAX_NODES);

    int numNodes = 0;
    char source = -1, destination = -1;
    FILE *file = fopen("shortest_distance.txt", "w"); // output file
    if (file == NULL) {
        printf("Error opening file for writing.\n");

    }
    int choice; //menu choice

    do {
        //menu
        printf("\nMenu:\n");
        printf("1. Load routers\n");
        printf("2. Enter source\n");
        printf("3. Enter destination\n");
        printf("4. Exit\n");
        printf("Choose an option: ");

        scanf("%d", &choice);

        switch (choice) {
            case 1:
                numNodes = readRoutersFromFile("routers.txt", graph);
                if (numNodes == -1) {
                    printf("Failed to load graph.\n");
                } else {
                    printf("Graph loaded successfully with %d nodes.\n", numNodes);
                }
            //     printGraph(graph);
                break;
            case 2:
                printf("Enter source router: ");
                scanf(" %c", &source);
            // check if graph contains the source else null
                if (!graphContains(graph, source)) {
                    source = -1;
                    printf("The source router you entered is not in the graph.\n");
                }
                break;
            case 3:
                //  if source == -1 then print "choose a source first"
                if (source == -1) {
                    printf("You need to enter a valid source first\n");
                    break;
                }

                printf("Enter destination router: ");
                scanf(" %c", &destination);


            //   if destination not in graph print error

                if (!graphContains(graph, destination)) {
                    printf("The destination router you entered is not in the graph.\n");
                }
                printf("\n BFS: \n");
                bfs(graph, source, destination, 0,NULL);

                printf("\nDijkstra's Algorithm: \n");
                dijkstra(graph, source, destination, 0,NULL);

                break;
            case 4:

                if (graphContains(graph, source) && graphContains(graph, destination)) {

                    bfs(graph, source, destination, 1, file);
                    dijkstra(graph, source, destination, 1, file);
                    printf("Results saved to shortest_distance.txt\n");
                    fclose(file);
                } else {
                    printf("Choose a correct source and destination first\n");
                }
                break;
            case 5:
                printGraph(graph);
                break;
            default:
                printf("Invalid option. Please try again.\n");
                break;
        }
    } while (choice != 4);

    free(graph);
    return 0;
}
