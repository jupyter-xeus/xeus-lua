// https://stackoverflow.com/questions/4508119/redirecting-redefining-print-for-embedded-lua

// copied from https://raw.githubusercontent.com/tomstitt/lupyter/main/lupyter/lua_runtime/lua_runtime.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <Python.h>
#include <patchlevel.h>


// Handle Python luaL_setfuncs
// PyMODINIT returns a PyObject * in py3 but nothing
// in py2, PyMODINIT_RET handles that
#if PY_MAJOR_VERSION == 3
#define PY3
#define PyMODINIT_RET(x) x

#elif PY_MAJOR_VERSION == 2
#define PY2
#define PyMODINIT_RET(x)
#define PyString_FromString PyUnicode_FromString
// https://docs.python.org/2.7/extending/newtypes.html
// Note that PyMODINIT_FUNC declares the function as void return type, declares
// any special linkage declarations required by the platform, and for C++ declares
// the function as extern "C".
#ifndef PyMODINIT_FUNC
#define PyMODINIT_FUNC void
#endif

#endif

// Handle Lua version
#if LUA_VERSION_NUM < 501
#error "Please use a newer version of Lua =]"

#elif LUA_VERSION_NUM < 502
void lua_pushglobaltable(lua_State * L) { lua_pushvalue(L, LUA_GLOBALSINDEX); }

#endif


const char * reserved_words[] = {
  "and", "break", "do", "else", "elseif", "end",
  "false", "for", "function", "if", "in",
  "local", "nil", "not", "or", "repeat", "return",
  "then", "true", "until", "while"
#if LUA_VERSION_NUM >= 502
  , "goto"
#endif
};


int is_identifier(char c) {
  return isalpha(c) || std::isdigit(c) || c == '_';
}


int get_metaindex(lua_State * L) {
  if (luaL_getmetafield(L, -1, "__index") == LUA_TNIL /* 0 in Lua51 */) {
    lua_pop(L, 1);
    return 0;
  }

  if (lua_type(L, -1) != LUA_TTABLE) {
    lua_pop(L, 2);
    return 0;
  }

  return 1;
}


int get_path(lua_State * L, const char * path, int path_length) {
  int offset = 0;
  char op = '.';
  lua_pushglobaltable(L);
  for (int i = 0; i < path_length; ++i) {
    if (path[i] == ':' || path[i] == '.') {
      if (lua_type(L, -1) != LUA_TTABLE && get_metaindex(L) == 0) {
        return 0;
      }
      lua_pushlstring(L, path+offset, i-offset);
      lua_gettable(L, -2);
      lua_replace(L, -2);
      op = path[i];
      offset = i+1;
    }
  }

  if (op == ':' && get_metaindex(L) == 0) {
    return 0;
  }

  if (lua_type(L, -1) != LUA_TTABLE) {
    lua_pop(L, 1);
    return 0;
  }

  return 1;
}


int table_matches(lua_State * L, int table_index, const char * identifier, int identifier_length, PyObject * py_matches) {
  int match_count = 0;
  lua_pushnil(L);
  // fix offset after pushing
  table_index = table_index < 0 ? table_index-1 : table_index;

  while (lua_next(L, table_index)) {
    if (lua_type(L, -2) == LUA_TSTRING) {
      const char * key = lua_tostring(L, -2);
      if (strncmp(identifier, key, identifier_length) == 0) {
        PyList_Append(py_matches, PyUnicode_FromString(key));
        match_count++;
      }
    }
    lua_pop(L, 1);
  }

  return match_count;
}


