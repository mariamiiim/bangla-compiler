# Bangla Compiler (Lexer + Parser + Symbol Table)

## Folder structure
```
bangla_compiler/
├── include/
│   ├── lexer.h
│   ├── parser.h
│   └── symboltable.h
├── src/
│   ├── lexer.cpp
│   ├── parser.cpp
│   ├── symboltable.cpp
│   └── main.cpp
├── program.bangla     <- sample program, edit this or pass your own file
├── build.bat           <- run this on Windows
├── build.sh            <- run this on Linux/macOS
└── bangla_compiler     <- prebuilt Linux binary (ready to run as-is)
```

## How to run

### Already on Linux (or WSL)?
Just run the prebuilt binary directly, no build step needed:
```
./bangla_compiler program.bangla
```
(if it says "permission denied", run `chmod +x bangla_compiler` first)

### Windows
1. Make sure you have a C++ compiler. Easiest options:
   - Install [MinGW-w64](https://www.mingw-w64.org/) and add it to PATH, **or**
   - Use an IDE like Code::Blocks or Dev-C++ (both bundle g++)
2. Double-click `build.bat` (or run it from a terminal in this folder).
3. This creates `bangla_compiler.exe`.
4. Run it:
   ```
   bangla_compiler.exe program.bangla
   ```

### macOS / Linux (building from source)
```
chmod +x build.sh
./build.sh
./bangla_compiler program.bangla
```

## Usage
```
bangla_compiler <filename.bangla>
```
If you omit the filename, it defaults to looking for `program.bangla` in the current folder.

## What it does
- **Lexer** (`lexer.cpp`) tokenizes Bangla-keyword source code (`ধরি`, `যদি`, `যতক্ষণ`, etc.), numbers, strings, identifiers, and operators.
- **Parser** (`parser.cpp`) checks the token stream against the language grammar (declarations, assignments, `যদি/নাহলে`, `যতক্ষণ` loops, expressions) and reports the exact line/token on a syntax error.
- **Symbol table** (`symboltable.cpp`) tracks declared variables so the parser can catch:
  - using a variable that was never declared
  - declaring the same variable twice

## Language quick reference
| Bangla       | Meaning                  |
|--------------|--------------------------|
| ধরি          | declare a variable       |
| সংখ্যা / লেখা | number / string type     |
| দেখাও        | print/show               |
| যদি ... তবে   | if ... then              |
| নাহলে        | else                     |
| যতক্ষণ ... করো | while ... do            |
| শেষ          | end block                |
| `<-`         | assignment                |

Example (`program.bangla`):
```
ধরি সংখ্যা x <- 10;
ধরি সংখ্যা y <- 20;

দেখাও x + y;

যদি x < y তবে
দেখাও "x ছোট";
নাহলে
দেখাও "x বড়";
শেষ

যতক্ষণ x < 20 করো
x <- x + 1;
শেষ
```
