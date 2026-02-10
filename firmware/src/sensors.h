#ifndef SENSORS_H
#define SENSORS_H

#include <stdint.h>
#include "protocol.h"

void sensors_init(void);
void sensors_read(sample_t *s);

#endif
