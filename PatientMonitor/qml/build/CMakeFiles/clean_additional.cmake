# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\PatientMonitor_QML_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\PatientMonitor_QML_autogen.dir\\ParseCache.txt"
  "PatientMonitor_QML_autogen"
  )
endif()
