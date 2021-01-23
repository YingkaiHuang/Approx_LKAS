#ifndef HALIDE_____auto_schedule_true_fwd_v3_h
#define HALIDE_____auto_schedule_true_fwd_v3_h
#include <stdint.h>

// Forward declarations of the types used in the interface
// to the Halide pipeline.
//
// For the definitions of these structs, include HalideRuntime.h

// Halide's representation of a multi-dimensional array.
// Halide::Runtime::Buffer is a more user-friendly wrapper
// around this. Its declaration is in HalideBuffer.h
struct halide_buffer_t;

// Metadata describing the arguments to the generated function.
// Used to construct calls to the _argv version of the function.
struct halide_filter_metadata_t;

#ifndef HALIDE_FUNCTION_ATTRS
#define HALIDE_FUNCTION_ATTRS
#endif



#ifdef __cplusplus
extern "C" {
#endif

int auto_schedule_true_fwd_v3(struct halide_buffer_t *_in_patch_buffer, struct halide_buffer_t *_ctrl_pts_h_buffer, struct halide_buffer_t *_weights_h_buffer, struct halide_buffer_t *_rev_tone_h_buffer, struct halide_buffer_t *_TsTw_tran_h_buffer, struct halide_buffer_t *_coefs_h_buffer, struct halide_buffer_t *_processed_buffer) HALIDE_FUNCTION_ATTRS;
int auto_schedule_true_fwd_v3_argv(void **args) HALIDE_FUNCTION_ATTRS;
const struct halide_filter_metadata_t *auto_schedule_true_fwd_v3_metadata() HALIDE_FUNCTION_ATTRS;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif
