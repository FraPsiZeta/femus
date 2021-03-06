#include "dense_matrixM.h"

// C++ Includes
#include <cmath>  // for sqrt

// Local Includes

#include "dense_vectorM.h"

// #ifdef MATRIX_HAVE_PETSC    //?
#include "petsc_macroM.h"

// EXTERN_C_FOR_PETSC_BEGIN
#include <petscblaslapack.h>
// EXTERN_C_FOR_PETSC_END
// #endif

// ------------------------------------------------------------
// Dense Matrix member functions

void DenseMatrixM::left_multiply(const DenseMatrixBaseM& M2) {
  if (this->use_blas)
    this->_multiply_blas(M2, LEFT_MULTIPLY);
  else {  // (*this) <- M2 * (*this) Where: (*this) = (m x n), M2      = (m x p), M3      = (p x n)
    // M3 is a copy of *this before it gets resize()d
    DenseMatrixM M3(*this);
    // Resize *this so that the result can fit
    this->resize(M2.m(), M3.n());
    // Call the multiply function in the base class
    this->multiply(*this, M2, M3);
  }
}

// =================================================
void DenseMatrixM::left_multiply_transpose(const DenseMatrixM& A) {
  if (this->use_blas)
    this->_multiply_blas(A, LEFT_MULTIPLY_TRANSPOSE);
  else {
    // Check to see if we are doing (A^T)*A
    if (this == &A) {
      // libmesh_here();
      DenseMatrixM B(*this);
      // Simple but inefficient way
      // return this->left_multiply_transpose(B);

      // More efficient, but more code way
      // If A is mxn, the result will be a square matrix of Size n x n.
      const int m = A.m();
      const int n = A.n();

      // resize() *this and also zero out all entries.
      this->resize(n, n);

      // Compute the lower-triangular part
      for (int i = 0; i < n; ++i)
        for (int j = 0; j <= i; ++j)
          for (int k = 0; k < m; ++k)  // inner products are over m
            (*this)(i, j) += B(k, i) * B(k, j);

      // Copy lower-triangular part into upper-triangular part
      for (int i = 0; i < n; ++i)
        for (int j = i + 1; j < n; ++j) (*this)(i, j) = (*this)(j, i);
    }

    else {
      DenseMatrixM B(*this);
      this->resize(A.n(), B.n());

      assert(A.m() == B.m());
      assert(this->m() == A.n());
      assert(this->n() == B.n());

      const int m_s = A.n();
      const int p_s = A.m();
      const int n_s = this->n();

      // Do it this way because there is a
      // decent chance (at least for constraint matrices)
      // that A.transpose(i,k) = 0.
      for (int i = 0; i < m_s; i++)
        for (int k = 0; k < p_s; k++)
          if (A.transpose(i, k) != 0.)
            for (int j = 0; j < n_s; j++) (*this)(i, j) += A.transpose(i, k) * B(k, j);
    }
  }
}

void DenseMatrixM::right_multiply(const DenseMatrixBaseM& M3) {
  if (this->use_blas)
    this->_multiply_blas(M3, RIGHT_MULTIPLY);
  else {
    // (*this) <- M3 * (*this)
    // Where:
    // (*this) = (m x n),
    // M2      = (m x p),
    // M3      = (p x n)

    // M2 is a copy of *this before it gets resize()d
    DenseMatrixM M2(*this);

    // Resize *this so that the result can fit
    this->resize(M2.m(), M3.n());

    this->multiply(*this, M2, M3);
  }
}

