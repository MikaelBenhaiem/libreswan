/* lset modifiers, for libreswan
 *
 * Copyright (C) 2017 Andrew Cagney
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

#include "constants.h"
#include "lmod.h"
#include "lswlog.h"

const lmod_t empty_lmod = {
	LEMPTY,
	LEMPTY,
};

bool lmod_empty(lmod_t mod)
{
	return mod.set == LEMPTY && mod.clr == LEMPTY;
}

void lmod_merge(lmod_t *lhs, lmod_t rhs)
{
	lhs->set = (lhs->set & ~rhs.clr) | rhs.set;
	lhs->clr = (lhs->clr & ~rhs.set) | rhs.clr;
}

lset_t lmod(lset_t set, lmod_t mod)
{
	return (set & ~mod.clr ) | mod.set;
}

lmod_t lmod_set(lmod_t mod, lset_t set)
{
	mod.set |= set;
	mod.clr &= ~set;
	return mod;
}

lmod_t lmod_clr(lmod_t mod, lset_t clr)
{
	mod.clr |= clr;
	mod.set &= ~clr;
	return mod;
}

bool lmod_is_set(lmod_t mod, lset_t set)
{
	return LIN(set, mod.set);
}

bool lmod_is_clr(lmod_t mod, lset_t clr)
{
	return LIN(clr, mod.clr);
}

bool lmod_arg(lmod_t *mod, enum_names *names,
	      lset_t all, lset_t none,
	      const char *optarg)
{
	if (streq(optarg, "all")) {
		mod->clr = LEMPTY;
		mod->set = all;
	} else if (streq(optarg, "none")) {
		mod->clr = none;
		mod->set = LEMPTY;
	} else {
		const char *arg = optarg;
		bool no = eat(arg, "no-");
		int ix = enum_match(names, arg);
		if (ix < 0) {
			return false;
		}
		lset_t bit = LELEM(ix);
		if (no) {
			mod->clr |= bit;
			mod->set &= ~bit;
		} else {
			mod->set |= bit;
			mod->clr &= ~bit;
		}
	}
	return true;
}

void lswlog_lmod(struct lswlog *buf, enum_names *names, lmod_t mod)
{
	lswlog_enum_lset_short(buf, names, mod.set);
	if (mod.clr != LEMPTY) {
		lswlogs(buf, " - ");
		lswlog_enum_lset_short(buf, names, mod.clr);
	}
}