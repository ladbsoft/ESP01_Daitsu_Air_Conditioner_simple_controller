// +------------------------------------------------------------------+
// |                       C O N S T A N T S                          |
// +------------------------------------------------------------------+
/**
 * Command value positions. "Filler" and/or unknown positions are not used, so no constant is needed.
 */
#define SPEEDBIT1 35
#define SPEEDBIT2 37
#define SPEEDBIT3 39
#define POWERBIT  45
#define TEMPBIT1  67
#define TEMPBIT2  69
#define TEMPBIT3  71
#define TEMPBIT4  73
#define MODEBIT1  75
#define MODEBIT2  77

/**
 * Logic values. A true is represented by a 348 microseconds pulse on average, and a false is 1.44ms.
 */
#define VALUETRUE  348
#define VALUEFALSE 1422

/**
 * Command frecuency seems to be 38KHz
 */
#define KHZ 38


/**
 * Logic values in commands are composed of a pulse with a specific duration. In this case, a "true" signal
 * is a pulse of roughly 0.35ms, and a "false" signal is about 1.4ms.
 */
const uint16_t logicValues[] {
  VALUEFALSE,
  VALUETRUE
};

/**
 * I'm not exactly sure if there is correlation between the temperature in degrees and the binary values. If there is, I
 * have not been able to find it. Here are the specific codes for every temperature.
 */
const boolean temperatureCommandValues[][4] { {false, false, false, true},    //OFF  -> 0001
                                              {},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},    //Filler positions
                                              {true,  true,  true,  true},    //17ºC -> 1111
                                              {true,  true,  true,  false},   //18ºC -> 1110
                                              {true,  true,  false, false},   //19ºC -> 1100
                                              {true,  true,  false, true},    //20ºC -> 1101
                                              {true,  false, false, true},    //21ºC -> 1001
                                              {true,  false, false, false},   //22ºC -> 1000
                                              {true,  false, true,  false},   //23ºC -> 1010
                                              {true,  false, true,  true},    //24ºC -> 1011
                                              {false, false, true,  true},    //25ºC -> 0011
                                              {false, false, true,  false},   //26ºC -> 0010
                                              {false, true,  true,  false},   //27ºC -> 0110
                                              {false, true,  true,  true},    //28ºC -> 0111
                                              {false, true,  false, true},    //29ºC -> 0101
                                              {false, true,  false, false} }; //30ºC -> 0100

/**
 * Weirdly enough, there is an special speed value for AUTO mode. Value AUTO for speed is not accepted by the machine,
 * so the special value is used.
 */
const boolean speedCommandValues[][3] { {false, true,  false},   //AUTO -> 010
                                        {false, true,  true},    //LOW  -> 011
                                        {true,  false, true},    //MED  -> 101
                                        {true,  true,  false},   //HIGH -> 110
                                        {true,  true,  true},    //SPECIAL -> 111
                                        {true,  false, false} }; //OFF -> 100

/**
 * Another weird point here is the value for COOL here is the same as the value for OFF mode. Oh well...
 */
const boolean modeCommandValues[][2] { {false, true},    //AUTO -> 01
                                       {true,  true},    //COOL -> 11, also OFF
                                       {true,  false},   //FAN  -> 10
                                       {false, false} }; //HEAT -> 00

// +------------------------------------------------------------------+
// |                         G L O B A L S                            |
// +------------------------------------------------------------------+

/**
 * Commands used by this machines are composed of two equal parts, of 99 pulses, separated by a 4.66ms pulse.
 * I have not decoded the use of every pulse, so special commands (set ON timer, set OFF timer, set time,
 * air direction, swing...) are not available in this sketch. Only commands to change temperature,
 * fan speed, mode and turning ON and OFF are available and hopefully working.
 * 
 * Every command "segment" (like "Speed and power") is repeated, once with positive values (1 = 1) and the
 * second time with inverted values (1 = 0). So, a speed of LOW and power ON would be "1422 348 348" "1422"
 * in the first segment and "348 1422 1422" "348" in the second segment.
 */
