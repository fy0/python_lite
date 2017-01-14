
if True:
    pass


if True:
    pass
else:
    pass


test = 1

if test == 1:
    pass
elif test == 2:
    a = 2
elif test == 4:
    a = 2
else:
    a = 3

test = 4

if test == 1:
    pass
elif test == 2:
    a = 2
elif test == 4:
    a = 2
else:
    a = 3

# It's different with cpython
if 1,2,3,4:
    pass


i = 0
while i < 10:
    i += 1


i = 0
while i < 10:
    break

    
i = 0
while i < 10:
    i += 1
    continue
    i = 1000


for i in [1,2,3]:
    a = i
    
for i in range(10):
    a = i
    break
    
    
for i in range(10):
    a = i
    continue
    a += 1

for i in 1, 2, 3: pass
for i, j, k in (): pass
    

if True: pass


if True: pass
else: pass


test = 1

if test == 1: pass
elif test == 2: a = 2
elif test == 4: a = 2
else: a = 3

test = 4

if test == 1: pass
elif test == 2: a = 2
elif test == 4: a = 2
else: a = 3

if 1: pass
if 1: pass
else: pass
if 0: pass
elif 0: pass
if 0: pass
elif 0: pass
elif 0: pass
elif 0: pass
else: pass


i = 0
while i < 10: i += 1

for i in range(10): pass

if 1: pass
if 1:
    pass
if 1:
    #
    #
    #
    pass
    pass
    #
    pass
    #

# test

if not 1: pass
if 1 and 1: pass
if 1 or 1: pass
if not not not 1: pass
if not 1 and 1 and 1: pass
if 1 and 1 or 1 and 1 and 1 or not 1 and 1: pass


# compare

if 1: pass
x = (1 == 1)
if 1 == 1: pass
if 1 != 1: pass
if 1 < 1: pass
if 1 > 1: pass
if 1 <= 1: pass
if 1 >= 1: pass
if 1 is 1: pass
if 1 is not 1: pass
if 1 in (): pass
if 1 not in (): pass
if 1 < 1 > 1 == 1 >= 1 <= 1 != 1 in 1 not in 1 is 1 is not 1: pass