void DenseMatrixM::right_multiply_transpose(const DenseMatrixM& B) {
  if (this->use_blas)
    this->_multiply_blas(B, RIGHT_MULTIPLY_TRANSPOSE);
  else {
    // Check to see if we are doing B*(B^T)
    if (this == &B) {
      // libmesh_here();
      DenseMatrixM A(*this);

      // Simple but inefficient way
      // return this->right_multiply_transpose(A);

      // More efficient, more code
      // If B is mxn, the result will be a square matrix of Size m x m.
      const int m = B.m();
      const int n = B.n();

      // resize() *this and also zero out all entries.
      this->resize(m, m);

      // Compute the lower-triangular part
      for (int i = 0; i < m; ++i)
        for (int j = 0; j <= i; ++j)
          for (int k = 0; k < n; ++k)  // inner products are over n
            (*this)(i, j) += A(i, k) * A(j, k);

      // Copy lower-triangular part into upper-triangular part
      for (int i = 0; i < m; ++i)
        for (int j = i + 1; j < m; ++j) (*this)(i, j) = (*this)(j, i);
    }

    else {
      DenseMatrixM A(*this);

      this->resize(A.m(), B.m());

      assert(A.n() == B.n());
      assert(this->m() == A.m());
      assert(this->n() == B.m());

      const int m_s = A.m();
      const int p_s = A.n();
      const int n_s = this->n();

      // Do it this way because there is a
      // decent chance (at least for constraint matrices)
      // that B.transpose(k,j) = 0.
      for (int j = 0; j < n_s; j++)
        for (int k = 0; k < p_s; k++)
          if (B.transpose(k, j) != 0.)
            for (int i = 0; i < m_s; i++) (*this)(i, j) += A(i, k) * B.transpose(k, j);
    }
  }
}

void DenseMatrixM::vector_mult(DenseVectorM& dest, const DenseVectorM& arg) const {
  const int n_rows = this->m();
  const int n_cols = this->n();

  // Make sure the sizes are compatible
  assert(n_cols == (int)arg.size());
  assert(n_rows == (int)dest.size());

  dest.zero();
  DenseMatrixM A(*this);

  for (int i = 0; i < n_rows; i++)
    for (int j = 0; j < n_cols; j++) dest(i) += A(i, j) * arg(j);
}

void DenseMatrixM::vector_mult_add(DenseVectorM& dest, const double factor, const DenseVectorM& arg) const {
  DenseVectorM temp(arg.size());
  this->vector_mult(temp, arg);
  dest.add(factor, temp);
}

// =============================================
void DenseMatrixM::get_principal_submatrix(int sub_m, int sub_n, DenseMatrixM& dest) const {
  assert((sub_m <= this->m()) && (sub_n <= this->n()));
  dest.resize(sub_m, sub_n);
  for (int i = 0; i < sub_m; i++)
    for (int j = 0; j < sub_n; j++) dest(i, j) = (*this)(i, j);
}

// ================================================
void DenseMatrixM::get_principal_submatrix(int sub_m, DenseMatrixM& dest) const {
  get_principal_submatrix(sub_m, sub_m, dest);
}

void DenseMatrixM::get_transpose(DenseMatrixM& dest) const {
  dest.resize(this->n(), this->m());
  for (int i = 0; i < dest.m(); i++)
    for (int j = 0; j < dest.n(); j++) dest(i, j) = (*this)(j, i);
}

// ===================================================
void DenseMatrixM::lu_solve(DenseVectorM& b, DenseVectorM& x) {
  // Check to be sure that the matrix is square before attempting
  // an LU-solve.  In general, one can compute the LU factorization of
  // a non-square matrix, but:
  //
  // Overdetermined systems (m>n) have a solution only if enough of
  // the equations are linearly-dependent.
  //
  // Underdetermined systems (m<n) typically have infinitely many
  // solutions.
  //
  // We don't want to deal with either of these ambiguous cases here...
  assert(this->m() == this->n());
  this->_decomposition_type = NONE;
  //   switch(this->_decomposition_type)
  //     {
  //     case NONE:
  //       {
  //         if (this->use_blas_lapack)
  this->_lu_decompose_lapack();
  //         else
  //           this->_lu_decompose ();
  //         break;
  //       }

  //     case LU_BLAS_LAPACK:
  //       {
  // Already factored, just need to call back_substitute.
  //         if (this->use_blas_lapack)
  //           break;
  //       }
  //       libmesh_fallthrough();

  //     case LU:
  //       {
  //         // Already factored, just need to call back_substitute.
  //         if (!(this->use_blas_lapack))
  //           break;
  //       }
  //       libmesh_fallthrough();
  //
  //     default:
  //       std::cout<<"Error! This matrix already has a different decomposition..."<<std::endl;
  //     }

  //   if (this->use_blas_lapack)
  this->_lu_back_substitute_lapack(b, x);
  //   else
  //     this->_lu_back_substitute (b, x);
}

