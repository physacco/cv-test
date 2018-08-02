SET(EXECUTABLES show_image show_image_gray show_raw_mat create_image histeq webp2jpg img2ppm)

FOREACH(EXE ${EXECUTABLES})
  ADD_EXECUTABLE(${EXE} "${EXE}.cpp")
  TARGET_LINK_LIBRARIES(${EXE} ${OpenCV_LIBS})
ENDFOREACH()
