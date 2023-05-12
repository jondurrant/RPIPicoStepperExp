/***
 * main.cpp - Test Stepper motor - 28BYJ-48
 * Motor connected via ULN2003 driver to:
 * GP02,03,04,05
 * LED on GP00
 * Jon Durrant
 * 4-Oct-2022
 *
 */

#include <iostream>

#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "BlinkAgent.h"
#include "Stepper28BYJ.h"

const auto RTOS_NAME = "FreeRTOS";

#define PULSE_LED 0
#define TASK_PRIORITY (tskIDLE_PRIORITY + 1UL)

/***
 * Freertos task stats for debug
 */
void runTimeStats()
{
  //char pcWriteBuffer[120] = "";
  TaskStatus_t* pxTaskStatusArray;
  volatile UBaseType_t uxArraySize, x;
  unsigned long        ulTotalRunTime;

  /* Make sure the write buffer does not contain a string. */
  //*pcWriteBuffer = 0x00;

  /* Take a snapshot of the number of tasks in case it changes while this
  function is executing. */
  uxArraySize = uxTaskGetNumberOfTasks();
  std::cout << "Number of tasks " << uxArraySize << std::endl;

  /* Allocate a TaskStatus_t structure for each task.  An array could be
  allocated statically at compile time. */
  pxTaskStatusArray = (TaskStatus_t*) pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));

  if (pxTaskStatusArray != NULL)
  {
    /* Generate raw status information about each task. */
    uxArraySize = uxTaskGetSystemState(
      pxTaskStatusArray,
      uxArraySize,
      &ulTotalRunTime
    );

    /* For each populated position in the pxTaskStatusArray array,
    format the raw data as human readable ASCII data. */
    for (x = 0; x < uxArraySize; x++)
    {
      std::cout
        << "Task: " << pxTaskStatusArray[x].xTaskNumber
        << " cPri:" << pxTaskStatusArray[x].uxCurrentPriority
        << " bPri:" << pxTaskStatusArray[x].uxBasePriority
        << " hw:" << pxTaskStatusArray[x].usStackHighWaterMark
        << " " << pxTaskStatusArray[x].pcTaskName
        << std::endl;
    }

    /* The array is no longer needed, free the memory it consumes. */
    vPortFree(pxTaskStatusArray);
  }
  else
  {
    std::cerr << "Failed to allocate space for stats" << std::endl;
  }

  HeapStats_t heapStats;
  vPortGetHeapStats(&heapStats);
  std::cout
    << "HEAP avl: " << heapStats.xAvailableHeapSpaceInBytes
    << ", blocks " << heapStats.xNumberOfFreeBlocks
    << ", alloc: " << heapStats.xNumberOfSuccessfulAllocations
    << ", free: " << heapStats.xNumberOfSuccessfulFrees
    << std::endl;
}

/***
 * Main boot task
 * @param params
 */
void main_task(void* params)
{
  int16_t rpm;

  std::cout << "Main task started" << std::endl;

  //Start up pulse blink LED
  BlinkAgent blink(PULSE_LED);
  blink.start("Blink", TASK_PRIORITY);

  // Construct stepper and start
  Stepper28BYJ stepper(2, 3, 4, 5);
  stepper.start("Stepper", TASK_PRIORITY);

  // Do 360 rotation test which should take approx 4 seconds
  std::cout << "Req CW 360 at MAX" << std::endl;
  stepper.step(2047, 0);
  vTaskDelay(pdMS_TO_TICKS(5000));


  // Loop through set of movements at different speeds
  uint16_t seq = 0;
  while (true)
  {
    runTimeStats();

    switch (seq % 3)
    {
      case 0:
        rpm = 7;
        break;
      case 1:
        rpm = 10;
        break;
      default:
        rpm = 14;
    }

    if (seq < 8)
    {
      std::cout << "Req CW 1/8 at " << rpm << std::endl;
      stepper.step(2048 / 8, rpm);;
    }

    if (seq >= 8)
    {
      std::cout << "Req CCW 1/8 at " << rpm << std::endl;
      stepper.step(2048 / -8, rpm);
    }

    seq++;

    if (seq > 15)
    {
      seq = 0;
    }

    vTaskDelay(3000);
  }

}


/***
 * Launch scheduler and book task
 */
void vLaunch(void)
{
  TaskHandle_t task;
  xTaskCreate(
    main_task,
    "MainThread",
    2048,
    NULL,
    TASK_PRIORITY,
    &task
  );

  /* Start the tasks and timer running. */
  vTaskStartScheduler();
}

/***
 * Main
 * @return
 */
int main(void)
{
  stdio_init_all();
  sleep_ms(2000);
  std::cout << "Starting" << std::endl;

  /* Configure the hardware ready to run the demo. */
  std::cout << "Starting " << RTOS_NAME << " on core 0:" << std::endl;
  vLaunch();

  return 0;
}
