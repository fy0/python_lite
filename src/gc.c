
#include "gc.h"
#include "debug.h"
#include "api.h"
#include "bind.h"
#include "intp.h"
#include "utils/misc.h"
#include "types/all.h"

int __upset_ret;
#define upset_new(I) kho_init(unique_ptr, (I));
#define upset_free(upset) kho_destroy(unique_ptr, (upset));
#define upset_add(upset, __obj) kho_put(unique_ptr, (upset), (__obj), &__upset_ret);
#define upset_len(upset) (pl_int_t)kho_size((upset))
#define upset_clear(upset) kho_clear_unique_ptr((upset))

PyLiteObject* upset_item(PyLiteUPSet *upset, pl_int32_t k) {
    return (kho_exist(upset, k)) ? castobj(kho_key(upset, k)) : NULL;
}

PyLiteObject* upset_has(PyLiteUPSet *upset, PyLiteObject *obj) {
    pl_uint32_t x = kho_get(unique_ptr, upset, obj);
    return (x != kho_end(upset)) ? kho_key(upset, x) : NULL;
}

pl_int_t upset_remove(PyLiteUPSet *upset, PyLiteObject *obj) {
    khiter_t x = kho_get(unique_ptr, upset, obj);
    if (x == kho_end(upset)) return -1;
    kho_del(unique_ptr, upset, x);
    return 0;
}

pl_int32_t upset_begin(PyLiteUPSet *upset) {
    pl_int32_t k = kho_begin(upset);
    while (k != kho_end(upset)) {
        if (kho_exist(upset, k)) return k;
        ++k;
    }
    return kho_end(upset);
}

pl_uint32_t upset_end(PyLiteUPSet *upset) {
    return kho_end(upset);
}

void upset_next(PyLiteUPSet *upset, pl_int32_t *k) {
    pl_int32_t key = *k;
    while (++key != kho_end(upset)) {
        if (kho_exist(upset, key)) {
            *k = key;
            return;
        }
    }
    *k = kho_end(upset);
}

PyLiteObject* upset_pop(PyLiteUPSet *upset) {
    khiter_t k;
    if (kho_size(upset) == 0)
        return NULL;
    k = upset_begin(upset);
    PyLiteObject *obj = upset_item(upset, k);
    kho_del(unique_ptr, upset, k);
    return obj;
}

void pylt_gc_add(PyLiteInterpreter *I, PyLiteObject *obj) {
    if (!I) return;
    if (!obj) return;
    // TODO: prevent immutable static objects be added to gc
    upset_add(I->gc.white, obj);
}

void pylt_gc_remove(PyLiteInterpreter *I, PyLiteObject *obj) {
    if (!I) return;
    upset_remove(I->gc.white, obj);
}

void pylt_gc_init(PyLiteInterpreter *I) {
    I->gc.white = upset_new(I);
    I->gc.grey = upset_new(I);
    I->gc.black = upset_new(I);

    I->gc.refs = upset_new(I);
    I->gc.statics = upset_new(I);
    I->gc.str_cached = pylt_obj_set_new(I);
}

