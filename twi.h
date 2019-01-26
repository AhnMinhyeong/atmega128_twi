#ifndef TWI_H_
#define TWI_H_

#include <stdint.h>
#include <stdbool.h>

// if not define: 만약 매크로가 등록되지 않았다면...
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define TWI_FREQ 400000UL

#define TWI_WRITE 0
#define TWI_READ 1

void twi_init(void);

// 0: 성공, 1: 실패
// slam: slave address(7bit) + mode(1bit)
uint8_t twi_start(uint8_t sla, uint8_t mode);

// 재시작
uint8_t twi_rep_start(uint8_t sla, uint8_t mode);

void twi_stop(void);

// 0: 성공, 1: 실패
uint8_t twi_write(uint8_t data);

// 받은 값
uint8_t twi_read(bool ack);

#endif /* TWI_H_ */