
def f1(): pass
f1()
f1(*())
f1(*(), **{})
def f2(one_argument): pass
def f3(two, arguments): pass

def a1(one_arg,): pass
def a2(two, args,): pass
def v0(*rest): pass
def v1(a, *rest): pass
def v2(a, b, *rest): pass

f1()
f2(1)
f2(1,)
f3(1, 2)
f3(1, 2,)
v0()
v0(1)
v0(1,)
v0(1,2)
v0(1,2,3,4,5,6,7,8,9,0)
v1(1)
v1(1,)
v1(1,2)
v1(1,2,3)
v1(1,2,3,4,5,6,7,8,9,0)
v2(1,2)
v2(1,2,3)
v2(1,2,3,4)
v2(1,2,3,4,5,6,7,8,9,0)

def d01(a=1): pass
d01()
d01(1)
d01(*(1,))
d01(**{'a':2})
def d11(a, b=1): pass
d11(1)
d11(1, 2)
d11(1, **{'b':2})
def d21(a, b, c=1): pass
d21(1, 2)
d21(1, 2, 3)
d21(*(1, 2, 3))
d21(1, *(2, 3))
d21(1, 2, *(3,))
d21(1, 2, **{'c':3})
def d02(a=1, b=2): pass
d02()
d02(1)
d02(1, 2)
d02(*(1, 2))
d02(1, *(2,))
d02(1, **{'b':2})
d02(**{'a': 1, 'b': 2})
def d12(a, b=1, c=2): pass
d12(1)
d12(1, 2)
d12(1, 2, 3)
def d22(a, b, c=1, d=2): pass
d22(1, 2)
d22(1, 2, 3)
d22(1, 2, 3, 4)
def d01v(a=1, *rest): pass
d01v()
d01v(1)
d01v(1, 2)
d01v(*(1, 2, 3, 4))
d01v(*(1,))
d01v(**{'a':2})
def d11v(a, b=1, *rest): pass
d11v(1)
d11v(1, 2)
d11v(1, 2, 3)
def d21v(a, b, c=1, *rest): pass
d21v(1, 2)
d21v(1, 2, 3)
d21v(1, 2, 3, 4)
d21v(*(1, 2, 3, 4))
d21v(1, 2, **{'c': 3})
def d02v(a=1, b=2, *rest): pass
d02v()
d02v(1)
d02v(1, 2)
d02v(1, 2, 3)
d02v(1, *(2, 3, 4))
d02v(**{'a': 1, 'b': 2})
def d12v(a, b=1, c=2, *rest): pass
d12v(1)
d12v(1, 2)
d12v(1, 2, 3)
d12v(1, 2, 3, 4)
d12v(*(1, 2, 3, 4))
d12v(1, 2, *(3, 4, 5))
d12v(1, *(2,), **{'c': 3})
def d22v(a, b, c=1, d=2, *rest): pass
d22v(1, 2)
d22v(1, 2, 3)
d22v(1, 2, 3, 4)
d22v(1, 2, 3, 4, 5)
d22v(*(1, 2, 3, 4))
d22v(1, 2, *(3, 4, 5))
d22v(1, *(2, 3), **{'d': 4})

'''
# not supported now
def pos0key1(*, key): return key
pos0key1(key=100)
def pos2key2(p1, p2, *, k1, k2=100): return p1,p2,k1,k2
pos2key2(1, 2, k1=100)
pos2key2(1, 2, k1=100, k2=200)
pos2key2(1, 2, k2=100, k1=200)
def pos2key2dict(p1, p2, *, k1=100, k2, **kwarg): return p1,p2,k1,k2,kwarg
pos2key2dict(1,2,k2=100,tokwarg1=100,tokwarg2=200)
pos2key2dict(1,2,tokwarg1=100,tokwarg2=200, k2=100)
'''


# test MAKE_CLOSURE with a variety of oparg's
closure = 1
def f(): return closure
def f(x=1): return closure
#def f(*, k=1): return closure
#def f() -> int: return closure

l1 = lambda : 0
#self.assertEqual(l1(), 0)
l2 = lambda : a[d] # XXX just testing the expression
#l3 = lambda : [2 < x for x in [-1, 3, 0]]
#self.assertEqual(l3(), [0, 1, 0])
l4 = lambda x = lambda y = lambda z=1 : z : y() : x()
#self.assertEqual(l4(), 1)
l5 = lambda x, y, z=2: x + y + z
#self.assertEqual(l5(1, 2), 5)
#self.assertEqual(l5(1, 2, 3), 6)
#l6 = lambda x, y, *, k=20: x+y+k