void pylt_gc_collect(PyLiteInterpreter *I) {
    PyLiteObject *k, *v;
    PyLiteUPSet *white = I->gc.white;
    PyLiteUPSet *grey = I->gc.grey;
    PyLiteContext *ctx = I->vm.ctx;

    /*for (pl_int_t k = upset_begin(white); k != upset_end(white); upset_next(white, &k)) {
        //wprintf(L"GC: ");
        //debug_print_obj(I, upset_item(white, k));
        //putwchar('\n');
    }*/

#define MOVE_WHITE(__obj) \
    if (upset_has(white, castobj(__obj))) { \
        upset_remove(white, castobj(__obj)); \
        upset_add(grey, castobj(__obj)); \
    }

    // 将被引用到的对象从 White 放到 Grey 集合中
    // 1. locals
    for (int i = kv_size(ctx->frames) - 1; i >= 0; --i) {
        PyLiteFrame *frame = &kv_A(ctx->frames, i);
        PyLiteDictObject *scope = frame->locals;

        pl_foreach_dict(I, it, scope) {
            pylt_obj_dict_keyvalue(I, scope, it, &k, &v);
            MOVE_WHITE(k);
            MOVE_WHITE(v);
        }

        MOVE_WHITE(frame->func);
    }
    // 2. stack
    for (pl_uint_t i = 0; i < kv_size(ctx->stack); ++i) {
        PyLiteObject *obj = castobj(kv_A(ctx->stack, i));
        MOVE_WHITE(obj);
    }
    // 3. types
    for (pl_uint_t i = 1; i < kv_size(I->cls_base); ++i) {
        PyLiteDictObject *dict = kv_A(I->cls_base, i)->ob_attrs;
        PyLiteTypeObject *type = pl_type_by_code(I, i);
        MOVE_WHITE(type->name);
        pl_foreach_dict(I, it, dict) {
            pylt_obj_dict_keyvalue(I, dict, it, &k, &v);
            MOVE_WHITE(k);
            MOVE_WHITE(v);
        }
    }
    // 4. references
    for (pl_uint32_t k = upset_begin(I->gc.refs); k != upset_end(I->gc.refs); upset_next(I->gc.refs, &k)) {
        PyLiteRef *ref = (PyLiteRef*)upset_item(I->gc.refs, k);
        MOVE_WHITE(ref->obj);
    }
    // 6. builtins
    MOVE_WHITE(I->vm.builtins);

    while (upset_len(grey)) {
        // 从 Grey 集合中移除一个对象O，并将O设置成Black状态
        PyLiteObject *obj = upset_pop(grey);
        upset_add(I->gc.black, obj);

        // 遍历 obj（如果可以的话），将子项从 white 移至 grey
        switch (obj->ob_type) {
            case PYLT_OBJ_TYPE_OBJ:
            case PYLT_OBJ_TYPE_INT:
            case PYLT_OBJ_TYPE_FLOAT:
            case PYLT_OBJ_TYPE_BOOL:

            case PYLT_OBJ_TYPE_STR:
            case PYLT_OBJ_TYPE_BYTES:

            case PYLT_OBJ_TYPE_TYPE:
            case PYLT_OBJ_TYPE_ITER:
            case PYLT_OBJ_TYPE_NONE:
            case PYLT_OBJ_TYPE_RANGE:
            case PYLT_OBJ_TYPE_BASE_EXCEPTION:
                break;
            case PYLT_OBJ_TYPE_MODULE: {
                // attr
                PyLiteDictObject *dict = castmod(obj)->ob_attrs;
                for (pl_int32_t it = pylt_obj_dict_begin(I, dict); it != pylt_obj_dict_end(I, dict); pylt_obj_dict_next(I, dict, &it)) {
                    pylt_obj_dict_keyvalue(I, dict, it, &k, &v);
                    MOVE_WHITE(k);
                    MOVE_WHITE(v);
                }
                break;
            }
            case PYLT_OBJ_TYPE_FUNCTION: {
                // 这里不甚合理，code最好是个引用
                PyLiteListObject *lst = castfunc(obj)->code.const_val;
                for (pl_int_t i = 0; i < lst->ob_size; ++i) {
                    MOVE_WHITE(lst->ob_val[i]);
                }

                pl_foreach_dict(I, it, castfunc(obj)->info.closure) {
                    PyLiteObject *key, *value;
                    pylt_obj_dict_keyvalue(I, castfunc(obj)->info.closure, it, &key, &value);
                    MOVE_WHITE(key);
                    MOVE_WHITE(value);
                }
            }
            case PYLT_OBJ_TYPE_CFUNCTION:
                MOVE_WHITE(castfunc(obj)->info.name);
                MOVE_WHITE(castfunc(obj)->info.doc);
                MOVE_WHITE(castfunc(obj)->info.params);
                MOVE_WHITE(castfunc(obj)->info.defaults);
                break;
            case PYLT_OBJ_TYPE_CODE: {
                PyLiteListObject *lst = castcode(obj)->const_val;
                for (pl_int_t i = 0; i < lst->ob_size; ++i) {
                    MOVE_WHITE(lst->ob_val[i]);
                }
                pl_foreach_set(I, it, castcode(obj)->closure) {
                    PyLiteObject *key = pylt_obj_set_itemvalue(I, castcode(obj)->closure, it);
                    MOVE_WHITE(key);
                }
                break;
            }
            case PYLT_OBJ_TYPE_LIST:
            case PYLT_OBJ_TYPE_SET:
            case PYLT_OBJ_TYPE_TUPLE: {
                PyLiteIterObject *iter = pylt_obj_iter_new(I, obj);
                for (PyLiteObject *item = pylt_obj_iter_next(I, iter); item; item = pylt_obj_iter_next(I, iter)) {
                    MOVE_WHITE(item);
                }
                break;
            }
            case PYLT_OBJ_TYPE_DICT: {
                for (pl_int32_t it = pylt_obj_dict_begin(I, castdict(obj)); it != pylt_obj_dict_end(I, castdict(obj)); pylt_obj_dict_next(I, castdict(obj), &it)) {
                    pylt_obj_dict_keyvalue(I, castdict(obj), it, &k, &v);
                    MOVE_WHITE(k);
                    MOVE_WHITE(v);
                }
                break;
            }
            case PYLT_OBJ_TYPE_PROP:
                MOVE_WHITE(castprop(obj)->fget.func);
                MOVE_WHITE(castprop(obj)->fset.func);
                break;
            default:
                if (obj->ob_type > PYLT_OBJ_BUILTIN_TYPE_NUM) {
                    // attr
                    PyLiteDictObject *dict = castcustom(obj)->ob_attrs;
                    for (pl_int32_t it = pylt_obj_dict_begin(I, dict); it != pylt_obj_dict_end(I, dict); pylt_obj_dict_next(I, dict, &it)) {
                        pylt_obj_dict_keyvalue(I, dict, it, &k, &v);
                        MOVE_WHITE(k);
                        MOVE_WHITE(v);
                    }
                }
        }
    }
#ifdef PL_DEBUG_INFO
    wprintf(L"gc collect %dw %db\n", (int)upset_len(white), (int)upset_len(I->gc.black));
#endif

    for (pl_uint32_t k = upset_begin(white); k != upset_end(white); upset_next(white, &k)) {
        PyLiteObject *obj = upset_item(white, k);
        //wprintf(L"test %7x [%d]\n", (unsigned int)(pl_uint_t)obj, (int)obj->ob_type);
    }
    // 将未标记对象全部释放
    for (pl_uint32_t k = upset_begin(white); k != upset_end(white); upset_next(white, &k)) {
        PyLiteObject *obj = upset_item(white, k);

        //wprintf(L"test %7x [%d]\n", (unsigned int)(pl_uint_t)obj, (int)obj->ob_type);
        // check releasable
        if (!(obj->ob_flags & PYLT_OBJ_FLAG_CANFREE)) {
            upset_add(I->gc.black, obj);
            continue;
        }

        // check static before free
        if (!(obj->ob_flags & PYLT_OBJ_FLAG_STATIC)) {
#ifdef PL_DEBUG_INFO
            //wprintf(L"gc free %7x [%d]\n", (unsigned int)(pl_uint_t)obj, (int)obj->ob_type);
#endif
            if (pl_isstrkind(obj)) {
                pylt_obj_set_remove(I, I->gc.str_cached, obj);
            }
            pylt_obj_free(I, obj);
        }
    }
    upset_clear(white);

    // 翻转 white(现在是一个空set) 和 black，等待下一轮回收
    I->gc.white = I->gc.black;
    I->gc.black = white;
}

