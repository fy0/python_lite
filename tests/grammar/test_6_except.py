
# raise

try: raise RuntimeError('just testing')
except RuntimeError: pass
try: raise KeyboardInterrupt
except KeyboardInterrupt: pass


# try - except - finally

try:
    1/0
except ZeroDivisionError:
    pass

try: 1/0
except EOFError: pass
except TypeError as msg: pass
except RuntimeError as msg: pass
except: pass

try: 1/0
except (EOFError, TypeError, ZeroDivisionError): pass
try: 1/0
except (EOFError, TypeError, ZeroDivisionError) as msg: pass
try: pass
finally: pass


try:
    a = 1
    asd
except OSError:
    b = 2
except NameError as e:
    #print(e)
    b = 4
except:
    b = 3
finally:
    b = 10


try:
    a = 1
except OSError:
    pass
except NameError:
    pass
except Exception as e:
    print(e)
    a = 2
finally:
    d = 4

c = 3

try:
    for i, j, k in ((1,2),):
        pass
except ValueError:
    pass

try:
    for i, j, k in ((1,2,3,4),):
        pass
except ValueError:
    pass
