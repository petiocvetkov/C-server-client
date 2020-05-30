void handleConnection(Node* list, int socketFd){
  long received_bytes = 0;
  Event receiveEvent = {0};

  if (socketFd <= 0)
      return;

  do {
    memset(&receiveEvent, 0, sizeof(Event));

    received_bytes = read(socketFd, &receiveEvent, sizeof(Event));
    if (received_bytes > 0) {
      pthread_mutex_lock(&lock);
      handleEvent(socketFd, list, receiveEvent);
      pthread_mutex_unlock(&lock);
    }
  } while(received_bytes > 0);
}

void handleEvent(int socketFd, Node* list, Event event){
  Message message;
  printEvent(&event);
  switch (event.eventType) {
    case ADD_PERSON:
      printEvent(&event);
      message = addElement(list, eventToPerson(event));
      (void) write(socketFd, &message, sizeof(Message));
      break;
    case GET_ALL:
      getAll(list, socketFd);
      break;
    case  REMOVE_BY_EGN:
      message = removePatient(list, eventToPerson(event));
      (void) write(socketFd, &message, sizeof(Message));
      break;
  }
}

Node* loadDate(){
  FILE* fid = fopen(FILE_NAME, "rb");
  if(fid == NULL){
    printf("Problem with openning file\n");
    return NULL;
  }
  Node* list  = malloc(sizeof(Node));
  list->value = NULL;
  list ->next = list;
  list ->previous = list;

  while (!feof(fid)) {
    Person* person = malloc(sizeof(Person));
    if(!fread(person, sizeof(Person), 1, fid)){
      free(person);
      break;
    }
    addElement(list, person);
  }

  printf("Loaded elements from file\n");
  printAll(list);

  fclose(fid);
  return list;
}

Message addElement(Node* list, Person* value){
  if (isExist(list, value->egn) != NULL) {
    return (Message) {0,"CreatePerson" , "Person with this egn exist"};
  }
  Node* new = malloc(sizeof(Node));
  new->value = value;
  new-> previous = list->previous;
  new->next = list;
  list->previous->next = new;
  list->previous = new;

  addElementToFile(value);
  return (Message){1, "CreatePerson", "Adding completed"};
}

void printAll(Node* list){
  Node* current = list->next;
  while (current != list) {
    printf("%s\n", current->value->name );
    current = current->next;
  }
}

Message removePerson(Node* list, char* egn){
  Node* foundElement = isExist(list, egn);
  if (foundElement != NULL) {
    foundElement->previous->next = foundElement->next;
    foundElement->next->previous = foundElement->previous;
    return (Message){1,"RemovePerson" ,"Person removed"};
  }

  return (Message){1,"RemovePerson" ,"Person not found"};

}

Node* isExist(Node* list, char* egn){
  Node* current = list->next;
  while (current != list) {
    if (!strcmp(current->value->egn, egn)) {
      return current;
      break;
    }
    current = current->next;
  }
  return NULL;
}

Person* eventToPerson(Event event){
  Person* person = malloc(sizeof(Person));
  strcpy(person->name, event.name);
  strcpy(person->egn, event.egn);
  strcpy(person->diagnosis, event.diagnosis);
  return person;
}

void addElementToFile(Person* person){
  FILE* fid = fopen(FILE_NAME, "ab");
  if(fid == NULL){
    printf("problem with add int file movie\n" );
    return;
  }
  fwrite(person, sizeof(Person), 1, fid);
  fclose(fid);
}

void getAll(Node* list, int socketFd){
  Node* current = list->next;
  while (current != list) {
    Event* event = personToEvent(current->value);
    (void) write(socketFd, event, sizeof(Event));
    free(event);
    current = current->next;
  }
  Event eventForEnd = {END, "", "", ""};
  (void) write(socketFd, &eventForEnd, sizeof(Event));
}

Event* personToEvent(Person* person){
  Event* event = malloc(sizeof(Event));
  strcpy(event->name, person->name);
  strcpy(event->egn, person->egn);
  strcpy(event->diagnosis, person->diagnosis);
  event->eventType = GET_ALL;
  return event;
}


void deleteElementAndSave(Node* list, Node* target){
    target->previous->next = target->next;
    target->next->previous = target->previous;
    free(target);
    saveListToFile(list);
}

void saveListToFile(Node* list){
  Node* current = list->next;
  FILE* fid = fopen(FILE_NAME, "wb");
  if(fid == NULL){
    printf("problem with add int file movie\n" );
    return;
  }
  while (current != list) {
    fwrite(current->value, sizeof(Person), 1, fid);
    current = current->next;
  }
  fclose(fid);
}

Message removePatient(Node* list, Person* person){

  Node* current = list->next;
  while (current != list) {
    if (strcmp(current->value->egn, person->egn) == 0) {
      deleteElementAndSave(list, current);
      return   (Message) {0,"RemovePersonByEgn" , "Person was removed successfully!"};
    }
    current = current->next;
  }
  return   (Message) {0,"RemovePersonByEgn" , "Person not found!"};

}
