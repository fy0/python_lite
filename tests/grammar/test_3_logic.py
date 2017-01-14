
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


i = 0
while i < 10: i += 1

for i in range(10): pass
