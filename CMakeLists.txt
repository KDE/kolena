project(kolena)

##################  Kolena version  ################################
set(CMAKE_KOLENA_VERSION_MAJOR 0)
set(CMAKE_KOLENA_VERSION_MINOR 1)
set(CMAKE_KOLENA_VERSION_RELEASE 0)
set(CMAKE_KOLENA_VERSION_STRING "${CMAKE_KOLENA_VERSION_MAJOR}.${CMAKE_KOLENA_VERSION_MINOR}.${CMAKE_KOLENA_VERSION_RELEASE}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${nepomuk_kde_SOURCE_DIR}/cmake)

# search packages used by KDE
set(KDE_MIN_VERSION "4.5.00")
find_package(KDE4 4.5.00 REQUIRED)
include(KDE4Defaults)
find_package(KDE4Workspace)
include(MacroLibrary)

find_package(Tesseract)
find_package(TIFF)

macro_log_feature( TESSERACT_FOUND "Tesseract"
  "A commercial quality OCR engine developed at HP in the 80's and early 90's."
  "http://code.google.com/p/tesseract-ocr/"
  TRUE "" "")
macro_log_feature( TIFF_FOUND "Tiff"
  "Library for manipulation of TIFF (Tag Image File Format) images - Required by the Olena annotation plugin."
  "http://www.remotesensing.org/libtiff/"
  TRUE "" "")


include_directories(
  ${QT_INCLUDES}
  ${KDE4_INCLUDES}
  ${SOPRANO_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_definitions(-DNDEBUG -DHAVE_TESSERACT_3)

set(kolena_SRCS
  olenatextextractionjob.cpp
)

kde4_add_library(kolena SHARED ${kolena_SRCS})

target_link_libraries(
  kolena
  ${QT_QTCORE_LIBRARY}
  ${KDE4_KIO_LIBS}
  ${TESSERACT_LIBRARY}
)

set_target_properties(kolena PROPERTIES VERSION "${CMAKE_KOLENA_VERSION_STRING}" SOVERSION ${CMAKE_KOLENA_VERSION_MAJOR})

install(TARGETS kolena DESTINATION ${LIB_INSTALL_DIR})
install(FILES
  olenatextextractionjob.h
  kolena_export.h
  DESTINATION include/kolena)

macro_display_feature_log()
