/// \file
/// Defines the raspberrypi revisions
typedef enum
{
  /// Uses the default revision as specified in the configuration file /etc/raspicomm.conf
  REVISION_DEFAULT = 0,

  /// Original Raspberry Pi release, Revision 1.0
  REVISION_1 = 1,

  /// Raspberry Pi Revision 2.0
  REVISION_2 = 2,

  REVISION_FIRST = 1,
  REVISION_LATEST = 2

} Revision;