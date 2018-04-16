if("master" STREQUAL "")
  message(FATAL_ERROR "Tag for git checkout should not be empty.")
endif()

set(run 0)

if("/home/abdulmohsij/Desktop/advanced-topics/DBoW2-master/build/dependencies/src/DLib-stamp/DLib-gitinfo.txt" IS_NEWER_THAN "/home/abdulmohsij/Desktop/advanced-topics/DBoW2-master/build/dependencies/src/DLib-stamp/DLib-gitclone-lastrun.txt")
  set(run 1)
endif()

if(NOT run)
  message(STATUS "Avoiding repeated git clone, stamp file is up to date: '/home/abdulmohsij/Desktop/advanced-topics/DBoW2-master/build/dependencies/src/DLib-stamp/DLib-gitclone-lastrun.txt'")
  return()
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E remove_directory "/home/abdulmohsij/Desktop/advanced-topics/DBoW2-master/build/dependencies/src/DLib"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to remove directory: '/home/abdulmohsij/Desktop/advanced-topics/DBoW2-master/build/dependencies/src/DLib'")
endif()

# try the clone 3 times incase there is an odd git clone issue
set(error_code 1)
set(number_of_tries 0)
while(error_code AND number_of_tries LESS 3)
  execute_process(
    COMMAND "/usr/bin/git" clone --origin "origin" "http://github.com/dorian3d/DLib" "DLib"
    WORKING_DIRECTORY "/home/abdulmohsij/Desktop/advanced-topics/DBoW2-master/build/dependencies/src"
    RESULT_VARIABLE error_code
    )
  math(EXPR number_of_tries "${number_of_tries} + 1")
endwhile()
if(number_of_tries GREATER 1)
  message(STATUS "Had to git clone more than once:
          ${number_of_tries} times.")
endif()
if(error_code)
  message(FATAL_ERROR "Failed to clone repository: 'http://github.com/dorian3d/DLib'")
endif()

execute_process(
  COMMAND "/usr/bin/git" checkout master
  WORKING_DIRECTORY "/home/abdulmohsij/Desktop/advanced-topics/DBoW2-master/build/dependencies/src/DLib"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to checkout tag: 'master'")
endif()

execute_process(
  COMMAND "/usr/bin/git" submodule init 
  WORKING_DIRECTORY "/home/abdulmohsij/Desktop/advanced-topics/DBoW2-master/build/dependencies/src/DLib"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to init submodules in: '/home/abdulmohsij/Desktop/advanced-topics/DBoW2-master/build/dependencies/src/DLib'")
endif()

execute_process(
  COMMAND "/usr/bin/git" submodule update --recursive 
  WORKING_DIRECTORY "/home/abdulmohsij/Desktop/advanced-topics/DBoW2-master/build/dependencies/src/DLib"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to update submodules in: '/home/abdulmohsij/Desktop/advanced-topics/DBoW2-master/build/dependencies/src/DLib'")
endif()

# Complete success, update the script-last-run stamp file:
#
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy
    "/home/abdulmohsij/Desktop/advanced-topics/DBoW2-master/build/dependencies/src/DLib-stamp/DLib-gitinfo.txt"
    "/home/abdulmohsij/Desktop/advanced-topics/DBoW2-master/build/dependencies/src/DLib-stamp/DLib-gitclone-lastrun.txt"
  WORKING_DIRECTORY "/home/abdulmohsij/Desktop/advanced-topics/DBoW2-master/build/dependencies/src/DLib"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to copy script-last-run stamp file: '/home/abdulmohsij/Desktop/advanced-topics/DBoW2-master/build/dependencies/src/DLib-stamp/DLib-gitclone-lastrun.txt'")
endif()

