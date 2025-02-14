#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
// Mohammed Hamo 1230394
#define inputFile "C:/Users/moham/OneDrive/Desktop/University/secondYear/firstSemester/dataStructure_comp2421/projects/input.txt" // for the dr : change the input file name if needed
#define outputFile "C:/Users/moham/OneDrive/Desktop/University/secondYear/firstSemester/dataStructure_comp2421/projects/output.txt" // for the dr : change the output file name if needed

typedef struct node *ptr; // to make dealing with the pointer of struct node easier

typedef struct node {
    // initializing struct node1
    int coefficient;
    int power;
    ptr next;
} node;

typedef ptr polynomial; // we will use this when dealing with pointers of struct node


typedef struct stackNode *stackPtr;  // to make dealing with the pointer of struct stackPtr easier

typedef struct stackNode {
    // initializing struct stackNode
    polynomial list;
    stackPtr next;
} stackNode;

typedef stackPtr Stack; //  this will be the node before the top of the stack

//used in project linked list functions prototypes
polynomial createNode(int coef, int pow);

void addToLinkedList(polynomial *head, int coefficient, int power);


//used in project stack functions prototypes
stackPtr createStackNode(polynomial polyHead);

void push(stackPtr *top, polynomial linkedList);

polynomial pop(stackPtr *top);

stackPtr peek(stackPtr top);

Stack copyStack(Stack original);

int isEmptyStack(Stack stack);


// program functions prototypes

void startCalculator();

void parsePolynomial(char *line, polynomial *poly);

Stack loadPolynomials(Stack *stack);

void printPolynomial(polynomial poly);

void printPolynomials(Stack stack);

void addPolynomial(Stack originalStack, Stack *resultStack);

polynomial addTwoPolynomials(polynomial p1, polynomial p2);

polynomial subtractTwoPolynomials(polynomial p1, polynomial p2);

void subtractPolynomial(Stack originalStack, Stack *resultStack);

polynomial multiplyTwoPolynomials(polynomial p1, polynomial p2);

void multiplyPolynomial(Stack originalStack, Stack *resultStack);

void printResults(Stack resultStack, char *operationNames[], int usedOperation[], int operationIndex);

void printResultsToFile(Stack resultStack, char *operationNames[], int usedOperation[], int operationIndex);

void exitProgram(Stack *stack, Stack *resultStack);

void printPolynomialResult(polynomial p);



polynomial createNode(int coef, int pow) { //creating new node for linked list - Time complexity : O(1)
    polynomial newNode = (polynomial) malloc(sizeof(node));
    if (newNode == NULL) {
        printf("Memory allocation failed");
        return NULL;
    }
    newNode->coefficient = coef;
    newNode->power = pow;
    newNode->next = NULL;
    return newNode;
}

void addToLinkedList(polynomial *head, int coefficient, int power) {  // Adding node to the end of a linkedList - Time complexity : O(n)
    polynomial newNode = createNode(coefficient, power);

    if (*head == NULL) {
        *head = newNode;
    } else {
        ptr curr = *head;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = newNode;
    }
}

void removeNode(polynomial *head, polynomial nodeToRemove) { // removing a node from linkedList - Time complexity : O(n)
    if (*head == NULL || nodeToRemove == NULL) return;

    if (*head == nodeToRemove) {
        // If the node to be removed is the head
        *head = nodeToRemove->next;
    } else {
        // Find the previous node
        polynomial prev = *head;
        while (prev != NULL && prev->next != nodeToRemove) {
            prev = prev->next;
        }

        if (prev != NULL) {
            // Remove the node from the list
            prev->next = nodeToRemove->next;
        }
    }

    // Free the memory of the removed node
    free(nodeToRemove);
}


stackPtr createStackNode(polynomial polyHead) { // Creating a stackNode that will contain the linkedList (polynomial) - Time complexity : O(1)
    stackPtr newNode = (stackPtr) malloc(sizeof(stackNode));
    if (newNode == NULL) {
        printf("Memory allocation failed\n\n");
        return 0;
    }
    newNode->list = polyHead;
    newNode->next = NULL;
    return newNode;
}


