/* Copyright (C) 2001-2006 Artifex Software, Inc.
   All Rights Reserved.
  
  This file is part of GNU ghostscript

  GNU ghostscript is free software; you can redistribute it and/or
  modify it under the terms of the version 2 of the GNU General Public
  License as published by the Free Software Foundation.

  GNU ghostscript is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with
  ghostscript; see the file COPYING. If not, write to the Free Software Foundation,
  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

*/

/* $Id: isdata.h,v 1.7 2007/09/11 15:24:07 Arabidopsis Exp $ */
/* Structure for expandable stacks of refs */
/* Requires iref.h */

#ifndef isdata_INCLUDED
#  define isdata_INCLUDED

/*
 * In order to detect under- and overflow with minimum overhead, we put
 * guard elements at the top and bottom of each stack block (see idsdata.h,
 * iesdata.h, and iosdata.h for details of the individual stacks).  Note that
 * the 'current' and 'next' arrays include the guard elements.  See
 * istack.h for the details of stack blocks.
 */

/*
 * The garbage collector requires that the entire contents of every block
 * be 'clean', i.e., contain legitimate refs; we also need to ensure that
 * at GC time, pointers in unused areas of a block will not be followed
 * (since they may be dangling).  We ensure this as follows:
 *      - When allocating a new block, we set the entire body to nulls.
 *      This is necessary because the block may be freed before the next GC,
 *      and the GC must be able to scan (parse) refs even if they are free.
 *      - When adding a new block to the top of the stack, we set to nulls
 *      the unused area of the new next-to-top blocks.
 *      - At the beginning of garbage collection, we set to nulls the unused
 *      elements of the top block.
 */

/*
 * Define pointers into stacks.  Formerly, these were short (unsegmented)
 * pointers, but this distinction is no longer needed.
 */
typedef ref *s_ptr;
typedef const ref *const_s_ptr;

/* Define an opaque allocator type. */
#ifndef gs_ref_memory_DEFINED
#  define gs_ref_memory_DEFINED
typedef struct gs_ref_memory_s gs_ref_memory_t;
#endif

/*
 * Define the state of a stack, other than the data it holds.
 * Note that the total size of a stack cannot exceed max_uint,
 * because it has to be possible to copy a stack to a PostScript array.
 */
#ifndef ref_stack_DEFINED
typedef struct ref_stack_s ref_stack_t;	/* also defined in idebug.h */
#  define ref_stack_DEFINED
#endif
/*
 * We divide the stack structure into two parts: ref_stack_params_t, which
 * is set when the stack is created and (almost) never changed after that,
 * and ref_stack_t, which changes dynamically.
 */
typedef struct ref_stack_params_s ref_stack_params_t;
struct ref_stack_s {
    /* Following are updated dynamically. */
    s_ptr p;			/* current top element */
    /* Following are updated when adding or deleting blocks. */
    s_ptr bot;			/* bottommost valid element */
    s_ptr top;			/* topmost valid element = */
				/* bot + data_size */
    ref current;		/* t_array for current top block */
    uint extension_size;	/* total sizes of extn. blocks */
    uint extension_used;	/* total used sizes of extn. blocks */
    /* Following are updated rarely. */
    ref max_stack;		/* t_integer, Max...Stack user param */
    uint requested;		/* amount of last failing */
				/* push or pop request */
    uint margin;		/* # of slots to leave between limit */
				/* and top */
    uint body_size;		/* data_size - margin */
    /* Following are set only at initialization. */
    ref_stack_params_t *params;
    gs_ref_memory_t *memory;	/* allocator for params and blocks */
};
#define public_st_ref_stack()	/* in istack.c */\
  gs_public_st_complex_only(st_ref_stack, ref_stack_t, "ref_stack_t",\
    ref_stack_clear_marks, ref_stack_enum_ptrs, ref_stack_reloc_ptrs, 0)
#define st_ref_stack_num_ptrs 2	/* current, params */

#endif /* isdata_INCLUDED */