// TODO: this misses tables with __index metafields and indexes using []
int complete(lua_State * L, const char * code, int cursor_pos, PyObject * py_matches) {
  int dot_loc = -1;
  int i;

  // cursor is one to the right of the starting char for completion
  cursor_pos -= 1;
  for (i = cursor_pos; i >= 0; --i) {
    if (code[i] == '.') {
      // check for string concat
      if (i > 0 && code[i-1] == '.') { break; }
      if (dot_loc == -1) { dot_loc = i; }
    }
    else if (code[i] == ':') {
      // check for '::', only in >= 5.2
#if LUA_VERSION_NUM >= 502
      if (i > 0 && code[i-1] == ':') { break; }
#endif
      if (dot_loc == -1) { dot_loc = i; }
      // invalid to have a colon after finding a dot/colon
      else { return 0; }
    }
    else if (!is_identifier(code[i])) {
      break;
    }
  }

  // break char is to the left of the start of the identifier
  int cursor_start = i+1;

  // don't try to match numbers
  if (std::isdigit(code[cursor_start])) { return 0; }

  int match_count = 0;
  if (dot_loc > 0) {
    const char * identifier = code+dot_loc+1;
    const char * path = code+cursor_start;
    int identifier_length = cursor_pos-dot_loc;
    int path_length = dot_loc-cursor_start+1;
    if (get_path(L, path, path_length) == 0) {
      return 0;
    }
    match_count = table_matches(L, -1, identifier, identifier_length, py_matches);
    // cursor_start for fields is just the start of the "basename"
    cursor_start = identifier-code;
  }

  else {
    int identifier_length = cursor_pos-cursor_start+1;
    const char * identifier = code+cursor_start;

    // check for global matches
    lua_pushglobaltable(L);
    match_count = table_matches(L, -1, identifier, identifier_length, py_matches);

    // check for reserved word match
    for (unsigned i = 0; i < sizeof(reserved_words)/sizeof(reserved_words[0]); ++i) {
      if (strncmp(identifier, reserved_words[i], identifier_length) == 0) {
        match_count++;
        PyList_Append(py_matches, PyUnicode_FromString(reserved_words[i]));
      }
    }
  }
  return cursor_start;
}


static void py_print(PyObject * py_print_cbk, const char * str) {
  PyObject * args = Py_BuildValue("(s)", str);
  // TODO: depricated
  PyObject * result = PyEval_CallObject(py_print_cbk, args);
  Py_DECREF(args);
  Py_DECREF(result);
}


void eval_code(lua_State * L, const char * code) {
  lua_pushfstring(L, "print(%s)", code);
  if (luaL_dostring(L, lua_tostring(L, -1))) {
    if (luaL_dostring(L, code)) {
      lua_getglobal(L, "print");
      lua_pushvalue(L, -2);
      lua_call(L, 1, 0);
    }
  }
  lua_settop(L, 0);
}


int print_cb(lua_State * L, PyObject * py_cbk) {
  lua_getglobal(L, "tostring");
  for (int i = 1; i <= lua_gettop(L)-1; ++i) {
    lua_pushvalue(L, -1);
    lua_pushvalue(L, i);
    lua_call(L, 1, 1);
    const char * tostr = lua_tostring(L, -1);
    if (tostr == NULL) {
      return luaL_error(L, "'tostring' returned NULL value\n");
    }
    if (i > 1) { py_print(py_cbk, "\t"); }
    py_print(py_cbk, tostr);
    lua_pop(L, 1);
  }
  py_print(py_cbk, "\n");
  return 0;
}


int py_print_lua_cb(lua_State * L) {
  PyObject * py_cbk = lua_touserdata(L, lua_upvalueindex(1));
  return print_cb(L, py_cbk);
}


typedef struct {
  PyObject_HEAD // ';' isn't need, it can cause compiler issues
  lua_State * L;
  PyObject * py_print_cbk;
} PyLuaRuntime;


static PyObject * lua_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
  PyLuaRuntime * self = (PyLuaRuntime *)type->tp_alloc(type, 0);
  if (self != NULL) {
    self->L = NULL;
    self->py_print_cbk = NULL;
  }

  return (PyObject *)self;
}


