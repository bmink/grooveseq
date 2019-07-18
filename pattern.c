#include <errno.h>
#include <ctype.h>
#include "pattern.h"
#include "bstr.h"
#include "blog.h"

#define MIDI_NOTE_MIN	0
#define MIDI_NOTE_MAX	127
#define MAX_PART	128

#define SEP_POS_1	3
#define SEP_POS_2	20
#define SEP_POS_3	37
#define SEP_POS_4	54
#define SEP_POS_5	71


static part_t	parts[MAX_PART];

static int	curbar;
static int	maxbar;

static miditick_t *ticks;

static int	pattern_inited;

int
pattern_init(void)
{
	curbar = 0;
	maxbar = 0;

	ticks = NULL;

	memset(parts, 0, sizeof(part_t) * MAX_PART);

	++pattern_inited;
	
	return 0;
}


int
pattern_parse_line(bstr_t *line)
{
	int	err;
	char	*str;
	int	note;
	int	cur;
	int	val;
	int	curstep;


	err = 0;

	if(!pattern_inited)
		return ENOEXEC;

	if(bstrempty(line))
		return 0;

	if(bstrbeginswith(line, "bar:")) {
		val = atoi(bget(line) + 4);
		if(curbar < 0 || curbar >= MAX_BAR) {
			blogf("Bar nr. out of bound.");
			err = EINVAL;
			goto end_label;
		}
		curbar = val;
		blogf("Processing bar %d", curbar);
		if(curbar > maxbar)
			maxbar = curbar;
		return 0;
	}

	if(bstrlen(line) < 71) {
		blogf("Line too short");
		err = ENOEXEC;
		goto end_label;
	}

	str = bget(line);

	if(str[SEP_POS_1] != '|' || str[SEP_POS_2] != '|' ||
	    str[SEP_POS_3] != '|' || str[SEP_POS_4] != '|' ||
	    str[SEP_POS_5] != '|') {
		blogf("Missing separator line.");
		err = ENOENT;
		goto end_label;
	}

	note = atoi(str);
	if(note < MIDI_NOTE_MIN || note > MIDI_NOTE_MAX) {
		blogf("Invalid MIDI note.");
		err = ENOENT;
		goto end_label;
	}

	curstep = 0;
	for(cur = SEP_POS_1 + 1; cur < SEP_POS_5; ++cur) {
		if(cur == SEP_POS_2 || cur == SEP_POS_3 || cur == SEP_POS_4)
			continue;

		if(isdigit(str[cur])) {
			parts[note].pa_bars[curbar].se_steps[curstep].st_on++;
			parts[note].pa_bars[curbar].se_steps[curstep].st_vel =
			    str[cur] - '0';
		} else if(str[cur] != ' ') {
			blogf("Invalid character at position %d", cur + 1);
			err = EINVAL;
			goto end_label;
		}

		++curstep;
	}
	

end_label:

	return err;
}


int
pattern_process()
{
	size_t	tickcnt;
	int	err;

	if(!pattern_inited)
		return ENOEXEC;

	err = 0;

	tickcnt = (maxbar + 1) * 4 * MIDITICK_PER_QUARTER_NOTE;

	blogf("Processing %d bars, %d ticks", maxbar + 1, tickcnt);

	ticks = malloc(sizeof(miditick_t) * tickcnt);
	if(ticks == NULL) {
		blogf("Couldn't allocate ticks array");
		err = ENOMEM;
		goto end_label;
	}

	
end_label:

	if(err != 0) {
		if(ticks != NULL) {
			// TODO free mididata
			free(ticks);
			ticks = NULL;
		}
	}

	


	return err;
}

int
pattern_uninit()
{

	if(ticks)
		// TODO free mididata
		free(ticks);

	ticks = NULL;

	memset(parts, 0, sizeof(part_t) * MAX_PART);

	curbar = 0;
	maxbar = 0;

	pattern_inited = 0;

	return 0;
}


