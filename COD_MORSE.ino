// morse-decoder-encode




// Acest sketch decodeaza secvente cu litere sau cifre
// si le afiseasa in serial monitor
//Un sunet lung si semnul intrebarii arata un caracter introdus necunoscut
//
// Pentru a invata codul Morse apasa un buton in serial monitor

// Sfaturi
//   Dots scurt,dash lung
//   Lasa un spatiu scurt dupa litere .
//   Spatiu nai lung intre cuvinte.

//
// Circuit:
// Piezo buzzer sau difuzor care se conecteaza la pinul 2 si ground
// Button connects between pin 8 and ground
// LED (+) la pin 13, (-) 220 Ohm rezistor la ground
//
int tonePin = 2;
int toneFreq = 1000;
int ledPin = 13;
int buttonPin = 8;
int debounceDelay = 30;

int dotLength = 240; 
// dotLength = unitate standard masurata in milisecunde
// 240 gives 5 words per minute (WPM) speed.
// WPM = 1200/dotLength.
// For other speeds, use dotLength = 1200/(WPM)
//
// Lungime derivate din lungimea dot ului
  int dotSpace = dotLength;
  int dashLength = dotLength*3;
  int letterSpace = dotLength*3;
  int wordSpace = dotLength*7; 
  float wpm = 1200./dotLength;
  
int t1, t2, onTime, gap;
bool newLetter, newWord, letterFound, keyboardText;
int lineLength = 0;
int maxLineLength = 20; 

char* letters[] = 
{
".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", // A-I
".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", // J-R 
"...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.." // S-Z
};

char* numbers[] = 
{
"-----", ".----", "..---", "...--", "....-", //0-4
".....", "-....", "--...", "---..", "----." //5-9
};

String dashSeq = "";
char keyLetter, ch;
int i, index;

