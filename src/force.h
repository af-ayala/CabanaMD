#ifndef FORCE_H
#define FORCE_H
#include<types.h>
#include<system.h>
#include<binning.h>
#include<neighbor.h>

class Force {
public:
  bool half_neigh, comm_newton;
  Force(char** args, System* system, bool half_neigh_);

  virtual void init_coeff(int nargs, char** args);

  virtual void compute(System* system, Binning* binning, Neighbor* neigh);

  virtual const char* name();
};

#include<modules_force.h>
#endif
