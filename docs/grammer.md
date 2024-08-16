### Language Grammar Overview

This grammar defines the structure of a simple programming language. The root of the syntax tree is the **Program** node, which consists of a sequence of statements. Below is a summary of the grammar, organized by the types of nodes that make up the Abstract Syntax Tree (AST).

---

#### 1. **Program**
- **Definition**: A Program is a sequence of statements.
- **Grammar**:
  ```
  Program ::= { Statement }
  ```

---

#### 2. **Statement**
A Statement can be one of the following:
- **Assignment**
- **LetStatement**
- **IfStatement**
- **WhileStatement**
- **ForStatement**
- **FunctionDefinition**
- **Return**
- **ExitStatement**

---

#### 3. **Assignment**
- **Definition**: Assigns an expression to an identifier (variable).
- **Grammar**:
  ```
  Assignment ::= Identifier "=" Expression
  ```

---

#### 4. **Expression**
An Expression can be one of the following:
- **IntLiteral**
- **Identifier**
- **BinaryOp**
- **FunctionCall**
- **BlockExpression**

---

#### 5. **IntLiteral**
- **Definition**: Represents an integer literal.
- **Grammar**:
  ```
  IntLiteral ::= INTEGER
  ```

---

#### 6. **Identifier**
- **Definition**: Represents a variable identifier.
- **Grammar**:
  ```
  Identifier ::= IDENTIFIER
  ```

---

#### 7. **BinaryOp**
- **Definition**: Represents a binary operation between two expressions.
- **Grammar**:
  ```
  BinaryOp ::= Expression BinaryOperator Expression
  ```

- **Note**: `BinaryOperator` can be `+`, `-`, `*`, `/`, etc.

---

#### 8. **ExitStatement**
- **Definition**: Represents an exit statement that can include an optional expression.
- **Grammar**:
  ```
  ExitStatement ::= "exit" [ Expression ]
  ```

---

#### 9. **LetStatement**
- **Definition**: Declares a new variable and optionally assigns it a value.
- **Grammar**:
  ```
  LetStatement ::= "let" Identifier [ "=" Expression ]
  ```

---

#### 10. **Block**
- **Definition**: A Block is a sequence of statements enclosed within braces `{}`.
- **Grammar**:
  ```
  Block ::= "{" { Statement } "}"
  ```

---

#### 11. **IfStatement**
- **Definition**: Represents an `if` condition with optional `else` block.
- **Grammar**:
  ```
  IfStatement ::= "if" "(" Expression ")" Block [ "else" Block ]
  ```

---

#### 12. **WhileStatement**
- **Definition**: Represents a `while` loop with a condition and body block.
- **Grammar**:
  ```
  WhileStatement ::= "while" "(" Expression ")" Block
  ```

---

#### 13. **ForStatement**
- **Definition**: Represents a `for` loop with initialization, condition, increment, and body block.
- **Grammar**:
  ```
  ForStatement ::= "for" "(" LetStatement ";" Expression ";" Assignment ")" Block
  ```

---

#### 14. **FunctionDefinition**
- **Definition**: Represents the definition of a function.
- **Grammar**:
  ```
  FunctionDefinition ::= "function" Identifier "(" [ Identifier { "," Identifier } ] ")" Block
  ```

---

#### 15. **Return**
- **Definition**: Represents a return statement that returns the result of an expression.
- **Grammar**:
  ```
  Return ::= "return" Expression
  ```

---

#### 16. **FunctionCall**
- **Definition**: Represents a function call with arguments.
- **Grammar**:
  ```
  FunctionCall ::= Identifier "(" [ Expression { "," Expression } ] ")"
  ```

---

#### 17. **BlockExpression**
- **Definition**: Represents an expression that is a block of statements.
- **Grammar**:
  ```
  BlockExpression ::= Block
  ```

---