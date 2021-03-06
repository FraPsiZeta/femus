// std lib includes ---------------------------------------------
#include <cstring>
#include <fstream>
#include <sstream>

// this class -----------------------------------------------
#include "sparse_MmatrixM.h"

// configuration files ---------------------------------------
#include "Solverlib_conf.h"  // in ./config

// local includes -----------------------------------------------
#include "laspack_MmatrixM.h"  // rectangular laspack matrix
#include "petsc_MmatrixM.h"    // rectangular petsc matrix
// #include "trilinos_epetra_matrix.h"  // trilinos matrix
#include "numeric_vectorM.h"  // vector
#include "parallelM.h"        // paralell methods

// hdf5 ----------------------------------------------------
#include "hdf5.h"  // hdf5 format

// ===========================================================
// Rectangular SparseMatrix Methods
// ===========================================================

// ===========================================================
// Build the appropriate vector
std::unique_ptr<SparseMMatrixM> SparseMMatrixM::build(
    const ParallelM::Communicator& comm,  ///< Matrix communicator
    const SolverPackageM solver_package   ///< solver package
) {
  // ------------------------------------------------------------

  switch (solver_package) {
#ifdef HAVE_LASPACKM
    case LASPACK_SOLVERSM: {
      std::unique_ptr<SparseMMatrixM> ap(new LaspackMMatrixM(comm));
      return ap;
    }
#endif
#ifdef HAVE_PETSCM
    case PETSC_SOLVERSM: {
      std::unique_ptr<SparseMMatrixM> ap(new PetscMMatrixM(comm));
      return ap;
    }
#endif
#ifdef HAVE_TRILINOSM
    case TRILINOS_SOLVERSM: {
      std::unique_ptr<SparseMMatrixM> ap(new EpetraMatrix<double>(comm));
      return ap;
    }
#endif
    default: std::cerr << "ERROR: SparseMMatrixM::build: Unrecognized: " << solver_package; abort();
  }
  std::cerr << "ERROR: SparseMMatrixM::build: Unrecognized: " << solver_package;
  abort();
  //   std::unique_ptr<SparseMMatrixM> ap(NULL);
  //   return ap;
}

// ===========================================================
void SparseMMatrixM::read(const std::string& /* name*/) {
  // -----------------------------------------------------------
  //   std::ifstream infile(name.c_str());
  //   if (!infile) {std::cerr << " SparseMMatrixM::read filename "
  //     << name << " not found "<<std::endl;abort();}
  //
  //   char  buf[256]="";  int nrow,len,ipos,n_sdom,nodes_proc_in;
  //
  //   infile >> buf >> buf >> buf;
  //   infile >> buf >> buf >> buf  >> nrow;  Graph graph;  graph.resize(nrow);
  //   infile >> n_sdom >> buf;
  //   uint *n_loc=new uint[n_sdom+1];
  //   for(uint ip=0;ip<=n_sdom;ip++)infile >>n_loc[ip];
  //   graph._m=nrow; graph._n=nrow;graph._ml=n_loc[libMesh::processor_id()+1]-n_loc[libMesh::processor_id()];
  //   graph._nl=n_loc[libMesh::processor_id()+1]-n_loc[libMesh::processor_id()];
  //   graph._ml_start=n_loc[libMesh::processor_id()];
  //
  //   // data
  //   for (uint i=0; i<(uint)nrow; i++) {
  //
  //     infile >> len;  graph[i].resize(len+1);
  //     infile >> nodes_proc_in;
  //     for (int j=0; j< len; j++) {infile  >>ipos; graph[i][j] = ipos-1;}
  //     // last stored value is the number of in-matrix nonzero values
  //     graph[i][len] = nodes_proc_in;
  // //     printf(" grPH %d %d %d \n",libMesh::processor_id(), i,nodes_proc_in);
  //   }
  //   infile.close();
  //   update_sparsity_pattern(graph); graph.clear();
  //   // std::cout << "   read matrix  " << std::endl;
}
// ===========================================================
void SparseMMatrixM::vector_mult(NumericVectorM& dest, const NumericVectorM& arg) const {
  dest.zero();
  this->vector_mult_add(dest, arg);
}

// ===========================================================
/// This functionality is actually implemented in the \p NumericVector class.
void SparseMMatrixM::vector_mult_add(NumericVectorM& dest, const NumericVectorM& arg) const {
  dest.add_vector(arg, *this);
}

