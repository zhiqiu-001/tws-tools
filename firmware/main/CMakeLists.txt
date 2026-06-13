set(SOURCES 
    "main.cc"
    "application.cc"
)

set(INCLUDE_DIRS ".")

# Add board common files
list(APPEND SOURCES
    "boards/common/board.cc"
)
list(APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/boards/common)

# Add board files according to BOARD_TYPE
if(CONFIG_BOARD_TYPE_BOSE_OPEN_EARBUD_ULTRA)
    set(BOARD_TYPE "bose-open-earbud-ultra")
elseif(CONFIG_BOARD_TYPE_LE_BOSE_QC_EARBUDS)
    set(BOARD_TYPE "le-bose-qc-earbuds")
endif()


file(GLOB BOARD_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD_TYPE}/*.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD_TYPE}/*.c
    )

list(APPEND SOURCES ${BOARD_SOURCES})

idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS ${INCLUDE_DIRS}
                    WHOLE_ARCHIVE
                    )