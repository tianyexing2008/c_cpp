/* generated by MKexpanded.sh */
#define NEED_NCURSES_CH_T 1
#include <curses.priv.h>

#ifndef CUR
#define CUR SP_TERMTYPE
#endif

#if NCURSES_EXPANDED
 void
_nc_toggle_attr_on (attr_t *S, attr_t at)
{
 
# 14 "gen81759.c" 3
{ if (((int)((((unsigned long)(
# 14 "gen81759.c"
at
# 14 "gen81759.c" 3
) & ((((1UL) << 8) - 1UL) << ((0) + 8))) >> 8))) > 0) { (
# 14 "gen81759.c"
*S
# 14 "gen81759.c" 3
) = ((
# 14 "gen81759.c"
*S
# 14 "gen81759.c" 3
) & ALL_BUT_COLOR) | (attr_t) (
# 14 "gen81759.c"
at
# 14 "gen81759.c" 3
); } else { (
# 14 "gen81759.c"
*S
# 14 "gen81759.c" 3
) |= (attr_t) (
# 14 "gen81759.c"
at
# 14 "gen81759.c" 3
); } ;}
# 14 "gen81759.c"
                     ;
}

 void
_nc_toggle_attr_off (attr_t *S, attr_t at)
{
 
# 20 "gen81759.c" 3
{ if (((int)((((unsigned long)(
# 20 "gen81759.c"
at
# 20 "gen81759.c" 3
) & ((((1UL) << 8) - 1UL) << ((0) + 8))) >> 8))) > 0) { (
# 20 "gen81759.c"
*S
# 20 "gen81759.c" 3
) &= ~(
# 20 "gen81759.c"
at
# 20 "gen81759.c" 3
|((((1UL) << 8) - 1UL) << ((0) + 8))); } else { (
# 20 "gen81759.c"
*S
# 20 "gen81759.c" 3
) &= ~(
# 20 "gen81759.c"
at
# 20 "gen81759.c" 3
); } ;}
# 20 "gen81759.c"
                      ;
}

 int
_nc_DelCharCost ( int count)
{
 return 
# 26 "gen81759.c" 3
       ((cur_term->type. Strings[105] != 0) ? SP->_dch_cost : ((cur_term->type. Strings[21] != 0) ? (SP->_dch1_cost * 
# 26 "gen81759.c"
       count
# 26 "gen81759.c" 3
       ) : 1000000))
# 26 "gen81759.c"
                                  ;
}

 int
_nc_InsCharCost ( int count)
{
 return 
# 32 "gen81759.c" 3
       ((cur_term->type. Strings[108] != 0) ? SP->_ich_cost : ((cur_term->type. Strings[31] && cur_term->type. Strings[42]) ? SP->_smir_cost + SP->_rmir_cost + (SP->_ip_cost * 
# 32 "gen81759.c"
       count
# 32 "gen81759.c" 3
       ) : ((cur_term->type. Strings[52] != 0) ? ((SP->_ich1_cost + SP->_ip_cost) * 
# 32 "gen81759.c"
       count
# 32 "gen81759.c" 3
       ) : 1000000)))
# 32 "gen81759.c"
                                  ;
}

 void
_nc_UpdateAttrs ( 
# 36 "gen81759.c" 3
                                                 chtype 
# 36 "gen81759.c"
                                                           c)
{
 
# 38 "gen81759.c" 3
if (!((((*((SP)->_current_attr))) & (chtype)((~(1UL - 1UL)) << ((0) + 8))) == ((
# 38 "gen81759.c"
c
# 38 "gen81759.c" 3
) & (chtype)((~(1UL - 1UL)) << ((0) + 8))))) { vidattr(((
# 38 "gen81759.c"
c
# 38 "gen81759.c" 3
) & (chtype)((~(1UL - 1UL)) << ((0) + 8)))); }
# 38 "gen81759.c"
                                ;
}

#if NCURSES_SP_FUNCS
 int
_nc_DelCharCost (int count)
{
 return _nc_DelCharCost (
# 45 "gen81759.c" 3
                                         SP
# 45 "gen81759.c"
                                                       , count);
}

 int
_nc_InsCharCost (int count)
{
 return _nc_InsCharCost(
# 51 "gen81759.c" 3
                                        SP
# 51 "gen81759.c"
                                                      , count);
}

 void
_nc_UpdateAttrs (
# 55 "gen81759.c" 3
                chtype 
# 55 "gen81759.c"
                          c)
{
 _nc_UpdateAttrs(
# 57 "gen81759.c" 3
                                 SP
# 57 "gen81759.c"
                                               ,c);
}
#endif
#else /* ! NCURSES_EXPANDED */
NCURSES_EXPORT(void) _nc_expanded (void) { }
#endif /* NCURSES_EXPANDED */