void DenseMatrixM::_lu_decompose_lapack() {
  // If this function was called, there better not be any
  // previous decomposition of the matrix.
  assert(this->_decomposition_type == NONE);

  // The calling sequence for dgetrf is:
  // dgetrf(M, N, A, lda, ipiv, info)

  // M (input)
  //   The number of rows of the matrix A.  M >= 0.
  // In C/C++, pass the number of *cols* of A
  PetscBLASInt M = this->n();

  // N (input)
  //   The number of columns of the matrix A.  N >= 0.
  // In C/C++, pass the number of *rows* of A
  PetscBLASInt N = this->m();

  // A (input/output) double precision array, dimension (LDA,N)
  //   On entry, the M-by-N matrix to be factored.
  //   On exit, the factors L and U from the factorization
  //   A = P*L*U; the unit diagonal elements of L are not stored.
  // Here, we pass &(_val[0]).

  // LDA (input)
  //     The leading dimension of the array A.  LDA >= max(1,M).
  PetscBLASInt LDA = M;

  // ipiv (output) integer array, dimension (min(m,n))
  //      The pivot indices; for 1 <= i <= min(m,n), row i of the
  //      matrix was interchanged with row IPIV(i).
  // Here, we pass &(_pivots[0]), a private class member used to store pivots
  this->_pivots.resize(std::min(M, N));

  // info (output)
  //      = 0:  successful exit
  //      < 0:  if INFO = -i, the i-th argument had an illegal value
  //      > 0:  if INFO = i, U(i,i) is exactly zero. The factorization
  //            has been completed, but the factor U is exactly
  //            singular, and division by zero will occur if it is used
  //            to solve a system of equations.
  PetscBLASInt INFO = 0;

  // Ready to call the actual factorization routine through PETSc's interface
  LAPACKgetrf_(&M, &N, &(this->_val[0]), &LDA, &(_pivots[0]), &INFO);

  // Check return value for errors
  if (INFO != 0) std::cout << "Error during Lapack LU factorization!" << std::endl;

  // Set the flag for LU decomposition
  this->_decomposition_type = LU_BLAS_LAPACK;
}

void DenseMatrixM::_lu_back_substitute(DenseVectorM& b, DenseVectorM& x) const {
  const unsigned int n_cols = this->n();

  assert(this->m() == n_cols);
  assert(this->m() == b.size());

  x.resize(n_cols);

  // A convenient reference to *this
  const DenseMatrixM& A = *this;

  // Temporary vector storage.  We use this instead of
  // modifying the RHS.
  DenseVectorM z = b;

  // Lower-triangular "top to bottom" solve step, taking into account pivots
  for (unsigned int i = 0; i < n_cols; ++i) {
    // Swap
    if (_pivots[i] != i) std::swap(z(i), z(_pivots[i]));

    x(i) = z(i);

    for (unsigned int j = 0; j < i; ++j) x(i) -= A(i, j) * x(j);

    x(i) /= A(i, i);
  }

  // Upper-triangular "bottom to top" solve step
  const unsigned int last_row = n_cols - 1;

  for (int i = last_row; i >= 0; --i) {
    for (int j = i + 1; j < static_cast<int>(n_cols); ++j) x(i) -= A(i, j) * x(j);
  }
}

// // ===========================================================
// void DenseMatrixM::_lu_decompose (const bool partial_pivot)
// {    // If this function was called, there better not be any
//     // previous decomposition of the matrix.
//     assert(this->_decomposition_type == NONE);
//     // Get the matrix size and make sure it is square
//     const int
//     m = this->m();   assert (m == this->n());
//     // A convenient reference to *this
//     DenseMatrixM& A = *this;
//
//     // Straight, vanilla LU factorization without pivoting
//     if (!partial_pivot){
//         // For each row in the matrix
//         for (int i=0; i<m; i++)  {
//             // Get the diagonal entry and take its inverse
//             const double diag = A(i,i);
//             assert (fabs(diag) < 1.e-20);
//             const double diag_inv = 1./diag;
//
//             // For each row in the submatrix
//             for (int j=i+1; j<m; j++)      {
//                 // Get the scale factor for this row
//                 const double fact = A(j,i)*diag_inv;
//                 // For each column in the subrow scale it by the factor
//                 for (int k=i+1; k<m; k++)  A(j,k) -= fact*A(i,k);
//             }
//         }
//     }
//     else
//     {   // Do partial pivoting.
//         exit(0);
//     }
//     // Set the flag for LU decomposition
//     this->_decomposition_type = LU;
// }

