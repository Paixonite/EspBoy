#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include "Arduino.h"
#include "audio_assets.h"

class SoundManager {
public:
    SoundManager(int buzzer_pin); // Construtor
    void play(const Note melody[], int melody_length);
    void stop();
    void loop(); // A função que deve ser chamada repetidamente

private:
    int _buzzer_pin;
    const Note* _current_melody;
    int _melody_length;
    int _current_note_index;
    unsigned long _note_start_time;
    bool _is_playing;
};

#endif // SOUNDMANAGER_H