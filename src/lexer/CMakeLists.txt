set(LEXER_SRC
  Node.cpp
  RegexParsing.cpp
  State.cpp
  StateMachine.cpp
)

add_library(lexer ${LEXER_SRC})
target_include_directories(lexer PUBLIC ../../include)
target_compile_options(lexer PRIVATE -Wall -Wextra -pedantic)

install(TARGETS lexer)
