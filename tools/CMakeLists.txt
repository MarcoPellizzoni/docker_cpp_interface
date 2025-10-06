##############################################################
# TOOLS
##############################################################


set(TGTS "")

set(SHELL_LIB_NAME "Shell" PARENT_SCOPE)
set(SHELL_LIB_NAME "Shell")
add_subdirectory(shell)
list(APPEND TGTS ${SHELL_LIB_NAME})

set_property(
    TARGET ${TGTS}
    PROPERTY FOLDER TOOLS
)

set(TOOLS_LIST ${TGTS} PARENT_SCOPE)

##############################################################