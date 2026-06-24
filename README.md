# NatrixLanguage

NatrixLanguage is an interpreted programming language written in C++20.

The project includes:

* Lexer
* Parser
* Runtime interpreter
* Error reporting system

No third-party libraries are used.

---

## Requirements

### Development

* Visual Studio 2022
* MSVC v143 Toolset
* C++20

### Runtime

Tested only on:

* Windows 10

---

## Build

Open the solution in Visual Studio 2022 and build the project.

Configuration:

* Toolset: MSVC v143
* Language Standard: C++20

No external dependencies are required.

---

## Running

Currently the interpreter loads source code from:

```text
first.npp
```

The file must be located in the same directory as the executable.

Example:

```text
NatrixLanguage.exe
first.npp
```

After launching the executable, the interpreter reads and executes the contents of `first.npp`.

---

# Language Overview

## Variables

```natrix
var a = 10;
var b = 5;
```

---

## Functions

```natrix
func Sum(a, b)
{
    return a + b;
}
```

---

## Conditions

```natrix
if (a > b) {
    return a;
}
else if (b > c) {
    return b;
}
else {
    return c;
}
```

---

## Loops

### While

```natrix
while (a < 10) {
    a++;
}
```

### For

```natrix
for (var i = 0; i < 10; i++)
{
}
```

---

## Supported Types

* Integer
* Floating Point
* Boolean
* String

---

# Error Codes

## Lexer Errors

| Code | Description     |
| ---- | --------------- |
| 1000 | Invalid syntax  |
| 1001 | Incorrect value |

## Parser Errors

| Code | Description                                  |
| ---- | -------------------------------------------- |
| 2000 | Invalid syntax                               |
| 2001 | Incorrect number of arguments                |
| 2002 | Unknown token                                |
| 2003 | Cannot apply operator to rvalue              |
| 2004 | The operator is not applicable to this value |
| 2005 | Incorrect value                              |
| 2006 | Unknown type                                 |

---

# Example

```natrix
func Factorial(n) {
    var result = 1;

    while (n > 1) {
        result *= n;
        n--;
    }

    return result;
}

var value = Factorial(5);
print(value);
```