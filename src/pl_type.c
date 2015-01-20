#include "pl_type.h"

#include <stdlib.h>
#include <string.h>

#include "pl_gc.h"
#include "pl_err.h"


// typename
const char *object_typename(enum_object_type_t type){
  switch(type){
  case TYPE_RAW       : return "TYPE_RAW      ";
  case TYPE_INT       : return "TYPE_INT      ";
  case TYPE_FLOAT     : return "TYPE_FLOAT    ";
  case TYPE_STR       : return "TYPE_STR      ";
  case TYPE_SYMBOL    : return "TYPE_SYMBOL   ";
  case TYPE_GC_BROKEN : return "TYPE_GC_BROKEN";
  case TYPE_EXTRA     : return "TYPE_EXTRA    ";
  case TYPE_REF       : return "TYPE_REF      ";
  default             : return "TYPE_UNKNOW   ";
  }
  return "TYPE_UNKNOW   ";
}

// object init/halt
#define TMP_OBJECT_INIT(_tname, tname_enum, args, body) \
int object##_tname##_init args{ \
  PL_CHECK_RET_BEG \
  object##_tname##_part_t *_part = NULL; \
  PL_ASSERT_NOT_NULL(thiz); \
  PL_ASSERT_NOT_NULL(_part = object_as##_tname (thiz)); \
  body; \
  PL_CHECK_RET_END \
}

TMP_OBJECT_INIT(_raw, TYPE_RAW, (object_t *thiz, void *ptr),
                _part->ptr = ptr;
               )

TMP_OBJECT_INIT(_int, TYPE_INT, (object_t *thiz, long int value),
                _part->value = value;
               )

TMP_OBJECT_INIT(_float, TYPE_FLOAT, (object_t *thiz, double value),
                _part->value = value;
               )

TMP_OBJECT_INIT(_str, TYPE_STR, (object_t *thiz, const char* text),
                _part->size = strlen(text);
                _part->str = malloc(_part->size + 1);
                memcpy(_part->str, text, _part->size);
                _part->str[_part->size] = '\0';
               )

TMP_OBJECT_INIT(_symbol, TYPE_SYMBOL, (object_t *thiz, const char* name),
                _part->size = strlen(name);
                _part->str = malloc(_part->size + 1);
                memcpy(_part->str, name, _part->size);
                _part->str[_part->size] = '\0';
               )

TMP_OBJECT_INIT(_gc_broken, TYPE_GC_BROKEN, (object_t *thiz, void *ptr),
                _part->ptr = (object_t*)ptr;
               )

TMP_OBJECT_INIT(_ref, TYPE_REF, (object_t *thiz, void *ptr),
                _part->ptr = (object_t*)ptr;
               )

#undef TMP_OBJECT_INIT

int object_raw_halt(object_t *thiz){
  if(object_as_raw(thiz)!=NULL) {free(object_as_raw(thiz)->ptr);}
  return 0;
}
int object_int_halt(object_t *thiz){
  return 0;
}
int object_float_halt(object_t *thiz){
  return 0;
}
int object_str_halt(object_t *thiz){
  if(object_as_str(thiz)!=NULL) {free(object_as_str(thiz)->str);}
  return 0;
}
int object_symbol_halt(object_t *thiz){
  if(object_as_symbol(thiz)!=NULL) {free(object_as_symbol(thiz)->str);}
  return 0;
}
int object_gc_broken_halt(object_t *thiz){
  return 0;
}
int object_ref_halt(object_t *thiz){
  return 0;
}

