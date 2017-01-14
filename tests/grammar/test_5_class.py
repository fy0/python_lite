
class B: pass
class B2(): pass
class C1(B): pass
class C2(B): pass
#class D(C1, C2, B): pass
class C:
    def meth1(self): pass
    def meth2(self, arg): pass
    def meth3(self, a1, a2): pass

#def class_decorator(x): return x
#@class_decorator
#class G: pass


class A:
    def test(self):
        print(self)
        return 1
        
    def test2(self, a):
        print(a)

    def __call__(self):
        print('__call__', self, type(self))
        
    def __hash__(self):
        return 111

a = A()
b = A()
A.b = 1

class B(A):
    pass

b = B()

assert B.__base__ == A
