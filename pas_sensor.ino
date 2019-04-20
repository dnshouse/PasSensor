const int pas_in = 2;                               //PAS Sensor read-Pin

volatile unsigned long last_pas_event = millis();   //Last change-time of PAS sensor status
volatile boolean pedaling = false;                  //Pedaling? (in forward direction!)
volatile int pedaling_on_times = 0;                 //How many times we had the on signal

volatile int pas_on_time = 0;                       //High-Time of PAS-Sensor-Signal (needed to determine pedaling direction)
volatile int pas_off_time = 0;                      //Low-Time of PAS-Sensor-Signal  (needed to determine pedaling direction)
volatile int pas_failtime = 0;                      //How many subsequent "wrong" PAS values?

const double pas_factor_min=0.5;                    //Larger or smaller than 1.0 and not 0.0!
const double pas_factor_max=0.9;                    //Larger or smaller than 1.0 and not 0.0!

const int pas_timeout=190;
const int pas_tolerance=1;                          //0... increase to make pas sensor slower but more tolerant against speed changes

void setup() { 
  pinMode(pas_in,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pas_in), pas_change, CHANGE);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(A4, OUTPUT);

  //Serial.begin(9600);
}

void loop() {
  if (((millis()-last_pas_event)>pas_timeout)||(pas_failtime>pas_tolerance)) {
    pedaling_on_times = 0;
    pedaling = false;
  }

  if(pedaling_on_times > 2) {
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(A4, HIGH);
  }else{
    digitalWrite(LED_BUILTIN, LOW);  
    digitalWrite(A4, LOW);
  }
}

void pas_change() {
  if (last_pas_event > (millis() - 10)) return;
  
  boolean pas_stat = digitalRead(pas_in);
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
