typedef struct Person{
  char name[50];
  char egn[11];
  char diagnosis[50];
}Person;

typedef struct Node{
  Person* value;
  struct Node* next;
  struct Node* previous;
}Node;
