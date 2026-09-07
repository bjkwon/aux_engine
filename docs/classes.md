# Class definitions

## Purpose

A class packages a fixed set of named member variables with methods that operate on them. An
instance is a struct carrying a hidden class tag; the tag is what routes `obj.method(...)` to the
class's code. The member set is fixed at construction — declared members may be reassigned, but no
new member can be added at runtime — while member *values* remain type-blind, like every other AUX
variable.

**Implementation:** `src/engine/xcope.cpp` (`parse_class_source`, `ensure_class_loaded`,
`try_instantiate_class_call`, `AuxScope::read_node`, `AuxScope::PrepareAndCallUDF`,
`AuxScope::SetVar`).

---

## Defining a class

A class lives in a `.aux` file named after the class. `myclass.aux`:

```text
class myclass
// member variable declarations
id=0
name=""
grade

// constructor; optional, named after the class
method myclass(i,n)
id=i
name=n

// method
method out=myfunc(arg)
out=name++sprintf("%d",id)
```

The file must begin with `class <Name>`. A trailing `end` is accepted but not required; without
one the definition runs to end of file.

### Member declarations

- Every member must be declared in the class file. `name=expr` gives a default; a bare `name`
  declares the member with a null value (`[]`).
- Defaults are evaluated once, when the class is first loaded.
- **Members must be declared before the first method.** A declaration placed after a `method`
  header is swallowed into that method's body and silently becomes an ordinary local — the class
  ends up without the member. This is a known rough edge; keep all declarations at the top.

### Methods

- A method runs from its header to the next `method`, an `end`, or end of file.
- Control-flow blocks inside a body (`if`, `for`, `while`, `switch`, `try`) use `end` as usual and
  are matched correctly.
- Members are visible inside a method body as bare names. Assigning one updates the object.
- A local variable that is not a declared member stays local; it does not become a member.
- **A method parameter may not share a name with a member.** Because members are bare names, a
  same-named parameter would hide the member for the whole call, leaving it unreachable. The class
  fails to load:

  ```text
  Method "show" parameter "id" collides with member "id" in class "shadow". Rename the parameter.
  ```

  Rename the parameter — this is why the constructor above takes `i,n` for members `id,name`.

### Constructor

The constructor is a method named after the class. It is optional; without one, an instance takes
the declared defaults. `method init(...)` is also accepted as a constructor name for
backward compatibility, but the class-named form is preferred.

---

## Using a class

```text
u = myclass(100,"bjkwon")   // construct
u.id                        // read a member       -> 100
u.grade = "A"               // write a declared member
u.myfunc(0)                 // call a method       -> "bjkwon100"
```

### Instantiation

Construction is a call, so it always takes parentheses — see
[syntax_conventions.md](syntax_conventions.md).

```text
u = myclass(100,"bjkwon")   // with constructor arguments
u = myclass()               // no constructor, or a constructor taking no arguments
```

A bare `myclass` is *not* a construction; it is an ordinary identifier lookup. If no variable of
that name exists, the class file is currently reported as a failed UDF parse rather than as an
undefined variable, so the diagnostic is poor. Always write `myclass()`.

A variable shadows a class of the same name, as it would shadow a function.

### Members

Declared members may be reassigned freely, including to a different type — members are type-blind:

```text
u.id = 0            // declared as a number
u.id = "unknown"    // now a string; allowed
```

Assigning an undeclared name is an error:

```text
u.address = "1234"
   Cannot add undeclared member .address to class object.
```

### Value semantics

Objects are values, not references — assignment copies, matching MATLAB. A method mutates the
object it is called on, but a copy made earlier is unaffected:

```text
p = mut()      // member v is 1
q = p          // copy
q.bump()       // increments v on q only
q.v            // -> 11
p.v            // -> 1
```

### Method dispatch

`obj.method(...)` is resolved through the receiver's class. A method is reached the same way from
anywhere, including from inside a UDF that happens to define a local subfunction of the same name —
the receiver determines the target, and the caller's locals do not shadow it.

A method cannot call a sibling method: there is no receiver in scope inside a method body, and an
unqualified call does not resolve to the class.

### Name casing

Class names and method names are case-insensitive; **member names are case-sensitive.**

```text
q = CASES()     // same class as cases()
c.GetN()        // same method as c.getn()
m.Val           // not the same member as m.val
```

---

## Reserved names

`__class` holds the internal class tag. It is hidden from previews, from `aux_get_struct`, and from
the member count, and it cannot be read or written from a script:

```text
u.__class
   .__class is reserved for internal class metadata.
```

A class file that declares a member named `__class` fails to load.

---

## Not supported

- **Inheritance.** `class Name` takes a bare name; there is no `extends` form.
- **Standalone `method` or `member function` files.** Method syntax is valid only inside a class
  definition. A `.aux` file whose first code line starts with either is rejected:

  ```text
  Standalone UDF files cannot start with "method" or "member function".
  Use "function" for a UDF or place a "method" inside a class definition.
  ```

  Inside a class file, methods must use `method`, not `member function`.
- **Deleting a member** from an instance.
- **Reloading a changed class file.** A class is parsed once per session and cached; editing the
  file afterward has no effect until the session restarts. Ordinary UDFs do hot-reload.
