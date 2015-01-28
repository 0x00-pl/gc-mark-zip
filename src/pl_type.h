#ifndef _PL_TYPE_H_
#define _PL_TYPE_H_

#include <limits.h>
#include <stddef.h>
#include "pl_err.h"

enum type_e{
  TYPE_RAW,
  TYPE_INT,
  TYPE_FLOAT,
  TYPE_STR,
  TYPE_SYMBOL,
  TYPE_GC_BROKEN,
  TYPE_REF,
  TYPE_VECTOR,
  TYPE_UNKNOW
};
typedef size_t enum_object_type_t;

// typename
const char *object_typename(enum_object_type_t);


struct object_t_decl;

typedef struct{
  int auto_free;
  void *ptr;
} object_raw_part_t;

typedef long int object_int_value_t;
typedef struct{
  object_int_value_t value;
} object_int_part_t;

typedef double object_float_value_t;
typedef struct{
  object_float_value_t value;
} object_float_part_t;

typedef struct{
  char *str;
  size_t size;
} object_str_part_t;

typedef struct{
  struct object_t_decl *name;
} object_symbol_part_t;

typedef struct{
  struct object_t_decl *ptr;
} object_gc_broken_part_t;

typedef struct{
  size_t count; // data count
  struct object_t_decl *pdata;
} object_vector_part_t;

typedef struct{
  struct object_t_decl *ptr;
} object_ref_part_t;

typedef struct object_header_t_decl{
  void *move_dest;
  size_t size; // sizeof(object_t) + array_size * sizeof(value)
  size_t type;
  size_t mark;
} object_header_t;

typedef struct object_t_decl {
  void *move_dest;
  size_t size; // sizeof(object_t) + array_size * sizeof(value)
  size_t type;
  size_t mark;
  union{
    object_raw_part_t _raw;
    object_int_part_t _int;
    object_float_part_t _float;
    object_str_part_t _str;
    object_symbol_part_t _symbol;
    object_gc_broken_part_t _gc_broken;
    object_vector_part_t _vector;
    object_ref_part_t _ref;
    char _unknow;
  } part;
} object_t;

// object init/halt
err_t *object_raw_init(err_t **err, object_t *thiz, void *ptr, int auto_free);
err_t *object_int_init(err_t **err, object_t *thiz, long int value);
err_t *object_float_init(err_t **err, object_t *thiz, double value);
err_t *object_str_init(err_t **err, object_t *thiz, const char *text);
err_t *object_symbol_init(err_t **err, object_t *thiz, object_t *name);
err_t *object_gc_broken_init(err_t **err, object_t *thiz, object_t *ptr);
err_t *object_vector_init(err_t **err, object_t *thiz);
err_t *object_ref_init(err_t **err, object_t *thiz, object_t *ptr);

err_t *object_raw_halt(err_t **err, object_t *thiz);
err_t *object_str_halt(err_t **err, object_t *thiz);
err_t *object_halt(err_t **err, object_t *obj);


err_t *object_raw_part_init(err_t **err, object_raw_part_t *part, void *ptr, int auto_free);
err_t *object_int_part_init(err_t **err, object_int_part_t *part, long int value);
err_t *object_float_part_init(err_t **err, object_float_part_t *part, double value);
err_t *object_str_part_init(err_t **err, object_str_part_t *part, const char* text);
err_t *object_symbol_part_init(err_t **err, object_symbol_part_t *part, object_t *name);
err_t *object_gc_broken_part_init(err_t **err, object_gc_broken_part_t *part, object_t *ptr);
err_t *object_vector_part_init(err_t **err, object_vector_part_t *part);
err_t *object_ref_part_init(err_t **err, object_ref_part_t *part, object_t *ptr);
err_t *object_part_halt(err_t **err, void *part, enum_object_type_t type);


// object copy
err_t *object_copy_init(err_t **err, object_t *src, object_t *dst);


// object cast
err_t *object_type_check(err_t **err, object_t *obj, enum_object_type_t type);
void* object_part(err_t **err, object_t *obj);
object_raw_part_t       *object_as_raw       (err_t **err, object_t *obj);
object_int_part_t       *object_as_int       (err_t **err, object_t *obj);
object_float_part_t     *object_as_float     (err_t **err, object_t *obj);
object_str_part_t       *object_as_str       (err_t **err, object_t *obj);
object_symbol_part_t    *object_as_symbol    (err_t **err, object_t *obj);
object_gc_broken_part_t *object_as_gc_broken (err_t **err, object_t *obj);
object_vector_part_t    *object_as_vector    (err_t **err, object_t *obj);
object_ref_part_t       *object_as_ref       (err_t **err, object_t *obj);


