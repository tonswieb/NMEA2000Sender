// Demo: NMEA2000 library. Sends NMEA2000 to WiFi in NMEA0183 and SeaSmart format.
//
// The code has been tested with ESP32.

#define ESP32_CAN_TX_PIN GPIO_NUM_2
#define ESP32_CAN_RX_PIN GPIO_NUM_15
#include <NMEA2000_CAN.h>
#include <N2kMessages.h>

// Set the information for other bus devices, which messages we support
const unsigned long TransmitMessages[] PROGMEM={127250L,127258L,128259UL,128267UL,129025UL,129026L,129029L,130306L,0};

void setup() {
  NMEA2000.SetN2kCANMsgBufSize(8);
  NMEA2000.SetN2kCANReceiveFrameBufSize(100);
  NMEA2000.SetProductInformation("00000001", // Manufacturer's Model serial code
                                 100, // Manufacturer's product code
                                 "Simple temp monitor",  // Manufacturer's Model ID
                                 "1.1.0.21 (2016-12-31)",  // Manufacturer's Software version code
                                 "1.1.0.0 (2016-12-31)" // Manufacturer's Model version
                                 );
  // Det device information
  NMEA2000.SetDeviceInformation(112233, // Unique number. Use e.g. Serial number.
                                130, // Device function=Temperature. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                75, // Device class=Sensor Communication Interface. See codes on  http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                2040 // Just choosen free from code list on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf                               
                               );
  NMEA2000.SetMode(tNMEA2000::N2km_NodeOnly,22);
  NMEA2000.EnableForward(false);
  NMEA2000.ExtendTransmitMessages(TransmitMessages);
  NMEA2000.Open();
}

//*****************************************************************************
void loop() {
  SendN2KMessages();
  NMEA2000.ParseMessages();
}

#define UpdatePeriod 500

void SendN2KMessages() {
  unsigned char seq = 1;
  uint16_t DaysSince1970 = 18090;
  double magHeading = 290.0;
  double variation = 10.0;
  double deviation = 0.0;
  double waterSpeed = 6.0;
  double groundSpeed = 6.0;
  double depthBelowTransducer = 2.0;
  double depthTransducerOffset = 0.0;
  double windSpeed = 15; //m/s
  double windAngle = DegToRad(90);
  double cog = 290.0;
  double sog = 6.0;
  double lat = 30.0;
  double lon = 20.0;
  double altitude = 1.0;
  double SecondsSinceMidnight = 7*3600;
  unsigned char nSatellites = 5;
  double HDOP=0.0;
  
  static unsigned long Updated=millis();
  tN2kMsg N2kMsg;

  if ( Updated+UpdatePeriod<millis() ) {
    Updated=millis();

    SetN2kTrueHeading(N2kMsg, seq, magHeading + variation);
    NMEA2000.SendMsg(N2kMsg);
    SetN2kMagneticHeading(N2kMsg, seq, magHeading, deviation, variation);
    NMEA2000.SendMsg(N2kMsg);
    SetN2kMagneticVariation(N2kMsg, seq, N2kmagvar_Manual, DaysSince1970, variation);
    NMEA2000.SendMsg(N2kMsg);
    SetN2kBoatSpeed(N2kMsg, seq, waterSpeed, groundSpeed, N2kSWRT_Paddle_wheel); 
    NMEA2000.SendMsg(N2kMsg);
    SetN2kWaterDepth(N2kMsg, seq, depthBelowTransducer, depthTransducerOffset);
    NMEA2000.SendMsg(N2kMsg);
    SetN2kLatLonRapid(N2kMsg, lat, lon);
    NMEA2000.SendMsg(N2kMsg);
    SetN2kCOGSOGRapid(N2kMsg, seq, N2khr_magnetic, cog, sog); 
    NMEA2000.SendMsg(N2kMsg); 
    SetN2kWindSpeed(N2kMsg, seq, windSpeed, windAngle, N2kWind_Apparent);
    NMEA2000.SendMsg(N2kMsg);
    SetN2kGNSS(N2kMsg, seq, DaysSince1970, SecondsSinceMidnight, lat, lon, altitude, N2kGNSSt_GPSSBASWAASGLONASS, N2kGNSSm_PreciseGNSS, nSatellites, HDOP);
    NMEA2000.SendMsg(N2kMsg);
  }
}
