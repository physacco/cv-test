SET(EXECUTABLES
  show_image
  show_image_gray
  show_raw_mat
  convert_raw_mat
  create_image
  histeq
  webp2jpg
)

FOREACH(EXE ${EXECUTABLES})
  ADD_EXECUTABLE(${EXE} "${EXE}.cpp")
  TARGET_LINK_LIBRARIES(${EXE} ${OpenCV_LIBS})
ENDFOREACH()

ADD_EXECUTABLE(mat2jpg mat2jpg.cpp)
TARGET_LINK_LIBRARIES(mat2jpg jpeg)

ADD_EXECUTABLE(mat2png mat2png.cpp)
TARGET_LINK_LIBRARIES(mat2png png)

ADD_EXECUTABLE(png2ppm png2ppm.cpp ppm.cpp)
TARGET_LINK_LIBRARIES(png2ppm png)

ADD_EXECUTABLE(img2ppm img2ppm.cpp ppm.cpp)
TARGET_LINK_LIBRARIES(img2ppm ${OpenCV_LIBS})
