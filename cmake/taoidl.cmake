# tao_idl custom command
set(idls_)
foreach(idl ${IDL})
	set(idls_ ${idls_} ${IDL_DIR}/${idl})
endforeach(idl)

add_custom_command(
	OUTPUT ${IDL_OUTPUT}
	DEPENDS ${idls_}
	COMMAND tao_idl
	ARGS ${idls_} -o "${STUBS_DIR}"
	COMMENT "Generating stubs + skeletons"
	WORKING_DIRECTORY ${IDL_DIR}
	VERBATIM)

add_custom_target(${PROJECT_NAME}_idl DEPENDS ${IDL_OUTPUT})
