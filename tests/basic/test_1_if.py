
#
a = 0

if 1:
    a = 1
    
assert a == 1

#
a = 0

if 0:
    a = 1

assert a == 0

#
if 1:
    a = 1
else:
    a = 2
    
assert a == 1

#
if 0:
    a = 2
else:
    a = 1
    
assert a == 1

#

def func(v):
    ret = 0
    if v == 1:
        ret = v
    elif v == 2:
        ret = 2
    elif v == 3:
        ret = 3
    return ret

for i in range(4):
    assert i == func(i)
