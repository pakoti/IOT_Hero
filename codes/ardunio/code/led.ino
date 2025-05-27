/* a prgram for blinking leds*/

const int LED=9;

void setup()
{
    pinMode(LED , OUTPUT); // GPIO05, Digital Pin D1
}

    void loop()


{
for (int i=100;i<=1000;i++){
    digitalWrite(LED,high);
    delay(i);
    digitalWrite(LED,low);  
    delay(i);

}
}