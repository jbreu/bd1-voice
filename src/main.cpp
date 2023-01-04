#include <Arduino.h>
#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

#include "bd1sounds_1.h"
#include "bd1sounds_2.h"
#include "bd1sounds_3.h"
#include "bd1sounds_4.h"
#include "bd1sounds_5.h"
#include "bd1sounds_6.h"
#include "bd1sounds_7.h"
#include "bd1sounds_8.h"
#include "bd1sounds_9.h"
#include "bd1sounds_10.h"
#include "bd1sounds_11.h"
#include "bd1sounds_12.h"
#include "bd1sounds_13.h"
#include "bd1sounds_14.h"
#include "bd1sounds_15.h"
#include "bd1sounds_16.h"
#include "bd1sounds_17.h"
#include "bd1sounds_18.h"
#include "bd1sounds_19.h"

AudioGeneratorMP3 *mp3;
AudioFileSourcePROGMEM *file;
AudioOutputI2S *out;

double threshold = 0.02;     // microphone threshold to consider sound coming in
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;
unsigned int upcomingSamples;
bool hearingSound;

void loadRandomMP3()
{
  int choice = (rand() % 19) + 1;

  file->close();

  switch (choice)
  {
  case 1:
    file->open(bd1_fallenorder_1_mp3, bd1_fallenorder_1_mp3_len);
    break;

  case 2:
    file->open(bd1_fallenorder_2_mp3, bd1_fallenorder_2_mp3_len);
    break;

  case 3:
    file->open(bd1_fallenorder_3_mp3, bd1_fallenorder_3_mp3_len);
    break;

  case 4:
    file->open(bd1_fallenorder_4_mp3, bd1_fallenorder_4_mp3_len);
    break;

  case 5:
    file->open(bd1_fallenorder_5_mp3, bd1_fallenorder_5_mp3_len);
    break;

  case 6:
    file->open(bd1_fallenorder_6_mp3, bd1_fallenorder_6_mp3_len);
    break;

  case 7:
    file->open(bd1_fallenorder_7_mp3, bd1_fallenorder_7_mp3_len);
    break;

  case 8:
    file->open(bd1_fallenorder_8_mp3, bd1_fallenorder_8_mp3_len);
    break;

  case 9:
    file->open(bd1_fallenorder_9_mp3, bd1_fallenorder_9_mp3_len);
    break;

  case 10:
    file->open(bd1_fallenorder_10_mp3, bd1_fallenorder_10_mp3_len);
    break;

  case 11:
    file->open(bd1_fallenorder_11_mp3, bd1_fallenorder_11_mp3_len);
    break;

  case 12:
    file->open(bd1_fallenorder_12_mp3, bd1_fallenorder_12_mp3_len);
    break;

  case 13:
    file->open(bd1_fallenorder_13_mp3, bd1_fallenorder_13_mp3_len);
    break;

  case 14:
    file->open(bd1_fallenorder_14_mp3, bd1_fallenorder_14_mp3_len);
    break;

  case 15:
    file->open(bd1_fallenorder_15_mp3, bd1_fallenorder_15_mp3_len);
    break;

  case 16:
    file->open(bd1_fallenorder_16_mp3, bd1_fallenorder_16_mp3_len);
    break;

  case 17:
    file->open(bd1_fallenorder_17_mp3, bd1_fallenorder_17_mp3_len);
    break;

  case 18:
    file->open(bd1_fallenorder_18_mp3, bd1_fallenorder_18_mp3_len);
    break;

  case 19:
    file->open(bd1_fallenorder_19_mp3, bd1_fallenorder_19_mp3_len);
    break;

  default:
    break;
  }

  Serial.printf("Chose %d\n", choice);
}

// https://learn.adafruit.com/adafruit-microphone-amplifier-breakout/measuring-sound-levels
double getSoundLevel()
{
  unsigned long startMillis = millis(); // Start of sample window
  unsigned int peakToPeak = 0;          // peak-to-peak level

  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;

  // collect data for 50 mS
  while (millis() - startMillis < sampleWindow)
  {
    sample = analogRead(A0);
    if (sample < 1024) // toss out spurious readings
    {
      if (sample > signalMax)
      {
        signalMax = sample; // save just the max levels
      }
      else if (sample < signalMin)
      {
        signalMin = sample; // save just the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin;       // max - min = peak-peak amplitude
  double volts = (peakToPeak * 5.0) / 1024; // convert to volts

  Serial.println(volts);

  analogWrite(D2, (int)min(255, (int)(255 * (volts / threshold))));

  return volts;
}

void setup()
{
  Serial.begin(115200);

  audioLogger = &Serial;
  delay(1000);

  srand(analogRead(A0));

  out = new AudioOutputI2S(0, AudioOutputI2S::EXTERNAL_I2S, 4096);
  mp3 = new AudioGeneratorMP3(nullptr, 4096);
  file = new AudioFileSourcePROGMEM();

  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);

  analogWrite(D1, LOW);
  analogWrite(D2, 255);
}

void loop()
{
  if (mp3->isRunning())
  {
    if (!mp3->loop())
    {
      mp3->stop();
      analogWrite(D1, LOW);
      analogWrite(D2, 255);
    }
  }
  else
  {
    if (upcomingSamples > 0)
    {
      loadRandomMP3();
      mp3->begin(file, out);
      analogWrite(D1, 255);
      analogWrite(D2, LOW);

      upcomingSamples--;
    }
    else
    {
      if (!hearingSound && getSoundLevel() > threshold)
      {
        hearingSound = true;
        return;
      }

      if (hearingSound && getSoundLevel() <= threshold)
      {
        hearingSound = false;
        delay(500);
        upcomingSamples = (rand() % 5) + 1;
        return;
      }

      delay(10);
    }
  }
}