void push(stackPtr *top, polynomial linkedList) { // pushing( adding a stackNode (poly) at the top of the stack ) - Time complexity : O(1)
    stackPtr newNode = createStackNode(linkedList);

    newNode->next = *top;
    *top = newNode;
}


polynomial pop(stackPtr *top) {// popping ( deleting ) poly from the top of the stack - Time complexity : O(1)
    if (*top == NULL) {
        return NULL;
    }
    stackPtr temp = *top;
    polynomial polyHead = temp->list;
    *top = (*top)->next;
    free(temp);
    return polyHead;
}

int isEmptyStack(stackPtr S) {// function returns 1 if the stack is empty or 0 if stack is not empty- Time complexity : O(1)
    if(S == NULL || S->next == NULL) return 1;
    return 0;
}

stackPtr peek(stackPtr top) { // - Time complexity : O(1)
    if(!isEmptyStack(top)) return top->next;

    printf("Empty stack\n");
    return top;
}


// Deep copy a polynomial (linked list of nodes)
polynomial deepCopyPolynomial(polynomial poly) {
    if (poly == NULL)
        return NULL; // Handle empty polynomial

    polynomial newPoly = malloc(sizeof(node)); // Create the first node of the new polynomial
    if (newPoly == NULL) {
        printf("Memory allocation failed for polynomial.\n");
        exit(1);
    }

    // Copy the first node
    newPoly->coefficient = poly->coefficient;
    newPoly->power = poly->power;
    newPoly->next = NULL;

    polynomial currentOld = poly->next;
    polynomial currentNew = newPoly;

    // Copy the remaining nodes
    while (currentOld != NULL) {
        polynomial newNode = malloc(sizeof(node));
        if (newNode == NULL) {
            printf("Memory allocation failed for node.\n");
            exit(1);
        }

        newNode->coefficient = currentOld->coefficient;
        newNode->power = currentOld->power;
        newNode->next = NULL;

        currentNew->next = newNode; // Link the new node
        currentNew = newNode; // Move to the new node
        currentOld = currentOld->next; // Move to the next node
    }

    return newPoly;
}

// Deep copy a stack of polynomials
Stack copyStack(Stack originalStack) {
    if (originalStack == NULL)
        return NULL; // empty stack

    // Create a new stack to store the copied elements
    Stack newStack = NULL;

    // Iterate through the original stack and deep copy each polynomial
    while (originalStack != NULL) {
        // Deep copy the polynomial
        polynomial copiedPoly = deepCopyPolynomial(originalStack->list);

        // Create a new stack node
        stackPtr newStackNode = malloc(sizeof(stackNode));
        if (newStackNode == NULL) {
            printf("Memory allocation failed for stack node.\n");
            exit(1);
        }

        newStackNode->list = copiedPoly;  // Assign copied polynomial to the new stack node
        newStackNode->next = newStack;    // Link the new stack node
        newStack = newStackNode;          // Move to the new top of the stack

        // Move to the next node in the original stack
        originalStack = originalStack->next;
    }

    return newStack;
}

// Function to parse a polynomial line we read from file and add terms to the linked list
void parsePolynomial(char *line, polynomial *poly) {
    int coef, pow, coef_sign, pow_sign;
    int i = 0;

    while (line[i] != '\0' && line[i] != '\n') {
        // These are the defaults for any plynomial
        coef = 1;
        pow = 0;
        coef_sign = 1;
        pow_sign = 1;
        // this is for the first term if it starts with a - like -x+2x^2.... etc
        if (line[i] == '-') {
            coef_sign = -1;
            i++;
        } else if (line[i] == '+') {
            // ths is for some test cases it wont affect in the normal ones
            i++;
        }

        // Here we used the ctype.h library
        // if it starts with a digit this means
        if (isdigit(line[i])) {
            coef = 0;
            while (isdigit(line[i])) { // if the coef is bigger than 9 this will be needed
                coef = coef * 10 + (line[i] - '0');
                i++;
            }
        }
        coef *= coef_sign;// this is connected with the first thing when we checked if the line[i] == '-'

        if (line[i] == 'x') {
            pow = 1; // again default power equals to 1
            i++;
            if (line[i] == '^') { // this only will be used if its bigger than 1
                i++;
                if (line[i] == '-') { // to check if the power is negative
                    pow_sign = -1;
                    i++;
                }
                pow = 0;
                while (isdigit(line[i])) { // same as above
                    pow = pow * 10 + (line[i] - '0');
                    i++;
                }
                pow *= pow_sign;
            }
        }

        addToLinkedList(poly, coef, pow); // finally after parsing the term we will add it to the polynomial
    }
}