int object_halt(object_t *obj){
  PL_CHECK_RET_BEG
  PL_ASSERT_NOT_NULL(obj);
  switch(obj->type){
  case TYPE_RAW:
    PL_CHECK_RET(object_raw_halt(obj));
    break;
  case TYPE_INT:
    PL_CHECK_RET(object_int_halt(obj));
    break;
  case TYPE_FLOAT:
    PL_CHECK_RET(object_float_halt(obj));
    break;
  case TYPE_STR:
    PL_CHECK_RET(object_str_halt(obj));
    break;
  case TYPE_SYMBOL:
    PL_CHECK_RET(object_symbol_halt(obj));
    break;
  case TYPE_GC_BROKEN:
    PL_CHECK_RET(object_gc_broken_halt(obj));
    break;
  case TYPE_EXTRA:
    // extra halt
    break;
  case TYPE_REF:
    PL_CHECK_RET(object_ref_halt(obj));
    break;
  default:
    PL_ASSERT(0, err_typecheck());
    break;
  }
  PL_CHECK_RET_END
}


// object copy init

int object_copy_init(object_t *src, object_t *dst){
  PL_CHECK_RET_BEG
  PL_ASSERT_NOT_NULL(src);
  PL_ASSERT_NOT_NULL(dst);
  
  switch(src->type){
  case TYPE_RAW:
  case TYPE_INT:
  case TYPE_FLOAT:
  case TYPE_STR:
  case TYPE_SYMBOL:
  case TYPE_GC_BROKEN:
  case TYPE_REF:
    memcpy(dst, src, src->size);
    break;
  case TYPE_EXTRA:
    memcpy(dst, src, src->size);
    // extra copy init
    break;
  default:
    PL_ASSERT(0, err_typecheck());
  }
  PL_CHECK_RET_END
}


// object cast

int object_type_check(object_t *obj, enum_object_type_t type){
  return obj!=NULL && obj->type == type;
}
void* object_part(object_t *obj){
  PL_ASSERT_EX(obj!=NULL, err_null(), return NULL);
  size_t obj_addr = (size_t)obj;
  return (void*)(obj_addr + sizeof(object_t));
}

#define TMP_OBJECT_AS_DECL(_tname, tname_enum) \
object##_tname##_part_t *object_as##_tname (object_t *obj){ \
  PL_ASSERT_EX(obj!=NULL, err_null(), return NULL); \
  PL_ASSERT_EX(object_type_check(obj, tname_enum), err_null(), return NULL); \
  return (object##_tname##_part_t*)object_part(obj); \
}

TMP_OBJECT_AS_DECL(_raw,       TYPE_RAW      )
TMP_OBJECT_AS_DECL(_int,       TYPE_INT      )
TMP_OBJECT_AS_DECL(_float,     TYPE_FLOAT    )
TMP_OBJECT_AS_DECL(_str,       TYPE_STR      )
TMP_OBJECT_AS_DECL(_symbol,    TYPE_SYMBOL   )
TMP_OBJECT_AS_DECL(_gc_broken, TYPE_GC_BROKEN)
TMP_OBJECT_AS_DECL(_extra,     TYPE_EXTRA    )
TMP_OBJECT_AS_DECL(_ref,       TYPE_REF      )

#undef TMP_OBJECT_AS_DECL


// object tuple
object_t *object_tuple_alloc(gc_manager_t *gcm, size_t size){
  object_t *ret = gc_manager_object_array_alloc(gcm, TYPE_REF, size);
  PL_ASSERT_EX(ret!=NULL, err_null(), return NULL);
  return ret;
}
int object_member_set_value(object_t *tuple, size_t offset, object_t *value){
  PL_CHECK_RET_BEG
  PL_ASSERT(offset<object_array_size(tuple), err_out_of_range());
  PL_ASSERT_NOT_NULL(object_as_ref(tuple));
  object_as_ref(tuple)[offset].ptr = value;
  PL_CHECK_RET_END
}
void *object_member(object_t *tuple, size_t offset){
  PL_ASSERT_EX(object_as_ref(tuple)!=NULL, err_null(), return NULL);
  PL_ASSERT_EX(offset<object_array_size(tuple), err_out_of_range(), return NULL);
  PL_ASSERT_EX(object_as_ref(tuple)[offset].ptr!=NULL, err_null(), return NULL);
  return object_part(object_as_ref(tuple)[offset].ptr);
}

