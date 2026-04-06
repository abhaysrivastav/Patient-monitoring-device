# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "CMakeFiles\\PatientMonitor_Widgets_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\PatientMonitor_Widgets_autogen.dir\\ParseCache.txt"
  "PatientMonitor_Widgets_autogen"
  )
endif()
