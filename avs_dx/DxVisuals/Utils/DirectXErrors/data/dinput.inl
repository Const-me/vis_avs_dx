// -------------------------------------------------------------
// dinput.h error codes
// -------------------------------------------------------------
//      CHK_ERR(DI_OK, "DI_OK")
//      CHK_ERR(DI_NOTATTACHED, "DI_NOTATTACHED")
//      CHK_ERR(DI_BUFFEROVERFLOW, "DI_BUFFEROVERFLOW")
//      CHK_ERR(DI_PROPNOEFFECT, "DI_PROPNOEFFECT")
//      CHK_ERR(DI_NOEFFECT, "DI_NOEFFECT")
//      CHK_ERR(DI_POLLEDDEVICE, "DI_POLLEDDEVICE")
//      CHK_ERR(DI_DOWNLOADSKIPPED, "DI_DOWNLOADSKIPPED")
//      CHK_ERR(DI_EFFECTRESTARTED, "DI_EFFECTRESTARTED")
//      CHK_ERR(DI_SETTINGSNOTSAVED_ACCESSDENIED, "DI_SETTINGSNOTSAVED_ACCESSDENIED")
//      CHK_ERR(DI_SETTINGSNOTSAVED_DISKFULL, "DI_SETTINGSNOTSAVED_DISKFULL")
//      CHK_ERR(DI_TRUNCATED, "DI_TRUNCATED")
//      CHK_ERR(DI_TRUNCATEDANDRESTARTED, "DI_TRUNCATEDANDRESTARTED")
//      CHK_ERR(DI_WRITEPROTECT, "DI_WRITEPROTECT")
// CHK_ERR( DIERR_OLDDIRECTINPUTVERSION, "The application requires a newer version of DirectInput." )	// Win32 error
//      CHK_ERR(DIERR_GENERIC, "DIERR_GENERIC")
//      CHK_ERR(DIERR_OLDDIRECTINPUTVERSION, "DIERR_OLDDIRECTINPUTVERSION")
// CHK_ERR( DIERR_BETADIRECTINPUTVERSION, "The application was written for an unsupported prerelease version of DirectInput." )	// Win32 error
// CHK_ERR( DIERR_BADDRIVERVER, "The object could not be created due to an incompatible driver version or mismatched or incomplete driver components." )	// Win32 error
//      CHK_ERR(DIERR_DEVICENOTREG, "DIERR_DEVICENOTREG")
//      CHK_ERR(DIERR_NOTFOUND, "The requested object does not exist.")
//      CHK_ERR(DIERR_OBJECTNOTFOUND, "DIERR_OBJECTNOTFOUND")
//      CHK_ERR(DIERR_INVALIDPARAM, "DIERR_INVALIDPARAM")
//      CHK_ERR(DIERR_NOINTERFACE, "DIERR_NOINTERFACE")
//      CHK_ERR(DIERR_GENERIC, "DIERR_GENERIC")
//      CHK_ERR(DIERR_OUTOFMEMORY, "DIERR_OUTOFMEMORY")
//      CHK_ERR(DIERR_UNSUPPORTED, "DIERR_UNSUPPORTED")
// CHK_ERR( DIERR_NOTINITIALIZED, "This object has not been initialized" )	// Win32 error
// CHK_ERR( DIERR_ALREADYINITIALIZED, "This object is already initialized" )	// Win32 error
//      CHK_ERR(DIERR_NOAGGREGATION, "DIERR_NOAGGREGATION")
//      CHK_ERR(DIERR_OTHERAPPHASPRIO, "DIERR_OTHERAPPHASPRIO")
CHK_ERR( DIERR_INPUTLOST, "Access to the device has been lost.  It must be re-acquired." )	// Win32 error, but this message is more specific
CHK_ERR( DIERR_ACQUIRED, "The operation cannot be performed while the device is acquired." )	// Win32 error, but this message is more specific
CHK_ERR( DIERR_NOTACQUIRED, "The operation cannot be performed unless the device is acquired." )	// Win32 error, but this message is more specific
//      CHK_ERR(DIERR_READONLY, "DIERR_READONLY")
//      CHK_ERR(DIERR_HANDLEEXISTS, "DIERR_HANDLEEXISTS")
CHK_ERR( DIERR_INSUFFICIENTPRIVS, "Unable to IDirectInputJoyConfig_Acquire because the user does not have sufficient privileges to change the joystick configuration. & An invalid media type was specified" )
CHK_ERR( DIERR_DEVICEFULL, "The device is full. & An invalid media subtype was specified." )
CHK_ERR( DIERR_MOREDATA, "Not all the requested information fit into the buffer. & This object can only be created as an aggregated object." )
CHK_ERR( DIERR_NOTDOWNLOADED, "The effect is not downloaded. & The enumerator has become invalid." )
CHK_ERR( DIERR_HASEFFECTS, "The device cannot be reinitialized because there are still effects attached to it. & At least one of the pins involved in the operation is already connected." )
CHK_ERR( DIERR_NOTEXCLUSIVEACQUIRED, "The operation cannot be performed unless the device is acquired in DISCL_EXCLUSIVE mode. & This operation cannot be performed because the filter is active." )
CHK_ERR( DIERR_INCOMPLETEEFFECT, "The effect could not be downloaded because essential information is missing.  For example, no axes have been associated with the effect, or no type-specific information has been created. & One of the specified pins supports no media types." )
CHK_ERR( DIERR_NOTBUFFERED, "Attempted to read buffered device data from a device that is not buffered. & There is no common media type between these pins." )
CHK_ERR( DIERR_EFFECTPLAYING, "An attempt was made to modify parameters of an effect while it is playing.  Not all hardware devices support altering the parameters of an effect while it is playing. & Two pins of the same direction cannot be connected together." )
CHK_ERR( DIERR_UNPLUGGED, "The operation could not be completed because the device is not plugged in. & The operation cannot be performed because the pins are not connected." )
CHK_ERR( DIERR_REPORTFULL, "SendDeviceData failed because more information was requested to be sent than can be sent to the device.  Some devices have restrictions on how much data can be sent to them.  (For example, there might be a limit on the number of buttons that can be pressed at once.) & No sample buffer allocator is available." )
CHK_ERR( DIERR_MAPFILEFAIL, "A mapper file function failed because reading or writing the user or IHV settings file failed. & A run-time error occurred." )

