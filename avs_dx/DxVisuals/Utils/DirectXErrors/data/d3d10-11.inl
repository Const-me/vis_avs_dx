// -------------------------------------------------------------
// d3d10.h error codes
// -------------------------------------------------------------
CHK_ERR( D3D10_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS, "The application has exceeded the maximum number of unique state objects per Direct3D device. The limit is 4096 for feature levels up to 11.1." )
CHK_ERR( D3D10_ERROR_FILE_NOT_FOUND, "The specified file was not found." )

// -------------------------------------------------------------
// dxgi.h error codes
// -------------------------------------------------------------
CHK_ERR( DXGI_STATUS_OCCLUDED, "The target window or output has been occluded. The application should suspend rendering operations if possible." )
CHK_ERR( DXGI_STATUS_CLIPPED, "The Present operation was partially invisible to the user." )
CHK_ERR( DXGI_STATUS_NO_REDIRECTION, "The driver is requesting that the DXGI runtime not use shared resources to communicate with the Desktop Window Manager." )
CHK_ERR( DXGI_STATUS_NO_DESKTOP_ACCESS, "The Present operation was not visible because the Windows session has switched to another desktop (for example, ctrl-alt-del)." )
CHK_ERR( DXGI_STATUS_GRAPHICS_VIDPN_SOURCE_IN_USE, "The Present operation was not visible because the target monitor was being used for some other purpose." )
CHK_ERR( DXGI_STATUS_MODE_CHANGED, "The Present operation was not visible because the display mode changed. DXGI will have re-attempted the presentation." )
CHK_ERR( DXGI_STATUS_MODE_CHANGE_IN_PROGRESS, "The Present operation was not visible because another Direct3D device was attempting to take fullscreen mode at the time." )
CHK_ERR( DXGI_ERROR_INVALID_CALL, "The application made a call that is invalid. Either the parameters of the call or the state of some object was incorrect. Enable the D3D debug layer in order to see details via debug messages." )
CHK_ERR( DXGI_ERROR_NOT_FOUND, "The item requested was not found. For GetPrivateData calls, this means that the specified GUID had not been previously associated with the object. If calling IDXGIFactory::EnumAdaptes, there is no adapter with the specified ordinal." )
CHK_ERR( DXGI_ERROR_MORE_DATA, "The caller did not supply a sufficiently large buffer." )
CHK_ERR( DXGI_ERROR_UNSUPPORTED, "The specified device interface or feature level is not supported on this system." )
CHK_ERR( DXGI_ERROR_DEVICE_REMOVED, "The GPU device instance has been suspended. Use GetDeviceRemovedReason to determine the appropriate action." )
CHK_ERR( DXGI_ERROR_DEVICE_HUNG, "The GPU will not respond to more commands, most likely because of an invalid command passed by the calling application." )
CHK_ERR( DXGI_ERROR_DEVICE_RESET, "The GPU will not respond to more commands, most likely because some other application submitted invalid commands. The calling application should re-create the device and continue." )
CHK_ERR( DXGI_ERROR_WAS_STILL_DRAWING, "The GPU was busy at the moment when the call was made, and the call was neither executed nor scheduled." )
CHK_ERR( DXGI_ERROR_FRAME_STATISTICS_DISJOINT, "The requested functionality is not supported by the device or the driver." )
CHK_ERR( DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE, "The requested functionality is not supported by the device or the driver." )
CHK_ERR( DXGI_ERROR_DRIVER_INTERNAL_ERROR, "An internal driver error occurred." )
CHK_ERR( DXGI_ERROR_NONEXCLUSIVE, "The application attempted to perform an operation on an DXGI output that is only legal after the output has been claimed for exclusive owenership." )
CHK_ERR( DXGI_ERROR_NOT_CURRENTLY_AVAILABLE, "The requested functionality is not supported by the device or the driver." )
CHK_ERR( DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED, "Remote desktop client disconnected." )
CHK_ERR( DXGI_ERROR_REMOTE_OUTOFMEMORY, "The device has been removed during a remote session because the remote computer ran out of memory." )

