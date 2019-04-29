/*************************************************************************\
Copyright (c) 1990      The Regents of the University of California
                        and the University of Chicago.
                        Los Alamos National Laboratory
Copyright (c) 2010-2015 Helmholtz-Zentrum Berlin f. Materialien
                        und Energie GmbH, Germany (HZB)
This file is distributed subject to a Software License Agreement found
in the file LICENSE that is included with this distribution.
\*************************************************************************/
/*************************************************************************\
                Generate tables for runtime sequencer
\*************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "analysis.h"
#include "main.h"
#include "sym_table.h"
#include "gen_ss_code.h"
#include "gen_code.h"
#include "node.h"
#include "var_types.h"
#include "gen_tables.h"
#include "seq_mask.h"
#include "seq_release.h"

static void gen_evflag_table(EvFlagList *ef_list, Options *options);
static void gen_monitor_masks(ChanList *chan_list, uint num_ss);
static void gen_channel_table(ChanList *chan_list, Options *options);
static void gen_state_table(Node *ss_list, uint num_event_flags, uint num_channels);
static void fill_state_struct(Node *sp, char *ss_name, uint ss_num);
static void gen_prog_table(Node *prog);
static void encode_options(Options *options);
static void encode_state_options(StateOptions options);
static void gen_ss_table(Node *ss_list);

/* Generate all kinds of tables for a SNL program. */
void gen_tables(Node *prog)
{
	Program *p;

	assert(prog->tag == D_PROG);
	p = prog->extra.e_prog;

	gen_code("\n/************************ Tables ************************/\n");
	gen_evflag_table(p->evflag_list, p->options);
	gen_monitor_masks(p->chan_list, p->num_ss);
	gen_channel_table(p->chan_list, p->options);
	gen_state_table(prog->prog_statesets, p->evflag_list->num_elems, p->chan_list->num_elems);
	gen_ss_table(prog->prog_statesets);
	gen_prog_table(prog);
}

/* Generate event flag table */
static void gen_evflag_table(EvFlagList *ef_list, Options *options)
{
	EvFlag *ef;

	if (ef_list->first)
	{
		gen_code("\n/* Event flag table */\n");
		gen_code("static seqEvFlag " NM_EVFLAGS "[] = {\n");
		foreach (ef, ef_list->first)
			gen_ef_entry(default_context(options), ef);
		gen_code("};\n");
	}
	else
	{
		gen_code("\n/* No event flag definitions */\n");
		gen_code("#define " NM_EVFLAGS " 0\n");
	}
}

typedef struct monitor_mask {
	seqMask	*mask;
	struct monitor_mask *next;
} MonitorMask;

static uint seq_mask_eq(seqMask *lhs, seqMask *rhs, unsigned num_words)
{
	uint n;
	for (n=0; n<num_words; n++)
	{
#ifdef DEBUG
		printf("lhs[%d]=0x%08x,rhs[%d]=0x%08x\n",n,lhs[n],n,rhs[n]);
#endif
		if (lhs[n] != rhs[n])
			return FALSE;
	}
	return TRUE;
}

static int find_mask(seqMask *needle, MonitorMask **phaystack, unsigned num_words)
{
	int i = 0;
	int n;
	MonitorMask *mm, *last=0;

	if (phaystack)
	{
		foreach (mm, *phaystack)
		{
			if (seq_mask_eq(needle, mm->mask, num_words))
			{
				return i;
			}
			last = mm;
			i++;
		}
	}
	/* not found? then add it to the list */
	mm = new(MonitorMask);
	mm->mask = newArray(seqMask, num_words);
	for (n = 0; n < num_words; n++)
		mm->mask[n] = needle[n];
	if (last)
		last->next = mm;
	else
		*phaystack = mm;
#ifdef DEBUG
	printf("find_mask returned %d\n",i);
#endif
	return i;
}

static void gen_monitor_masks(ChanList *chan_list, uint num_ss)
{
	MonitorMask *first_mask = 0;
	seqMask *mask;
	Chan *chan;
	uint num_words = NWORDS(num_ss);
	int num_masks = -1;

	gen_code("\n/* Monitor masks */\n");
	mask = newArray(seqMask, num_words);
	foreach (chan, chan_list->first)
	{
		Monitor *mp;
		uint n;
		foreach (mp, chan->monitor)
		{
			if (mp->scope->tag == D_PROG)
			{
				uint i;
				for (i=0; i<num_ss; i++)
					bitSet(mask, i);
			}
			else
				bitSet(mask, mp->scope->extra.e_ss->index);
		}
		chan->mon_mask_index = find_mask(mask, &first_mask, num_words);
		if (chan->mon_mask_index > num_masks)
		{
			num_masks = chan->mon_mask_index;
			/* generate code for a new mask */
			gen_code("static const seqMask " NM_MONMASK "_%d[] = {\n", num_masks);
			for (n = 0; n < num_words; n++)
				gen_code("\t0x%08x,\n", mask[n]);
			gen_code("};\n");
		}
		/* reset mask */
		for (n = 0; n < num_words; n++)
			mask[n] = 0;
	}
}

