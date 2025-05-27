/* a program for controlling relay with button*/
const int BUTTON=2;
const int Relay=9;

void setup()
{
    pinMode(Relay , OUTPUT); // GPIO09, Digital Pin D1
    pinMode(BUTTON , INPUT); // GPIO09, Digital Pin D1
    
}

    void loop()
{
  if (digitalRead(BUTTON)==LOW){
    digitalWrite(Relay,LOW);
  }
 else {
    digitalWrite(Relay,HIGH);
 }
}