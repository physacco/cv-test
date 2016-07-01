SET(EXECUTABLES show_image show_image_gray create_image histeq)

FOREACH(EXE ${EXECUTABLES})
  ADD_EXECUTABLE(${EXE} "${EXE}.cpp")
  TARGET_LINK_LIBRARIES(${EXE} ${OpenCV_LIBS})
ENDFOREACH()