Stack loadPolynomials(Stack *stack) { // this function will read from the file and send line by line to another function that will partition it and extract terms from it
    FILE *file = fopen(inputFile, "r"); // the inputFile is constant defined at the top of the program
    if (!file) {
        printf("Error opening file\n");
        return NULL;
    }
    char line[280]; // I assumed that the max length of a line is 280 character

    // Read line by line and parse it as a polynomial then push it onto the stack
    while (fgets(line, sizeof(line), file)) {
        polynomial polyHead = NULL; // we will initialize a polynomial
        parsePolynomial(line, &polyHead);// we will add every term to the poly
        push(stack, polyHead); // if the line is ended we will push the poly to the stack containing the polys
    }
    printf("Loaded Successfully.\n\n");
    fclose(file);
    return *stack;
}

void printPolynomial(polynomial poly) { // printing the polynomials details
    polynomial current = poly;
    while (current != NULL) {
        printf("Coefficient = %d, Power = %d\n", current->coefficient, current->power); // print the details for the terms
        current = current->next;
    }
}

void printPolynomials(Stack stack) { // function to print polynomials
    printf("\n printing... \n\n\n");

    if (stack == NULL) {
        printf("The stack is empty\n");
        return;
    }

    Stack c = copyStack(stack);
    // Output the polynomials from the stack
    int polyNum = 1;
    while (c != NULL) {
        polynomial polyHead = pop(&c); // poping line by line ( poly by poly)
        printf("Polynomial %d:\n", polyNum++);

        printPolynomialResult(polyHead); // print polynomial like the input file format
        printf("Details : \n");
        printPolynomial(polyHead); // printing the details of each poly
        printf("\n");

        // Free memory for each polynomial
        while (polyHead != NULL) {
            polynomial temp = polyHead;
            polyHead = polyHead->next;
            free(temp);
        }
    }
}


polynomial addTwoPolynomials(polynomial p1, polynomial p2) { // Here is the function to add two polynomials coming from popping them from the stack
    polynomial result = NULL;

    // First loop goes through each term in the first polynomial
    for (polynomial ptr1 = p1; ptr1 != NULL; ptr1 = ptr1->next) {
        int found = 0;

        // nested loop finds matching power in the second polynomial
        for (polynomial ptr2 = p2; ptr2 != NULL; ptr2 = ptr2->next) {
            if (ptr1->power == ptr2->power) {
                int sumCoef = ptr1->coefficient + ptr2->coefficient;
                addToLinkedList(&result, sumCoef, ptr1->power); // Add the term regardless of coefficient value
                found = 1;
                break;
            }
        }

        // If no matching power was found in p2, add the term p1 as is
        if (!found) {
            addToLinkedList(&result, ptr1->coefficient, ptr1->power);
        }
    }

    // Add remaining terms from p2 that have no matching power in p1
    for (polynomial ptr2 = p2; ptr2 != NULL; ptr2 = ptr2->next) {
        int found = 0;

        // Check if this term's power already exists in the result
        for (polynomial resPtr = result; resPtr != NULL; resPtr = resPtr->next) {
            if (resPtr->power == ptr2->power) {
                found = 1;
                break;
            }
        }

        // If the term's power wasn't found, add it to the result
        if (!found) {
            addToLinkedList(&result, ptr2->coefficient, ptr2->power);
        }
    }

    // Iterate through the list and remove nodes with zero coefficients
    polynomial current = result;
    while (current != NULL) {
        if (current->coefficient == 0) {
            polynomial temp = current;
            current = current->next;
            removeNode(&result, temp); // Remove the node with zero coefficient
        } else {
            current = current->next;
        }
    }

    return result;
}