// -------------------------------------------------------------
// dinputd.h error codes
// -------------------------------------------------------------
// CHK_ERR( DIERR_NOMOREITEMS, "No more items." )	Win32 error
CHK_ERR( DIERR_DRIVERFIRST, "Device driver-specific codes. Unless the specific driver has been precisely identified, no meaning should be attributed to these values other than that the driver originated the error." )
CHK_ERR( DIERR_DRIVERFIRST + 1, "DIERR_DRIVERFIRST+1" )
CHK_ERR( DIERR_DRIVERFIRST + 2, "DIERR_DRIVERFIRST+2" )
CHK_ERR( DIERR_DRIVERFIRST + 3, "DIERR_DRIVERFIRST+3" )
CHK_ERR( DIERR_DRIVERFIRST + 4, "DIERR_DRIVERFIRST+4" )
CHK_ERR( DIERR_DRIVERFIRST + 5, "DIERR_DRIVERFIRST+5" )
CHK_ERR( DIERR_DRIVERLAST, "Device installer errors." )
CHK_ERR( DIERR_INVALIDCLASSINSTALLER, "Registry entry or DLL for class installer invalid or class installer not found." )
CHK_ERR( DIERR_CANCELLED, "The user cancelled the install operation. & The stream already has allocated samples and the surface doesn't match the sample format." )
CHK_ERR( DIERR_BADINF, "The INF file for the selected device could not be found or is invalid or is damaged. & The specified purpose ID can't be used for the call." )