void DenseMatrixM::_lu_back_substitute_lapack(const DenseVectorM& b, DenseVectorM& x) {
  // The calling sequence for getrs is:
  // dgetrs(TRANS, N, NRHS, A, LDA, IPIV, B, LDB, INFO)

  // trans (input)
  //       'n' for no transpose, 't' for transpose
  char TRANS[] = "t";

  // N (input)
  //   The order of the matrix A.  N >= 0.
  PetscBLASInt N = this->m();

  // NRHS (input)
  //      The number of right hand sides, i.e., the number of columns
  //      of the matrix B.  NRHS >= 0.
  PetscBLASInt NRHS = 1;

  // A (input) double precision array, dimension (LDA,N)
  //   The factors L and U from the factorization A = P*L*U
  //   as computed by dgetrf.
  // Here, we pass &(_val[0])

  // LDA (input)
  //     The leading dimension of the array A.  LDA >= max(1,N).
  PetscBLASInt LDA = N;

  // ipiv (input) int array, dimension (N)
  //      The pivot indices from DGETRF; for 1<=i<=N, row i of the
  //      matrix was interchanged with row IPIV(i).
  // Here, we pass &(_pivots[0]) which was computed in _lu_decompose_lapack

  // B (input/output) double precision array, dimension (LDB,NRHS)
  //   On entry, the right hand side matrix B.
  //   On exit, the solution matrix X.
  // Here, we pass a copy of the rhs vector's data array in x, so that the
  // passed right-hand side b is unmodified.  I don't see a way around this
  // copy if we want to maintain an unmodified rhs in LibMesh.
  x = b;
  std::vector<double>& x_vec = x.get_values();

  // We can avoid the copy if we don't care about overwriting the RHS: just
  // pass b to the Lapack routine and then swap with x before exiting
  // std::vector<T> & x_vec = b.get_values();

  // LDB (input)
  //     The leading dimension of the array B.  LDB >= max(1,N).
  PetscBLASInt LDB = N;

  // INFO (output)
  //      = 0:  successful exit
  //      < 0:  if INFO = -i, the i-th argument had an illegal value
  PetscBLASInt INFO = 0;

  // Finally, ready to call the Lapack getrs function
  LAPACKgetrs_(TRANS, &N, &NRHS, &(_val[0]), &LDA, &(_pivots[0]), &(x_vec[0]), &LDB, &INFO);

  // Check return value for errors
  if (INFO != 0) std::cout << "Error during Lapack LU solve!" << std::endl;

  // Don't do this if you already made a copy of b above
  // Swap b and x.  The solution will then be in x, and whatever was originally
  // in x, maybe garbage, maybe nothing, will be in b.
  // FIXME: Rewrite the LU and Cholesky solves to just take one input, and overwrite
  // the input.  This *should* make user code simpler, as they don't have to create
  // an extra vector just to pass it in to the solve function!
  // b.swap(x);
}

