/// \file

#ifdef __cplusplus
extern "C" {
#endif

/// Defines the Joystick Buttons
typedef enum
{
  Left = 1 << 0,
  Up = 1 << 1,
  Right = 1 << 2,
  Down = 1 << 3,
  Pressed = 1 << 4

} Buttons;

#ifdef __cplusplus
}
#endif