#define TMP_OBJECT_MEMBER_DECL(_tname, tname_enum) \
object##_tname##_part_t *object_member##_tname (object_t *tuple, size_t offset){ \
  PL_ASSERT_EX(object_as_ref(tuple)!=NULL, err_null(), return NULL); \
  PL_ASSERT_EX(object_type_check(object_as_ref(tuple)[offset].ptr, tname_enum), err_null(), return NULL); \
  return (object##_tname##_part_t*)object_member(tuple, offset); \
}

TMP_OBJECT_MEMBER_DECL(_raw,       TYPE_RAW      )
TMP_OBJECT_MEMBER_DECL(_int,       TYPE_INT      )
TMP_OBJECT_MEMBER_DECL(_float,     TYPE_FLOAT    )
TMP_OBJECT_MEMBER_DECL(_str,       TYPE_STR      )
TMP_OBJECT_MEMBER_DECL(_symbol,    TYPE_SYMBOL   )
TMP_OBJECT_MEMBER_DECL(_gc_broken, TYPE_GC_BROKEN)
TMP_OBJECT_MEMBER_DECL(_extra,     TYPE_EXTRA    )
TMP_OBJECT_MEMBER_DECL(_ref,       TYPE_REF      )

#undef TMP_OBJECT_MEMBER_DECL

// object size

size_t object_sizeof_part(enum_object_type_t obj_type){
  switch(obj_type){
  case TYPE_RAW:       return sizeof(object_raw_part_t);
  case TYPE_INT:       return sizeof(object_int_part_t);
  case TYPE_FLOAT:     return sizeof(object_float_part_t);
  case TYPE_STR:       return sizeof(object_str_part_t);
  case TYPE_SYMBOL:    return sizeof(object_symbol_part_t);
  case TYPE_GC_BROKEN: return sizeof(object_gc_broken_part_t);
  case TYPE_EXTRA:     return sizeof(object_extra_part_t);
  case TYPE_REF:       return sizeof(object_ref_part_t);
  default:             break;
  }
  PL_ASSERT_EX(0, err_typecheck(), 0);
  return 0;
}

size_t object_array_sizeof(enum_object_type_t obj_type, size_t n){
  return sizeof(object_header_t) + n * object_sizeof_part(obj_type);
}

size_t object_sizeof(enum_object_type_t obj_type){
  return object_array_sizeof(obj_type, 1);
}

size_t object_array_size(object_t *obj){
  PL_ASSERT_EX(obj!=NULL, err_null(), return 0);
  size_t value_size = obj->size - sizeof(object_header_t);
  return value_size / object_sizeof_part(obj->type);
}
void* object_array_index(object_t *obj, size_t index){
  PL_ASSERT_EX(obj!=NULL, err_null(), return NULL);
  size_t obj_addr = (size_t)obj;
  size_t ret_addr = obj_addr + sizeof(object_header_t) + index * object_sizeof_part(obj->type);
  if(ret_addr >= obj_addr + obj->size) {return NULL;}
  
  return (void*)ret_addr;
}


// object gc support

int object_mark(object_t *obj, int mark){
  PL_CHECK_RET_BEG
  object_ref_part_t *value_ref = NULL;
  size_t count;
  
  if(obj==NULL) {return 0;}
  if(obj->mark == mark) {return 0;}
  
  obj->mark = mark;
  
  count = object_array_size(obj);
  PL_ASSERT(count>0, err_out_of_range());
  
  switch(obj->type){
  case TYPE_RAW:
  case TYPE_INT:
  case TYPE_FLOAT:
  case TYPE_STR:
  case TYPE_SYMBOL:
    break;
  case TYPE_EXTRA:
    // extra-mark
    break;
  case TYPE_REF:
    value_ref = object_as_ref(obj);
    PL_ASSERT_NOT_NULL(value_ref);
    while(count --> 0){
      PL_CHECK_RET(object_mark(value_ref[count].ptr, mark));
    }
    break;
  default:
    PL_ASSERT(0, err_typecheck());
    break;
  }
  PL_CHECK_RET_END
}


