
#include "gc.h"
#include "state.h"
#include "debug.h"
#include "api.h"
#include "bind.h"
#include "types/all.h"

int __upset_ret;
#define upset_new(state) kho_init(unique_ptr, (state));
#define upset_free(upset) kho_destroy(unique_ptr, (upset));
#define upset_add(upset, __obj) kho_put(unique_ptr, (upset), (__obj), &__upset_ret);
#define upset_len(upset) (pl_int_t)kho_size((upset))
#define upset_clear(upset) kho_clear_unique_ptr((upset))

PyLiteObject* upset_item(PyLiteUPSet *upset, pl_int_t k) {
    return (kho_exist(upset, k)) ? castobj(kho_key(upset, k)) : NULL;
}

PyLiteObject* upset_has(PyLiteUPSet *upset, PyLiteObject *obj) {
    khiter_t x = kho_get(unique_ptr, upset, obj);
    return (x != kho_end(upset)) ? kho_key(upset, x) : NULL;
}

pl_int_t upset_remove(PyLiteUPSet *upset, PyLiteObject *obj) {
    khiter_t x = kho_get(unique_ptr, upset, obj);
    if (!(upset->flags && (kho_exist(upset, x)))) return -1;
    kho_del(unique_ptr, upset, x);
    return 0;
}

pl_int_t upset_begin(PyLiteUPSet *upset) {
    pl_int_t k = kho_begin(upset);
    while (k != kho_end(upset)) {
        if (kho_exist(upset, k)) return k;
        ++k;
    }
    return kho_end(upset);
}

pl_int_t upset_end(PyLiteUPSet *upset) {
    return kho_end(upset);
}

void upset_next(PyLiteUPSet *upset, pl_int_t *k) {
    pl_int_t key = *k;
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

void pylt_gc_add(PyLiteState *state, PyLiteObject *obj) {
    if (!obj) return;
    if (!(pl_isstrkind(obj) && pylt_obj_set_has(state, state->gc.str_static, obj))) {
        upset_add(state->gc.white, obj);
    }
}

void pylt_gc_remove(PyLiteState *state, PyLiteObject *obj) {
    upset_remove(state->gc.white, obj);
}

void pylt_gc_init(PyLiteState *state) {
    state->gc.white = upset_new(state);
    state->gc.grey = upset_new(state);
    state->gc.black = upset_new(state);

    state->gc.str_static = pylt_obj_set_new(state);
    state->gc.str_cached = pylt_obj_set_new(state);
}

void pylt_gc_collect(PyLiteState *state) {
    PyLiteObject *k, *v;
    PyLiteUPSet *white = state->gc.white;
    PyLiteUPSet *grey = state->gc.grey;
    PyLiteFrame *frame = pylt_vm_curframe(state);

    /*for (pl_int_t k = upset_begin(white); k != upset_end(white); upset_next(white, &k)) {
        //printf("GC: ");
        //debug_print_obj(state, upset_item(white, k));
        //putchar('\n');
    }*/

#define MOVE_WHITE(__obj) \
    if (upset_has(white, castobj(__obj))) { \
        upset_remove(white, castobj(__obj)); \
        upset_add(grey, castobj(__obj)); \
    }

    // 将被引用到的对象从 White 放到 Grey 集合中
    // 1. locals
    for (int i = kv_size(frame->var_tables) - 1; i >= 0; --i) {
        PyLiteDictObject *scope = kv_A(frame->var_tables, i);

        for (pl_int_t it = pylt_obj_dict_begin(state, scope); it != pylt_obj_dict_end(state, scope); pylt_obj_dict_next(state, scope, &it)) {
            pylt_obj_dict_keyvalue(state, scope, it, &k, &v);
            MOVE_WHITE(k);
            MOVE_WHITE(v);
        }

        MOVE_WHITE(frame->func);
    }
    // 2. stack
    for (pl_uint_t i = 0; i < kv_size(state->vm.stack); ++i) {
        PyLiteObject *obj = castobj(kv_A(state->vm.stack, i));
        MOVE_WHITE(obj);
    }
    // 3. types
    for (pl_uint_t i = 1; i < kv_size(state->cls_base); ++i) {
        PyLiteDictObject *dict = kv_A(state->cls_base, i)->ob_attrs;
        PyLiteTypeObject *type = pylt_api_gettype(state, i);
        MOVE_WHITE(type->name);
        for (pl_int_t it = pylt_obj_dict_begin(state, dict); it != pylt_obj_dict_end(state, dict); pylt_obj_dict_next(state, dict, &it)) {
            pylt_obj_dict_keyvalue(state, dict, it, &k, &v);
            MOVE_WHITE(k);
            MOVE_WHITE(v);
        }
    }
    // 4. frame code
    MOVE_WHITE(frame->code);

    while (upset_len(grey)) {
        // 从 Grey 集合中移除一个对象O，并将O设置成Black状态
        PyLiteObject *obj = upset_pop(grey);
        upset_add(state->gc.black, obj);

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
                PyLiteDictObject *dict = castmod(obj)->attrs;
                for (pl_int_t it = pylt_obj_dict_begin(state, dict); it != pylt_obj_dict_end(state, dict); pylt_obj_dict_next(state, dict, &it)) {
                    pylt_obj_dict_keyvalue(state, dict, it, &k, &v);
                    MOVE_WHITE(k);
                    MOVE_WHITE(v);
                }
                break;
            }
            case PYLT_OBJ_TYPE_FUNCTION: {
                // 这里不甚合理
                PyLiteListObject *lst = castfunc(obj)->code.const_val;
                for (pl_int_t i = 0; i < lst->ob_size; ++i) {
                    MOVE_WHITE(lst->ob_val[i]);
                }
            }
            case PYLT_OBJ_TYPE_CFUNCTION:
                MOVE_WHITE(castfunc(obj)->info.name);
                MOVE_WHITE(castfunc(obj)->info.doc);

                for (pl_int_t i = 0; i < castfunc(obj)->info.length; ++i) {
                    MOVE_WHITE(castfunc(obj)->info.params[i]);
                }

                if (castfunc(obj)->info.defaults) {
                    for (pl_int_t i = 0; i < castfunc(obj)->info.length; ++i) {
                        if (castfunc(obj)->info.defaults[i] > PARAM_KWARGS) {
                            MOVE_WHITE(castfunc(obj)->info.defaults[i]);
                        }
                    }
                }
                break;
            case PYLT_OBJ_TYPE_CODE: {
                PyLiteListObject *lst = castcode(obj)->const_val;
                for (pl_int_t i = 0; i < lst->ob_size; ++i) {
                    MOVE_WHITE(lst->ob_val[i]);
                }
                break;
            }
            case PYLT_OBJ_TYPE_LIST:
            case PYLT_OBJ_TYPE_SET:
            case PYLT_OBJ_TYPE_TUPLE: {
                PyLiteIterObject *iter = pylt_obj_iter_new(state, obj);
                for (PyLiteObject *item = pylt_obj_iter_next(state, iter); item; item = pylt_obj_iter_next(state, iter)) {
                    MOVE_WHITE(item);
                }
                break;
            }
            case PYLT_OBJ_TYPE_DICT: {
                for (pl_int_t it = pylt_obj_dict_begin(state, castdict(obj)); it != pylt_obj_dict_end(state, castdict(obj)); pylt_obj_dict_next(state, castdict(obj), &it)) {
                    pylt_obj_dict_keyvalue(state, castdict(obj), it, &k, &v);
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
                    for (pl_int_t it = pylt_obj_dict_begin(state, dict); it != pylt_obj_dict_end(state, dict); pylt_obj_dict_next(state, dict, &it)) {
                        pylt_obj_dict_keyvalue(state, dict, it, &k, &v);
                        MOVE_WHITE(k);
                        MOVE_WHITE(v);
                    }
                }
        }
    }

    printf("gc collect %dw %db\n", upset_len(white), upset_len(state->gc.black));

    // 将未标记对象全部释放
    for (pl_int_t k = upset_begin(white); k != upset_end(white); upset_next(white, &k)) {
        PyLiteObject *obj = upset_item(white, k);

        // check static before free
        if (!(pl_isstrkind(obj) && pylt_obj_set_has(state, state->gc.str_static, obj))) {
            pylt_api_output_str(state, pylt_obj_str_new_from_cformat(state, "gc free %p %s ", obj, pylt_obj_to_str(state, obj)));
            printf("[%d]\n", obj->ob_type);
            if (pl_isstrkind(obj)) {
                pylt_obj_set_remove(state, state->gc.str_cached, obj);
            }
            pylt_obj_free(state, obj);
        }
    }
    upset_clear(white);

    // 翻转 white(现在是一个空set) 和 black，等待下一轮回收
    state->gc.white = state->gc.black;
    state->gc.black = white;
}

