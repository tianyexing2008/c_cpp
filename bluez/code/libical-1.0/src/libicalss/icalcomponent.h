/* -*- Mode: C -*- */
/*======================================================================
 FILE: icalcomponent.h
 CREATOR: eric 20 March 1999


  (C) COPYRIGHT 1999 Eric Busboom 
  http://www.softwarestudio.org

 This program is free software; you can redistribute it and/or modify
 it under the terms of either:

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.fsf.org/copyleft/lesser.html

  Or:

    The Mozilla Public License Version 1.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/

 The Original Code is eric. The Initial Developer of the Original
 Code is Eric Busboom
  The original code is icalcomponent.h

======================================================================*/

#ifndef ICALCOMPONENT_H
#define ICALCOMPONENT_H

#include <libicalss/icalproperty.h>
#include <libicalss/icalvalue.h>
#include <libicalss/icalenums.h> /* defines icalcomponent_kind */

typedef void icalcomponent;

icalcomponent* icalcomponent_new(icalcomponent_kind kind);
icalcomponent* icalcomponent_new_clone(icalcomponent* component);
icalcomponent* icalcomponent_new_from_string(char* str);
icalcomponent* icalcomponent_vanew(icalcomponent_kind kind, ...);
void icalcomponent_free(icalcomponent* component);

char* icalcomponent_as_ical_string(icalcomponent* component);

int icalcomponent_is_valid(icalcomponent* component);

icalcomponent_kind icalcomponent_isa(icalcomponent* component);

int icalcomponent_isa_component (void* component);

/* 
 * Working with properties
 */

void icalcomponent_add_property(icalcomponent* component,
				icalproperty* property);

void icalcomponent_remove_property(icalcomponent* component,
				   icalproperty* property);

int icalcomponent_count_properties(icalcomponent* component,
				   icalproperty_kind kind);

/* Iterate through the properties */
icalproperty* icalcomponent_get_current_property(icalcomponent* component);

icalproperty* icalcomponent_get_first_property(icalcomponent* component,
					      icalproperty_kind kind);
icalproperty* icalcomponent_get_next_property(icalcomponent* component,
					      icalproperty_kind kind);

/* Return a null-terminated array of icalproperties*/

icalproperty** icalcomponent_get_properties(icalcomponent* component,
					      icalproperty_kind kind);


/* 
 * Working with components
 */ 


void icalcomponent_add_component(icalcomponent* parent,
				icalcomponent* child);

void icalcomponent_remove_component(icalcomponent* parent,
				icalcomponent* child);

int icalcomponent_count_components(icalcomponent* component,
				   icalcomponent_kind kind);

/* Iterate through components */
icalcomponent* icalcomponent_get_current_component (icalcomponent* component);

icalcomponent* icalcomponent_get_first_component(icalcomponent* component,
					      icalcomponent_kind kind);
icalcomponent* icalcomponent_get_next_component(icalcomponent* component,
					      icalcomponent_kind kind);

/* Return a null-terminated array of icalproperties*/
icalproperty** icalcomponent_get_component(icalcomponent* component,
					      icalproperty_kind kind);

/* Working with embedded error properties */

int icalcomponent_count_errors(icalcomponent* component);
void icalcomponent_strip_errors(icalcomponent* component);


/* Internal operations. You don't see these... */
icalcomponent* icalcomponent_get_parent(icalcomponent* component);
void icalcomponent_set_parent(icalcomponent* component, 
			      icalcomponent* parent);

#endif /* !ICALCOMPONENT_H */



