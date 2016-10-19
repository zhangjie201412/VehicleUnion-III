#include "led_task.h"
#include "utils.h"
#include "cJSON.h"
#include "flash.h"
#include "wdg.h"
#include "sim800.h"

//������ƿ�
OS_TCB Led0TaskTCB;
//�����ջ	
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];

//led0������
void led0_task(void *p_arg)
{
    cJSON *root;
    char *out;
    char *bytes;
	OS_ERR err;

	p_arg = p_arg;
    sim800_setup();
	while(1)
	{
        root = cJSON_CreateObject();
		OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ200ms
        cJSON_AddStringToObject(root, "name", "zhangjie");
        cJSON_AddNumberToObject(root, "age", 28);
        out = cJSON_Print(root);
        logi("out: %s", out);


        cJSON_Delete(root);
        free(out);
        iwdg_feed();
	}
}
