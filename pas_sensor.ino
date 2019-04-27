#define PAS_SENSOR_PIN 2
#define THROTTLE_PIN 3

#define MIN_PEDALLING_TRUE_PULSES 2

volatile unsigned long last_pas_event = millis();   //Last change-time of PAS sensor status
volatile boolean pedaling = false;                  //Pedaling? (in forward direction!)
volatile int pedaling_on_times = 0;                 //How many times we had the on signal

volatile int pas_on_time = 0;                       //High-Time of PAS-Sensor-Signal (needed to determine pedaling direction)
volatile int pas_off_time = 0;                      //Low-Time of PAS-Sensor-Signal  (needed to determine pedaling direction)
volatile int pas_failtime = 0;                      //How many subsequent "wrong" PAS values?

const double pas_factor_min=1.08;                    //Larger or smaller than 1.0 and not 0.0!
const double pas_factor_max=1.45;                    //Larger or smaller than 1.0 and not 0.0!

const int pas_timeout=190;
const int pas_tolerance=0;                          //0... increase to make pas sensor slower but more tolerant against speed changes

void setup() { 
  pinMode(PAS_SENSOR_PIN,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PAS_SENSOR_PIN), pas_change, CHANGE);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(THROTTLE_PIN, OUTPUT);

  //Serial.begin(9600);
}

void loop() {
  if (((millis()-last_pas_event)>pas_timeout)||(pas_failtime>pas_tolerance)) {
    pedaling_on_times = 0;
    pedaling = false;
  }

  if(pedaling_on_times >= MIN_PEDALLING_TRUE_PULSES) {
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(THROTTLE_PIN, HIGH);
  }else{
    digitalWrite(LED_BUILTIN, LOW);  
    digitalWrite(THROTTLE_PIN, LOW);
  }
}

void pas_change() {
  if (last_pas_event > (millis() - 5)) return;
  
  boolean pas_stat = digitalRead(PAS_SENSOR_PIN);
  if (pas_stat) {
    pas_off_time = millis() - last_pas_event;
  } else {
    pas_on_time = millis() - last_pas_event;
  }
  
  last_pas_event = millis();

  pas_failtime = pas_failtime + 1;
  double pas_factor = (double)pas_on_time / (double)pas_off_time;
  if ((pas_factor > pas_factor_min) && (pas_factor < pas_factor_max)) {
    pedaling = true;
    pas_failtime = 0;
  }

  if(pedaling == true) {
    pedaling_on_times = pedaling_on_times + 1;
    if(pedaling_on_times > 10) pedaling_on_times = 5;
  }

  //Serial.println(pas_factor);
}
