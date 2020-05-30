#define ADD_PERSON 1
#define GET_ALL 2
#define REMOVE_BY_EGN 3
#define END 0

typedef struct Message{
  size_t success;
  char type[50];
  char message[100];
}Message;

typedef struct Event{
  short eventType;
  char name[50];
  char egn[50];
  char diagnosis[50];
}Event;

void printEvent(Event* event){
  printf("eventType: %d\n", event->eventType);
  printf("name: %s\n", event->name);
  printf("egn: %s\n", event->egn);
  printf("diagnosis: %s\n", event->diagnosis);

}
