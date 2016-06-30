
#include "gc.h"
#include "state.h"

void pylt_gc_add(PyLiteState *state, PyLiteObject *obj) {
    pylt_obj_set_add(state, state->gc.white, obj);
}

void pylt_gc_init(PyLiteState *state) {
    state->gc.white = pylt_obj_set_new(state);
    state->gc.grey = pylt_obj_set_new(state);
    state->gc.black = pylt_obj_set_new(state);
    state->gc.statics = pylt_obj_set_new(state);
}

void pylt_gc_collect(PyLiteState *state) {
    PyLiteObject *k, *v;
    PyLiteSetObject *white = state->gc.white;
    PyLiteSetObject *grey = state->gc.grey;
    PyLiteFrame *frame = pylt_vm_curframe(state);

    // 将被引用到的对象从 White 设置成 Grey，并放到 Grey 集合中
    for (int i = kv_size(frame->var_tables) - 1; i >= 0; --i) {
        PyLiteDictObject *scope = kv_A(frame->var_tables, i);

        for (pl_int_t it = pylt_obj_dict_begin(state, scope); it != pylt_obj_dict_end(state, scope); pylt_obj_dict_next(state, scope, &it)) {
            pylt_obj_dict_keyvalue(state, scope, it, &k, &v);
            if (pylt_obj_set_has(state, white, v)) {
                pylt_obj_set_remove(state, white, v);
                pylt_obj_set_add(state, grey, v);
            }
        }
    }

    while (pylt_obj_set_len(state, grey)) {
        // 从 Grey 集合中移除一个对象O，并将O设置成Black状态
        PyLiteObject *obj = pylt_obj_set_pop(state, grey);
        pylt_obj_set_add(state, state->gc.black, obj);

        // 遍历 obj（如果可以的话），将子项从 white 移至 grey
        if (pylt_obj_iterable(state, obj)) {
            PyLiteIterObject *iter = pylt_obj_iter_new(state, obj);
            for (PyLiteObject *item = pylt_obj_iter_next(state, iter); item; item = pylt_obj_iter_next(state, iter)) {
                if (pylt_obj_set_has(state, white, item)) {
                    pylt_obj_set_remove(state, white, item);
                    pylt_obj_set_add(state, grey, item);
                }
            }
        }
    }

    // 将未标记对象全部释放
    for (pl_int_t k = pylt_obj_set_begin(state, white); k != pylt_obj_set_end(state, white); pylt_obj_set_next(state, white, &k)) {
        pylt_obj_free(state, pylt_obj_set_itemvalue(state, white, k));
    }
    pylt_obj_set_clear(state, white);

    // 翻转 white(现在是一个空set) 和 black，等待下一轮回收
    state->gc.white = state->gc.black;
    state->gc.black = white;
}

void pylt_gc_finalize(PyLiteState *state) {
    pylt_gc_static_release(state);
    pylt_obj_set_free(state, state->gc.white);
    pylt_obj_set_free(state, state->gc.grey);
    pylt_obj_set_free(state, state->gc.black);
    pylt_obj_set_free(state, state->gc.statics);
}

void pylt_gc_static_add(PyLiteState *state, PyLiteObject *obj) {
    //pylt_obj_set_add(state, state->gc.g_static, obj);
}

void pylt_gc_local_add(PyLiteState *state, PyLiteObject *obj) {
    //pylt_obj_set_add(state, state->gc.g_local, obj);
}

pl_bool_t pylt_gc_isstatic(PyLiteState *state, PyLiteObject *obj) {
    return pylt_obj_set_has(state, state->gc.statics, obj);
}

void pylt_gc_local_release(PyLiteState *state) {
}

void pylt_gc_static_release(PyLiteState *state) {
}
