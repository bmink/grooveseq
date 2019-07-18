#ifndef PATTERN_H
#define PATTERN_H

#include "barr.h"
#include "bstr.h"


#define STEPS_PER_BAR	64
#define MAX_BAR		512

typedef struct step {
	int	st_on;
	int	st_vel;
} step_t;


typedef struct bar {
	step_t	se_steps[STEPS_PER_BAR];
	int	se_probability; /* etc... */
} bar_t;


typedef struct part {
	bar_t	pa_bars[MAX_BAR];
} part_t;

#define MIDITICK_PER_QUARTER_NOTE	24

typedef struct miditick {
	bstr_t	*mt_data;
} miditick_t;


int pattern_init(void);
int pattern_parse_line(bstr_t *);
int pattern_process(void);
int pattern_uninit(void);



#endif
