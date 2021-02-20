#include <Arduino_FreeRTOS.h>
#include <queue.h>

typedef char  B;
typedef short H; // intも2byte
typedef long  W;
typedef unsigned char  UB;
typedef unsigned short UH; // intも2byte
typedef unsigned long  UW;
typedef void *VP;     /* 不定型データへのポインタ */
typedef void (*FP)(); /* 一般関数ポインタ */

#define MAX_QUEUE_NUM (5)
QueueHandle_t queue_TaskBlink1;
QueueHandle_t queue_TaskBlink2;

void FirstTask( void *pvParameters );
void TaskBlink1( void *pvParameters );
void TaskBlink2( void *pvParameters );

unsigned long g_cnt;

void setup()
{
  g_cnt = 0;
  
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  xTaskCreate(FirstTask, "create FirstTask", 128, NULL, 5, NULL);

//  xTaskCreate(
//    TaskFunc     // タスク関数のエントリー・ポインタ
//    ,  "task name"   // タスク名
//    ,  128       // 作成されるタスクのパラメータとして使用するポインタ
//    ,  NULL      // スタックサイズ(スタック深さを指定．ARMはスタック幅は4バイトのためスタックを100に指定すると400バイト使用)
//    ,  1         // タスクが走るべきである優先権(数値が大きいと優先度高)
//    ,  NULL );   // vTaskDeleteでタスク削除するために使用

  vTaskStartScheduler(); // FreeRTOSの動作開始
}

void loop()
{
}

// 参考
// 1tick待たせる場合:vTaskDelay(1);
// ms単位で待たせたい場合はportTICK_RATE_MSで割る
// 10ms待たせる場合:vTaskDelay(10/portTICK_RATE_MS);

// xQueueHandle xQueueCreate( unsigned portBASE_TYPE uxQueueLength, unsigned portBASE_TYPE uxItemSize );
// 待ち行列が収容可能なアイテムの最大数, 待ち行列の各項目が必要とするバイト数

void FirstTask(void *pvParameters)
{
  Serial.println("FirstTask Start");

  queue_TaskBlink1 = xQueueCreate(MAX_QUEUE_NUM, 1);
  queue_TaskBlink2 = xQueueCreate(MAX_QUEUE_NUM, 1);
  
  xTaskCreate(TaskBlink1, "task1", 128, NULL, 3, NULL );
  Serial.println("Create TaskBlink1 Task");

  xTaskCreate(TaskBlink2, "task2", 128, NULL, 2, NULL);
  Serial.println("Create TaskBlink2 Task");

  Serial.println("FirstTask End");
  vTaskDelete(NULL); // xTaskはタスクハンドルだが，自分自身を停止したい場合はNULLを入れる
}

void TaskBlink1(void *pvParameters)
{
  Serial.println("run TaskBlink1 Task");
  while(1)
  {
    unsigned char buf;
    xQueueReceive(queue_TaskBlink1, &buf, portMAX_DELAY);
    Serial.print("TaskBlink1 Task (");
    Serial.print(buf);
    Serial.println(")");
  }
}

void TaskBlink2(void *pvParameters)  
{
  Serial.println("run TaskBlink2 Task");
  while(1)
  {
    unsigned char  cnt = g_cnt;
    Serial.println("TaskBlink2 Task");
    g_cnt++;
    xQueueSend(queue_TaskBlink1, &cnt, 0); // キューがFULLの場合の待ち時間0
    while(1)
    {
      if (Serial.available()) {
        unsigned char input = Serial.read();
        if(input >= '0' && input <= '9')
        {
          Serial.print("input key = ");
          Serial.println(input);
          break;
        }
      }
    }
  }
}
