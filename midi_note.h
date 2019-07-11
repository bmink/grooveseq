#ifndef MIDI_NOTE_H
#define MIDI_NOTE_H

typedef struct midi_note {
	int	mn_code;
	char	*mn_name;
} midi_note_t;

const char *midinote_getname(int);
int midinote_getcode(const char *);

#endif
