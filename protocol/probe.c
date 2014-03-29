#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../pilight.h"
#include "common.h"
#include "log.h"
#include "protocol.h"
#include "hardware.h"
#include "binary.h"
#include "gc.h"
#include "probe.h"

void probeInit(void) {
    protocol_register(&probe);
    protocol_set_id(probe, "probe");
    protocol_device_add(probe, "probe", "Probe protocol");
        //protocol_plslen_add(probe, 293);
        //protocol_plslen_add(probe, 294);
        protocol_plslen_add(probe, 297);
        probe->devtype = RAW;
        probe->hwtype = RF433;
        probe->pulse = 4;
        probe->rawlen = 164;
        probe->lsb = 3;

        options_add(&probe->options, 'i', "id", has_value, config_id, "^([0-9]{1,7}|[1-5][0-9]{7}|6([0-6][0-9]{6}|7(0[0-9]{5}|10([0-7][0-9]{3}|8([0-7][0-9]{2}|8([0-5][0-9]|6[0-3]))))))$");
        options_add(&probe->options, 's', "sensor", has_value, config_id, "^([0-9]{1}|[1][0-5])$");
        options_add(&probe->options, 'v', "value", has_value, config_value, "^([0-9]{1}|[1][0-5])$");
        options_add(&probe->options, 't', "temperature", has_value, config_value, "^([0-9]{1}|[1][0-5])$");
        options_add(&probe->options, 'h', "humidity", has_value, config_value, "^([0-9]{1}|[1][0-5])$");
        
        probe->parseBinary=&probeParseBinary;
        probe->createCode=&probeCreateCode;
        probe->printHelp=&probePrintHelp;
        probe->checkValues=&probeCheckValues;
}

void probeParseBinary(void) {
        int value = binToDecRev(probe->binary, 30, 39);
        int sensor = binToDecRev(probe->binary, 26, 29);
        int id = binToDecRev(probe->binary, 0, 25);        

        probeCreateMessage(id, sensor, value);
}

void probeCreateMessage(int id, int sensor, int value) {
        probe->message = json_mkobject();
        json_append_member(probe->message, "id", json_mknumber(id));

        if(sensor ==  3) {
                json_append_member(probe->message, "temperature", json_mknumber(value));
        }

        if(sensor ==  2) {
                json_append_member(probe->message, "humidity", json_mknumber(value));
        }
}

int probeCreateCode(JsonNode *code) {
        int id = -1;
        int sensor = -1;
        int value = -1;
        char *tmp;
        
        json_find_number(code, "id", &id);
        json_find_number(code, "sensor", &sensor);
        json_find_number(code, "value", &value);
        
        if(json_find_string(code, "id", &tmp) == 0)
                id=atoi(tmp);
        if(json_find_string(code, "sensor", &tmp) == 0)
                sensor = atoi(tmp);
        if(json_find_string(code, "value", &tmp) == 0)
                value = atoi(tmp);

        if(id == -1 || sensor == -1 || value== -1) {
                logprintf(LOG_ERR, "probe: insufficient number of arguments ");
                return EXIT_FAILURE;
        } else if(id > 67108863 || id < 1) {
                logprintf(LOG_ERR, "probe: invalid id range");
                return EXIT_FAILURE;
        } else if(sensor > 15 || sensor < 0) {
                logprintf(LOG_ERR, "probe: invalid sensor range");
                return EXIT_FAILURE;
        } else if(value > 1023 || value < 0) {
                logprintf(LOG_ERR, "probe: invalid value range");
                return EXIT_FAILURE;
        } else {
                probeCreateMessage(id, sensor, value);
                probeCreateStart();
                probeClearCode();
                probeCreateId(id);
                probeCreateSensor(sensor);
                if(value > -1) {
                        probeCreateValue(value);
                }
                probeCreateFooter();
        }
        return EXIT_SUCCESS;
}

void probeCreateStart(void) {
        probe->raw[0]=probe->plslen->length;
        probe->raw[1]=(10*probe->plslen->length);
}

void probeClearCode(void) {
        probeCreateLow(2,164);
}

void probeCreateId(int id) {
        int binary[255];
        int length = 0;
        int i=0, x=0;

        length = decToBin(id, binary);
        for(i=0;i<=length;i++) {
                if(binary[i]==1) {
                        x=((length-i)+1)*4;
                        probeCreateHigh(106-x, 106-(x-3));
                }
        }
}

void probeCreateSensor(int sensor) {
        int binary[255];
        int length = 0;
        int i=0, x=0;

        length = decToBin(sensor, binary);
        for(i=0;i<=length;i++) {
                if(binary[i]==1) {
                        x=((length-i)+1)*4;
                        probeCreateHigh(122-x, 122-(x-3));
                }
        }
}

void probeCreateValue(int value) {
        int binary[255];
        int length = 0;
        int i=0, x=0;

        length = decToBin(value, binary);
        for(i=0;i<=length;i++) {
                if(binary[i]==1) {
                        x=((length-i)+1)*4;
                        probeCreateHigh(162-x, 162-(x-3));
                }
        }
}

void probeCreateFooter(void) {
        probe->raw[163]=(PULSE_DIV*probe->plslen->length);
}

void probePrintHelp(void) {
        printf("\t -s --sensor=sensor\t\t\tcontrol a device with this sensor code\n");
        printf("\t -i --id=id\t\t\tcontrol a device with this id\n");
        printf("\t -a --all\t\t\tsend command to all devices with this id\n");
        printf("\t -d --dimlevel=dimlevel\t\tsend a specific dimlevel\n");
}

int probeCheckValues(JsonNode *code) {
    // Need some checking :P
    return 0;
}

void probeCreateLow(int s, int e) {
        int i;

        for(i=s;i<=e;i+=4) {
                probe->raw[i]=(probe->plslen->length);
                probe->raw[i+1]=(probe->plslen->length);
                probe->raw[i+2]=(probe->plslen->length);
                probe->raw[i+3]=(probe->pulse*probe->plslen->length);
        }
}

void probeCreateHigh(int s, int e) {
        int i;

        for(i=s;i<=e;i+=4) {
                probe->raw[i]=(probe->plslen->length);
                probe->raw[i+1]=(probe->pulse*probe->plslen->length);
                probe->raw[i+2]=(probe->plslen->length);
                probe->raw[i+3]=(probe->plslen->length);
        }
}