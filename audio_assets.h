#ifndef AUDIO_ASSETS_H
#define AUDIO_ASSETS_H

// Inclui as definições de frequência das notas (copiado do seu pitches.h)
#define REST      0
#define NOTE_C4   262
#define NOTE_D4   294
#define NOTE_E4   330
#define NOTE_F4   349
#define NOTE_G4   392
#define NOTE_A4   440
#define NOTE_B4   494
#define NOTE_C5   523
#define NOTE_G5   784
// Adicione outras notas que precisar...

// Uma estrutura para deixar a criação de melodias mais organizada
// Cada 'Note' terá uma frequência e uma duração.
struct Note {
    int frequency; // Frequência em Hz (ex: NOTE_C4)
    int duration;  // Duração em milissegundos
};

// --- NOSSAS MELODIAS ---

// Melodia para quando a cobra come uma maçã (curta e aguda)
const Note MELODY_APPLE[] = {
    {NOTE_C5, 100}, // Nota C5 por 100ms
    {NOTE_G5, 100}  // Nota G5 por 100ms
};
// Calcula o tamanho do array automaticamente
const int MELODY_APPLE_LENGTH = sizeof(MELODY_APPLE) / sizeof(Note);


// Melodia para a tela de Game Over (mais longa e grave)
const Note MELODY_GAME_OVER[] = {
    {NOTE_G4, 250},
    {NOTE_F4, 250},
    {NOTE_E4, 250},
    {NOTE_D4, 500}
};
const int MELODY_GAME_OVER_LENGTH = sizeof(MELODY_GAME_OVER) / sizeof(Note);

#endif // AUDIO_ASSETS_H