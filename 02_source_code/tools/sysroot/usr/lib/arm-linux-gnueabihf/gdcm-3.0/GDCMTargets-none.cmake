#----------------------------------------------------------------
# Generated CMake target import file for configuration "None".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "gdcmjpeg8" for configuration "None"
set_property(TARGET gdcmjpeg8 APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(gdcmjpeg8 PROPERTIES
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/lib/arm-linux-gnueabihf/libgdcmjpeg8.so.3.0.8"
  IMPORTED_SONAME_NONE "libgdcmjpeg8.so.3.0"
  )

list(APPEND _IMPORT_CHECK_TARGETS gdcmjpeg8 )
list(APPEND _IMPORT_CHECK_FILES_FOR_gdcmjpeg8 "${_IMPORT_PREFIX}/lib/arm-linux-gnueabihf/libgdcmjpeg8.so.3.0.8" )

# Import target "gdcmjpeg12" for configuration "None"
set_property(TARGET gdcmjpeg12 APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(gdcmjpeg12 PROPERTIES
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/lib/arm-linux-gnueabihf/libgdcmjpeg12.so.3.0.8"
  IMPORTED_SONAME_NONE "libgdcmjpeg12.so.3.0"
  )

list(APPEND _IMPORT_CHECK_TARGETS gdcmjpeg12 )
list(APPEND _IMPORT_CHECK_FILES_FOR_gdcmjpeg12 "${_IMPORT_PREFIX}/lib/arm-linux-gnueabihf/libgdcmjpeg12.so.3.0.8" )

# Import target "gdcmjpeg16" for configuration "None"
set_property(TARGET gdcmjpeg16 APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(gdcmjpeg16 PROPERTIES
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/lib/arm-linux-gnueabihf/libgdcmjpeg16.so.3.0.8"
  IMPORTED_SONAME_NONE "libgdcmjpeg16.so.3.0"
  )

list(APPEND _IMPORT_CHECK_TARGETS gdcmjpeg16 )
list(APPEND _IMPORT_CHECK_FILES_FOR_gdcmjpeg16 "${_IMPORT_PREFIX}/lib/arm-linux-gnueabihf/libgdcmjpeg16.so.3.0.8" )

# Import target "gdcmCommon" for configuration "None"
set_property(TARGET gdcmCommon APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(gdcmCommon PROPERTIES
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/lib/arm-linux-gnueabihf/libgdcmCommon.so.3.0.8"
  IMPORTED_SONAME_NONE "libgdcmCommon.so.3.0"
  )

list(APPEND _IMPORT_CHECK_TARGETS gdcmCommon )
list(APPEND _IMPORT_CHECK_FILES_FOR_gdcmCommon "${_IMPORT_PREFIX}/lib/arm-linux-gnueabihf/libgdcmCommon.so.3.0.8" )

# Import target "gdcmDICT" for configuration "None"
set_property(TARGET gdcmDICT APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(gdcmDICT PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_NONE "gdcmDSED;gdcmIOD"
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/lib/arm-linux-gnueabihf/libgdcmDICT.so.3.0.8"
  IMPORTED_SONAME_NONE "libgdcmDICT.so.3.0"
  )

list(APPEND _IMPORT_CHECK_TARGETS gdcmDICT )
list(APPEND _IMPORT_CHECK_FILES_FOR_gdcmDICT "${_IMPORT_PREFIX}/lib/arm-linux-gnueabihf/libgdcmDICT.so.3.0.8" )

# Import target "gdcmDSED" for configuration "None"
set_property(TARGET gdcmDSED APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(gdcmDSED PROPERTIES
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/lib/arm-linux-gnueabihf/libgdcmDSED.so.3.0.8"
  IMPORTED_SONAME_NONE "libgdcmDSED.so.3.0"
  )

list(APPEND _IMPORT_CHECK_TARGETS gdcmDSED )
list(APPEND _IMPORT_CHECK_FILES_FOR_gdcmDSED "${_IMPORT_PREFIX}/lib/arm-linux-gnueabihf/libgdcmDSED.so.3.0.8" )

# Import target "gdcmIOD" for configuration "None"
set_property(TARGET gdcmIOD APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(gdcmIOD PROPERTIES
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/lib/arm-linux-gnueabihf/libgdcmIOD.so.3.0.8"
  IMPORTED_SONAME_NONE "libgdcmIOD.so.3.0"
  )

list(APPEND _IMPORT_CHECK_TARGETS gdcmIOD )
list(APPEND _IMPORT_CHECK_FILES_FOR_gdcmIOD "${_IMPORT_PREFIX}/lib/arm-linux-gnueabihf/libgdcmIOD.so.3.0.8" )

# Import target "gdcmMSFF" for configuration "None"
set_property(TARGET gdcmMSFF APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(gdcmMSFF PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_NONE "gdcmjpeg8;gdcmjpeg12;gdcmjpeg16"
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/lib/arm-linux-gnueabihf/libgdcmMSFF.so.3.0.8"
  IMPORTED_SONAME_NONE "libgdcmMSFF.so.3.0"
  )

list(APPEND _IMPORT_CHECK_TARGETS gdcmMSFF )
list(APPEND _IMPORT_CHECK_FILES_FOR_gdcmMSFF "${_IMPORT_PREFIX}/lib/arm-linux-gnueabihf/libgdcmMSFF.so.3.0.8" )

# Import target "gdcmMEXD" for configuration "None"
set_property(TARGET gdcmMEXD APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(gdcmMEXD PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_NONE "gdcmMSFF;gdcmDICT;gdcmDSED;gdcmIOD"
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/lib/arm-linux-gnueabihf/libgdcmMEXD.so.3.0.8"
  IMPORTED_SONAME_NONE "libgdcmMEXD.so.3.0"
  )

list(APPEND _IMPORT_CHECK_TARGETS gdcmMEXD )
list(APPEND _IMPORT_CHECK_FILES_FOR_gdcmMEXD "${_IMPORT_PREFIX}/lib/arm-linux-gnueabihf/libgdcmMEXD.so.3.0.8" )

# Import target "vtkgdcm" for configuration "None"
set_property(TARGET vtkgdcm APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(vtkgdcm PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_NONE "gdcmMSFF;vtkCommonCore;vtkImagingCore;vtkImagingSources;vtkIOImage;vtkIOLegacy;vtkRenderingCore;vtkRenderingFreeType;vtkRenderingOpenGL2;vtkFiltersParallelDIY2;vtkInteractionStyle;vtkIOMPIImage"
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/lib/arm-linux-gnueabihf/libvtkgdcm.so.3.0.8"
  IMPORTED_SONAME_NONE "libvtkgdcm.so.3.0"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkgdcm )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkgdcm "${_IMPORT_PREFIX}/lib/arm-linux-gnueabihf/libvtkgdcm.so.3.0.8" )

# Import target "vtkgdcmsharpglue" for configuration "None"
set_property(TARGET vtkgdcmsharpglue APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(vtkgdcmsharpglue PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_NONE ""
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/lib/cli/vtkgdcm-sharp-3.0/libvtkgdcmsharpglue.so"
  IMPORTED_NO_SONAME_NONE "TRUE"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkgdcmsharpglue )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkgdcmsharpglue "${_IMPORT_PREFIX}/lib/cli/vtkgdcm-sharp-3.0/libvtkgdcmsharpglue.so" )

# Import target "vtkgdcmJava" for configuration "None"
set_property(TARGET vtkgdcmJava APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(vtkgdcmJava PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_NONE "vtkgdcm"
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/lib/arm-linux-gnueabihf/jni/libvtkgdcmJava.so"
  IMPORTED_NO_SONAME_NONE "TRUE"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkgdcmJava )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkgdcmJava "${_IMPORT_PREFIX}/lib/arm-linux-gnueabihf/jni/libvtkgdcmJava.so" )

# Import target "vtkgdcmPython" for configuration "None"
set_property(TARGET vtkgdcmPython APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(vtkgdcmPython PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_NONE ""
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/lib/python3/dist-packages/vtkgdcmPython.cpython-39-arm-linux-gnueabihf.so"
  IMPORTED_NO_SONAME_NONE "TRUE"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkgdcmPython )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkgdcmPython "${_IMPORT_PREFIX}/lib/python3/dist-packages/vtkgdcmPython.cpython-39-arm-linux-gnueabihf.so" )

# Import target "vtkgdcmPythonD" for configuration "None"
set_property(TARGET vtkgdcmPythonD APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(vtkgdcmPythonD PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_NONE "vtkgdcm;vtkCommonCorePythonD;vtkImagingCorePythonD;vtkImagingSourcesPythonD;vtkIOImagePythonD;vtkIOLegacyPythonD;vtkRenderingCorePythonD;vtkRenderingFreeTypePythonD;vtkRenderingOpenGL2PythonD;vtkFiltersParallelDIY2PythonD;vtkInteractionStylePythonD;vtkIOMPIImagePythonD"
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/lib/arm-linux-gnueabihf/libvtkgdcmPythonD.so.3.0.8"
  IMPORTED_SONAME_NONE "libvtkgdcmPythonD.so.3.0"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkgdcmPythonD )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkgdcmPythonD "${_IMPORT_PREFIX}/lib/arm-linux-gnueabihf/libvtkgdcmPythonD.so.3.0.8" )

# Import target "gdcmdump" for configuration "None"
set_property(TARGET gdcmdump APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(gdcmdump PROPERTIES
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/bin/gdcmdump"
  )

list(APPEND _IMPORT_CHECK_TARGETS gdcmdump )
list(APPEND _IMPORT_CHECK_FILES_FOR_gdcmdump "${_IMPORT_PREFIX}/bin/gdcmdump" )

# Import target "gdcmdiff" for configuration "None"
set_property(TARGET gdcmdiff APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(gdcmdiff PROPERTIES
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/bin/gdcmdiff"
  )

list(APPEND _IMPORT_CHECK_TARGETS gdcmdiff )
list(APPEND _IMPORT_CHECK_FILES_FOR_gdcmdiff "${_IMPORT_PREFIX}/bin/gdcmdiff" )

# Import target "gdcmraw" for configuration "None"
set_property(TARGET gdcmraw APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(gdcmraw PROPERTIES
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/bin/gdcmraw"
  )

list(APPEND _IMPORT_CHECK_TARGETS gdcmraw )
list(APPEND _IMPORT_CHECK_FILES_FOR_gdcmraw "${_IMPORT_PREFIX}/bin/gdcmraw" )

# Import target "gdcmscanner" for configuration "None"
set_property(TARGET gdcmscanner APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(gdcmscanner PROPERTIES
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/bin/gdcmscanner"
  )

list(APPEND _IMPORT_CHECK_TARGETS gdcmscanner )
list(APPEND _IMPORT_CHECK_FILES_FOR_gdcmscanner "${_IMPORT_PREFIX}/bin/gdcmscanner" )

# Import target "gdcmanon" for configuration "None"
set_property(TARGET gdcmanon APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(gdcmanon PROPERTIES
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/bin/gdcmanon"
  )

list(APPEND _IMPORT_CHECK_TARGETS gdcmanon )
list(APPEND _IMPORT_CHECK_FILES_FOR_gdcmanon "${_IMPORT_PREFIX}/bin/gdcmanon" )

# Import target "gdcmgendir" for configuration "None"
set_property(TARGET gdcmgendir APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(gdcmgendir PROPERTIES
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/bin/gdcmgendir"
  )

list(APPEND _IMPORT_CHECK_TARGETS gdcmgendir )
list(APPEND _IMPORT_CHECK_FILES_FOR_gdcmgendir "${_IMPORT_PREFIX}/bin/gdcmgendir" )

# Import target "gdcmimg" for configuration "None"
set_property(TARGET gdcmimg APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(gdcmimg PROPERTIES
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/bin/gdcmimg"
  )

list(APPEND _IMPORT_CHECK_TARGETS gdcmimg )
list(APPEND _IMPORT_CHECK_FILES_FOR_gdcmimg "${_IMPORT_PREFIX}/bin/gdcmimg" )

# Import target "gdcmconv" for configuration "None"
set_property(TARGET gdcmconv APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(gdcmconv PROPERTIES
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/bin/gdcmconv"
  )

list(APPEND _IMPORT_CHECK_TARGETS gdcmconv )
list(APPEND _IMPORT_CHECK_FILES_FOR_gdcmconv "${_IMPORT_PREFIX}/bin/gdcmconv" )

# Import target "gdcmtar" for configuration "None"
set_property(TARGET gdcmtar APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(gdcmtar PROPERTIES
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/bin/gdcmtar"
  )

list(APPEND _IMPORT_CHECK_TARGETS gdcmtar )
list(APPEND _IMPORT_CHECK_FILES_FOR_gdcmtar "${_IMPORT_PREFIX}/bin/gdcmtar" )

# Import target "gdcminfo" for configuration "None"
set_property(TARGET gdcminfo APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(gdcminfo PROPERTIES
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/bin/gdcminfo"
  )

list(APPEND _IMPORT_CHECK_TARGETS gdcminfo )
list(APPEND _IMPORT_CHECK_FILES_FOR_gdcminfo "${_IMPORT_PREFIX}/bin/gdcminfo" )

# Import target "gdcmscu" for configuration "None"
set_property(TARGET gdcmscu APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(gdcmscu PROPERTIES
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/bin/gdcmscu"
  )

list(APPEND _IMPORT_CHECK_TARGETS gdcmscu )
list(APPEND _IMPORT_CHECK_FILES_FOR_gdcmscu "${_IMPORT_PREFIX}/bin/gdcmscu" )

# Import target "gdcmxml" for configuration "None"
set_property(TARGET gdcmxml APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(gdcmxml PROPERTIES
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/bin/gdcmxml"
  )

list(APPEND _IMPORT_CHECK_TARGETS gdcmxml )
list(APPEND _IMPORT_CHECK_FILES_FOR_gdcmxml "${_IMPORT_PREFIX}/bin/gdcmxml" )

# Import target "gdcmpap3" for configuration "None"
set_property(TARGET gdcmpap3 APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(gdcmpap3 PROPERTIES
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/bin/gdcmpap3"
  )

list(APPEND _IMPORT_CHECK_TARGETS gdcmpap3 )
list(APPEND _IMPORT_CHECK_FILES_FOR_gdcmpap3 "${_IMPORT_PREFIX}/bin/gdcmpap3" )

# Import target "gdcmpdf" for configuration "None"
set_property(TARGET gdcmpdf APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(gdcmpdf PROPERTIES
  IMPORTED_LOCATION_NONE "${_IMPORT_PREFIX}/bin/gdcmpdf"
  )

list(APPEND _IMPORT_CHECK_TARGETS gdcmpdf )
list(APPEND _IMPORT_CHECK_FILES_FOR_gdcmpdf "${_IMPORT_PREFIX}/bin/gdcmpdf" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
