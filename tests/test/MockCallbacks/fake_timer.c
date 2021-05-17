#include "fake_timer.h"


// Real tick implementations should be created
TickType_t xTaskGetTickCount( void ) {
	return 0;
}