void addPolynomial(Stack originalStack, Stack *resultStack) { // - Time complexity : O(n)
    // First we copy a stack to make sure not to deal with the original stack and destroy it
     Stack copy = copyStack(originalStack);

    // we will pop two polys and then call the addTwoPolynomials function to add them from the copy stack
    polynomial sum = NULL;
    if (copy != NULL) {
        sum = pop(&copy);
    }

    while (copy != NULL) {
        polynomial nextPoly = pop(&copy);
        polynomial newSum = addTwoPolynomials(sum, nextPoly);

        // Free the old sum
        while (sum != NULL) {
            polynomial temp = sum;
            sum = sum->next;
            free(temp);
        }

        sum = newSum;
    }

    // Finally Push the final result to the result stack
    push(resultStack, sum);
    printf("Added.\n\n");
}



polynomial subtractTwoPolynomials(polynomial p1, polynomial p2) { // the same as addition but here we subtract the two polynomials
    polynomial result = NULL;

    // First loop goes through each term in the first polynomial
    for (polynomial ptr1 = p1; ptr1 != NULL; ptr1 = ptr1->next) {
        int found = 0;

        // Nested loop finds matching power in the second polynomial
        for (polynomial ptr2 = p2; ptr2 != NULL; ptr2 = ptr2->next) {
            if (ptr1->power == ptr2->power) {
                int diffCoef = ptr1->coefficient - ptr2->coefficient;
                addToLinkedList(&result, diffCoef, ptr1->power);
                found = 1;
                break;
            }
        }

        // If no matching power was found in p2 add the term from p1 as is
        if (!found) {
            addToLinkedList(&result, ptr1->coefficient, ptr1->power);
        }
    }

    // Add remaining terms from p2 that have no matching power in p1 with negated coefficients
    for (polynomial ptr2 = p2; ptr2 != NULL; ptr2 = ptr2->next) {
        int found = 0;

        // Check if this term's power already exists in the result
        for (polynomial resPtr = result; resPtr != NULL; resPtr = resPtr->next) {
            if (resPtr->power == ptr2->power) {
                found = 1;
                break;
            }
        }

        // If the term's power wasn't found, add it to the result with a negated coefficient
        if (!found) {
            addToLinkedList(&result, -ptr2->coefficient, ptr2->power);
        }
    }

    // Remove any nodes with a zero coefficient from the result
    polynomial current = result;
    while (current != NULL) {
        if (current->coefficient == 0) {
            polynomial temp = current;
            current = current->next;
            removeNode(&result, temp);
        } else {
            current = current->next;
        }
    }

    return result;
}


void subtractPolynomial(Stack originalStack, Stack *resultStack) { //The same idea as addition - Time complexity : O(n)
    Stack copy = copyStack(originalStack);

    // Initialize the result as the first polynomial in the stack
    polynomial difference = NULL;
    if (copy != NULL) {
        difference = pop(&copy);
    }

    // Perform subtraction of each polynomial in the stack
    while (copy != NULL) {
        polynomial nextPoly = pop(&copy);
        polynomial newDifference = subtractTwoPolynomials(difference, nextPoly);

        // Free the previous difference
        while (difference != NULL) {
            polynomial temp = difference;
            difference = difference->next;
            free(temp);
        }

        difference = newDifference;
    }

    // Push the final result onto the result stack
    push(resultStack, difference);
    printf("Subtracted.\n\n");
}

