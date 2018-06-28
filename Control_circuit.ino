/* Macro Define */
#include "TM1637Display.h"
#include "TM1637.h"
#define DIRECTION               4          /* sig pin of the Grove Buzzer */
#define PWM                     3
#define CLK                    39
#define DIO                    38


//const int CLK = 39; //set clk pin
//const int DIO = 38; // set DIO

TM1637Display display(CLK, DIO); //set up 4 digit display 

/* Array of values which assist the conversion of the thermocouple voltage to a temperature value */
const float Var_VtoT_K[3][10] =
{
    {0, 2.5173462*10, -1.1662878, -1.0833638, -8.9773540/10, -3.7342377/10,
    -8.6632643/100, -1.0450598/100, -5.1920577/10000},

    {0, 2.508355*10, 7.860106/100, -2.503131/10, 8.315270/100,
    -1.228034/100, 9.804036/10000, -4.413030/100000, 1.057734/1000000, -1.052755/100000000},

    {-1.318058*100, 4.830222*10, -1.646031, 5.464731/100, -9.650715/10000,
    8.802193/1000000, -3.110810/100000000}
};

int duty_high = 500;
int duty_low = 500;
bool direction = LOW;
int desired = 0;
int actual = 0;
int error = 0;

/* the setup() method runs once, when the sketch starts */
void setup()
{
/*Thermocouple code */
  Serial.begin(115200);
  Serial.println("grove - hight temperature sensor test demo");

  /* set buzzer pin as output */
  pinMode( PWM, OUTPUT); 
  pinMode( DIRECTION, OUTPUT);

// setup brightess 
  display.setBrightness(0x0a); //set the display 
}

void loop()
{
    actual = TempRead();
    error = desired - actual;
    //output temp
    display.showNumberDec(actual);
    PWMpulse(duty_high, duty_low, direction);
}

void PWMpulse(int duty_high, int duty_low, int direction){
    digitalWrite(PWM, HIGH);
    delayMicroseconds(duty_high);
    digitalWrite(PWM, LOW);
    delayMicroseconds(duty_low);
    digitalWrite( DIRECTION, direction);
}

/* Read the thermocouple temperature and retrn it */
int TempRead(){
 int j = 0;
    float sum = 0;
    float VOL_OFFSET = 350;     /* Offset voltage, mv */
    float AMP_AV = 54.16;       /* Av of amplifier */
    int tempRead = 0;

    for(j = 0; j < 10; j++)
    {
          float vout = (float)analogRead(26)/4095*3.3*1000;
          float mV  = (vout - VOL_OFFSET)/AMP_AV;

           int i = 0;
           float value = 0;

           if(mV >= -6.478 && mV < 0)
           {
               value = Var_VtoT_K[0][8];
               for(i = 8; i >0; i--)
               value = mV * value + Var_VtoT_K[0][i-1];
           }
           else if(mV >= 0 && mV < 20.644)
           {
               value = Var_VtoT_K[1][9];
               for(i = 9; i >0; i--)
               {
                   value = mV * value + Var_VtoT_K[1][i-1];
               }
           }
           else if(mV >= 20.644 && mV <= 54.900)
           {
               value = Var_VtoT_K[2][6];
               for(i = 6; i >0; i--)
               value = mV * value + Var_VtoT_K[2][i-1];
           }
       tempRead = value + 20;
       sum = sum + tempRead;
       tempRead = sum/10;
    }
    Serial.println(tempRead);
    return tempRead;
}

