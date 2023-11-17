// TODO add eeprom support.

#define REDPIN 5
#define GREENPIN 6
#define BLUEPIN 3

// the global cmd buffer used to store a cmd untill all of it is received and processed :)
String buf = "";

// the current active effect.
//  0 = swirl
//  1 = static
int RGB_MODE = 0;

int RED_VAL = 0;
int GREEN_VAL = 0;
int BLUE_VAL = 255;

// the values represent the state of the swirl effect other than rgb values.
int SWIRL_DELAY = 5;
// im creating this flag cause i have to first increase then decrease so yeah.
bool swirl_flag = false;

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0; // will store last time LED was updated

void setup()
{
    pinMode(REDPIN, OUTPUT);
    pinMode(GREENPIN, OUTPUT);
    pinMode(BLUEPIN, OUTPUT);
    analogWrite(REDPIN, RED_VAL);
    analogWrite(GREENPIN, GREEN_VAL);
    analogWrite(BLUEPIN, BLUE_VAL);
    Serial.begin(9600);
    buf.reserve(13);
}

void loop()
{
    if (Serial.available() > 0)
    {
        char c = Serial.read();
        if (c == '\n')
        {
            Serial.print("processing cmd: ");
            Serial.println(buf);
            // first char is what mode to use.
            char mode = buf.charAt(0);
            if (mode == 's')
            {
                Serial.println("Setting static colors.");
                // set the rgb mode so that effect related stuff doesnt run.
                RGB_MODE = 1;
                // convert string slice to int for required clr value and set the pin.
                buf.remove(0, 2);
                RED_VAL = buf.toInt();
                analogWrite(REDPIN, RED_VAL);
                buf.remove(0, 4);
                GREEN_VAL = buf.toInt();
                analogWrite(GREENPIN, GREEN_VAL);
                buf.remove(0, 4);
                BLUE_VAL = buf.toInt();
                analogWrite(BLUEPIN, BLUE_VAL);
            }
            else if (mode == 'r')
            {
                Serial.println("Starting colour swirl.");
                RGB_MODE = 0;
                buf.remove(0, 2);
                SWIRL_DELAY = buf.toInt();
                set_swirl_inital_state();
                Serial.println(SWIRL_DELAY);
            }
            else
            {
                Serial.println("Unsupported cmd");
            }
            // reset the buffer cause need to wait for new cmd now!
            buf = "";
        }
        else
        {
            buf += c;
        }
    }

    // dont really need to run animation if effect is static.
    if (RGB_MODE != 1)
    {
        unsigned long currentMillis = millis();

        // Calculate the time elapsed since the last change
        // if its longer than delay then the animation function is called to update the state.
        if ((currentMillis - previousMillis) >= SWIRL_DELAY)
        {
            // Save the current time for the next iteration
            previousMillis = currentMillis;
            swirl();
        }
    }
}

void set_swirl_inital_state()
{
    RED_VAL = 0;
    GREEN_VAL = 0;
    BLUE_VAL = 255;
    analogWrite(REDPIN, RED_VAL);
    analogWrite(GREENPIN, GREEN_VAL);
    analogWrite(BLUEPIN, BLUE_VAL);
    swirl_flag = false;
    analogWrite(BLUEPIN, BLUE_VAL);
}

// modified from https://learn.adafruit.com/rgb-led-strips/arduino-code
void swirl()
{
    // first check which part of the animation needs to be run
    if (!swirl_flag)
    {
        //  fade from blue to violet
        if (RED_VAL < 255)
        {
            RED_VAL++;
            analogWrite(REDPIN, RED_VAL);
            return;
        }
        // fade from violet to red
        if (BLUE_VAL > 0)
        {
            BLUE_VAL--;
            analogWrite(BLUEPIN, BLUE_VAL);
            return;
        }
        // fade from red to yellow
        if (GREEN_VAL < 255)
        {
            GREEN_VAL++;
            analogWrite(GREENPIN, GREEN_VAL);
            return;
        }
    }
    else
    {
        if (RED_VAL > 0)
        {
            RED_VAL--;
            analogWrite(REDPIN, RED_VAL);
            return;
        }
        // fade from green to teal
        if (BLUE_VAL < 255)
        {
            BLUE_VAL++;
            analogWrite(BLUEPIN, BLUE_VAL);
            return;
        }
        // fade from teal to blue
        if (GREEN_VAL > 0)
        {
            GREEN_VAL--;
            analogWrite(GREENPIN, GREEN_VAL);
            return;
        }
    }
    // if we reached here then one half of animation is done
    // and swirl flag needs to be flipped since none of the returns were called.
    swirl_flag = !swirl_flag;
}

// prints all the RGB clr values to Serial.
//  void print_clr_vals() {
//    Serial.print("RED: ");
//    Serial.print(RED_VAL);
//    Serial.print(" GREEN: ");
//    Serial.print(GREEN_VAL);
//    Serial.print(" BLUE: ");
//    Serial.print(BLUE_VAL);
//    Serial.println();
//  }
