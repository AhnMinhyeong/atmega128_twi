#include "twi.h"
#include <util/twi.h>

#ifndef _BV
#define _BV(bit) (1 << bit)
#endif

void twi_init(void)
{
	TWSR = 0; // 프리스케일 1로 만들기
	// TWI 속도 설정
	TWBR = (F_CPU / TWI_FREQ - 16) / 2;
	PORTD = 0x03; // 내부 풀업 활성화
}

// 0: 성공, 1: 실패
// slam: slave address(7bit) + mode(1bit)
uint8_t twi_start(uint8_t sla, uint8_t mode)
{
	uint8_t status;
	
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTA);
	
	// TWINT가 하드웨어 적으로 활성화 될때까지 대기
	while ( !(TWCR & _BV(TWINT)) );
	
	status = TW_STATUS;
	// TW_START 또는 TW_REP_START 둘중 다 아닐 경우 실패
	if ( !(status == TW_START || status == TW_REP_START) ) return 1;
	
	TWDR = sla << 1 | mode;
	// 활성화된 TWINT에 1을 넣어서 꺼줌으로 다음 상태 진행
	// 다음 상태는 주소를 전송하는 것
	TWCR = _BV(TWINT) | _BV(TWEN);
	
	// 전송완료가 될때까지 대기
	while ( !(TWCR & _BV(TWINT)) );
	
	status = TW_STATUS;
	// 상태가 TW_MT_SLA_ACK 또는 TW_MR_SLA_ACK 둘다 아니면 실패
	if ( !(status == TW_MT_SLA_ACK || status == TW_MR_SLA_ACK) ) return 1;
	
	// 버스 장악하고 대상 찾기까지 성공
	return 0;
}

// 굳이 하나 더 만드는 이유는 사용할 때 구분하기 위해서
uint8_t twi_rep_start(uint8_t sla, uint8_t mode)
{
	return twi_start(sla, mode);
}

void twi_stop(void)
{
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO);
}

// 0: 성공, 1: 실패
uint8_t twi_write(uint8_t data)
{
	TWDR = data; // 데이터 작성
	TWCR = _BV(TWINT) | _BV(TWEN); // 데이터 전송
	while ( !(TWCR & _BV(TWINT)) ); // 전송 완료 대기
	
	// 상태가 TW_MT_DATA_ACK가 아니라면 실패
	if (TW_STATUS != TW_MT_DATA_ACK) return 1;
	
	return 0;
}

// 받은 값
uint8_t twi_read(bool ack)
{
	if (ack) // 다 받음
		TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
	else
		TWCR = _BV(TWINT) | _BV(TWEN);
		
	while ( !(TWCR & _BV(TWINT)) );
	
	return TWDR; // 받은 값 반환
}