void DenseMatrixM::_lu_decompose() {
  // If this function was called, there better not be any
  // previous decomposition of the matrix.
  assert(this->_decomposition_type == NONE);

  // Get the matrix size and make sure it is square
  const unsigned int n_rows = this->m();

  // A convenient reference to *this
  DenseMatrixM& A = *this;

  _pivots.resize(n_rows);

  for (unsigned int i = 0; i < n_rows; ++i) {
    // Find the pivot row by searching down the i'th column
    _pivots[i] = i;

    // std::abs(complex) must return a Real!
    double the_max = std::abs(A(i, i));
    for (unsigned int j = i + 1; j < n_rows; ++j) {
      double candidate_max = std::abs(A(j, i));
      if (the_max < candidate_max) {
        the_max = candidate_max;
        _pivots[i] = j;
      }
    }

    // libMesh::out << "the_max=" << the_max << " found at row " << _pivots[i] << std::endl;

    // If the max was found in a different row, interchange rows.
    // Here we interchange the *entire* row, in Gaussian elimination
    // you would only interchange the subrows A(i,j) and A(p(i),j), for j>i
    if (_pivots[i] != i) {
      for (unsigned int j = 0; j < n_rows; ++j) std::swap(A(i, j), A(_pivots[i], j));
    }

    // If the max abs entry found is zero, the matrix is singular
    //       if (A(i,i) <1.e-20)
    //         libmesh_error_msg("Matrix A is singular!");

    // Scale upper triangle entries of row i by the diagonal entry
    // Note: don't scale the diagonal entry itself!
    const double diag_inv = 1. / A(i, i);
    for (unsigned int j = i + 1; j < n_rows; ++j) A(i, j) *= diag_inv;

    // Update the remaining sub-matrix A[i+1:m][i+1:m]
    // by subtracting off (the diagonal-scaled)
    // upper-triangular part of row i, scaled by the
    // i'th column entry of each row.  In terms of
    // row operations, this is:
    // for each r > i
    //   SubRow(r) = SubRow(r) - A(r,i)*SubRow(i)
    //
    // If we were scaling the i'th column as well, like
    // in Gaussian elimination, this would 'zero' the
    // entry in the i'th column.
    for (unsigned int row = i + 1; row < n_rows; ++row)
      for (unsigned int col = i + 1; col < n_rows; ++col) A(row, col) -= A(row, i) * A(i, col);

  }  // end i loop

  // Set the flag for LU decomposition
  this->_decomposition_type = LU;
}

// ========================================
double DenseMatrixM::det() {
  // First LU decompose the matrix (without partial pivoting).
  // Note that the lu_decompose routine will check to see if the
  // matrix is square so we don't worry about it.
  if (this->_decomposition_type == NONE)
    this->_lu_decompose();
  else if (this->_decomposition_type != LU) {
    std::cerr << "Error! Can't compute the determinant under "
              << "the current decomposition." << std::endl;
    exit(0);
  }

  // A variable to keep track of the running product of diagonal terms.
  double determinant = 1.;
  // Loop over diagonal terms, computing the product.
  for (int i = 0; i < this->m(); i++) determinant *= (*this)(i, i);
  // Return the determinant
  return determinant;
}

// The cholesky solve function first decomposes the matrix
// with cholesky_decompose and then uses the cholesky_back_substitute
// routine to find the solution x.

void DenseMatrixM::cholesky_solve(DenseVectorM& b, DenseVectorM& x) {
  // Check for a previous decomposition
  switch (this->_decomposition_type) {
    case NONE: {
      this->_cholesky_decompose();
      break;
    }

    case CHOLESKY: {
      // Already factored, just need to call back_substitute.
      break;
    }

    default: {
      std::cerr << "Error! This matrix already has a "
                << "different decomposition..." << std::endl;
      exit(0);
    }
  }

  // Perform back substitution
  this->_cholesky_back_substitute(b, x);
}

// This algorithm is based on the Cholesky decomposition in
// the Numerical Recipes in C book.

void DenseMatrixM::_cholesky_decompose() {
  // If we called this function, there better not be any
  // previous decomposition of the matrix.
  assert(this->_decomposition_type == NONE);

  // Shorthand notation for number of rows and columns.
  const int m = this->m(), n = this->n();

  // Just to be really sure...
  assert(m == n);

  // A convenient reference to *this
  DenseMatrixM& A = *this;

  for (int i = 0; i < m; ++i) {
    for (int j = i; j < n; ++j) {
      for (int k = 0; k < i; ++k) A(i, j) -= A(i, k) * A(j, k);

      if (i == j) {
#ifndef LIBMESH_USE_COMPLEX_NUMBERS
        if (A(i, j) <= 0.0) {
          std::cerr << "Error! Can only use Cholesky decomposition "
                    << "with symmetric positive definite matrices." << std::endl;
          exit(0);
        }
#endif

        A(i, i) = std::sqrt(A(i, j));
      } else
        A(j, i) = A(i, j) / A(i, i);
    }
  }

  // Set the flag for CHOLESKY decomposition
  this->_decomposition_type = CHOLESKY;
}

