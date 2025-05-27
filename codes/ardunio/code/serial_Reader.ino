/* a program for reading analog input of a pot in serial interface*/
int Val=0; //integer value of pot
const int Pot=0; // pot on pin 0

void setup()
{
Serial.begin(9600);
    
}

    void loop()
{
  Val=analogRead(Pot);
  Serial.println(Val);
  delay(500);

}