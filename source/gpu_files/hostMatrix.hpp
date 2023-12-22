// Matrix class for host matrices stored in
// column-major format
//
// Niklas Fejes 2012-2013
//

#ifndef __HOST_MATRIX_HPP__
#define __HOST_MATRIX_HPP__

#include <cstdio>
#include <stdlib.h>
#include "matrix.hpp"


template <typename T, size_t D=1, size_t I=0, size_t J=0, size_t K=0, size_t L=0>
class hostMatrix : public matrix<T,D,I,J,K,L> {
private:
	// Initiate
        // Changed new to calloc > Thomas Nystrand
	bool init(size_t i, size_t j, size_t k, size_t l) {
		// Calculate size
		//size_t size = i * j * k * l * sizeof(T);
		size_t size = i * j * k * l;
		// Deallocate data if already allocated
		free();

		// Allocate new memory
		//this->data = new T[size];
		this->data = (T*)calloc(size,sizeof(T));
		// If new failed
		if (this->data == NULL) {
			#ifdef DEBUG
			printf("hostMatrix::initiate: failed to allocate %ld bytes\n", size);
			#endif
			return false;
		}

		// Set dimensions
		for (int n = 0; n < D; n++) switch (n) {
			case 0: this->dim_size[n] = i; break;
			case 1: this->dim_size[n] = j; break;
			case 2: this->dim_size[n] = k; break;
			case 3: this->dim_size[n] = l; break;
		}
		return true;
	}
public:
	// Constructors
	hostMatrix() {}
	hostMatrix(const hostMatrix<T,D,I,J,K,L> &m) {clone(m);}

	// Destructor
	~hostMatrix() {free();}

	// Initiate
	bool initiate(size_t i)
		{__MAT_TEST_DIM(1); return init(i,1,1,1);}
	bool initiate(size_t i, size_t j)
		{__MAT_TEST_DIM(2); return init(i,j,1,1);}
	bool initiate(size_t i, size_t j, size_t k)
		{__MAT_TEST_DIM(3); return init(i,j,k,1);}
	bool initiate(size_t i, size_t j, size_t k, size_t l)
		{__MAT_TEST_DIM(4); return init(i,j,k,l);}

	bool initiate(matrix<T,D,I,J,K,L> &m) {
		return init((D < 1 ? 1 : m.dimension_size(0)), 
		            (D < 2 ? 1 : m.dimension_size(1)), 
		            (D < 3 ? 1 : m.dimension_size(2)), 
		            (D < 4 ? 1 : m.dimension_size(3)));
	}

	// Changed delete to ::free > Thomas Nystrand
	void free() {
		if (this->data != NULL)
			::free(this->data);
			//delete this->data;
		this->data = NULL;
		for (size_t i = 0; i < D; i++)
			this->dim_size[i] = 0;
	}

	// Swap pointers
	inline void swap(hostMatrix<T,D,I,J,K,L> &m) {
		#ifdef DEBUG
		for (int n = 0; n < D; n++) if (this->dim_size[n] != m.dim_size[n]) {
			printf("Warning: swapping pointers between matrices with different sizes\n");
			__MAT_ERR();
			break;
		}
		#endif
		T * tmp    = this->data;
		this->data = m.data;
		m.data     = tmp;
	}

	// Copy data
	inline void memcopy(const hostMatrix<T,D,I,J,K,L> &m) {
		#ifdef DEBUG
		for (int n = 0; n < D; n++) if (this->dim_size[n] != m.dim_size[n]) {
			printf("Warning: copying data between matrices with different sizes\n");
			__MAT_ERR();
			break;
		}
		#endif
		memcpy(this->data, m.data, this->data_size());
	}

	// Clone
	bool clone(const hostMatrix<T,D,I,J,K,L> &m) {
		// Zero matrix?
		if (m.data_size() == 0) {
			free();
			for (int n = 0; n < D; n++)
				this->dim_size[n] = m.dim_size[n];
			return true;
		}

		// Can we keep old data?
		if (this->data && m.size() == this->size()) {
			// Update dimensions
			for (int i = 0; i < D; i++)
				this->dim_size[i] = m.dim_size[i];
			// Copy data
			memcopy(m);
			return true;
		}

		// Free old
		free();

		// Initiate new
		if (!init(m.dimension_size(0), 
		          (D < 2 ? 1 : m.dimension_size(1)), 
		          (D < 3 ? 1 : m.dimension_size(2)), 
		          (D < 4 ? 1 : m.dimension_size(3))))
			return false;

		// Copy the data
		memcopy(m);
		
		return true;
	}
};

#endif

