#include "seq.h"
#include "seq_debug.h"

/*
 * Create an event flag.
 */
evflag seq_efCreate(PROG_ID sp, unsigned ef_num, unsigned val)
{
    evflag ef = sp->eventFlags + ef_num;

    assert(ef_num > 0 && ef_num <= sp->numEvFlags);
    ef->value = val;
    /* allocate the set of channel numbers synced to this event flag */
    ef->synced = newArray(seqMask, NWORDS(sp->numChans));
    if (!ef->synced) {
        errlogSevPrintf(errlogFatal, "efCreate: calloc failed\n");
        return 0;
    }
    ef->lock = epicsMutexCreate();
    if (!ef->lock) {
        errlogSevPrintf(errlogFatal, "efCreate: epicsMutexCreate failed\n");
        return 0;
    }
    DEBUG("efCreate returns %p\n", ef);
    return ef;
}

/*
 * Synchronize pv with an event flag.
 * ev_flag == 0 means unSync.
 */
epicsShareFunc void seq_pvSync(SS_ID ss, CH_ID ch, evflag new_ev_flag)
{
	seq_pvArraySync(ss, &ch, 1, new_ev_flag);
}

/*
 * Array variant of seq_pvSync.
 */
epicsShareFunc void seq_pvArraySync(SS_ID ss, CH_ID *chs, unsigned length, evflag new_ev_flag)
{
    PROG *sp = ss->prog;
    unsigned n;

    DEBUG("pvSync: new_ev_flag=%p\n", new_ev_flag);
    for (n = 0; n < length; n++) {
        CH_ID ch = chs[n];
        unsigned nch = ch - sp->chan;

        if (ch->syncedTo != new_ev_flag) {
            if (ch->syncedTo) {
                epicsMutexMustLock(ch->syncedTo->lock);
                bitClear(ch->syncedTo->synced, nch);
                epicsMutexUnlock(ch->syncedTo->lock);
            }
            if (new_ev_flag) {
                epicsMutexMustLock(new_ev_flag->lock);
                bitSet(new_ev_flag->synced, nch);
                epicsMutexUnlock(new_ev_flag->lock);
            }
            ch->syncedTo = new_ev_flag;
            DEBUG("pvSync: syncedTo=%p\n", ch->syncedTo);
        }
    }
}

/*
 * Set an event flag, then wake up each state
 * set that might be waiting on that event flag.
 */
epicsShareFunc void seq_efSet(SS_ID ss, evflag ev_flag)
{
    PROG *sp = ss->prog;
    unsigned efNum = ev_flag - sp->eventFlags;

    assert(efNum > 0 && efNum <= sp->numEvFlags);
    epicsMutexMustLock(ev_flag->lock);
    ev_flag->value = TRUE;
    ss_wakeup(sp, efNum);
    epicsMutexUnlock(ev_flag->lock);
}

/*
 * Return whether event flag is set.
 */
epicsShareFunc boolean seq_efTest(SS_ID ss, evflag ev_flag)
/* event flag */
{
    PROG *sp = ss->prog;
    boolean isSet;
    unsigned efNum = ev_flag - sp->eventFlags;

    assert(efNum > 0 && efNum <= sp->numEvFlags);
    epicsMutexMustLock(ev_flag->lock);
    isSet = ev_flag->value;
    if (isSet && optTest(sp, OPT_SAFE)) {
        ss_read_buffer_selective(sp, ss, ev_flag);
    }
    if (ss->eval_when) {
        bitSet(ss->mask, efNum);
    }
    epicsMutexUnlock(ev_flag->lock);
    return isSet;
}

/*
 * Clear event flag.
 */
epicsShareFunc boolean seq_efClear(SS_ID ss, evflag ev_flag)
{
    PROG *sp = ss->prog;
    boolean isSet;
    unsigned efNum = ev_flag - sp->eventFlags;

    assert(efNum > 0 && efNum <= sp->numEvFlags);
    epicsMutexMustLock(ev_flag->lock);
    isSet = ev_flag->value;
    ev_flag->value = FALSE;
    ss_wakeup(sp, efNum);
    epicsMutexUnlock(ev_flag->lock);
    return isSet;
}

/*
 * Clear event flag and return whether it was set.
 */
epicsShareFunc boolean seq_efTestAndClear(SS_ID ss, evflag ev_flag)
{
    PROG *sp = ss->prog;
    boolean isSet;
    unsigned efNum = ev_flag - sp->eventFlags;

    assert(efNum > 0 && efNum <= sp->numEvFlags);
    epicsMutexMustLock(ev_flag->lock);
    isSet = ev_flag->value;
    ev_flag->value = FALSE;
    if (isSet && optTest(sp, OPT_SAFE)) {
        ss_read_buffer_selective(sp, ss, ev_flag);
    }
    if (ss->eval_when) {
        bitSet(ss->mask, efNum);
    }
    epicsMutexUnlock(ev_flag->lock);
    return isSet;
}
