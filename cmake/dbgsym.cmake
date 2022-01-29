find_package(Python3 COMPONENTS Interpreter)

if(NOT Python3_FOUND)
    message(FATAL_ERROR "Need Python3")
endif()

set(DBGSYM_GENSYM "${CMAKE_CURRENT_LIST_DIR}/dbgsym-gensym.py")

function(debugsym SOURCE_TARGET DEBUG_TARGET)
    set(SOURCE_FILE "${SOURCE_TARGET}.elf")
    set(DEBUG_FILE "${DEBUG_TARGET}.elf")
    set(DEBUG_SRC "${DEBUG_TARGET}.s")
    set(DEBUG_OBJ "${DEBUG_TARGET}.s.o")
    set(DEBUG_SECTION "${DEBUG_TARGET}.s.dbgsym")

    add_custom_command(
        OUTPUT ${DEBUG_SRC}
        COMMAND "${DBGSYM_GENSYM}"
            -i "${SOURCE_FILE}"
            -o "${DEBUG_SRC}"
            --mcu="${CMX_MCUNAME}"
            --dump-util="${CMAKE_OBJDUMP}"
        DEPENDS ${SOURCE_TARGET}
        COMMENT "Dumping Symbol Table"
    )

    add_custom_command(
        OUTPUT ${DEBUG_OBJ}
        COMMAND "${CMAKE_ASM_COMPILER}"
            -o "${DEBUG_OBJ}"
            -c "${DEBUG_SRC}"
        DEPENDS ${DEBUG_SRC}
        COMMENT "Assembling Symbol Table Dump"
    )

    add_custom_command(
        OUTPUT ${DEBUG_SECTION}
        COMMAND "${CMAKE_OBJCOPY}"
            --only-section .dbgsym
            -O binary
            "${DEBUG_OBJ}"
            "${DEBUG_SECTION}"
        DEPENDS ${DEBUG_OBJ}
        COMMENT "Perparing Symbol Table Dump for inclusion"
    )

    add_custom_command(
        OUTPUT ${DEBUG_FILE}
        COMMAND "${CMAKE_OBJCOPY}"
            --set-section-flags ".dbgsym=contents,alloc,readonly,data"
            --update-section ".dbgsym=${DEBUG_SECTION}"
            --set-section-flags ".dbgsym=contents,alloc,readonly,data"
            "${SOURCE_FILE}"
            "${DEBUG_FILE}"
        DEPENDS ${SOURCE_TARGET} ${DEBUG_SECTION}
        COMMENT "Inserting Symbol Table Dump into Target"
    )

    add_custom_target(
        ${DEBUG_TARGET}
        DEPENDS ${DEBUG_FILE}
    )

    add_custom_command(
        TARGET ${DEBUG_TARGET} POST_BUILD
        COMMAND "${CMAKE_SIZE_UTIL}"
        ${DEBUG_FILE}
    )

    mcu_elf2lst(${DEBUG_TARGET})
    mcu_elf2bin(${DEBUG_TARGET})
endfunction()
