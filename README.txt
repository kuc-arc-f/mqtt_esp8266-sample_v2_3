//------------------------------------
// dot matrix LED, with esp8266 
// @Author  : Kouji Nakashima / kuc-arc-f.com
// @date    : 2016-06-30
//------------------------------------

[summary]

# arduino
arduino SDK: 1.6.5
/arduino/mqtt_esp8266-sample_v2_3
MQTT-Sub, 
esp8266 ,http send

# mbed -- dot matrix, driver.
 mbed/dot_matrix_v3_3

*) create table, reqquire.
CREATE TABLE IF NOT EXISTS T_MATRIX_DAT (
  id int(11) NOT NULL AUTO_INCREMENT,
  CITY   text DEFAULT NULL,
  COND text DEFAULT NULL,
  COND_TXT int(11) DEFAULT NULL,
  created datetime DEFAULT NULL,
  modified datetime DEFAULT NULL,
  PRIMARY KEY ( id )
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=248 ;



