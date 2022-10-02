TaskHandle_t Task1;
TaskHandle_t Task2;

void setup(){
  Serial.begin(115200);
  Serial.print("setup() running on core ");
  Serial.println(xPortGetCoreID());

  xTaskCreatePinnedToCore(
      Task1code, /* Function to implement the task */
      "Task1", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &Task1,  /* Task handle. */
      0); /* Core where the task should run */
  delay(500); 

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
      Task2code,   /* Task function. */
      "Task2",     /* name of task. */
      10000,       /* Stack size of task */
      NULL,        /* parameter of the task */
      1,           /* priority of the task */
      &Task2,      /* Task handle to keep track of created task */
      1);          /* pin task to core 1 */
    delay(500); 

//  vTaskDelete(Task1);
}

void loop(){
  Serial.print("loop() running on core ");
  Serial.print(xPortGetCoreID());
  Serial.println("------------");
  delay(1000);
}

void Task1code( void * parameter) {
  for(;;) {
    Serial.print("Task1  running on core ");
    Serial.print(xPortGetCoreID());
    Serial.println("------");
    delay(500);
  }
}

void Task2code( void * parameter) {
  for(;;) {
    Serial.print("Task2  running on core ");
    Serial.print(xPortGetCoreID());
    Serial.println("----");
    delay(333);
  }
}
