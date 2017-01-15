
# pylite 简蟒 ver 0.0.9

A simple, lightweight implementation of python3-like language.

## Roadmap

### ver 0.1

* ~~lexical scope~~

* ~~exception support~~

* ~~try - except - finally~~

* ~~simple import~~

* ~~slice grammar~~

* ~~lambda function~~

* ~~argument unpack~~

* package import

* basic testcase


### ver 0.2

* eval

* xxx if xxx else xxx grammar

* print fix for cycle reference

* filesystem

* ~~IO stream~~

* 'super' type

* repl


### ver 0.3

* dll load

* decorator

* ~~multiple value assign~~

* builtin regex engine

* retval type hints


### ver 0.4

* global/nonlocal

* local variables optimization

* unique integers

* operator overloading support


### ver 0.5

* yield support

* clear memory leaks


### ver 0.6

* make closure pass by reference


### ver 0.X

* f-string

* async/await

* x for x in [...]

* nameless function

* libffi

* JIT



### won't support

* encodings except utf-8/ucs2/ucs4

* while/for - else 

* multiple inheritance

## build

cmake is required, and a c99 compiler:

* gcc 4.8+

* clang 3.4+

* vs2013+

```bash
mkdir build
cd build
cmake ..
```

**make** (Linux/Mac/Mingw)
Open **pylite.sln** and build. (Visual Studio)

## Example

hello.py
```python
print('Hello World!')
```
shell:
```bash
pylite hello.py
```

No REPL yet.


## License
* Zlib
