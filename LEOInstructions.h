/*
 *  LEOInstructions.h
 *  Leonie
 *
 *  Created by Uli Kusterer on 17.09.10.
 *  Copyright 2010 Uli Kusterer. All rights reserved.
 *
 */

#ifndef LEO_INSTRUCTIONS_H
#define LEO_INSTRUCTIONS_H		1

#include "LEOInterpreter.h"
#include <sys/types.h>


#define	ABNORMAL_EXIT_INSTR				0
#define	EXIT_TO_SHELL_INSTR				1
#define	NO_OP_INSTR						2
#define	PUSH_STR_FROM_TABLE_INSTR		3
#define	PRINT_VALUE_INSTR				4
#define	POP_VALUE_INSTR					5



extern LEOInstructionFuncPtr	gInstructions[];
extern size_t					gNumInstructions;



#endif // LEO_INSTRUCTIONS_H
