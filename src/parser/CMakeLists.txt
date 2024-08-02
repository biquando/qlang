set(PARSER_SRC
  IndentedStream.cpp
  ParseContext.cpp
  Token.cpp
)

add_library(parser ${PARSER_SRC})
target_include_directories(parser PUBLIC ../../include)

if(MSVC)
  target_compile_options(parser PRIVATE /Wall)
else()
  target_compile_options(parser PRIVATE -Wall -Wextra -pedantic)
endif()

install(TARGETS parser)
