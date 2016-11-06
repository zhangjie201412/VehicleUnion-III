#ifndef __RF_MODULE_H__
#define __RF_MODULE_H__

#define IMMO_LOCK           0x44
#define IMMO_UNLOCK         0x88

void rf_module_init(void);
void rf_send_high(void);
void rf_send_low(void);
void rf_send_start(void);
void rf_send(int data);
void rf_lock(void);
void rf_unlock(void);

#endif
