
# value assign

a = 1
简蟒 = 2
a2, b2 = 1, 2
[a3, b3] = 1, 2
(a4, b4) = 1, 2

a, b = b, a = a4, b4
a, b = b, a

a5, b5 = b5, a5 = 1, 2

# misc

pass
assert 1
assert 1,2

# operator

1 or 2
1 and 0 or 2

a == 1
a != 1
a < 2
a <= 2
a > 2
a >= 2
a == 1,2
a == (1,2)

lst = [1,2,3]
1 in lst
1 not in lst
1 + 1 not in lst
1 is 1
1 is not (1,)
b = a | 简蟒
c = a ^ 简蟒
d = a & 简蟒
a << 1
a >> 1
+a + -a
a * b
a / b
a // b
a % b
a ** 2

lst = [1,2,3]
lst[1]
list.__base__
list.__base__.__base__

lst[:]
lst[:3]
lst[:-1]
lst[0:3]
lst[0:-1]
lst[-1:]
lst[0:10]

lst[0] = 5
lst[:2] = 1,2
lst[:2] = [1,2]


# del
a = 1
del a

a = [1,2,3]
del a[0]

a = [1,2,3]
del a[:2]

a = [1,2,3]
del a[:]

a = [1,2,3]
del a[:10]


a = [1,2,3]
del a[1:10]

a = [1,2,3]
del a[1:-1]

a = [1,2,3]
del a[:-1]

a = [1,2,3]
del a[1:]

a = [1,2,3,4,5]
del a[::2]

class _:
    pass

a = _()
a.aaa = 1
del a.aaa
