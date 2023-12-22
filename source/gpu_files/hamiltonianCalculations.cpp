#include <stdio.h>

#include "real_type.h"

#include "hamiltonianCalculations.hpp"

#include "matrix.hpp"

// C implementation
void HamiltonianCalculations::heisge_jij(
		matrix<real,3,3> &beff, 
		const matrix<real,3,3> &emomM,
		const matrix<real,3,3> &emom, 
		const matrix<real,3,3> &external_field) {

	size_t N = beff.dimension_size(1); // Second dimension
	size_t M = beff.dimension_size(2); // Third dimension
	
	// Loop over ensembles
	#pragma omp parallel for collapse(2)
	for (size_t k = 0; k < M; k++) {
		for (size_t i = 0; i < N; i++) {
			beff_s[0]=0.0; beff_s[1]=0.0; beff_s[2]=0.0;
			// Sum exchange terms
			heisenberg_field(i, k, emomM, beff_s);
			if (do_dm) {dzyalonshinskii_moriya_field(i, k, emomM, beff_s);}

			// Field
			beff(0,i,k) = beff_s[0] + external_field(0,i,k);
			beff(1,i,k) = beff_s[1] + external_field(1,i,k);
			beff(2,i,k) = beff_s[2] + external_field(2,i,k);	
		}
	}
}

inline void HamiltonianCalculations::heisenberg_field(
		const size_t i, const size_t k, const matrix<real,3,3> &emomM, real *beff_s) {
	size_t lsize = nlistsize[i];
	for (size_t j = 0; j < lsize; j++) {
		int n = nlist(j,i)-1;
		real coup = ncoup(j,i);
		beff_s[0] += coup * emomM(0,n,k);
		beff_s[1] += coup * emomM(1,n,k);
		beff_s[2] += coup * emomM(2,n,k);
	}
}
inline void HamiltonianCalculations::dzyalonshinskii_moriya_field(
		const size_t i, const size_t k, const matrix<real,3,3> &emomM, real* beff_s) {
	size_t lsize = dmlistsize[i];
	for (int j=1;j<lsize;j++) {
		beff_s[0] -= dm_vect(3,j,i)*emomM(2,dmlist(j,i),k) +
			dm_vect(2,j,i)*emomM(3,dmlist(j,i),k);
		beff_s[1] -= dm_vect(1,j,i)*emomM(3,dmlist(j,i),k) +
			dm_vect(3,j,i)*emomM(1,dmlist(j,i),k);
		beff_s[2] -= dm_vect(2,j,i)*emomM(1,dmlist(j,i),k) +
			dm_vect(1,j,i)*emomM(2,dmlist(j,i),k);
	}
}
