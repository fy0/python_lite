
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>

#include "cio.h"
#include "../api.h"
#include "../bind.h"
#include "../types/all.h"
#include "../utils/misc.h"
#include "../utils/io.h"


PyLiteObject* pylt_mods_cio_fopen(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    PyLiteStrObject *fn = caststr(args[0]);
    PyLiteStrObject *mode = caststr(args[1]);
    FILE *fp = pylt_io_fopen(I, fn, mode);

    if (!fp) {
        switch (errno) {
            case ENOENT:
                pl_error(I, pl_static.str.FileNotFoundError, "[Errno 2] No such file or directory: %r", args[0]);
                break;
            case EACCES: // open dir, windows
                pl_error(I, pl_static.str.PermissionError, "[Errno 13] Permission denied: %r", args[0]);
                break;
            case EISDIR: // open dir, linux
                pl_error(I, pl_static.str.IsADirectoryError, "[Errno 21] Is a directory: %r", args[0]);
                break;
            default:
                pl_error(I, pl_static.str.OSError, "[Errno %d] File Open Error: %r", pylt_obj_int_new(I, errno), args[0]);
        }
    }

    return castobj(pylt_obj_cptr_new(I, fp, false));
}

PyLiteObject* pylt_mods_cio_fclose(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_int_new(I, fclose(castcptr(args[0])->ob_ptr)));
}


PyLiteObject* pylt_mods_cio_fsize(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    FILE *fp = castcptr(args[0])->ob_ptr;
    struct stat stbuf;
    fstat(fileno(fp), &stbuf);    
    return castobj(pylt_obj_int_new(I, stbuf.st_size));
}


PyLiteObject* pylt_mods_cio_fread(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    // 这里有必要提一句，在Windows上，'r'模式下，fread之后的ftell就是一定会返回错误的值！
    // 不是BUG！切到'rb'就好了
    FILE *fp = castcptr(args[0])->ob_ptr;
    pl_int_t size = castint(args[1])->ob_val;

    /*long pos = ftell(fp);
    fseek(fp, 0, SEEK_END);
    long maxsize = ftell(fp) - pos;
    fseek(fp, pos, SEEK_SET);
    if (size > maxsize) size = maxsize;*/

    uint8_t *buf = pylt_malloc(I, size+1);
    size_t read = fread(buf, size, 1, fp);
    if (read != 0) {
        PyLiteObject *ret = castobj(pylt_obj_bytes_new(I, (const char*)buf, size, true));
        pylt_free(I, buf, size);
        return ret;
    }
    pylt_free(I, buf, size+1);
    return NULL;
}

// def fwrite(file, buffer: bytes)
PyLiteObject* pylt_mods_cio_fwrite(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    FILE *fp = castcptr(args[0])->ob_ptr;
    PyLiteBytesObject *buf = castbytes(args[1]);
    return castobj(pylt_obj_int_new(I, fwrite(buf->ob_val, buf->ob_size, 1, fp)));
}

// def ftell(file)
PyLiteObject* pylt_mods_cio_ftell(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_int_new(I, ftell(castcptr(args[0])->ob_ptr)));
}


// def fseek(file, offset: int, origin = SEEK_CUR: int)
PyLiteObject* pylt_mods_cio_fseek(PyLiteInterpreter *I, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_int_new(I, fseek(castcptr(args[0])->ob_ptr, castint(args[1])->ob_val, castint(args[2])->ob_val)));
}


PyLiteModuleObject* pylt_mods_cio_loader(PyLiteInterpreter *I) {
    PyLiteModuleObject *mod = pylt_obj_module_new(I, _S(cio));

    pylt_obj_mod_setattr(I, mod, _NS(I, "SEEK_CUR"), castobj(pylt_obj_int_new(I, SEEK_CUR)));
    pylt_obj_mod_setattr(I, mod, _NS(I, "SEEK_END"), castobj(pylt_obj_int_new(I, SEEK_END)));
    pylt_obj_mod_setattr(I, mod, _NS(I, "SEEK_SET"), castobj(pylt_obj_int_new(I, SEEK_SET)));

    pylt_cfunc_register(I, mod, _NS(I, "fopen"), _NST(I, 2, "fn", "mode"), NULL, NULL, &pylt_mods_cio_fopen);
    pylt_cfunc_register(I, mod, _NS(I, "fclose"), _NST(I, 1, "file"), NULL, NULL, &pylt_mods_cio_fclose);
    pylt_cfunc_register(I, mod, _NS(I, "fsize"), _NST(I, 1, "file"), NULL, NULL, &pylt_mods_cio_fsize);
    pylt_cfunc_register(I, mod, _NS(I, "fread"), _NST(I, 2, "file", "size"), NULL, NULL, &pylt_mods_cio_fread);
    pylt_cfunc_register(I, mod, _NS(I, "fwrite"), _NST(I, 2, "file", "buf"), NULL, NULL, &pylt_mods_cio_fwrite);
    pylt_cfunc_register(I, mod, _NS(I, "ftell"), _NST(I, 1, "file"), NULL, NULL, &pylt_mods_cio_ftell);
    pylt_cfunc_register(I, mod, _NS(I, "fseek"), _NST(I, 3, "file", "offset", "origin"), _NT(I, 3, &PyLiteParamUndefined, &PyLiteParamUndefined, pylt_obj_int_new(I, SEEK_CUR)), NULL, &pylt_mods_cio_fseek);

    return mod;
}

pl_bool_t pylt_mods_cio_register(PyLiteInterpreter *I) {
    return pylt_mod_register(I, _S(cio), &pylt_mods_cio_loader, true);
}
