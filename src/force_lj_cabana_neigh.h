/****************************************************************************
 * Copyright (c) 2018-2019 by the Cabana authors                            *
 * All rights reserved.                                                     *
 *                                                                          *
 * This file is part of the Cabana library. Cabana is distributed under a   *
 * BSD 3-clause license. For the licensing terms see the LICENSE file in    *
 * the top-level directory.                                                 *
 *                                                                          *
 * SPDX-License-Identifier: BSD-3-Clause                                    *
 ****************************************************************************/

//************************************************************************
//  ExaMiniMD v. 1.0
//  Copyright (2018) National Technology & Engineering Solutions of Sandia,
//  LLC (NTESS).
//
//  Under the terms of Contract DE-NA-0003525 with NTESS, the U.S. Government
//  retains certain rights in this software.
//
//  ExaMiniMD is licensed under 3-clause BSD terms of use: Redistribution and
//  use in source and binary forms, with or without modification, are
//  permitted provided that the following conditions are met:
//
//    1. Redistributions of source code must retain the above copyright notice,
//       this list of conditions and the following disclaimer.
//
//    2. Redistributions in binary form must reproduce the above copyright notice,
//       this list of conditions and the following disclaimer in the documentation
//       and/or other materials provided with the distribution.
//
//    3. Neither the name of the Corporation nor the names of the contributors
//       may be used to endorse or promote products derived from this software
//       without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY NTESS "AS IS" AND ANY EXPRESS OR IMPLIED
//  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
//  IN NO EVENT SHALL NTESS OR THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
//  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
//  IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
//  Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//************************************************************************

#ifndef FORCE_LJ_CABANA_NEIGH_H
#define FORCE_LJ_CABANA_NEIGH_H
#include<vector>
#include<types.h>
#include<system.h>

class Force {
private:
  int N_local,ntypes;
  typename AoSoA::member_slice_type<Positions> x;
  typename AoSoA::member_slice_type<Forces> f;
  typename AoSoA::member_slice_type<Forces>::atomic_access_slice f_a;
  typename AoSoA::member_slice_type<IDs> id;
  typename AoSoA::member_slice_type<Types> type;

  int step;

  t_verletlist_full init_fullneigh_list();
  t_verletlist_half init_halfneigh_list();

  typedef Kokkos::View<T_F_FLOAT**> t_fparams;
  typedef Kokkos::View<const T_F_FLOAT**,
      Kokkos::MemoryTraits<Kokkos::RandomAccess>> t_fparams_rnd;
  t_fparams lj1,lj2,cutsq;
  t_fparams_rnd rnd_lj1,rnd_lj2,rnd_cutsq;

  T_F_FLOAT stack_lj1[MAX_TYPES_STACKPARAMS+1][MAX_TYPES_STACKPARAMS+1]; // hardwired space for 12 atom types
  T_F_FLOAT stack_lj2[MAX_TYPES_STACKPARAMS+1][MAX_TYPES_STACKPARAMS+1];
  T_F_FLOAT stack_cutsq[MAX_TYPES_STACKPARAMS+1][MAX_TYPES_STACKPARAMS+1];

public:
  typedef T_V_FLOAT value_type;

  struct TagFullNeigh {};

  struct TagHalfNeigh {};

  struct TagFullNeighPE {};

  struct TagHalfNeighPE {};

  typedef Kokkos::RangePolicy<TagFullNeigh,Kokkos::IndexType<T_INT> > t_policy_full_neigh_stackparams;
  typedef Kokkos::RangePolicy<TagHalfNeigh,Kokkos::IndexType<T_INT> > t_policy_half_neigh_stackparams;
  typedef Kokkos::RangePolicy<TagFullNeighPE,Kokkos::IndexType<T_INT> > t_policy_full_neigh_pe_stackparams;
  typedef Kokkos::RangePolicy<TagHalfNeighPE,Kokkos::IndexType<T_INT> > t_policy_half_neigh_pe_stackparams;

  bool half_neigh, comm_newton;
  T_X_FLOAT neigh_cut;

  t_verletlist_full neigh_list_full;
  t_verletlist_half neigh_list_half;

  Force(System* system, bool half_neigh_);

  void init_coeff(T_X_FLOAT neigh_cut, std::vector<int> force_types, std::vector<double> force_coeff);

  void create_neigh_list(System* system);

  void compute(System* system);
  T_F_FLOAT compute_energy(System* system);

  KOKKOS_INLINE_FUNCTION
  void operator() (TagFullNeigh, const T_INT& i) const;

  KOKKOS_INLINE_FUNCTION
  void operator() (TagHalfNeigh, const T_INT& i) const;

  KOKKOS_INLINE_FUNCTION
  void operator() (TagFullNeighPE, const T_INT& i, T_V_FLOAT& PE) const;

  KOKKOS_INLINE_FUNCTION
  void operator() (TagHalfNeighPE, const T_INT& i, T_V_FLOAT& PE) const;

  const char* name();
};

#endif
