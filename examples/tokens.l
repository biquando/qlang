 // keywords
kwLet       "let"
kwNamespace "namespace"
kwStruct    "struct"
kwReturn    "return"
kwIf        "if"
kwElse      "else"
kwfor       "for"
kwBreak     "break"
kwContinue  "continue"
kwIn        "in"
kwLength    "length"
kwDefer     "defer"

 // types
typeVoid    "void"
typeBool    "bool"
typeChar    "char"
typeU8      "u8"
typeU16     "u16"
typeU32     "u32"
typeU64     "u64"
typeUsize   "usize"
typeI8      "i8"
typeI16     "i16"
typeI32     "i32"
typeI64     "i64"
typeIsize   "isize"
typeF32     "f32"
typeF64     "f64"

 // literals
litDec      [0-9]+
litHex      0x[0-9a-fA-F]+
litFloat    [0-9+](\.[0-9]*)?
litStr      \"([^"\\\n]|\\.)*\"

 // single-char operators
opLParen    "("
opRParen    ")"
opLBracket  "["
opRBracket  "]"
opLBrace    "{"
opRBrace    "}"
opLAngle    "<"
opRAngle    ">"
opPlus      "+"
opMinus     "-"
opStar      "*"
opSlash     "/"
opPercent   "%"
opEquals    "="
opDot       "."
opComma     ","
opColon     ":"
opSemicolon ";"
opAnd       "&"
opOr        "|"
opXor       "^"
opBang      "!"
opAt        "@"
opTilde     "~"

 // multi-char operators
opArrow     "->"
opCmpEq     "=="
opCmpNeq    "!="
opCmpGe     ">="
opCmpLe     "<="
opCmpAnd    "&&"
opCmpOr     "||"
opPlusEq    "+="
opMinusEq   "-="
opStarEq    "*="
opSlashEq   "/="
opPercentEq "%="
opAndEq     "&="
opOrEq      "|="
opXorEq     "^="
opLShift    "<<="
opRShift    ">>="
opLShiftEq  "<<="
opRShiftEq  ">>="

id          [a-zA-Z_][0-9a-zA-Z_]+