void setup() 
{
  delay(500);
  pinMode(ledPin, OUTPUT);
  pinMode(tonePin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println();
  Serial.println("-------------------------------");
  Serial.println("Morse Code decoder/encoder");
  Serial.print("Speed=");
  Serial.print(wpm);
  Serial.print("wpm, ");
  Serial.print("dot=");
  Serial.print(dotLength);
  Serial.println("ms");   
  
// Testarea LED ului si ton ului
  tone(tonePin, toneFreq);
  digitalWrite(ledPin, HIGH);
  delay(2000);
  digitalWrite(ledPin, LOW);
  noTone(tonePin);
  delay(600);

/
//A
   Serial.print("A .-  ");
   index = 'A'-65;
   flashSequence(letters[index]);
   delay(wordSpace);
//B
   Serial.print("B -...  ");
   index = 'B'-65;
   flashSequence(letters[index]);
   delay(wordSpace);
//C
   Serial.print("C -.-.  ");
   index = 'C'-65;
   flashSequence(letters[index]);
   delay(wordSpace);

  Serial.println();
  Serial.println("-------------------------------");
  Serial.println("Apasa campul in Serial Monitor,");
  Serial.println("Tasteaza text si apasa enter, or");
  Serial.println("Decodeaza");
  Serial.println("-------------------------------");
      
  newLetter = false; //if false, do NOT check for end of letter gap
  newWord = false;  //if false, do NOT check for end of word gap
  keyboardText = false; 
}

void loop() 
{
// Check to see if something has been entered on the keyboard
  if (Serial.available() > 0)
  {
    if (keyboardText == false) 
    {
      Serial.println();
      Serial.println("-------------------------------");
    }
    keyboardText = true;
    ch = Serial.read();
    if (ch >= 'a' && ch <= 'z')
    { ch = ch-32; }
    
    if (ch >= 'A' && ch <= 'Z')
    {
      Serial.print(ch); 
      Serial.print(" ");
      Serial.println(letters[ch-'A']);
      flashSequence(letters[ch-'A']);
      delay(letterSpace);
    }
    if (ch >= '0' && ch <= '9')
    {
      Serial.print(ch);
      Serial.print(" ");
      Serial.println(numbers[ch-'0']);
      flashSequence(numbers[ch-'0']);
      delay(letterSpace);
    }
    if (ch == ' ')
    {
      Serial.println("_");
      delay(wordSpace);    
    } 

// Prineaza un header dupa ultimul text de la tastatura 
     if (Serial.available() <= 0) 
     {
      Serial.println();
      Serial.println("Scrie textul:");
      Serial.println("-------------------------------");
      keyboardText = false;
     }
  }
 
  if (digitalRead(buttonPin) == LOW ) //butonul e apasat
  {
    newLetter = true; 
    newWord = true;
    t1=millis(); //timpul la apasarea butonului
    digitalWrite(ledPin, HIGH); //pornim led ul
    tone(tonePin, toneFreq);
    delay(debounceDelay);     
    while (digitalRead(buttonPin) == LOW ) // asteapta pana cand butonul nu mai este apasat
      {delay(debounceDelay);}
      delay(debounceDelay);
       
    t2 = millis();  //timpul la ridicare degetului de pe buton
    onTime=t2-t1;  //length of dot or dash keyed in
    digitalWrite(ledPin, LOW); //torn off LED and tone
    noTone(tonePin); 
    
//verifica daca e dot sau dash

    if (onTime <= dotLength*1.5) //allow for 50% longer 
      {dashSeq = dashSeq + ".";} //build dot/dash sequence
    else 
      {dashSeq = dashSeq + "-";}
  }  //end button press section
  
// look for a gap >= letterSpace to signal end letter
// end of letter when gap >= letterSpace

  gap=millis()-t2; 
  if (newLetter == true && gap>=letterSpace)  
  { 
    
//check through letter sequences to find matching dash sequence

    letterFound = false; keyLetter = 63; //char 63 este "?"
    for (i=0; i<=25; i++)
    {
      if (dashSeq == letters[i]) 
      {
        keyLetter = i+65;
        letterFound = true;   
        break ;    //nu mai verifica daca litera este gasita
      }
    }
    if(letterFound == false) //verifica numere
    {
      for (i=0; i<=10; i++)
      {
      if (dashSeq == numbers[i]) 
        {
          keyLetter = i+48;
          letterFound = true;   
          break ;    //nu mai verifica daca numarul este gasit
        }
      }
    }    
    Serial.print(keyLetter);
    if(letterFound == false) //buzz for secventa necunoscuta
    {
      tone(tonePin, 100, 500);
    }  
    newLetter = false; //reset
    dashSeq = "";
    lineLength=lineLength+1;
  }  
  
// litera cheie a fost identificata si printata

// cand gap-ul este >= wordSpace, insereaza spatiu intre cuvinte
// lengthen the word space by 50% to allow for variation

  if (newWord == true && gap>=wordSpace*1.5)
    { 
     newWord = false; 
     Serial.print("_");  
     lineLength=lineLength+1;
     
// flash  ca sa indice un cuvant nou

    digitalWrite(ledPin, HIGH);
    delay(25);
    digitalWrite(ledPin, LOW);       
    } 

// insereaza linebreakuri

  if (lineLength >= maxLineLength) 
    {
      Serial.println();
      lineLength = 0;
    }      
} 

void flashSequence(char* sequence)
{
  int i = 0;
  while (sequence[i] == '.' || sequence[i] == '-')
  {
    flashDotOrDash(sequence[i]);
    i++;
  }
}

void flashDotOrDash(char dotOrDash)
{
  digitalWrite(ledPin, HIGH);
  tone(tonePin, toneFreq);
  if (dotOrDash == '.')
   { delay(dotLength); }
     else
   { delay(dashLength); }

  digitalWrite(ledPin, LOW);
  noTone(tonePin);
  delay(dotLength); 
}
//Sfarsitul codului