CHK_ERR( DXGI_ERROR_ACCESS_LOST, "The keyed mutex was abandoned.")
CHK_ERR( DXGI_ERROR_WAIT_TIMEOUT, "The timeout value has elapsed and the resource is not yet available." )
CHK_ERR( DXGI_ERROR_SESSION_DISCONNECTED, "The output duplication has been turned off because the Windows session ended or was disconnected. This happens when a remote user disconnects, or when \"switch user\" is used locally." )
CHK_ERR( DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE, "The DXGI outuput (monitor) to which the swapchain content was restricted, has been disconnected or changed." )
CHK_ERR( DXGI_ERROR_CANNOT_PROTECT_CONTENT, "DXGI is unable to provide content protection on the swapchain. This is typically caused by an older driver, or by the application using a swapchain that is incompatible with content protection." )
CHK_ERR( DXGI_ERROR_ACCESS_DENIED, "The application is trying to use a resource to which it does not have the required access privileges. This is most commonly caused by writing to a shared resource with read-only access." )
CHK_ERR( DXGI_ERROR_NAME_ALREADY_EXISTS, "The application is trying to create a shared handle using a name that is already associated with some other resource." )
CHK_ERR( DXGI_ERROR_SDK_COMPONENT_MISSING, "The application requested an operation that depends on an SDK component that is missing or mismatched." )
CHK_ERR( DXGI_ERROR_NOT_CURRENT, "The DXGI objects that the application has created are no longer current & need to be recreated for this operation to be performed." )
CHK_ERR( DXGI_ERROR_HW_PROTECTION_OUTOFMEMORY, "Insufficient HW protected memory exits for proper function." )
CHK_ERR( DXGI_ERROR_DYNAMIC_CODE_POLICY_VIOLATION, "Creating this device would violate the process's dynamic code policy." )
CHK_ERR( DXGI_ERROR_NON_COMPOSITED_UI, "The operation failed because the compositor is not in control of the output." )

// DXGI errors that are internal to the Desktop Window Manager
CHK_ERR( DXGI_STATUS_UNOCCLUDED, "The swapchain has become unoccluded." )
CHK_ERR( DXGI_STATUS_DDA_WAS_STILL_DRAWING, "The adapter did not have access to the required resources to complete the Desktop Duplication Present() call, the Present() call needs to be made againc" )
CHK_ERR( DXGI_ERROR_MODE_CHANGE_IN_PROGRESS, "An on-going mode change prevented completion of the call. The call may succeed if attempted later." )
CHK_ERR( DXGI_STATUS_PRESENT_REQUIRED, "The present succeeded but the caller should present again on the next V-sync, even if there are no changes to the content." )

// DXGI errors that are produced by the D3D Shader Cache component
CHK_ERR( DXGI_ERROR_CACHE_CORRUPT, "The cache is corrupt and either could not be opened or could not be reset." )
CHK_ERR( DXGI_ERROR_CACHE_FULL, "This entry would cause the cache to exceed its quota. On a load operation, this may indicate exceeding the maximum in-memory size." )
CHK_ERR( DXGI_ERROR_CACHE_HASH_COLLISION, "A cache entry was found, but the key provided does not match the key stored in the entry." )
CHK_ERR( DXGI_ERROR_ALREADY_EXISTS, "The desired element already exists." )

// DXGI DDI
CHK_ERR( DXGI_DDI_ERR_WASSTILLDRAWING, "The GPU was busy when the operation was requested." )
CHK_ERR( DXGI_DDI_ERR_UNSUPPORTED, "The driver has rejected the creation of this resource." )
CHK_ERR( DXGI_DDI_ERR_NONEXCLUSIVE, "The GPU counter was in use by another process or d3d device when application requested access to it." )

// -------------------------------------------------------------
// d3d11.h error codes
// -------------------------------------------------------------
CHK_ERR( D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS, "There are too many unique state objects." )
CHK_ERR( D3D11_ERROR_FILE_NOT_FOUND, "File not found" )
CHK_ERR( D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS, "The application has exceeded the maximum number of unique view objects per Direct3D device. The limit is 2^20 for feature levels up to 11.1." )
CHK_ERR( D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD, "Deferred context requires Map-Discard usage pattern" )