polynomial multiplyTwoPolynomials(polynomial p1, polynomial p2) {
    polynomial result = NULL;

    // Outer loop for each term in p1
    for (polynomial ptr1 = p1; ptr1 != NULL; ptr1 = ptr1->next) {
        // Inner loop for each term in p2
        for (polynomial ptr2 = p2; ptr2 != NULL; ptr2 = ptr2->next) {
            // Multiply the coefficients and add the powers
            int coef = ptr1->coefficient * ptr2->coefficient;
            int power = ptr1->power + ptr2->power;

            // Add this term to the result polynomial
            polynomial current = result;
            polynomial prev = NULL;
            int found = 0;

            // Check if a term with this power already exists
            while (current != NULL) {
                if (current->power == power) {
                    // If it exists, add the coefficient to the existing term
                    current->coefficient += coef;
                    found = 1;
                    break;
                }
                prev = current;
                current = current->next;
            }

            // If no term with this power exists, create a new node
            if (!found) {
                polynomial newNode = createNode(coef, power);
                if (prev == NULL) {
                    result = newNode;
                } else {
                    prev->next = newNode;
                }
            }
        }
    }

    return result;
}

void multiplyPolynomial(Stack originalStack, Stack *resultStack) {
    // Copy the original stack ( we will do the rest of the code with the copy )
    Stack copy = copyStack(originalStack);

    // Initialize the product with the first polynomial in the stack
    polynomial product = NULL;
    if (copy != NULL) {
        product = pop(&copy);
    }

    // Multiply each polynomial from the stack
    while (copy != NULL) {
        polynomial nextPoly = pop(&copy);
        polynomial newProduct = multiplyTwoPolynomials(product, nextPoly);

        // Free the previous product
        while (product != NULL) {
            polynomial temp = product;
            product = product->next;
            free(temp);
        }

        product = newProduct;
    }

    // Push the final product to the result stack
    push(resultStack, product);
    printf("Multiplied.\n\n");
}

void printPolynomialResult(polynomial p) {
    if (p == NULL) {
        printf("0\n");
        return;
    }

    polynomial current = p;
    int firstTerm = 1; // This is to handle the formatting of the first term

    while (current != NULL) {
        int coef = current->coefficient;
        int power = current->power;

        // skipping zero coefficients
        if (coef == 0) {
            current = current->next;
            continue;
        }

        // printing the sign for the first term
        if (firstTerm) {
            if (coef < 0) {
                printf("%d", coef);
            } else if (coef > 1) {
                printf("%d", coef);
            }

            firstTerm = 0;
        } else {
            if (coef < 0) {
                printf("-%d", -coef);
            } else if (coef > 1) {
                printf("+%d", coef);
            }
        }

        // printing the power part of the term
        if (power != 0) {
            if (power == 1)
                printf("x");
            else
                printf("x^%d", power);
        }

        current = current->next;
    }

    printf("\n");
}


void printResults(Stack resultStack, char *operationNames[], int usedOperation[], int operationIndex) {
    printf("Printing Results..\n\n");
    if (resultStack == NULL) {
        printf("The results stack is empty.\n");
        return;
    }
    Stack copy = copyStack(resultStack);

    int polyNum = 1;
    operationIndex =0;
    while (copy != NULL) {
        printf("Result Polynomial %d: for %s\n", polyNum++, operationNames[usedOperation[operationIndex]]);
        polynomial polyHead = pop(&copy); // Pop each polynomial from the results stack
        printPolynomialResult(polyHead);
        operationIndex++;
        // Free memory for each polynomial after printing
        while (polyHead != NULL) {
            polynomial temp = polyHead;
            polyHead = polyHead->next;
            free(temp);
        }
        printf("\n");
    }
}