int object_move(object_t *obj_old, object_t *obj_new){
  PL_CHECK_RET_BEG
  PL_ASSERT_NOT_NULL(obj_old);
  PL_ASSERT_NOT_NULL(obj_new);
  if(obj_old != obj_new){
    memmove(obj_new, obj_old, obj_old->size);
  }
  PL_CHECK_RET_END
}


int obj_ptr_fix_gc_broken(object_t **pobj){
  PL_CHECK_RET_BEG
  PL_ASSERT_NOT_NULL(pobj);
  PL_ASSERT_NOT_NULL(*pobj);
  
  object_gc_broken_part_t *broken_value = NULL;
  
  if((*pobj)->type == TYPE_GC_BROKEN){
    broken_value = object_as_gc_broken(*pobj);
    PL_ASSERT_NOT_NULL(broken_value);
    *pobj = broken_value->ptr;
  }
  PL_CHECK_RET_END
}
int object_fix_gc_broken(object_t *obj){
  PL_CHECK_RET_BEG
  PL_ASSERT_NOT_NULL(obj);
  object_ref_part_t *value_ref = NULL;
  size_t count = object_array_size(obj);
  PL_ASSERT(count>0, err_out_of_range());
  
  switch(obj->type){
  case TYPE_RAW:
  case TYPE_INT:
  case TYPE_FLOAT:
  case TYPE_STR:
  case TYPE_SYMBOL:
    break;
  case TYPE_EXTRA:
    // extra-fix_gc_broken
    break;
  case TYPE_REF:
    value_ref = object_as_ref(obj);
    PL_ASSERT_NOT_NULL(value_ref);
    while(count --> 0){
      PL_CHECK_RET(obj_ptr_fix_gc_broken(&(value_ref[count].ptr)));
    }
    break;
  default:
    break;
  }
  PL_CHECK_RET_END
}


int object_ptr_rebase(object_t **pobj, object_t *old_pool, size_t old_pool_size, object_t *new_pool){
  PL_CHECK_RET_BEG
  PL_ASSERT_NOT_NULL(pobj);
  PL_ASSERT_NOT_NULL(*pobj);
  PL_ASSERT_NOT_NULL(old_pool);
  PL_ASSERT_NOT_NULL(new_pool);
  size_t old_pool_addr     = (size_t)old_pool;
  size_t old_pool_end_addr = (size_t)old_pool + old_pool_size;
  size_t new_pool_addr     = (size_t)new_pool;
  size_t pobj_addr         = (size_t)*pobj;
  size_t new_pobj_addr;
  
  if(pobj_addr<old_pool_addr || old_pool_end_addr<=pobj_addr) {return 0;}
  
  new_pobj_addr = pobj_addr - old_pool_addr + new_pool_addr;
  
  *pobj = (object_t*)new_pobj_addr;
  
  PL_CHECK_RET_END
}

int object_rebase(object_t *obj, object_t *old_pool, size_t old_pool_size, object_t *new_pool){
  PL_CHECK_RET_BEG
  PL_ASSERT_NOT_NULL(obj);
  size_t count = object_array_size(obj);
  object_ref_part_t *value_ref = NULL;
  
  switch(obj->type){
  case TYPE_RAW:
  case TYPE_INT:
  case TYPE_FLOAT:
  case TYPE_STR:
  case TYPE_SYMBOL:
    break;
  case TYPE_EXTRA:
    // extra-rebase
    break;
  case TYPE_REF:
    value_ref = (object_ref_part_t*)object_part(obj);
    PL_ASSERT_NOT_NULL(value_ref);
    while(count --> 0){
      PL_CHECK_RET(object_ptr_rebase(&(value_ref[count].ptr), old_pool, old_pool_size, new_pool));
    }
    break;
  default:
    break;
  }
  
  PL_CHECK_RET_END
}
