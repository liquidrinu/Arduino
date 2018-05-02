#include <SD.h>                      // need to include the SD library
#define SD_ChipSelectPin 53  //example uses hardware SS pin 53 on Mega2560
//#define SD_ChipSelectPin 10  //using digital pin 4 on arduino nano 328
#include <TMRpcm.h>           //  also need to include this library...

/*  FUNCTIONS():
  TMRpcm audio;

  au.play("filename");    plays a file
  au.play("filename",30); plays a file starting at 30 seconds into the track
  au.speakerPin = 11;     set to 5,6,11 or 46 for Mega, 9 for Uno, Nano, etc.
  au.disable();           disables the timer on output pin and stops the music
  au.stopPlayback();      stops the music, but leaves the timer running
  au.isPlaying();         returns 1 if music playing, 0 if not
  au.pause();             pauses/unpauses playback
  au.quality(1);          Set 1 for 2x oversampling
  au.volume(0);           1(up) or 0(down) to control volume
  au.setVolume(0);        0 to 7. Set volume level
  au.loop(1);             0 or 1. Can be changed during playback for full control of looping.
*/
//////////////////////////////////////////////////////////////////////////////////////////////////
// Logix
/////////

int potRead = A0;
int potVol = A7;
int vol; // mapped reading;

int track = "mySong.wav"; // place path for track name from SD here

TMRpcm au;   // create an object for use in this sketch

int speakerPin = 11;

void setup() {

  // config
  sd_check();

  pinMode(53, OUTPUT);

  Serial.begin(9600);

  au.speakerPin = 11; //11 on Mega, 9 on Uno, Nano, etc
  au.quality(1);
  au.play(track); // filename
  au.volume(1);
  au.loop(1);

}

void loop(void) {

}

// FUNCTIONS

int sd_check() {
  if (!SD.begin(SD_ChipSelectPin)) {  // see if the card is present and can be initialized:
    Serial.println("SD fail");
    return;   // don't do anything more if not
  }
};