// ===============================================================
void SparseMMatrixM::zero_rows(std::vector<int>&, double) {
  std::cout << " This functionality isn't implemented or stubbed in every subclass yet ";
  abort();
}

  // void SparseMMatrixM::print(std::ostream& os) const
  //{
  //   parallel_only();
  //
  //   libmesh_assert (this->initialized());
  //
  //   // We'll print the matrix from processor 0 to make sure
  //   // it's serialized properly
  //   if (libMesh::processor_id() == 0){
  // //       libmesh_assert(this->_dof_map->first_dof() == 0);
  //       for (unsigned int i=this->_dof_map->first_dof();
  //            i!=this->_dof_map->end_dof(); ++i){
  //           for (unsigned int j=0; j<this->n(); j++)   os << (*this)(i,j) << " ";
  //           os << std::endl;
  //         }
  //
  //       std::vector<unsigned int> ibuf, jbuf;
  //       std::vector<double> cbuf;
  //       unsigned int currenti = this->_dof_map->end_dof();
  //       for (unsigned int p=1; p < libMesh::n_processors(); ++p)  {
  //           Parallel::receive(p, ibuf);
  //           Parallel::receive(p, jbuf);
  //           Parallel::receive(p, cbuf);
  //           libmesh_assert(ibuf.size() == jbuf.size());
  //           libmesh_assert(ibuf.size() == cbuf.size());
  //
  //           if (ibuf.empty())
  //             continue;
  //           libmesh_assert(ibuf.front() >= currenti);
  //           libmesh_assert(ibuf.back() >= ibuf.front());
  //
  //           unsigned int currentb = 0;
  //           for (;currenti <= ibuf.back(); ++currenti)            {
  //               for (unsigned int j=0; j<this->n(); j++)                {
  //                   if (currentb < ibuf.size() && ibuf[currentb] == currenti && jbuf[currentb] == j) {
  // 	              os << cbuf[currentb] << " ";
  // 	              currentb++;
  //                     }
  //                   else	os << static_cast<double>(0.0) << " ";
  //                 }
  //               os << std::endl;
  //             }
  //         }
  //       for (; currenti != this->m(); ++currenti)   {
  //           for (unsigned int j=0; j<this->n(); j++)  os << static_cast<double>(0.0) << " ";
  //           os << std::endl;
  //         }
  //     }
  //   else
  //     {
  //       std::vector<unsigned int> ibuf, jbuf;
  //       std::vector<double> cbuf;
  //
  //       // We'll assume each processor has access to entire
  //       // matrix rows, so (*this)(i,j) is valid if i is a local index.
  //       for (unsigned int i=this->_dof_map->first_dof();
  //            i!=this->_dof_map->end_dof(); ++i)        {
  //           for (unsigned int j=0; j<this->n(); j++)	    {
  //               double c = (*this)(i,j);
  //               if (c != static_cast<double>(0.0))                {
  //                   ibuf.push_back(i); jbuf.push_back(j); cbuf.push_back(c);
  //                 }
  // 	    }
  //         }
  //       Parallel::send(0,ibuf); Parallel::send(0,jbuf); Parallel::send(0,cbuf);
  //     }
  //}

#if HDF5_VERSION == 188

// =====================================================
/// This function reads len sparse matrix structures
void SparseMMatrixM::read_len_hdf5(
    const std::string namefile,  // file name
    const std::string mode,      // linear or quadratic
    int len_row[],               // row lengths
    int len_off_row[]            // row off entry lengths
) {
  // -------------------------------------------------------
  hid_t file_id = H5Fopen(namefile.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);

  // row lengths -------------------------------------------------------------
  std::ostringstream name_dst;
  name_dst.str("");
  name_dst << "LEN0" << mode;
  hid_t dataset = H5Dopen(file_id, name_dst.str().c_str());
  hid_t status = H5Dread(dataset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, len_row);
  assert(status == 0);
  // matrix row off entry lengths --------------------------------------------
  name_dst.str("");
  name_dst << "OFFLEN0" << mode;
  dataset = H5Dopen(file_id, name_dst.str().c_str());
  status = H5Dread(dataset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, len_off_row);

  H5Fclose(file_id);
  return;
}

// =====================================================
/// This function reads pos sparse matrix structure
void SparseMMatrixM::read_pos_hdf5(
    const std::string namefile,  // file name
    const std::string mode,      // linear or quadratic
    int pos_row[],               // compressed row positions
    double val[]                 // compressed row values
) {
  hid_t file_id = H5Fopen(namefile.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);

  std::ostringstream name_dst;
  name_dst.str("");
  name_dst << "POS0" << mode;
  hid_t dataset = H5Dopen(file_id, name_dst.str().c_str());
  hid_t status = H5Dread(dataset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, pos_row);
  assert(status == 0);
  name_dst.str("");
  name_dst << "VAL0" << mode;
  dataset = H5Dopen(file_id, name_dst.str().c_str());
  status = H5Dread(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, val);

  H5Fclose(file_id);
  return;
}

