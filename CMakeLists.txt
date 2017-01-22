add_clang_executable(clang-c-ast
  clang-c-ast.cpp
  )

if (LLVM_BUILD_STATIC)
  target_link_libraries(clang-c-ast
    libclang_static
    )
else()
  target_link_libraries(clang-c-ast
    libclang
    )
endif()

set_target_properties(clang-c-ast
  PROPERTIES
  LINKER_LANGUAGE CXX)

target_link_libraries(clang-c-ast
  clangAST
  )