void DenseMatrixM::_cholesky_back_substitute(DenseVectorM& b, DenseVectorM& x) const {
  // Shorthand notation for number of rows and columns.
  const int m = this->m(), n = this->n();

  // Just to be really sure...
  assert(m == n);

  // A convenient reference to *this
  const DenseMatrixM& A = *this;

  // Now compute the solution to Ax =b using the factorization.
  x.resize(m);

  // Solve for Ly=b
  for (int i = 0; i < n; ++i) {
    for (int k = 0; k < i; ++k) b(i) -= A(i, k) * x(k);

    x(i) = b(i) / A(i, i);
  }

  // Solve for L^T x = y
  for (int i = 0; i < n; ++i) {
    const int ib = (n - 1) - i;

    for (int k = (ib + 1); k < n; ++k) x(ib) -= A(k, ib) * x(k);

    x(ib) /= A(ib, ib);
  }
}

#ifdef MATRIX_HAVE_PETSC

void DenseMatrixM::_multiply_blas(const DenseMatrixBaseM& other, _BLAS_Multiply_Flag flag) {
  int result_size = 0;

  // For each case, determine the size of the final result make sure
  // that the inner dimensions match
  switch (flag) {
    case LEFT_MULTIPLY: {
      result_size = other.m() * this->n();
      if (other.n() == this->m()) break;
    }
    case RIGHT_MULTIPLY: {
      result_size = other.n() * this->m();
      if (other.m() == this->n()) break;
    }
    case LEFT_MULTIPLY_TRANSPOSE: {
      result_size = other.n() * this->n();
      if (other.m() == this->m()) break;
    }
    case RIGHT_MULTIPLY_TRANSPOSE: {
      result_size = other.m() * this->m();
      if (other.n() == this->n()) break;
    }
    default: {
      std::cout << "Unknown flag selected or matrices are ";
      std::cout << "incompatible for multiplication." << std::endl;
      exit(0);
    }
  }

  // For this to work, the passed arg. must actually be a DenseMatrixM
  const DenseMatrixM* const_that = dynamic_cast<const DenseMatrixM*>(&other);
  if (!const_that) {
    std::cerr << "Unable to cast input matrix to usable type." << std::endl;
    exit(0);
  }

  // Also, although 'that' is logically const in this BLAS routine,
  // the PETSc BLAS interface does not specify that any of the inputs are
  // const.  To use it, I must cast away const-ness.
  DenseMatrixM* that = const_cast<DenseMatrixM*>(const_that);

  // Initialize A, B pointers for LEFT_MULTIPLY* cases
  DenseMatrixM *A = this, *B = that;

  // For RIGHT_MULTIPLY* cases, swap the meaning of A and B.
  // Here is a full table of combinations we can pass to BLASgemm, and what the answer is when finished:
  // pass A B   -> (Fortran) -> A^T B^T -> (C++) -> (A^T B^T)^T -> (identity) -> B A   "lt multiply"
  // pass B A   -> (Fortran) -> B^T A^T -> (C++) -> (B^T A^T)^T -> (identity) -> A B   "rt multiply"
  // pass A B^T -> (Fortran) -> A^T B   -> (C++) -> (A^T B)^T   -> (identity) -> B^T A "lt multiply t"
  // pass B^T A -> (Fortran) -> B A^T   -> (C++) -> (B A^T)^T   -> (identity) -> A B^T "rt multiply t"
  if (flag == RIGHT_MULTIPLY || flag == RIGHT_MULTIPLY_TRANSPOSE) std::swap(A, B);

  // transa, transb values to pass to blas
  char transa[] = "n", transb[] = "n";

  // Integer values to pass to BLAS:
  //
  // M
  // In Fortran, the number of rows of op(A),
  // In the BLAS documentation, typically known as 'M'.
  //
  // In C/C++, we set:
  // M = n_cols(A) if (transa='n')
  //     n_rows(A) if (transa='t')
  int M = static_cast<int>(A->n());

  // N
  // In Fortran, the number of cols of op(B), and also the number of cols of C.
  // In the BLAS documentation, typically known as 'N'.
  //
  // In C/C++, we set: N = n_rows(B) if (transb='n')     n_cols(B) if (transb='t')
  int N = static_cast<int>(B->m());

  // K
  // In Fortran, the number of cols of op(A), and also
  // the number of rows of op(B). In the BLAS documentation,
  // typically known as 'K'.
  //
  // In C/C++, we set: K = n_rows(A) if (transa='n')     n_cols(A) if (transa='t')
  int K = static_cast<int>(A->m());
  // LDA (leading dimension of A). In our cases LDA is always the number of columns of A.
  int LDA = static_cast<int>(A->n());
  // LDB (leading dimension of B).  In our cases, LDB is always the number of columns of B.
  int LDB = static_cast<int>(B->n());

  if (flag == LEFT_MULTIPLY_TRANSPOSE) {
    transb[0] = 't';
    N = static_cast<int>(B->n());
  }

  else if (flag == RIGHT_MULTIPLY_TRANSPOSE) {
    transa[0] = 't';
    std::swap(M, K);
  }

  // LDC (leading dimension of C).  LDC is the number of columns in the solution matrix.
  int LDC = M;

  // Scalar values to pass to BLAS    //
  // scalar multiplying the whole product AB
  double alpha = 1.;
  // scalar multiplying C, which is the original matrix.
  double beta = 0.;
  // Storage for the result
  std::vector<double> result(result_size);
  // Finally ready to call the BLAS
  BLASgemm_(
      transa, transb, &M, &N, &K, &alpha, &(A->_val[0]), &LDA, &(B->_val[0]), &LDB, &beta, &result[0], &LDC);

  // Update the relevant dimension for this matrix.
  switch (flag) {
    case LEFT_MULTIPLY: this->_m = other.m(); break;
    case RIGHT_MULTIPLY: this->_n = other.n(); break;
    case LEFT_MULTIPLY_TRANSPOSE: this->_m = other.n(); break;
    case RIGHT_MULTIPLY_TRANSPOSE: this->_n = other.m(); break;
    default: std::cout << "Unknown flag selected." << std::endl; exit(0);
  }

  // Swap my data vector with the result
  this->_val.swap(result);
}

