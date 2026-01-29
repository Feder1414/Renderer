cmake_minimum_required(VERSION 3.29)
project(graficos)

function(copyDirectories target_name)
    foreach(directory IN LISTS ARGN)
        set(custom_target_name "${target_name}_${directory}")
        add_custom_target(${custom_target_name}
                COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/${directory}"
                "$<TARGET_FILE_DIR:${target_name}>/${directory}"
                COMMENT "Copying data"
        )
        add_dependencies(${target_name} ${custom_target_name})

    endforeach ()
endfunction()



set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)
add_subdirectory(external)

add_subdirectory(src)




#add_executable(graficos main.cpp
#        src/Model.h)

#target_link_libraries(graficos PRIVATE
#        glfw3
#        glad
#        OpenGL::GL)


#add_subdirectory(cmake-4.2.0-tutorial-source/Step2)
#add_subdirectory(cmake-4.2.0-tutorial-source/Step3)
#add_subdirectory(cmake-4.2.0-tutorial-source/Step4)
#add_subdirectory(cmake-4.2.0-tutorial-source/Step5)
#add_subdirectory(cmake-4.2.0-tutorial-source/Step6)
#add_subdirectory(cmake-4.2.0-tutorial-source/Step7)
#add_subdirectory(cmake-4.2.0-tutorial-source/Step8)
#add_subdirectory(cmake-4.2.0-tutorial-source/Step9)
#add_subdirectory(cmake-4.2.0-tutorial-source/Step10)
#add_subdirectory(cmake-4.2.0-tutorial-source/Step11)


