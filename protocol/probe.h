#ifndef _PROTOCOL_PROBE_H_
#define _PROTOCOL_PROBE_H_

struct protocol_t *probe;
void probeInit(void);

void probeCreateMessage(int id, int sensor, int value);
void probeParseBinary(void);
//int probeCheckSettings(JsonNode *code);
int probeCreateCode(JsonNode *code);
void probeCreateLow(int s, int e);
void probeCreateHigh(int s, int e);
void probeClearCode(void);
void probeCreateStart(void);
void probeCreateId(int id);
void probeCreateSensor(int sensor);
void probeCreateValue(int value);
void probeCreateFooter(void);
void probePrintHelp(void);
int probeCheckValues(JsonNode *code);

#endif