#else
// ==========================================================
void DenseMatrixM::_multiply_blas(const DenseMatrixBaseM&, _BLAS_Multiply_Flag) {
  std::cerr << "No PETSc-provided BLAS available!" << std::endl;
  exit(0);
}

#endif

// This routine is commented out since it is not really a memory
// efficient implementation.  Also, you don't *need* the inverse
// for anything, instead just use lu_solve to solve Ax=b.
//
// void DenseMatrixM::inverse ()
// {
//   // First LU decompose the matrix (without partial pivoting).
//   // Note that the lu_decompose routine will check to see if the
//   // matrix is square so we don't worry about it.
//   if (!this->_lu_decomposed)
//     this->_lu_decompose(false);

//   // A unit vector which will be used as a rhs
//   // to pick off a single value each time.
//   DenseVector  <double> e;
//   e.resize(this->m());

//   // An empty vector which will be used to hold the solution
//   // to the back substitutions.
//   DenseVector  <double> x;
//   x.resize(this->m());

//   // An empty dense matrix to store the resulting inverse
//   // temporarily until we can overwrite A.
//   DenseMatrixM inv;
//   inv.resize(this->m(), this->n());

//   // Resize the passed in matrix to hold the inverse
//   inv.resize(this->m(), this->n());

//   for (int j=0; j<this->n(); ++j)
//     {
//       e.zero();
//       e(j) = 1.;
//       this->_lu_back_substitute(e, x, false);
//       for (int i=0; i<this->n(); ++i)
// 	inv(i,j) = x(i);
//     }

//   // Now overwrite all the entries
//   *this = inv;
// }
