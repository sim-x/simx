#include <ctype.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "kernel/universe.h"
#include "ssf.h"

namespace minissf {

int Universe::args_nmachs;
int Universe::args_rank;
int* Universe::args_mach_nprocs;
int Universe::args_nprocs; // for current machine
VirtualTime Universe::args_global_thresh;
VirtualTime Universe::args_local_thresh; // for current machine
VirtualTime Universe::args_training_len;
bool Universe::args_global_thresh_set;
bool Universe::args_local_thresh_set;
int Universe::args_seed;
int Universe::args_naligns;
unsigned int Universe::args_debug_mask;
STRING Universe::args_outfile;
VirtualTime Universe::args_progress_interval;
VirtualTime Universe::args_endtime;
double Universe::args_speedup;

int Universe::total_num_procs = 0;

Universe::CommandLineOptionStruct Universe::command_option_array[] = {
  { Universe::OPTION_ALIGN, "-a",
    "-a <A> : set max number of alignments per processor (by default, A=1; 0=unbounded)" },
  { Universe::OPTION_DEBUG, "-d",
    "-d <D> : set debug level (by default, D=1; 0=silent,1=brief,2=report)" },
  { Universe::OPTION_INTV, "-i",
    "-i <I> : show simulation progress at given time interval" },
  { Universe::OPTION_NPROCS, "-n",
    "-n <N> : set number of processors on all machines (by default, N=1)" },
  { Universe::OPTION_SET_NPROCS, "--set-nprocs",
    "--set-nprocs <M> <N> : set number of processors on machine M to be N specifically" },
  { Universe::OPTION_OFILE, "-o",
    "-o <F> : set output file (by default, using standard output)" },
  { Universe::OPTION_SEED, "-s",
    "-s <S> : set global random seed (set S=0 to get seed from clock)" },
  { Universe::OPTION_GT, "-T",
    "-T <T> : manually set global synchronization threshold" },
  { Universe::OPTION_LT, "-t",
    "-t <t> : manually set local synchronization threshold on all machines" },
  { Universe::OPTION_SET_LOCAL_THRESH, "--set-local-thresh",
    "--set-local-thresh <M> <t> : manually set local synchronization threshold on machine M" },
  { Universe::OPTION_SET_TRAINING_LEN, "--set-training-len",
    "--set-training-len <L> : set min threshold training duration (default is 5% of simulation time)" },
  { Universe::OPTION_ENDOFOPT, "--",
    "-- : end of parsing minissf command-line (after which user options may start without conflicts)" },
  { Universe::OPTION_NONE, 0, "" }
};

#define OPTCHECK(cond, complaint) \
  { if(!(cond)) { SSF_THROW("bad command-line argument: " << p->str << ": " << complaint); return 1; }}
static bool ISINT(char* str) { for(char* myp=str; *myp; myp++) if(!isdigit(*myp)) return false; return true; }
//static bool ISXINT(char* str) { if((*str++ == '0') && (*str++ == 'x')) 
//  for(char* myp=str; *myp; myp++) if(!isxdigit(*myp)) return false; return true; }
int Universe::parse_command_line(int& argc, char**& argv)
{
  int i, offset = 1;
  int a_n = 1, a_s = 54321, a_d = 1;
  MAP(int,int) a_m;
  STRING a_f;
  VirtualTime a_i = 0;
  VirtualTime a_T = VirtualTime::INFINITY, a_t = VirtualTime::INFINITY;
  VirtualTime a_l = 0;
  bool set_a_T = false, set_a_t = false;
  MAP(int,VirtualTime) a_u;
  int a_a = 1;

  for(i=1; i<argc; i++) {
    CommandLineOptionStruct* p;
    for(p=command_option_array; p->str; p++)
      if(strcmp(argv[i], p->str) == 0) break;
    switch(p->opt) {
    case OPTION_NPROCS: {
      ++i;
      OPTCHECK(i<argc, "argument missing");
      OPTCHECK(ISINT(argv[i]), "invalid argument");
      a_n = atoi(argv[i]);
      OPTCHECK(a_n>0, "invalid number of processors");
      break;
    }
    case OPTION_SET_NPROCS: {
      ++i;
      OPTCHECK(i<argc, "argument missing");
      OPTCHECK(ISINT(argv[i]), "invalid argument");
      int m = atoi(argv[i]);
      OPTCHECK(m>=0, "invalid machine id");
      ++i;
      OPTCHECK(i<argc, "argument missing");
      OPTCHECK(ISINT(argv[i]), "invalid argument");
      int n = atoi(argv[i]);
      OPTCHECK(n>0, "invalid number of processors");
      OPTCHECK(a_m.insert(MAKE_PAIR(m,n)).second, "duplicate setting");
      break;
    }
    case OPTION_SEED: {
      ++i;
      OPTCHECK(i<argc, "argument missing");
      OPTCHECK(ISINT(argv[i]), "invalid argument");
      a_s = atoi(argv[i]);
      break;
    }
    case OPTION_OFILE: {
      ++i;
      OPTCHECK(i<argc, "argument missing");
      a_f = argv[i];
      break;
    }
    case OPTION_INTV: {
      ++i;
      OPTCHECK(i<argc, "argument missing");
      a_i.fromString(argv[i]);
      OPTCHECK(a_i>0, "invalid progress interval");
      break;
    }
    case OPTION_GT: {
      ++i;
      OPTCHECK(i<argc, "argument missing");
      a_T.fromString(argv[i]);
      //OPTCHECK(a_T>=0, "invalid global thresh");
      if(a_T < 0) a_T = VirtualTime::INFINITY;
      set_a_T = true;
      break;
    }
    case OPTION_LT: {
      ++i;
      OPTCHECK(i<argc, "argument missing");
      a_t.fromString(argv[i]);
      //OPTCHECK(a_t>=0, "invalid local thresh");
      if(a_t < 0) a_t = VirtualTime::INFINITY;
      set_a_t = true;
      break;
    }
    case OPTION_SET_LOCAL_THRESH: {
      ++i;
      OPTCHECK(i<argc, "argument missing");
      OPTCHECK(ISINT(argv[i]), "invalid argument");
      int m = atoi(argv[i]);
      OPTCHECK(m>=0, "invalid machine id");
      ++i;
      VirtualTime t;
      t.fromString(argv[i]);
      OPTCHECK(t>=0, "invalid local thresh");
      if(t < 0) t = VirtualTime::INFINITY;
      OPTCHECK(a_u.insert(MAKE_PAIR(m,t)).second, "duplicate setting");
      break;
    }
    case OPTION_SET_TRAINING_LEN: {
      ++i;
      OPTCHECK(i<argc, "argument missing");
      a_l.fromString(argv[i]);
      OPTCHECK(a_l>0, "invalid training length");
      break;
    }
    case OPTION_DEBUG: {
      ++i;
      OPTCHECK(i<argc, "argument missing");
      OPTCHECK(ISINT(argv[i]), "invalid argument");
      a_d = atoi(argv[i]);
      OPTCHECK(a_d>=0, "invalid debug level");
      break;
    }
    case OPTION_ALIGN: {
      ++i;
      OPTCHECK(i<argc, "argument missing");
      OPTCHECK(ISINT(argv[i]), "invalid argument");
      a_a = atoi(argv[i]);
      OPTCHECK(a_a>=0, "invalid auto-alignment factor");
      break;
    }
    case OPTION_ENDOFOPT: {
      ++i;
      goto stop;
    }
    default: {
      argv[offset++] = argv[i];
      break;
    }}
  }
    
 stop:
  // arrange the rest (unparsed) arguments
  for(int j=i; j<argc; j++)
    argv[offset++] = argv[j];
  if ( argc > 0 && argv != NULL )
    {
      argv[offset] = 0;
      argc = offset;
    }
#ifdef HAVE_MPI_H
  ssf_mpi_comm_size(MPI_COMM_WORLD, &args_nmachs);
  ssf_mpi_comm_rank(MPI_COMM_WORLD, &args_rank);
#else
  args_nmachs = 1; args_rank = 0;
#endif
  args_mach_nprocs = new int[args_nmachs]; 
  assert(args_mach_nprocs);
  for(int k=0; k<args_nmachs; k++) args_mach_nprocs[k] = a_n;
  for(MAP(int,int)::iterator iter = a_m.begin();
      iter != a_m.end(); iter++) {
    if((*iter).first >= args_nmachs) 
      SSF_THROW("bad command-line argument: machine index out of range");
    args_mach_nprocs[(*iter).first] = (*iter).second;
  }
  a_m.clear();
  args_nprocs = args_mach_nprocs[args_rank];

  args_global_thresh = a_T;
  args_global_thresh_set = set_a_T;
  args_local_thresh = a_t;
  args_local_thresh_set = set_a_t;
  args_training_len = a_l;
  for(MAP(int,VirtualTime)::iterator iter_t = a_u.begin();
      iter_t != a_u.end(); iter_t++) {
    if((*iter_t).first >= args_nmachs) 
      SSF_THROW("bad command-line argument: machine index out of range");
    if((*iter_t).first == args_rank) {
      args_local_thresh = (*iter_t).second;
      args_local_thresh_set = true;
    }
  }
  a_u.clear();

  if(!a_s) args_seed = Random::make_new_seed();
  else args_seed = a_s;

  if(a_a <= 0) args_naligns = a_a;
  else args_naligns = a_a*args_nprocs;

  args_progress_interval = a_i;
  args_outfile = a_f;

  if(!args_outfile.empty()) {
    std::stringstream ss(std::stringstream::in | std::stringstream::out);
    ss << args_outfile;
    if(args_nmachs > 1) ss << "-" << args_rank;
    int outf = creat(ss.str().c_str(), 0600);
    if(outf < 0) SSF_THROW("can't open output file: " << args_outfile);
    close(STDOUT_FILENO);
    if(-1 == dup(outf)) SSF_THROW("can't duplicate file descriptor");
    close(outf);
  }

  if(a_d == 0) args_debug_mask = DEBUG_FLAG_SILENT;
  else if(a_d == 1) args_debug_mask = DEBUG_FLAG_BRIEF;
  else if(a_d == 2) args_debug_mask = DEBUG_FLAG_BRIEF|DEBUG_FLAG_REPORT;
  else if(a_d == 3) args_debug_mask = DEBUG_FLAG_BRIEF|DEBUG_FLAG_REPORT|DEBUG_FLAG_LPSCHED;
  else if(a_d == 4) args_debug_mask = DEBUG_FLAG_BRIEF|DEBUG_FLAG_REPORT|DEBUG_FLAG_TMSCHED;
  else if(a_d == 5) args_debug_mask = DEBUG_FLAG_BRIEF|DEBUG_FLAG_REPORT|DEBUG_FLAG_LPSCHED|DEBUG_FLAG_TMSCHED;
  else if(a_d == 6) args_debug_mask = DEBUG_FLAG_BRIEF|DEBUG_FLAG_REPORT|DEBUG_FLAG_MPIMSG;
  else if(a_d == 7) args_debug_mask = 0xff; // everything
  else SSF_THROW("unknown debug level");

  return 0;
}

void Universe::print_command_line(FILE* outfile)
{
  fprintf(outfile, "SSF command-line options:\n");
  for(CommandLineOptionStruct* p = command_option_array; p->str; p++)
    fprintf(outfile, "  %s\n", p->info);
}

}; /*namespace minissf*/

/*
 * Copyright (c) 2011-2013 Florida International University.
 *
 * Permission is hereby granted, free of charge, to any individual or
 * institution obtaining a copy of this software and associated
 * documentation files (the "software"), to use, copy, modify, and
 * distribute without restriction.
 *
 * The software is provided "as is", without warranty of any kind,
 * express or implied, including but not limited to the warranties of
 * merchantability, fitness for a particular purpose and
 * noninfringement.  In no event shall Florida International
 * University be liable for any claim, damages or other liability,
 * whether in an action of contract, tort or otherwise, arising from,
 * out of or in connection with the software or the use or other
 * dealings in the software.
 *
 * This software is developed and maintained by
 *
 *   Modeling and Networking Systems Research Group
 *   School of Computing and Information Sciences
 *   Florida International University
 *   Miami, Florida 33199, USA
 *
 * You can find our research at http://www.primessf.net/.
 */
