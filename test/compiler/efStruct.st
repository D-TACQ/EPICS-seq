program efStructTest

option +s;

struct Limit {
  double pv val;
  evflag flag;
};

struct Limits {
  struct Limit lo;
  struct Limit hi;
};

struct Limits limits = {
  { pv ("", monitor, sync limits.lo.flag), 0 },
  { pv ("", monitor, sync limits.hi.flag), 0 }
};

ss val {
  state START {
    when ( efTestAndClear( limits.lo.flag ) && limits.lo.val > limits.hi.val ) {
      limits.hi.val = limits.lo.val;
      pvPut( limits.hi.val );
    } state START

    when ( efTestAndClear( limits.hi.flag ) && limits.hi.val < limits.lo.val ) {
      limits.lo.val = limits.hi.val;
      pvPut( limits.lo.val );
    } state START
  }
}
