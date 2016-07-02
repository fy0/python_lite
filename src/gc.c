
#include "gc.h"
#include "state.h"
#include "debug.h"
#include "api.h"
#include "bind.h"

int __upset_ret;
#define upset_new(state) kho_init(unique_ptr, (state));
#define upset_free(upset) kho_destroy(unique_ptr, (upset));
#define upset_add(upset, obj) kho_put(unique_ptr, (upset), (obj), &__upset_ret);
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
    if (kho_exist(upset, x) != 1) return -1;
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
    upset_add(state->gc.white, obj);
}

void pylt_gc_init(PyLiteState *state) {
    state->gc.white = upset_new(state);
    state->gc.grey = upset_new(state);
    state->gc.black = upset_new(state);
}

void pylt_gc_collect(PyLiteState *state) {
    PyLiteObject *k, *v;
    PyLiteUPSet *white = state->gc.white;
    PyLiteUPSet *grey = state->gc.grey;
    PyLiteFrame *frame = pylt_vm_curframe(state);

    /*for (pl_int_t k = upset_begin(white); k != upset_end(white); upset_next(white, &k)) {
        printf("GC: ");
        debug_print_obj(state, upset_item(white, k));
        putchar('\n');
    }*/

#define MOVE_WHITE(__obj) \
    if (upset_has(white, (__obj))) { \
        upset_remove(white, (__obj)); \
        upset_add(grey, (__obj)); \
    }

    // 将被引用到的对象从 White 放到 Grey 集合中
    for (int i = kv_size(frame->var_tables) - 1; i >= 0; --i) {
        PyLiteDictObject *scope = kv_A(frame->var_tables, i);

        for (pl_int_t it = pylt_obj_dict_begin(state, scope); it != pylt_obj_dict_end(state, scope); pylt_obj_dict_next(state, scope, &it)) {
            pylt_obj_dict_keyvalue(state, scope, it, &k, &v);
            MOVE_WHITE(v);
        }
    }
    for (pl_uint_t i = 0; i < kv_size(state->vm.stack); ++i) {
        PyLiteObject *obj = castobj(kv_A(state->vm.stack, i));
        MOVE_WHITE(obj);
    }

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

            case PYLT_OBJ_TYPE_MODULE:
            case PYLT_OBJ_TYPE_FUNCTION:
            case PYLT_OBJ_TYPE_CFUNCTION:
            case PYLT_OBJ_TYPE_CODE:

            case PYLT_OBJ_TYPE_TYPE:
            case PYLT_OBJ_TYPE_ITER:
            case PYLT_OBJ_TYPE_PROP:
            case PYLT_OBJ_TYPE_NONE:
            case PYLT_OBJ_TYPE_RANGE:
            case PYLT_OBJ_TYPE_BASE_EXCEPTION:
                break;
            case PYLT_OBJ_TYPE_LIST:
            case PYLT_OBJ_TYPE_SET:
            case PYLT_OBJ_TYPE_TUPLE: {
                PyLiteIterObject *iter = pylt_obj_iter_new(state, obj);
                for (PyLiteObject *item = pylt_obj_iter_next(state, iter); item; item = pylt_obj_iter_next(state, iter)) {
                    MOVE_WHITE(item);
                }
            }
            //case PYLT_OBJ_TYPE_DICT: 
            default:
                if (obj->ob_type > PYLT_OBJ_BUILTIN_TYPE_NUM) {
                    ;
                }
        }
    }

    printf("gc collect %dw %db\n", upset_len(white), upset_len(state->gc.black));

    // 将未标记对象全部释放
    for (pl_int_t k = upset_begin(white); k != upset_end(white); upset_next(white, &k)) {
        PyLiteObject *obj = upset_item(white, k);
        pylt_api_output_str(state, pylt_obj_str_new_from_cformat(state, "gc free %p %s\n", obj, pylt_obj_to_str(state, obj)));

        //printf("gc free 0x%p\n", obj);
        pylt_obj_free(state, upset_item(white, k));
    }
    upset_clear(white);

    // 翻转 white(现在是一个空set) 和 black，等待下一轮回收
    state->gc.white = state->gc.black;
    state->gc.black = white;
}

void pylt_gc_finalize(PyLiteState *state) {
    //pylt_gc_static_release(state);
    upset_free(state->gc.white);
    upset_free(state->gc.grey);
    upset_free(state->gc.black);
}

void pylt_gc_static_add(PyLiteState *state, PyLiteObject *obj) {
    //upset_add(state, state->gc.g_static, obj);
}

void pylt_gc_static_release(PyLiteState *state) {
}
