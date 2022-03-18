// $Id: CommandInt.cpp,v 2.6 2001/02/21 16:19:23 dulimart Exp $

/*
 * Filename: CommandInt.cpp
 * Author: Hans Dulimarta [dulimart@computer.org]
 *
 * $Log: CommandInt.cpp,v $
 * Revision 2.6  2001/02/21 16:19:23  dulimart
 * Removed dependency to Robot class.
 *
 * Revision 2.5  2000/10/02 20:29:20  dulimart
 * Modified the warning message to display the string in modmap
 *
 * Revision 2.4  2000/09/26 18:04:13  dulimart
 * Replaced set of integers (for initial states) with "set" of bit.
 * Added select() in GetCmd() to enable reading commands from
 * different sources
 *
 * Revision 2.3  2000/07/11 13:47:45  dulimart
 * Added istream for command source and ostream for command outputs.
 *
 * Revision 2.2  2000/05/23 16:59:31  dulimart
 * Simplified DoCmd, print menu from GetCmd, Modified AddModule to use vector
 *
 * Revision 2.1  2000/04/24 19:19:48  dulimart
 * Add "help"
 *
 *
 * Copyright (c) 2000   Hans Dulimarta, Ph.D.
 */
#include <ctype.h> 
#include <string.h> 
#include <stdlib.h> 
#include "CommandInt.h"

#include "io.h"
#include "mios.h"

#define MAX_MODULE 20

CommandInt::CommandInt(istream* s, ostream* t) {
   int k;

   state = 0;
   cmd_src = s;
   out_dst = t;
   modlist = new (Module*) [MAX_MODULE];
   for (k=0; k<MAX_MODULE; k++)
      modlist[k] = 0;
}

CommandInt::~CommandInt() {
   delete [] modlist;
}

Module::Module (Command* m, const char *mdesc, 
      const char* is, int fs, char k)
   : module (m), finalstate(fs), key(k)
{
   char *p, *is_str;
   int s;

   is_str = new char [strlen(is)+1];
   strcpy (is_str, is);
   p = strtok (is_str, " ");
   initstates = 0;
   while (p) {
      /* 
       * WARNING: 
       * this method works only for non-neg state in the range 0..31 
       */
      s = atoi (p);
      initstates |= (1 << s);
      p = strtok (0, " ");
   }
   delete [] is_str;

   if (mdesc) {
      s = strlen(mdesc);
      menu = new char [s+1];
      strcpy (menu, mdesc);
   }
   else
      menu = 0;
}

Module::Module (const Module& m) {
   // this should not be called
}

Module::~Module () {
   delete [] menu;
}

void CommandInt::AddModule (Module *x) {
   static int n = 0;

   if (n < MAX_MODULE) {
      modlist[n++] = x;
   }
}

void CommandInt::GetCmd() {
   char com_ch;
   int k;
   Module* p, *mod_map[128];
   fd_set my_set;

   for (k=0; k<128; k++)
      mod_map [k] = 0;
   fputc ('\n', out_dst->fileno());
   for (k = 0; k < MAX_MODULE && modlist[k]; k++) {
      p = modlist[k];
      if (Accepted (p->initstates)) {
#ifdef DEBUG
         if (mod_map [p->key]) {
            my_fprintf (STDERR, "WARNING: Key '%c' has been assign to '%s'\n",
                  p->key, mod_map[p->key]->menu);
         }
#endif
         mod_map [p->key] = p;
      }
   }
   for (k=0; k<128; k++) {
      if (mod_map[k]) {
         p = mod_map[k];
         if (p->menu)
            my_fprintf (out_dst->fileno(), "[%c]  %s\n", p->key, p->menu);
      }
   }
   cerr << "State=" << state << " > ";
   active_module = 0;
   while (1) {
      FD_ZERO (&my_set);
      FD_SET (STDIN, &my_set);
      FD_SET (RECVER, &my_set);
      select (5, &my_set, (struct timeval*) 0);
      for (k = 0; k < 5; k++)
         if (FD_ISSET (k, &my_set)) {
            stdin = (FILE) k;
            break;
         }

      com_ch = toupper(fgetc(cmd_src->fileno()));
      //my_printf ("%s: com_ch = %c\n", __PRETTY_FUNCTION__, com_ch);
      if (mod_map [com_ch]) {
         active_module = mod_map[com_ch];
         break;
      }
   }
}

bool CommandInt::Accepted (int s) const {
   if (s == 0)
      return 1;
   else {
      return s & (1 << state);
   }
}

void CommandInt::CmdLoop() {
   int quit = 0;

   while (!quit) {
      GetCmd();
      quit = DoCmd ();
   }
}

int CommandInt::DoCmd () {
   int quit, ns;

   quit = 0;
   if (active_module->module) {
      // module definition is not empty
      ns = active_module->module->Execute(state);
      if (active_module->finalstate != -1)
         state = active_module->finalstate;
      else if (ns != -1) {
         state = ns;
      }
   }
   else {
      // no modules is defined for this command, quit
      quit = 1;
   }
   return quit;
}
