/* This file was automatically generated by CasADi 3.6.3.
 *  It consists of:
 *   1) content generated by CasADi runtime: not copyrighted
 *   2) template code copied from CasADi source: permissively licensed (MIT-0)
 *   3) user code: owned by the user
 *
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef casadi_real
#define casadi_real double
#endif

#ifndef casadi_int
#define casadi_int long long int
#endif

int attitude_error(const casadi_real** arg, casadi_real** res, casadi_int* iw, casadi_real* w, int mem);
int attitude_error_alloc_mem(void);
int attitude_error_init_mem(int mem);
void attitude_error_free_mem(int mem);
int attitude_error_checkout(void);
void attitude_error_release(int mem);
void attitude_error_incref(void);
void attitude_error_decref(void);
casadi_int attitude_error_n_in(void);
casadi_int attitude_error_n_out(void);
casadi_real attitude_error_default_in(casadi_int i);
const char* attitude_error_name_in(casadi_int i);
const char* attitude_error_name_out(casadi_int i);
const casadi_int* attitude_error_sparsity_in(casadi_int i);
const casadi_int* attitude_error_sparsity_out(casadi_int i);
int attitude_error_work(casadi_int* sz_arg, casadi_int* sz_res, casadi_int* sz_iw, casadi_int* sz_w);
#define attitude_error_SZ_ARG 4
#define attitude_error_SZ_RES 1
#define attitude_error_SZ_IW 0
#define attitude_error_SZ_W 31
int quaternion_to_euler(const casadi_real** arg, casadi_real** res, casadi_int* iw, casadi_real* w, int mem);
int quaternion_to_euler_alloc_mem(void);
int quaternion_to_euler_init_mem(int mem);
void quaternion_to_euler_free_mem(int mem);
int quaternion_to_euler_checkout(void);
void quaternion_to_euler_release(int mem);
void quaternion_to_euler_incref(void);
void quaternion_to_euler_decref(void);
casadi_int quaternion_to_euler_n_in(void);
casadi_int quaternion_to_euler_n_out(void);
casadi_real quaternion_to_euler_default_in(casadi_int i);
const char* quaternion_to_euler_name_in(casadi_int i);
const char* quaternion_to_euler_name_out(casadi_int i);
const casadi_int* quaternion_to_euler_sparsity_in(casadi_int i);
const casadi_int* quaternion_to_euler_sparsity_out(casadi_int i);
int quaternion_to_euler_work(casadi_int* sz_arg, casadi_int* sz_res, casadi_int* sz_iw, casadi_int* sz_w);
#define quaternion_to_euler_SZ_ARG 1
#define quaternion_to_euler_SZ_RES 1
#define quaternion_to_euler_SZ_IW 0
#define quaternion_to_euler_SZ_W 16
#ifdef __cplusplus
} /* extern "C" */
#endif
