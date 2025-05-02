function(run_python_script TARGET_NAME INPUT_FILE OUTPUT_FILE)
    #TARGET_NAME is just a unique identifier
    message(STATUS "Generating mock is running")
    find_package(Python3 REQUIRED COMPONENTS Interpreter)

    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/ut/generateMock.py ${INPUT_FILE} ${OUTPUT_FILE}
        DEPENDS ${CMAKE_SOURCE_DIR}/ut/generateMock.py
        COMMENT "Generating ${INPUT_FILE} -> ${OUTPUT_FILE}"
    )

    add_custom_target(${TARGET_NAME} ALL DEPENDS ${OUTPUT_FILE})
endfunction()
