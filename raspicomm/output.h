/// \file

#ifdef __cplusplus
extern "C" {
#endif

/// Output flags that are used for manipulation of the outputs.
/// @see raspicomm_SetOutputs
/// @see raspicomm_GetOutputs
/// @see raspicomm_ClearOutputs
typedef enum 
{ 
  /// Output/LED 1 (Green)
  LED1 = 1 << 0,

  /// Output/LED 2 (Red)
  LED2 = 1 << 1
} Outputs;

#ifdef __cplusplus
}
#endif