#ifndef PATTERN_H
#define PATTERN_H

#include "barr.h"
#include "bstr.h"

typedef struct step {
	int	st_on;
	int	st_level;
} step_t;

typedef struct sequence {
	int	se_midinote;
	barr_t	*se_steps;
} seqeuence_t;


typedef struct miditick {
	bstr_t	*mt_data;
} miditick_t;


extern barr_t	*ticks;
extern barr_t	*curtick;
extern int	curbar;
extern int	curbeat;

#endif
