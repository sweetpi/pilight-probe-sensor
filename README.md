pilight-probe-sensor
====================

A custom 433mhz sensor for pilight.

protocol
-------

The probe protocol for pilight See: http://www.pilight.org/development/protocols/#where for install.

###config

    "probe": {
      "name": "Probe Device",
      "uuid": "0365-00-00-65-000300",
      "protocol": [ "probe" ],
      "id": [{
              "id": "8934000",
              "sensor": 3
      }],
      "temperature": 196,
      "value": 1,
      "humidity": 222
    }

attiny
-----

compile with arduino?...


Related discussions:
-------------------

  * https://github.com/pimatic/pimatic/issues/74
  * http://forum.pilight.org/Thread-Idea-PIR-wireless-motion-sensor?page=5