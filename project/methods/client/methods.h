void start(int socketFd){
  int option;
  while (1) {
    printf("Choose option\n");
    printf("|   1. ADD NEW PATIENT                         |\n");
    printf("|   2. GET ALL PATIENTS                        |\n");
    printf("|   3. REMOVE PATIENT BY EGN                   |\n");
    printf("|   0. EXIT                                    |\n");
    scanf("%d", &option);
    switch (option) {
      case 0:
        return;
        break;
      case 1:
        addPatient(socketFd);
        break;
      case 2:
        getAll(socketFd);
        break;
      case 3:
      deletePatient(socketFd);
        break;
    }
  }
}

void addPatient(int socketFd){
  Event* event = malloc(sizeof(Event));
  event->eventType = ADD_PERSON;
  printf("Enter patient name:");
  scanf("%s",event->name );
  do {
    printf("Enter patient egn(10 symbols):");
    scanf("%s",event->egn );
  } while(validateEgn(event->egn) == 0);
  printf("Enter patient diagnosis:");
  scanf("%s",event->diagnosis );
  (void) write(socketFd, event, sizeof(Event));
  free(event);
  Message message = {0};
  long received_bytes;
  received_bytes = read(socketFd, &message, sizeof(Message));
  if (received_bytes > 0) {
    printf("\n%s\n\n", message.message);
  }
}

void getAll(int socketFd){
    Event* eventForSend = malloc(sizeof(Event));
    eventForSend->eventType = GET_ALL;
    (void) write(socketFd, eventForSend, sizeof(Event));
    free(eventForSend);
    long received_bytes;
    Event event = {0};
    do {
      received_bytes = read(socketFd, &event, sizeof(Event));
      if (received_bytes > 0 && event.eventType != END ) {
        printPersonFromEvent(&event);
      }
    } while(received_bytes > 0 && event.eventType != END);
}

void deletePatient(int socketFd){
  Event* event = malloc(sizeof(Event));
  event->eventType = REMOVE_BY_EGN;
  do {
    printf("Enter patient egn(10 symbols):");
    scanf("%s",event->egn );
  } while(validateEgn(event->egn) == 0);
  (void) write(socketFd, event, sizeof(Event));
  free(event);
  Message message = {0};
  long received_bytes;
  received_bytes = read(socketFd, &message, sizeof(Message));
  if (received_bytes > 0) {
    printf("\n%s\n\n", message.message);
  }
}

void printPersonFromEvent(Event* event){
  printf("\nPerson details...\n");
  printf("Name: %s\n", event->name);
  printf("Egn: %s\n", event->egn);
  printf("Diagnosis: %s\n\n", event->diagnosis);
}


short validateEgn(char* egn){
  size_t len = strlen(egn);

  if (len != 10)
    return 0;

  size_t i;
  for (i = 0; i < len; i++) {
    if (egn[i] < '0' || egn[i] > '9')
      return 0;
  }
  return 1;

}