uint16_t commandTemplate[] = {4660, 4240, 768,  1422, 768,  348,  768,  1422, 768,  1422, 768,  348,  768,  348,  768,  1422, //Unknown - Header?
                              768,  348,  768,  348,  768,  1422, 768,  348,  768,  348,  768,  1422, 768,  1422, 768,  348,  //Unknown - Filler? Other functions?
                              768,  1422, 768,  0,    768,  0,    768,  0,    768,  1422, 768,  1422, 768,  0,    768,  1422, //Speed and ON/OFF
                              768,  1422, 768,  0,    768,  0,    768,  0,    768,  348,  768,  348,  768,  0,    768,  348,  //Speed and ON/OFF - repetition (inverted)
                              768,  348,  768,  0,    768,  0,    768,  0,    768,  0,    768,  0,    768,  0,    768,  348,  //Temperature and mode
                              768,  348,  768,  0,    768,  0,    768,  0,    768,  0,    768,  0,    768,  0,    768,  1422, //Temperature and mode - repetition (inverted)
                              768,  1422, 768,                                                                                //Unknown - Trailer?
                              4660,                                                                                           //Separator - Command repetition ---------------------
                              4660, 4240, 768,  1422, 768,  348,  768,  1422, 768,  1422, 768,  348,  768,  348,  768,  1422, //Unknown - Header?
                              768,  348,  768,  348,  768,  1422, 768,  348,  768,  348,  768,  1422, 768,  1422, 768,  348,  //Unknown - Filler? Other functions?
                              768,  1422, 768,  0,    768,  0,    768,  0,    768,  1422, 768,  1422, 768,  0,    768,  1422, //Speed and ON/OFF
                              768,  1422, 768,  0,    768,  0,    768,  0,    768,  348,  768,  348,  768,  0,    768,  348,  //Speed and ON/OFF - repetition (inverted)
                              768,  348,  768,  0,    768,  0,    768,  0,    768,  0,    768,  0,    768,  0,    768,  348,  //Temperature and mode
                              768,  348,  768,  0,    768,  0,    768,  0,    768,  0,    768,  0,    768,  0,    768,  1422, //Temperature and mode - repetition (inverted)
                              768,  1422, 768};                                                                               //Unknown - Trailer?

char inChar;
boolean powerState;
byte temperature;
byte mode;
byte fanSpeed;
IRsend irsend(IRSenderPin);

// +------------------------------------------------------------------+
// |                         H E A D E R S                            |
// +------------------------------------------------------------------+
void setPower(boolean state);
void sendPowerCommand(boolean state);
void setTemperature(byte temperature);
void sendTemperatureCommand(byte temperature);
void setFanSpeed(byte fanSpeed);
void setMode(byte mode);
void sendModeCommand(byte mode);
void commandSetup();


// +------------------------------------------------------------------+
// |                     S U B R O U T I N E S                        |
// +------------------------------------------------------------------+
void setPower(boolean state) {
  /**
   * A power-on command is loaded with value false. This lets me think I may have mismatched false and true values in
   * every value of the commands, but, it's too late to apologize :)
   */
  if (state) {
    commandTemplate[POWERBIT]     = VALUEFALSE;
    commandTemplate[POWERBIT+16]  = VALUETRUE;
    commandTemplate[POWERBIT+100] = VALUEFALSE;
    commandTemplate[POWERBIT+116] = VALUETRUE;

    setTemperature(temperature);
    setFanSpeed(fanSpeed);
    setMode(mode);
  } else {
    commandTemplate[POWERBIT]     = VALUETRUE;
    commandTemplate[POWERBIT+16]  = VALUEFALSE;
    commandTemplate[POWERBIT+100] = VALUETRUE;
    commandTemplate[POWERBIT+116] = VALUEFALSE;

    setMode(1); //OFF (also COOL)
    setFanSpeed(5); //OFF
    setTemperature(0); //OFF
  }
}

void sendPowerCommand(boolean state) {
  setPower(state);

  if (state) {
    powerState = state;
  }

  irsend.sendRaw(commandTemplate, sizeof(commandTemplate) / sizeof(commandTemplate[0]), KHZ);
  delay(40);
  
  if (!state) {
    powerState = state;
  }
}

void setTemperature(byte temperature) {
/**
 * Temperature values are stored in the temperatureCommandValues array in the corresponding
 * index of every temperature, so the array is accessed by it.
 */
  commandTemplate[TEMPBIT1]     = logicValues[temperatureCommandValues[temperature][0]];
  commandTemplate[TEMPBIT1+16]  = logicValues[!temperatureCommandValues[temperature][0]];
  commandTemplate[TEMPBIT1+100] = logicValues[temperatureCommandValues[temperature][0]];
  commandTemplate[TEMPBIT1+116] = logicValues[!temperatureCommandValues[temperature][0]];

  commandTemplate[TEMPBIT2]     = logicValues[temperatureCommandValues[temperature][1]];
  commandTemplate[TEMPBIT2+16]  = logicValues[!temperatureCommandValues[temperature][1]];
  commandTemplate[TEMPBIT2+100] = logicValues[temperatureCommandValues[temperature][1]];
  commandTemplate[TEMPBIT2+116] = logicValues[!temperatureCommandValues[temperature][1]];

  commandTemplate[TEMPBIT3]     = logicValues[temperatureCommandValues[temperature][2]];
  commandTemplate[TEMPBIT3+16]  = logicValues[!temperatureCommandValues[temperature][2]];
  commandTemplate[TEMPBIT3+100] = logicValues[temperatureCommandValues[temperature][2]];
  commandTemplate[TEMPBIT3+116] = logicValues[!temperatureCommandValues[temperature][2]];

  commandTemplate[TEMPBIT4]     = logicValues[temperatureCommandValues[temperature][3]];
  commandTemplate[TEMPBIT4+16]  = logicValues[!temperatureCommandValues[temperature][3]];
  commandTemplate[TEMPBIT4+100] = logicValues[temperatureCommandValues[temperature][3]];
  commandTemplate[TEMPBIT4+116] = logicValues[!temperatureCommandValues[temperature][3]];
}

