/*
 *  LEOScript.h
 *  Leonie
 *
 *  Created by Uli Kusterer on 16.10.10.
 *  Copyright 2010 Uli Kusterer. All rights reserved.
 *
 */

#ifndef LEO_SCRIPT_H
#define	LEO_SCRIPT_H		1

/*!
	@header LEOScript
	A script is a reference-counted data type that collects all the methods
	(functions and commands) of an object. After you create a script and
	populate it with handlers, it is intended to be immutable. If you want to
	remove a handler or otherwise change a script afterwards, create a new
	script.
	
	This way, if a script is changed, any still executing instances of that
	script can finish on their own. This is especially needed for scripts that
	modify themselves.
	
	For similar reasons, we reference the owning object using a LEOObjectID and
	LEOObjectSeed, so a script can delete the object owning it and still finish
	executing.
*/

// -----------------------------------------------------------------------------
//	Headers:
// -----------------------------------------------------------------------------

#include "LEOInterpreter.h"


// -----------------------------------------------------------------------------
/*!	Every method is represented by a struct like this:
	@field handlerName		The name of this handler. Case INsensitive.
	@field numInstructions	The number of instructions in the instructions
							array.
	@field instructions		An array that holds the instructions for this
							handler. */
// -----------------------------------------------------------------------------

typedef struct LEOHandler
{
	char			handlerName[256];
	size_t			numInstructions;
	LEOInstruction	*instructions;
} LEOHandler;


// -----------------------------------------------------------------------------
/*!	Every object has a script, which is a grouping of functions and commands:
	@field referenceCount		The number of owners this script currently has.
								Owners are e.g. either contexts running the
								script or the object the script belongs to.
	@field ownerObject			The object that this script belongs to, i.e. the
								'me' or 'self'.
	@field ownerObjectSeed		The 'seed' for the ownerObject slot, for the
								case where the owner has been released and the
								slot re-used.
	@field numFunctions			The number of function handler entries in the
								functions array.
	@field functions			An array of handlers implementing the functions
								this script implements.
	@field numCommands			The number of command handler entries in the
								commands array.
	@field commands				An array of handlers implementing the commands
								this script implements. */
// -----------------------------------------------------------------------------

typedef struct LEOScript
{
	size_t				referenceCount;
	LEOObjectID			ownerObject;		// Deletion-safe reference to owning object.
	LEOObjectSeed		ownerObjectSeed;
	size_t				numFunctions;
	LEOHandler			*functions;
	size_t				numCommands;
	LEOHandler			*commands;
} LEOScript;



/*!
	Creates a script referencing the given owner. The LEOScript* is reference-
	counted and its reference count is set to 1, so when you're done with it,
	you can release it. When a script is executing, it should usually be retained
	so that deleting its owner doesn't pull out the rug from under the interpreter.
	@seealso //leo_ref/c/func/LEOScriptRetain LEOScriptRetain
	@seealso //leo_ref/c/func/LEOScriptRelease LEOScriptRelease
*/
LEOScript*	LEOScriptCreateForOwner( LEOObjectID ownerObject, LEOObjectSeed ownerSeed );	// Gives referenceCount of 1.

/*!
	Acquire ownership of the given script, so that when the current owner releases
	it the object still stays around for your use. Increases the reference count
	by 1.
	@result Returns inScript so you can assign it to a variable in a struct,
			or a global, or whatever makes sense.
	@seealso //leo_ref/c/func/LEOScriptCreateForOwner LEOScriptCreateForOwner
	@seealso //leo_ref/c/func/LEOScriptRelease LEOScriptRelease
*/
LEOScript*	LEOScriptRetain( LEOScript* inScript );		// Adds 1 to referenceCount. Returns inScript.

/*!
	Give up ownership of the given script. You acquire ownership by either
	creating the script, or by retaining it. Giving up ownership decreases its
	reference count by 1. When the last owner releases the object (and the
	reference count reaches 0), the script is freed.
	@seealso //leo_ref/c/func/LEOScriptCreateForOwner LEOScriptCreateForOwner
	@seealso //leo_ref/c/func/LEOScriptRetain LEOScriptRetain
*/
void		LEOScriptRelease( LEOScript* inScript );	// Subtracts 1 from referenceCount. If it hits 0, disposes of inScript.

/*!
	Create a new command handler with the given name and return a pointer to it.
	Use this only when initially setting up a script and parsing/compiling
	bytecode into it. Whenever you add a new handler to a script, all pointers
	to existing LEOHandlers in the script may be invalidated, including the ones
	returned by this call.
	
	Note that handler names are case-INsensitive.
	@seealso //leo_ref/c/func/LEOScriptAddFunctionHandlerNamed LEOScriptAddFunctionHandlerNamed
	@seealso //leo_ref/c/func/LEOHandlerAddInstruction LEOHandlerAddInstruction
	@seealso //leo_ref/c/func/LEOScriptFindCommandHandlerNamed LEOScriptFindCommandHandlerNamed
*/
LEOHandler*	LEOScriptAddCommandHandlerNamed( LEOScript* inScript, const char* inName );	// Invalidates all LEOHandler pointers anyone may have into this script.

/*!
	Create a new function handler with the given name and return a pointer to it.
	Use this only when initially setting up a script and parsing/compiling
	bytecode into it. Whenever you add a new handler to a script, all pointers
	to existing LEOHandlers in the script may be invalidated, including the ones
	returned by this call.
	
	Note that handler names are case-INsensitive.
	@seealso //leo_ref/c/func/LEOScriptAddCommandHandlerNamed LEOScriptAddCommandHandlerNamed
	@seealso //leo_ref/c/func/LEOHandlerAddInstruction LEOHandlerAddInstruction
	@seealso //leo_ref/c/func/LEOScriptFindFunctionHandlerNamed LEOScriptFindFunctionHandlerNamed
*/
LEOHandler*	LEOScriptAddFunctionHandlerNamed( LEOScript* inScript, const char* inName );	// Invalidates all LEOHandler pointers anyone may have into this script.

/*!
	Return a pointer to a command handler with the given name.
	
	Note that handler names are case-INsensitive.
	@seealso //leo_ref/c/func/LEOScriptFindFunctionHandlerNamed LEOScriptFindFunctionHandlerNamed
	@seealso //leo_ref/c/func/LEOScriptAddCommandHandlerNamed LEOScriptAddCommandHandlerNamed
*/
LEOHandler*	LEOScriptFindCommandHandlerNamed( LEOScript* inScript, const char* inName );

/*!
	Return a pointer to a function handler with the given name.
	
	Note that handler names are case-INsensitive.
	@seealso //leo_ref/c/func/LEOScriptFindCommandHandlerNamed LEOScriptFindCommandHandlerNamed
	@seealso //leo_ref/c/func/LEOScriptAddFunctionHandlerNamed LEOScriptAddFunctionHandlerNamed
*/
LEOHandler*	LEOScriptFindFunctionHandlerNamed( LEOScript* inScript, const char* inName );

/*!
	Add a copy of the given method to the given handler.
	Use this only when initially setting up a script and parsing/compiling
	bytecode into it. Pointers to instructions inside a handler may not stay
	valid after a call to this.
	@seealso //leo_ref/c/func/LEOScriptAddCommandHandlerNamed LEOScriptAddCommandHandlerNamed
	@seealso //leo_ref/c/func/LEOScriptAddFunctionHandlerNamed LEOScriptAddFunctionHandlerNamed
*/
void	LEOHandlerAddInstruction( LEOHandler* inHandler, LEOInstruction instr );


#endif // LEO_SCRIPT_H