void pylt_gc_finalize(PyLiteState *state) {
    upset_free(state->gc.white);
    upset_free(state->gc.grey);
    upset_free(state->gc.black);
    pylt_obj_set_free(state, state->gc.str_static);
    pylt_obj_set_free(state, state->gc.str_cached);
}

void pylt_gc_freeall(PyLiteState *state) {
    PyLiteUPSet *white = state->gc.white;
    for (pl_int_t k = upset_begin(white); k != upset_end(white); upset_next(white, &k)) {
        /*PyLiteObject *obj = upset_item(white, k);
        printf("[%d] ", obj->ob_type);
        if (obj->ob_type == 5) pylt_api_output_str(state, obj);
        putchar('\n'); */
        pylt_obj_free(state, upset_item(white, k));
    }
}

PyLiteObject* _pylt_gc_cache_add(PyLiteState *state, PyLiteObject *key) {
    PyLiteSetObject *cache = state->gc.str_cached;
    PyLiteObject *ret = pylt_obj_set_has(state, cache, key);
    if (!ret) pylt_obj_set_add(state, cache, key);
    pylt_gc_add(state, ret ? ret : key);
    return ret;
}

PyLiteStrObject* pylt_gc_cache_str_add(PyLiteState *state, PyLiteStrObject *key) {
    PyLiteStrObject* ret = caststr(_pylt_gc_cache_add(state, castobj(key)));
    if (ret) {
        pylt_obj_str_free(state, key);
        return ret;
    }
    return key;
}

void pylt_gc_make_str_static(PyLiteState *state, PyLiteObject *obj) {
    upset_remove(state->gc.white, obj);
    pylt_obj_set_add(state, state->gc.str_static, obj);
}

PyLiteBytesObject* pylt_gc_cache_bytes_add(PyLiteState *state, PyLiteBytesObject *key) {
    PyLiteBytesObject* ret = castbytes(_pylt_gc_cache_add(state, castobj(key)));
    if (ret) {
        pylt_obj_bytes_free(state, key);
        return ret;
    }
    return key;
}

void pylt_gc_static_release(PyLiteState *state) {
}