void sendTemperatureCommand(byte temperature) {
  setTemperature(temperature);

  if (powerState) {
    irsend.sendRaw(commandTemplate, sizeof(commandTemplate) / sizeof(commandTemplate[0]), KHZ);
    delay(40);
  }
}

void setFanSpeed(byte fanSpeed) {
  /**
   * Speed values are stored in the array the same way as temperature: every value into its corresponding
   * index, being 0 = automatic speed, 1 = slow, 2 = medium and 3 = high. There are two special speeds:
   * the value for the auto mode (index 4) and the value for OFF (index 5). This subroutine is called
   * with the corresponding values for each case.
   */
  commandTemplate[SPEEDBIT1]     = logicValues[speedCommandValues[fanSpeed][0]];
  commandTemplate[SPEEDBIT1+16]  = logicValues[!speedCommandValues[fanSpeed][0]];
  commandTemplate[SPEEDBIT1+100] = logicValues[speedCommandValues[fanSpeed][0]];
  commandTemplate[SPEEDBIT1+116] = logicValues[!speedCommandValues[fanSpeed][0]];

  commandTemplate[SPEEDBIT2]     = logicValues[speedCommandValues[fanSpeed][1]];
  commandTemplate[SPEEDBIT2+16]  = logicValues[!speedCommandValues[fanSpeed][1]];
  commandTemplate[SPEEDBIT2+100] = logicValues[speedCommandValues[fanSpeed][1]];
  commandTemplate[SPEEDBIT2+116] = logicValues[!speedCommandValues[fanSpeed][1]];

  commandTemplate[SPEEDBIT3]     = logicValues[speedCommandValues[fanSpeed][2]];
  commandTemplate[SPEEDBIT3+16]  = logicValues[!speedCommandValues[fanSpeed][2]];
  commandTemplate[SPEEDBIT3+100] = logicValues[speedCommandValues[fanSpeed][2]];
  commandTemplate[SPEEDBIT3+116] = logicValues[!speedCommandValues[fanSpeed][2]];
}

void sendSpeedCommand(byte fanSpeed) {
  setFanSpeed(fanSpeed);

  if (powerState) {
    irsend.sendRaw(commandTemplate, sizeof(commandTemplate) / sizeof(commandTemplate[0]), KHZ);
    delay(40);
  }
}

void setMode(byte mode) {
  /**
   * As temperature and speed, modes are stored into the corresponding index of modeCommandValues,
   * being 0 = automatic, 1 = cool, 2 = fan and 3 = heat.
   */
  commandTemplate[MODEBIT1]     = logicValues[modeCommandValues[mode][0]];
  commandTemplate[MODEBIT1+16]  = logicValues[!modeCommandValues[mode][0]];
  commandTemplate[MODEBIT1+100] = logicValues[modeCommandValues[mode][0]];
  commandTemplate[MODEBIT1+116] = logicValues[!modeCommandValues[mode][0]];

  commandTemplate[MODEBIT2]     = logicValues[modeCommandValues[mode][1]];
  commandTemplate[MODEBIT2+16]  = logicValues[!modeCommandValues[mode][1]];
  commandTemplate[MODEBIT2+100] = logicValues[modeCommandValues[mode][1]];
  commandTemplate[MODEBIT2+116] = logicValues[!modeCommandValues[mode][1]];

// Special case: Set special speed for AUTO mode
  if (mode == 0) {
    setFanSpeed(4);
  } else {
    setFanSpeed(fanSpeed);
  }

// Special case: Set no temperature for FAN mode
  if (mode == 2) {
    setTemperature(0);
  } else {
    setTemperature(temperature);
  }
}

void sendModeCommand(byte mode) {
  setMode(mode);

  if (powerState) {
    irsend.sendRaw(commandTemplate, sizeof(commandTemplate) / sizeof(commandTemplate[0]), KHZ);
    delay(40);
  }
}

// +------------------------------------------------------------------+
// |                           S E T U P                              |
// +------------------------------------------------------------------+

void commandSetup() {
  temperature = 23;             //23ºC
  setTemperature(temperature);
  fanSpeed = 0;                 //AUTO
  setFanSpeed(fanSpeed);
  mode = 0;                     //AUTO
  setMode(mode);
}
