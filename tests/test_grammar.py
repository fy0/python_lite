# coding: utf-8
# Interpreter: python3
# This just tests whether the parser accepts them all.

import os

PYLT_BIN = './pylite'

for i in os.listdir('grammar'):
    if i.endswith('.py'):
        ret = os.popen('"%s" "./grammar/%s"' % (PYLT_BIN, i)).read()

        if 'ERROR at line' in ret:
            state = 'X'
        elif 'Traceback (most recent call last)' in ret:
            state = '-'
        else:
            state = 'O'
        print(ret)

        print('[%s] %s' % (state, i[:-3]))
        if state == 'X':
            i = ret.index('ERROR')
            print(ret[i:].strip())
