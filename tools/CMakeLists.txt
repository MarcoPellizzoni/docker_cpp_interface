##############################################################
# TOOLS
##############################################################


set(TGTS "")

set(SHELL_LIB_NAME "shell" PARENT_SCOPE)
set(SHELL_LIB_NAME "shell")
add_subdirectory(shell)
list(APPEND TGTS ${SHELL_LIB_NAME})

set_property(
    TARGET ${TGTS}
    PROPERTY FOLDER Tools
)

set(TOOLS_LIST ${TGTS} PARENT_SCOPE)

##############################################################