/**
 * @file CommandInt.h
 *
 * @author Hans Dulimarta [dulimart@computer.org]
 *
 * @brief Command Interpreter with run-time installed executable
 *    module.
 */
/* 
 * $Id: CommandInt.h,v 2.8 2001/02/21 16:19:23 dulimart Exp $
 *
 * $Log: CommandInt.h,v $
 * Revision 2.8  2001/02/21 16:19:23  dulimart
 * Removed dependency to Robot class.
 *
 * Revision 2.7  2000/10/13 15:41:54  dulimart
 * Modified comment lines to doxygen enabled ones.
 *
 *
 * Copyright (c) 2000  Hans Dulimarta, Ph.D.
 */
#ifndef COMMAND_INT_H
#define COMMAND_INT_H

#include "stream.h"
#include "Command.h"

/**
   Module is an abstraction of callable function used by
   a CommandInterpreter.
*/
class Module {
   public:
      /**
         Construct a callable module
         @param ptr pointer to function that will activate this module
         @param desc textual description of this module
         @param istate a string of number indicating the states in which
         this module can be called
         @param fstate is the new state to be set in the CommandInterpreter
         when this module finish execution. A value of -1 indicates that
         this module does not change the current state
         @param key the character to be used to activate this module from 
         a key press
       */
      Module (Command *ptr, const char *desc, 
            const char* istate, int fstate, char key);

      /// destructor

      ~Module();

      /// pointer to function
      Command* module;

      /// initial states encoded as set of bit
      int initstates;

      /// final state to set when this module finished execution
      int finalstate;

      /// menu description of this module
      char *menu;

      /// key press which will activate this module
      char key;
   protected:
      /// copy constructor is not available to public
      Module (const Module&);
};


/**
   CommandInt is an abstraction of a command interpreter. To handle
   different context of operation, a finite state machine is created.
   It determines the currect set of callable modules based on the
   current state, reads key presses given by a user, call the module 
   associated with the key press and set a new state.
*/ 
class CommandInt {
   public:
      /**
         User-defined constructor
         @param r pointer to Robot object
         @param istr pointer to the input stream where the commands
         are to be read from
         @param ostr pointer to the output stream where the output
         are to be sent to
      */
      CommandInt(istream* istr, ostream* ostr);

      /// Destructor
      ~CommandInt();

      /// Get the current state of the FSM
      int GetState () const { return state; }

      /** 
         Set the state of the FSM
         @param s the new state to set
      */
      void SetState (int s) { state = s; }

      /**
         Add a callable module to the interpreter
         @param m pointer to the callable module
      */
      void AddModule (Module *m);

      /// execute a loop for processing all the commands
      void CmdLoop();

      /**
         Set the input stream
         @param s pointer to the new input stream
      */
      void SetIStream (istream *s) { cmd_src = s; }

      /**
         Set the output stream
         @param s pointer to the new output stream
      */
      void SetOStream (ostream *s) { out_dst = s; }

      /// Run the command
      int DoCmd();
   private:

      /**
         Check if the current state is in the set of initial state 
         of a module.
         @param s set of initial states of a module
      */
      bool Accepted (int s) const;

      /**
         Read command from input device Terminal/Receiver
      */
      void GetCmd();

      Module **modlist;
      Module* active_module;
      int state;
      istream* cmd_src;
      ostream* out_dst;
};
#endif
