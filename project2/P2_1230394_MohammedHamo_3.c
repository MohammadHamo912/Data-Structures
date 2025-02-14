#include <stdio.h>
#include <stdlib.h>
/*
    Project 2 runner up tournament
    Mohammed Hamo 1230394
    Section 3

*/
struct QueueRecord {
    // Queue structure
    // we used the array (not the linked list) to implement the queue
    int Capacity;
    int Front;
    int Rear;
    int Size;
    int *Array;
};

typedef struct QueueRecord *Queue;

struct StackRecord {
    // Stack structure
    // we used the array (not the linked list )to implement the stack
    int Capacity;
    int Top;
    int *Array;
};

typedef struct StackRecord *Stack;

Queue CreateQueue(int MaxElements) {
    // create a new Queue with specified maximum capacity

    if (MaxElements <= 0) {
        printf("Invalid queue size.\n");
        return NULL;
    }
    Queue Q = (Queue) malloc(sizeof(struct QueueRecord));
    if (Q == NULL) {
        printf("Memory allocation failed for queue.\n");
        return NULL;
    }
    Q->Array = (int *) malloc(sizeof(int) * MaxElements);
    if (Q->Array == NULL) {
        printf("Memory allocation failed for queue array.\n");
        free(Q);
        return NULL;
    }
    Q->Capacity = MaxElements;
    Q->Front = 0;
    Q->Rear = -1;
    Q->Size = 0;
    return Q;
}

int IsEmptyQueue(Queue Q) {
    // check if the queue is empty
    if (Q == NULL) {
        printf("Queue is empty\n");
        return 1;
    }

    return Q->Size == 0;
}

void Enqueue(Queue Q, int x) {
    if (Q == NULL) {
        printf("Queue is not created yet\n");
        return;
    }
    if (Q->Size >= Q->Capacity) {
        printf("Queue overflow.\n");
        return;
    }
    Q->Rear = (Q->Rear + 1) % Q->Capacity;
    Q->Array[Q->Rear] = x;
    Q->Size++;
}

int Dequeue(Queue Q) {
    if (Q == NULL) {
        printf("Cannot dequeue from a NULL queue.\n");
        return -10002;
    }

    if (IsEmptyQueue(Q)) {
        printf("Queue underflow.\n");
        return -10002;
    }
    int Value = Q->Array[Q->Front];
    Q->Front = (Q->Front + 1) % Q->Capacity;
    Q->Size--;
    return Value;
}

Queue CopyQueue(Queue Q) {
    if (Q == NULL) {
        printf("Cannot copy a NULL queue.\n");
        return NULL;
    }
    Queue NewQueue = CreateQueue(Q->Capacity);
    if (NewQueue == NULL) {
        printf("Failed to allocate memory for copied queue.\n");
        return NULL;
    }
    for (int i = 0; i < Q->Size; i++) {
        int index = (Q->Front + i) % Q->Capacity;
        Enqueue(NewQueue, Q->Array[index]);
    }
    return NewQueue;
}

void DisposeQueue(Queue Q) {
    if (Q == NULL) {
        return;
    }
    free(Q->Array);
    free(Q);
}

// Stack functions
Stack CreateStack(int MaxElements) {
    if (MaxElements <= 0) {
        printf("Invalid stack size.\n");
        return NULL;
    }

    Stack S = (Stack) malloc(sizeof(struct StackRecord));
    if (S == NULL) {
        printf("Memory allocation failed for stack.\n");
        return NULL;
    }

    S->Array = (int *) malloc(sizeof(int) * MaxElements);
    if (S->Array == NULL) {
        printf("Memory allocation failed for stack array.\n");
        free(S);
        return NULL;
    }
    S->Capacity = MaxElements;
    S->Top = -1;
    return S;
}

int IsEmptyStack(Stack S) {
    if (S == NULL) {
        printf("Stack is NULL.\n");
        return -1;
    }
    return S->Top == -1;
}

void Push(Stack S, int x) {
    if (S == NULL) {
        printf("Cannot push to a NULL stack.\n");
        return;
    }

    if (S->Top >= S->Capacity - 1) {
        printf("Stack overflow..\n");
        return;
    }

    S->Array[++S->Top] = x;
}

int Pop(Stack S) {
    if (S == NULL) {
        printf("Cannot pop from a NULL stack.\n");
        return -1;
    }

    if (IsEmptyStack(S)) {
        printf("Stack underflow. Cannot pop.\n");
        return -1;
    }
    return S->Array[S->Top--];
}

void DisposeStack(Stack S) {
    if (S == NULL) {
        return;
    }
    free(S->Array);
    free(S);
}