// =====================================================
/// This function reads quad-linear matrix dimensions
int SparseMMatrixM::read_dim_hdf5(
    const std::string namefile,  // file name
    const std::string mode,      // linear or quadratic
    int ldim[]                   // dimension (dim[0]=quad dim[1]=linear)
) {
  hid_t file_id = H5Fopen(namefile.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
  //   int ldim[2];
  std::ostringstream name_dst;
  name_dst.str("");
  name_dst << "DIM0" << mode;
  // linear ------------------------------------------------
  hid_t dataset = H5Dopen(file_id, name_dst.str().c_str());
  hid_t status = H5Dread(dataset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, ldim);
  assert(status == 0);

  //   dim[1]=ldim[0];dim[3]=ldim[1];
  //   // quad -> dim[0]  ------------------------------------------------
  //   dataset=H5Dopen(file_id,"DIM02", H5P_DEFAULT);
  //   status=H5Dread(dataset,H5T_NATIVE_INT,H5S_ALL,H5S_ALL,H5P_DEFAULT,ldim);
  //   dim[0]=ldim[0];dim[2]=ldim[1];

  H5Fclose(file_id);
  return ((int)status);
}

#else

// =====================================================
/// This function reads len sparse matrix structures
void SparseMMatrixM::read_len_hdf5(
    const std::string namefile,  // file name
    const std::string mode,      // linear or quadratic
    int len_row[],               // row lengths
    int len_off_row[]            // row off entry lengths
) {
  // -------------------------------------------------------
  hid_t file_id = H5Fopen(namefile.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);

  // row lengths -------------------------------------------------------------
  std::ostringstream name_dst;
  name_dst.str("");
  name_dst << "LEN0" << mode;
  hid_t dataset = H5Dopen(
      file_id, name_dst.str().c_str()
#if HDF5_VERSIONM != 1808
                   ,
      H5P_DEFAULT
#endif
  );
  hid_t status = H5Dread(dataset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, len_row);
  assert(status == 0);
  // matrix row off entry lengths --------------------------------------------
  name_dst.str("");
  name_dst << "OFFLEN0" << mode;
  dataset = H5Dopen(
      file_id, name_dst.str().c_str()
#if HDF5_VERSIONM != 1808
                   ,
      H5P_DEFAULT
#endif
  );
  status = H5Dread(dataset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, len_off_row);

  H5Fclose(file_id);
  return;
}

// =====================================================
/// This function reads pos sparse matrix structure
void SparseMMatrixM::read_pos_hdf5(
    const std::string namefile,  // file name
    const std::string mode,      // linear or quadratic
    int pos_row[],               // compressed row positions
    double val[]                 // compressed row values
) {
  hid_t file_id = H5Fopen(
      namefile.c_str(), H5F_ACC_RDWR

      ,
      H5P_DEFAULT

  );

  std::ostringstream name_dst;
  name_dst.str("");
  name_dst << "POS0" << mode;
  hid_t dataset = H5Dopen(
      file_id, name_dst.str().c_str()
#if HDF5_VERSIONM != 1808
                   ,
      H5P_DEFAULT
#endif
  );
  hid_t status = H5Dread(dataset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, pos_row);
  assert(status == 0);
  name_dst.str("");
  name_dst << "VAL0" << mode;
  dataset = H5Dopen(
      file_id, name_dst.str().c_str()
#if HDF5_VERSIONM != 1808
                   ,
      H5P_DEFAULT
#endif
  );
  status = H5Dread(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, val);

  H5Fclose(file_id);
  return;
}

// =====================================================
/// This function reads quad-linear matrix dimensions
int SparseMMatrixM::read_dim_hdf5(
    const std::string namefile,  // file name
    const std::string mode,      // linear or quadratic
    int ldim[]                   // dimension (dim[0]=quad dim[1]=linear)
) {
  hid_t file_id = H5Fopen(namefile.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
  //   int ldim[2];
  std::ostringstream name_dst;
  name_dst.str("");
  name_dst << "DIM0" << mode;
  // linear ------------------------------------------------
  hid_t dataset = H5Dopen(
      file_id, name_dst.str().c_str()
#if HDF5_VERSIONM != 1808
                   ,
      H5P_DEFAULT
#endif
  );
  hid_t status = H5Dread(dataset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, ldim);
  //  assert(status==1);

  //   dim[1]=ldim[0];dim[3]=ldim[1];
  //   // quad -> dim[0]  ------------------------------------------------
  //   dataset=H5Dopen(file_id,"DIM02", H5P_DEFAULT);
  //   status=H5Dread(dataset,H5T_NATIVE_INT,H5S_ALL,H5S_ALL,H5P_DEFAULT,ldim);
  //   dim[0]=ldim[0];dim[2]=ldim[1];

  H5Fclose(file_id);
  return ((int)status);
}

#endif

//------------------------------------------------------------------
// Explicit instantiations
// template class SparseMatrix<Number>;