/* Generate channel table with data for each defined channel */
static void gen_channel_table(ChanList *chan_list, Options *options)
{
	Chan *cp;

	if (chan_list->first)
	{
		gen_code("\n/* Channel table */\n");
		gen_code("static seqChan " NM_CHANS "[] = {\n");
		gen_code("\t/* chOffset, chName, valOffset, expr, type, count, efNum, queueSize, queueIndex */\n");
		foreach (cp, chan_list->first)
		{
			gen_channel_entry(default_context(options), cp);
		}
		gen_code("};\n");
	}
	else
	{
		gen_code("\n/* No channel definitions */\n");
		gen_code("#define " NM_CHANS " 0\n");
	}
}

/* Generate state event mask and table */
static void gen_state_table(Node *ss_list, uint num_event_flags, uint num_channels)
{
	Node	*ssp;
	Node	*sp;
	uint	ss_num = 0;

	/* NOTE: Bit zero of event mask is not used. Bit 1 to num_event_flags
	   are used for event flags, then come channels. */

	/* For each state set... */
	foreach (ssp, ss_list)
	{
		/* Generate table of state structures */
		gen_code("\n/* State table for state set \"%s\" */\n", ssp->token.str);
		gen_code("static seqState " NM_STATES "_%s[] = {\n", ssp->token.str);
		foreach (sp, ssp->ss_states)
		{
			fill_state_struct(sp, ssp->token.str, ss_num);
		}
		gen_code("};\n");
		ss_num++;
	}
}

/* Generate a state struct */
static void fill_state_struct(Node *sp, char *ss_name, uint ss_num)
{
	gen_code("\t{\n");
	gen_code("\t/* state name */        \"%s\",\n", sp->token.str);
	gen_code("\t/* transition function */" NM_TRANS "_%s_%d_%s,\n", ss_name, ss_num, sp->token.str);
	gen_code("\t/* entry function */    ");
	if (sp->state_entry)
		gen_code(NM_ENTRY "_%s_%d_%s,\n", ss_name, ss_num, sp->token.str);
	else
		gen_code("0,\n");
	gen_code("\t/* exit function */     ");
	if (sp->state_exit)
		gen_code(NM_EXIT "_%s_%d_%s,\n", ss_name, ss_num, sp->token.str);
	else
		gen_code("0,\n");
	gen_code("\t/* state options */     ");
	encode_state_options(sp->extra.e_state->options);
	gen_code("\n\t},\n");
}

/* Generate the state option bitmask */
static void encode_state_options(StateOptions options)
{
	gen_code("(0");
	if (!options.do_reset_timers)
		gen_code(" | OPT_NORESETTIMERS");
	if (!options.no_entry_from_self)
		gen_code(" | OPT_DOENTRYFROMSELF");
	if (!options.no_exit_to_self)
		gen_code(" | OPT_DOEXITTOSELF");
	gen_code(")");
} 

/* Generate state set table, one entry for each state set */
static void gen_ss_table(Node *ss_list)
{
	Node	*ssp;
	int	num_ss;

	gen_code("\n/* State set table */\n");
	gen_code("static seqSS " NM_STATESETS "[] = {\n");
	num_ss = 0;
	foreach (ssp, ss_list)
	{
		if (num_ss > 0)
			gen_code("\n");
		num_ss++;
		gen_code("\t{\n");
		gen_code("\t/* state set name */    \"%s\",\n", ssp->token.str);
		gen_code("\t/* states */            " NM_STATES "_%s,\n", ssp->token.str);
		gen_code("\t/* number of states */  %d\n", ssp->extra.e_ss->num_states);
		gen_code("\t},\n");
	}
	gen_code("};\n");
}

/* Generate a single program structure ("seqProgram") */
static void gen_prog_table(Node *prog)
{
	Program *p = prog->extra.e_prog;

	gen_code("\n/* Program table (global) */\n");
	gen_code("seqProgram %s = {\n", prog->token.str);
	gen_code("\t/* magic number */      %d,\n", MAGIC);
	gen_code("\t/* program name */      \"%s\",\n", prog->token.str);
	gen_code("\t/* channels */          " NM_CHANS ",\n");
	gen_code("\t/* num. channels */     %d,\n", p->chan_list->num_elems);
	gen_code("\t/* state sets */        " NM_STATESETS ",\n");
	gen_code("\t/* num. state sets */   %d,\n", p->num_ss);
	if (p->options->reent)
		gen_code("\t/* user var size */     sizeof(struct %s),\n", NM_VARS);
	else
		gen_code("\t/* user var size */     0,\n");
	gen_code("\t/* param */             \"%s\",\n",
		prog->prog_param ? prog->prog_param->token.str : "");
	gen_code("\t/* event flags */       " NM_EVFLAGS ",\n");
	gen_code("\t/* num. event flags */  %d,\n", p->evflag_list->num_elems);
	gen_code("\t/* encoded options */   "); encode_options(p->options);
	gen_code("\t/* init func */         " NM_INIT ",\n");
	gen_code("\t/* entry func */        %s,\n", prog->prog_entry ? NM_ENTRY : "0");
	gen_code("\t/* exit func */         %s,\n", prog->prog_exit ? NM_EXIT : "0");
	gen_code("\t/* num. queues */       %d\n", p->syncq_list->num_elems);
	gen_code("};\n");
}

static void encode_options(Options *options)
{
	gen_code("(0");
	if (options->async)
		gen_code(" | OPT_ASYNC");
	if (options->conn)
		gen_code(" | OPT_CONN");
	if (options->debug)
		gen_code(" | OPT_DEBUG");
	if (options->reent)
		gen_code(" | OPT_REENT");
	if (options->safe)
		gen_code(" | OPT_SAFE");
	gen_code("),\n");
}