// Helper function to print a polynomial to a file
void printPolynomialResultToFile(polynomial p, FILE *file) {
    if (p == NULL) {
        fprintf(file, "0\n");
        return;
    }

    polynomial current = p;
    int firstTerm = 1; // This flag helps with formatting the first term

    while (current != NULL) {
        int coef = current->coefficient;
        int power = current->power;

        // skipping zero coefficients
        if (coef == 0) {
            current = current->next;
            continue;
        }

        // printing the sign and coefficient
        if (firstTerm) {
            if (coef < 0) {
                fprintf(file, "%d", coef);
            } else if (coef > 1) {
                fprintf(file, "%d", coef);
            }
            firstTerm = 0;
        } else {
            if (coef < 0) {
                fprintf(file, "-%d", -coef);
            } else if (coef > 1) {
                fprintf(file, "+%d", coef);
            }
        }

        // Print the variable and exponent
        if (power != 0) {
            if (power == 1) {
                fprintf(file, "x");
            } else {
                fprintf(file, "x^%d", power);
            }
        }

        current = current->next;
    }

    fprintf(file, "\n"); // End of polynomial line
}

void printResultsToFile(Stack resultStack, char *operationNames[], int usedOperation[], int operationIndex) {
    FILE *file = fopen(outputFile,"w");
    if (!file) {
        printf("Error opening file\n");
        return;
    }

    if (resultStack == NULL) {
        fprintf(file, "The results stack is empty.\n\n");
        return;
    }
    operationIndex--;

    int polyNum = 1;
    while (resultStack != NULL) {
        fprintf(file, "Result Polynomial %d: for %s\n", polyNum++, operationNames[usedOperation[operationIndex]]);
        operationIndex--;
        polynomial polyHead = pop(&resultStack); // Pop each polynomial from the stack
        printPolynomialResultToFile(polyHead, file);

        // Free memory for each polynomial after printing
        while (polyHead != NULL) {
            polynomial temp = polyHead;
            polyHead = polyHead->next;
            free(temp);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

void freePolynomial(polynomial poly) { // function used to free nodes in linked list term by term
    while (poly != NULL) {
        polynomial temp = poly;
        poly = poly->next;
        free(temp);
    }
}

void exitProgram(Stack *stack, Stack *resultStack) { // we will use this function at the end of our program when we want to free all cells
    // Free all polynomials from the main stack
    while (*stack != NULL) {
        polynomial poly = pop(stack);
        freePolynomial(poly);
    }

    // Free all polynomials from the result stack
    while (*resultStack != NULL) {
        polynomial poly = pop(resultStack);
        freePolynomial(poly);
    }

    printf("Exiting the program, Thank you!\n");
    exit(0);
}

void startCalculator() {
    printf("Welcome to the polynomials Calculator\n\n");
    Stack stack = NULL;
    // The main stack for the input file

    Stack resultsStack = NULL;
    //the stack that will save the results after operations

    // the next 3 lines are used for mentioning the operation was used when printing
    char *operations[] = {"Addition Operation", "Subtraction Operation", "Multiplication Operation"};
    int usedOperation[20];
    int operationIndex = 0;

    int running = 1;

    int choice;
    while (running) {
        printf("Choose what you want from the menu : \n");

        printf("1. Load the polynomials  file \n"
            "2. Print the polynomials \n"
            "3. Add the polynomials \n"
            "4. Subtract the polynomials \n"
            "5. Multiply the polynomials \n"
            "6. Print the results to the screen \n"
            "7. Print the results to file \n"
            "8. Exit\n");


        scanf("%d", &choice);
        switch (choice) {
            case 1:
                stack = loadPolynomials(&stack);
                break;
            case 2:
                printPolynomials(stack);
                break;
            case 3:
                usedOperation[operationIndex++] = 0;
                addPolynomial(stack, &resultsStack);
                break;
            case 4:

                usedOperation[operationIndex++] = 1;
                subtractPolynomial(stack, &resultsStack);
                break;
            case 5:

                usedOperation[operationIndex++] = 2;
                multiplyPolynomial(stack, &resultsStack);
                break;
            case 6:
                printResults(resultsStack, operations, usedOperation, operationIndex);
                break;
            case 7:
                printResultsToFile(resultsStack, operations, usedOperation, operationIndex);
                break;
            case 8:
                exitProgram(&stack, &resultsStack);
                running = 0;
                break;
            default:
                printf("Invalid Choice\n Enter from 1 to 8");
        }
    }
}

int main(void) {
    startCalculator();
    return 0;
}