void pylt_gc_finalize(PyLiteInterpreter *I) {
    upset_free(I->gc.white);
    upset_free(I->gc.grey);
    upset_free(I->gc.black);

    upset_free(I->gc.refs);
    upset_free(I->gc.statics);
    //pylt_obj_set_free(I, I->gc.str_cached);
#ifdef PL_DEBUG_INFO
    wprintf(L"mem unfreed: %u\n", (unsigned int)I->mem_used);
#endif
}

void pylt_gc_freeall(PyLiteInterpreter *I) {
    PyLiteUPSet *white = I->gc.white;
    for (pl_int32_t k = upset_begin(white); k != upset_end(white); upset_next(white, &k)) {
        /*PyLiteObject *obj = upset_item(white, k);
        wprintf(L"[%d] ", obj->ob_type);
        if (obj->ob_type == 5) pl_outputstr(I, obj);
        putwchar('\n'); */
        pylt_obj_free(I, upset_item(white, k));
    }
}

PyLiteObject* _pylt_gc_cache_add(PyLiteInterpreter *I, PyLiteObject *key) {
    PyLiteSetObject *cache = I->gc.str_cached;
    PyLiteObject *ret = pylt_obj_set_has(I, cache, key);
    if (!ret) {
        pylt_obj_set_add(I, cache, key);
    }
    return ret;
}

PyLiteStrObject* pylt_gc_cache_str_add(PyLiteInterpreter *I, PyLiteStrObject *key) {
    PyLiteStrObject* ret = caststr(_pylt_gc_cache_add(I, castobj(key)));
    if (ret) {
        pylt_gc_remove(I, castobj(key));
        pylt_obj_str_release(I, key);
        return ret;
    } 
    return key;
}

PyLiteBytesObject* pylt_gc_cache_bytes_add(PyLiteInterpreter *I, PyLiteBytesObject *key) {
    PyLiteBytesObject* ret = castbytes(_pylt_gc_cache_add(I, castobj(key)));
    if (ret) {
        pylt_gc_remove(I, castobj(key));
        pylt_obj_bytes_release(I, key);
        return ret;
    }
    return key;
}

void pylt_gc_static_add(PyLiteInterpreter *I, PyLiteObject *obj) {
    if (!(obj->ob_flags | PYLT_OBJ_FLAG_STATIC)) {
        obj->ob_flags |= PYLT_OBJ_FLAG_STATIC;
        upset_add(I->gc.statics, obj);
    }
}

void pylt_gc_static_remove(PyLiteInterpreter *I, PyLiteObject *obj) {
    if (obj->ob_flags | PYLT_OBJ_FLAG_STATIC) {
        obj->ob_flags ^= PYLT_OBJ_FLAG_STATIC;
        upset_remove(I->gc.statics, obj);
    }
}

void pylt_gc_ref_add(PyLiteInterpreter *I, PyLiteRef *ref) {
    // actually is not object type
    upset_add(I->gc.refs, castobj(ref));
}

void pylt_gc_ref_remove(PyLiteInterpreter *I, PyLiteRef *ref) {
    upset_remove(I->gc.refs, castobj(ref));
}


void pylt_gc_static_release(PyLiteInterpreter *I) {
}