static int lua_init(PyLuaRuntime * self, PyObject * args, PyObject * kwargs) {
  PyObject * cbk = NULL;
  if (!PyArg_ParseTuple(args, "|O", &cbk)) {
    return -1;
  }

  if (cbk && !PyCallable_Check(cbk)) {
    PyErr_SetString(PyExc_TypeError, "expected callable");
    return -1;
  }

  // handle multiple init calls
  if (self->L) {
    lua_close(self->L);
    self->L = NULL;
  }
  if (self->py_print_cbk) {
    Py_XDECREF(self->py_print_cbk);
    self->py_print_cbk = NULL;
  }

  self->L = luaL_newstate();
  luaL_openlibs(self->L);
  // if the callback was given, replace 'print'
  // TODO: stdout and stderr as well
  if (cbk) {
    Py_INCREF(cbk);
    lua_pushlightuserdata(self->L, cbk);
    lua_pushcclosure(self->L, py_print_lua_cb, 1);
    lua_setglobal(self->L, "print");
  }
  self->py_print_cbk = cbk;

  return 0;
}


static void lua_dealloc(PyLuaRuntime * self) {
  if (self->L) { lua_close(self->L); }
  Py_XDECREF(self->py_print_cbk);
  Py_TYPE(self)->tp_free((PyObject *)self);
}


static PyObject * lua_eval(PyLuaRuntime * self, PyObject * args) {
  const char * code;
  if (!PyArg_ParseTuple(args, "s", &code)) {
    return NULL;
  }
  eval_code(self->L, code);

  return Py_None;
}


static PyObject * lua_complete(PyLuaRuntime * self, PyObject * args) {
  const char * code;
  int cursor_pos = -1;
  if (!PyArg_ParseTuple(args, "s|i", &code, &cursor_pos)) {
    return NULL;
  }

  PyObject * py_matches = PyList_New(0);
  if (cursor_pos == -1) { cursor_pos = strlen(code); }
  int cursor_start = complete(self->L, code, cursor_pos, py_matches);
  PyObject * res = PyTuple_New(2);
  PyTuple_SetItem(res, 0, py_matches);
  PyTuple_SetItem(res, 1, PyLong_FromLong(cursor_start));

  return res;
}


static PyMethodDef pylua_methods[] = {
   {"eval", (PyCFunction)lua_eval, METH_VARARGS,
     "$.eval(code) -> None\n\nEvaluate the Lua string."},
   {"complete", (PyCFunction)lua_complete, METH_VARARGS,
     "$.complete(code[, cursor_pos]) -> (list, int)\n\n"
     "Returns a list of possible matches and a starting cursor position."},
   {NULL}
};


static PyTypeObject PyLuaRuntimeType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "lupyter.lua_runime.LuaRuntime",
  .tp_doc = "(print_callback: (str) -> None) -> new LuaRuntime",
  .tp_basicsize = sizeof(PyLuaRuntime),
  .tp_flags = Py_TPFLAGS_DEFAULT,
  .tp_new = lua_new,
  .tp_init = (initproc)lua_init,
  .tp_dealloc = (destructor)lua_dealloc,
  .tp_methods = pylua_methods,
};


#define MOD_NAME "lupyter.lua_runtime"
#define MOD_DOC "Lua runtime wrapper for executing and completing Lua blobs"
#ifdef PY3
static PyModuleDef lup_module = {
  PyModuleDef_HEAD_INIT,
  .m_name = MOD_NAME,
  .m_doc = MOD_DOC,
  .m_size = -1,
};
#endif


PyMODINIT_FUNC
#if defined (PY3)
PyInit_lua_runtime()
#elif defined (PY2)
initlua_runtime()
#endif
{
  if (PyType_Ready(&PyLuaRuntimeType) < 0) { return PyMODINIT_RET(NULL); }

#if defined (PY3)
  PyObject * m = PyModule_Create(&lup_module);
#elif defined (PY2)
  PyObject * m = Py_InitModule3(MOD_NAME, NULL, MOD_DOC);
#endif
  if (m == NULL) { return PyMODINIT_RET(NULL); }

  Py_INCREF(&PyLuaRuntimeType);
  PyModule_AddObject(m, "LuaRuntime", (PyObject *)&PyLuaRuntimeType);
  PyMODINIT_RET(return m;)
}