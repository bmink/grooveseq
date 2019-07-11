P = grooveseq
OBJS = main.o midi_osx.o midi_msg.o pattern.o midi_note.o
CFLAGS = -g -Wall -Wstrict-prototypes
LDLIBS = -lb -framework CoreMIDI -framework CoreServices

$(P): $(OBJS)
	$(CC) -o $(P) $(LDFLAGS) $(OBJS) $(LDLIBS)

clean:
	rm -f *o; rm -f $(P)