// prototypes
int playMatch(int team1, int team2, int round);

int simulateTournament(Queue Q, int teamCount, int globalWinner, Stack Losers);

int tournament(Queue Q, int teamCount);

// here we execute the match
int playMatch(int team1, int team2, int round) {
    if (round % 2 == 1) return (team1 < team2) ? team1 : team2; // Odd round: smaller ID wins
    return (team1 > team2) ? team1 : team2; // Even round: larger ID wins
}

int simulateTournament(Queue Q, int teamCount, int globalWinner, Stack Losers) {
    /* Here we simulate the tournament three times : ( we will use this function three times)
    first time we assumed that the global winner is -10002 which is out of id's range and that's
    because we still don't know who is the winner but our main goal first time is to determine the winner

    second time we simulate this tournament to push all teams who played against the winner into a stack
    then we will use this stack to make the runner-up tournament

    third time we will simulate the runner-up tournament and find who is the real runner-up

*/
    int round = 1; // this is the round counter to determine what round we are at

    while (Q->Size > 1) {
        // we will continue playing matches till we have one player left, he will be the winner
        int initialSize = Q->Size;
        int matches = Q->Size / 2; // because every team will play only one match at the current round
        Queue NextRound = CreateQueue(teamCount);
        // this is a temporary queue that will save the winners who will play the next round

        // Handle odd team count
        if (initialSize % 2 == 1) {
            // if the initialSize of the queue( number of teams at the current round) is odd then we will do the following :
            // we will re-enqueue all teams except the last one
            // so the last team will be at the first of our queue, and then we will remove it from this queue and enqueue it
            // to the first match at the next round
            for (int i = 0; i < initialSize - 1; i++) {
                int team = Dequeue(Q);
                Enqueue(Q, team);
            }
            // here we are sure that the last team the first element in our queue so we will Dequeue it
            int lastTeam = Dequeue(Q);
            Enqueue(NextRound, lastTeam); // we Add the last team to the front of the next round queue
        }


        for (int i = 0; i < matches; i++) {
            // in this loop we simulate every match and the winner will be chosen based on our current round
            int team1 = Dequeue(Q); // first team
            int team2 = Dequeue(Q); // second team
            int winner = playMatch(team1, team2, round);
            // you can take a look in playMatch function to see how we determine the winner
            Enqueue(NextRound, winner); // enqueue the winner to the next round

            // this function will be used only in the second use of the simulateTournament function
            // if the global winner is not -10002 (the flag we put that is not in the range of id's, then here we should have
            // sent the real global winner to the function ,and we want to keep track on the players he played against
            // If the current match loser played against the global winner, push to Losers stack
            if (globalWinner != -10002 && (winner == globalWinner)) {
                int loser = (winner == team1) ? team2 : team1;
                Push(Losers, loser);
            }
        }

        DisposeQueue(Q); // make the queue empty to make it ready for the next round
        Q = NextRound; // the q will be holding the teams for the next round
        round++;
    }

    int winner = Dequeue(Q); //after the queue only have one winner then we will return it
    DisposeQueue(Q); // free the queue

    return winner;
}

// Main tournament function
int tournament(Queue Q, int teamCount) {
    // Copy the original queue to reuse it later while saving the losers
    Queue OriginalQueue = CopyQueue(Q);
    // Simulate the tournament to find the global winner
    Stack Losers = CreateStack(teamCount); // we
    int globalWinner = simulateTournament(Q, teamCount, -10002, Losers);

    // Simulate the tournament again to track losers against the global winner
    simulateTournament(OriginalQueue, teamCount, globalWinner, Losers);

    // we have already saved the losers in a stack now we will move the elements to a queue to simulate the runner-up tournament
    // Create a queue for the runner-up tournament
    Queue RunnerUpQueue = CreateQueue(teamCount);
    while (!IsEmptyStack(Losers)) {
        Enqueue(RunnerUpQueue, Pop(Losers));
    }
    DisposeStack(Losers);

    // Here we simulate the runner-up tournament
    int runnerUp = simulateTournament(RunnerUpQueue, teamCount, -10002, NULL);
    // there is no need for the global winner nor the losers stack
    return runnerUp;
}

int main() {
    int n, teamID;
    while (1) {
        scanf("%d", &n);
        if (n == -9999) break;

        Queue Q = CreateQueue(n);
        for (int i = 0; i < n; i++) {
            scanf("%d", &teamID);
            Enqueue(Q, teamID);
        }

        int runnerUp = tournament(Q, n);
        printf("%d\n", runnerUp);
        DisposeQueue(Q); // free the queue to play the next tournament.
    }

    return 0;
}
