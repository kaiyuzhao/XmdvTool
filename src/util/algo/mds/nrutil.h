//#include <MDS_misc.h>
//extern void eigsrt(MDS_Vector &vec,MDS_Matrix &mat,int n);
#ifdef __cplusplus
extern "C" {
#endif

void eigsrtd(double *d,double **v,int n);
void jacobid(double **a,int n,double *d,double **v,int *nrot);

float *vector();
float **matrix();
float **convert_matrix();
double *dvector();
double **dmatrix();
int *ivector();
int **imatrix();
float **submatrix();
void free_vector();
void free_dvector();
void free_ivector();
void free_matrix();
void free_dmatrix();
void free_imatrix();
void free_submatrix();
void free_convert_matrix();
void nrerror();

#ifdef __cplusplus
}
#endif