// object tuple
struct gc_manager_t_decl;
object_t *object_tuple_alloc(err_t **err, struct gc_manager_t_decl *gcm, size_t size);
err_t *object_member_set_value(err_t **err, object_t *tuple, size_t index, object_t *value);
void                    *object_member           (err_t **err, object_t *tuple, size_t offset);
object_raw_part_t       *object_member_raw       (err_t **err, object_t *tuple, size_t offset);
object_int_part_t       *object_member_int       (err_t **err, object_t *tuple, size_t offset);
object_float_part_t     *object_member_float     (err_t **err, object_t *tuple, size_t offset);
object_str_part_t       *object_member_str       (err_t **err, object_t *tuple, size_t offset);
object_symbol_part_t    *object_member_symbol    (err_t **err, object_t *tuple, size_t offset);
object_gc_broken_part_t *object_member_gc_broken (err_t **err, object_t *tuple, size_t offset);
object_vector_part_t    *object_member_vector    (err_t **err, object_t *tuple, size_t offset);
object_ref_part_t       *object_member_ref       (err_t **err, object_t *tuple, size_t offset);

// object vecter
struct gc_manager_t_decl;
err_t *object_vector_part_pop(err_t **err, object_vector_part_t *vector_part, object_t *dest);
void *object_vector_part_top(err_t **err, object_vector_part_t *vector_part, object_t *dest);
object_t *object_vector_part_to_array(err_t **err, object_vector_part_t *vector_part, struct gc_manager_t_decl *gcm);
void *object_vector_part_index(err_t **err, object_vector_part_t *vector_part, int index, object_t *dest);
enum_object_type_t object_vector_part_type(err_t **err, object_vector_part_t *vector_part);


err_t *object_vector_push(err_t **err, struct gc_manager_t_decl *gcm, object_t *vec, object_t *item);
err_t *object_vector_pop(err_t **err, object_t *vec, object_t *dest);
void *object_vector_top(err_t **err, object_t *vec, object_t *dest);
object_t *object_vector_to_array(err_t **err, object_t *vec, struct gc_manager_t_decl *gcm);
void *object_vector_index(err_t **err, object_t *vec, int index, object_t *dest);
object_t *object_vector_ref_index(err_t **err, object_t *vec, int index);
enum_object_type_t object_vector_type(err_t **err, object_t *vec);
size_t object_vector_count(err_t **err, object_t *vec);

//object str
int object_str_eq(object_t *s1, object_t *s2);

// object size
size_t object_sizeof(err_t **err, enum_object_type_t obj_type);
size_t object_sizeof_part(err_t **err, enum_object_type_t obj_type);
size_t object_array_sizeof(err_t **err, enum_object_type_t obj_type, size_t n);
size_t object_array_count(err_t **err, object_t *obj);
void* object_array_index(err_t **err, object_t *obj, size_t index);
#define OBJ_ARR_AT(array, _part_name, index) ((&((array)->part._part_name))[index])

// object verbose
err_t *object_verbose(err_t** err, object_t* obj, int recursive, size_t indentation, size_t limit);

// object gc support
err_t *object_mark(err_t **err, object_t *obj, size_t mark, size_t limit);
err_t *object_ptr_rebase(err_t **err, object_t **pobj, object_t *old_pool, size_t old_pool_size, object_t *new_pool);
err_t *object_rebase(err_t **err, object_t *obj, object_t *old_pool, size_t old_pool_size, object_t *new_pool);
err_t *object_move(err_t **err, object_t *obj_old, object_t *obj_new);
err_t *object_part_move(err_t **err, void *part_src, void *part_dst, enum_object_type_t type);

err_t *object_ptr_gc_relink(err_t **err, object_t **pobj);
err_t *object_gc_relink(err_t **err, object_t *obj);
// err_t *obj_ptr_fix_gc_broken(err_t **err, object_t **pobj);
// err_t *object_fix_gc_broken(err_t **err, object_t *obj);
#endif
