/*
The MIT License (MIT)

Copyright (c) 2015 - Latino

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "khash.h"
#include "latino.h"
#include "object.h"
#include "libmem.h"
#include "utils.h"

KHASH_MAP_INIT_INT64(env, lat_object);
typedef khash_t(env) lat_env;
lat_env* globals;

struct sym_key {
  const char* ptr;
  size_t len;
};

static khint_t
sym_hash(struct sym_key key)
{
  const char* s = key.ptr;
  khint_t h;
  size_t len = key.len;
  h = *s++;
  while (len--) {
    h = (h << 5) - h + (khint_t)*s++;
  }
  return h;
}

static khint_t
sym_eq(struct sym_key a, struct sym_key b)
{
  if (a.len != b.len)
    return false;
  if (memcmp(a.ptr, b.ptr, a.len) == 0)
    return true;
  return false;
}

KHASH_INIT(sym, struct sym_key, lat_object*, 1, sym_hash, sym_eq);
static khash_t(sym) * sym_table;

static lat_object* str_new(const char* p, size_t len)
{
  lat_object* str = (lat_object*)lat_asignar_memoria(sizeof(lat_object));
  str->type = T_STR;
  str->data_size = len;
  str->data.str = (char*)p;
  return str;
}

static lat_object* str_intern(const char* p, size_t len)
{
  khiter_t k;
  struct sym_key key;
  int ret;
  lat_object* str;
  if (!sym_table) {
    sym_table = kh_init(sym);
  }
  key.ptr = p;
  key.len = len;
  k = kh_put(sym, sym_table, key, &ret);
  if (ret == 0) {
    return kh_value(sym_table, k);
  }
  str = str_new(p, len);
  kh_key(sym_table, k).ptr = str->data.str;
  kh_value(sym_table, k) = str;
  return str;
}

lat_object* lat_cadena_hash(const char* p, size_t len)
{
  if (p && (len < MAX_STR_INTERN)) {
    return str_intern(p, len);
  }
  return str_new(p, len);
}

void lat_comparar(lat_vm* vm)
{
  lat_object* b = lat_desapilar(vm);
  lat_object* a = lat_desapilar(vm);
  vm->regs[255] = lat_entero_nuevo(vm, strcmp(lat_obtener_cadena(a), lat_obtener_cadena(b)));
}

void lat_concatenar(lat_vm* vm)
{
  lat_object* b = lat_desapilar(vm);
  lat_object* a = lat_desapilar(vm);
  vm->regs[255] = lat_cadena_nueva(vm, concat(lat_obtener_cadena(a), lat_obtener_cadena(b)));
}

void lat_contiene(lat_vm* vm)
{
  lat_object* b = lat_desapilar(vm);
  lat_object* a = lat_desapilar(vm);
  char *result = strstr(lat_obtener_cadena(a), lat_obtener_cadena(b));
  if (result != NULL){
	  vm->regs[255] = vm->true_object;
  }
  else{
	  vm->regs[255] = vm->false_object;
  }
}

void lat_copiar(lat_vm* vm)
{
  lat_object* b = lat_desapilar(vm);
  vm->regs[255] = lat_clonar_objeto(vm, b);
}

void lat_termina_con(lat_vm* vm)
{
  lat_object* b = lat_desapilar(vm);
  lat_object* a = lat_desapilar(vm);
  if (endsWith(lat_obtener_cadena(a), lat_obtener_cadena(b))){
    vm->regs[255] = vm->true_object;
  }
  else{
    vm->regs[255] = vm->false_object;
  }
}

void lat_es_igual(lat_vm* vm)
{
  lat_object* b = lat_desapilar(vm);
  lat_object* a = lat_desapilar(vm);
  if (strcmp(lat_obtener_cadena(a), lat_obtener_cadena(b)) == 0){
    vm->regs[255] = vm->true_object;
  }
  else{
    vm->regs[255] = vm->false_object;
  }
}

/*
void lat_format(lat_vm* vm){

}
*/

void lat_indice(lat_vm* vm){
  lat_object* b = lat_desapilar(vm);
  lat_object* a = lat_desapilar(vm);
  vm->regs[255] = lat_entero_nuevo(vm, indexOf(lat_obtener_cadena(a), lat_obtener_cadena(b)));
}

void lat_insertar(lat_vm* vm){
  lat_object* c = lat_desapilar(vm);
  lat_object* b = lat_desapilar(vm);
  lat_object* a = lat_desapilar(vm);
  vm->regs[255] = lat_cadena_nueva(vm, insert(lat_obtener_cadena(a), lat_obtener_cadena(b), lat_obtener_entero(c)));
}

void lat_ultimo_indice(lat_vm* vm){
  lat_object* b = lat_desapilar(vm);
  lat_object* a = lat_desapilar(vm);
  vm->regs[255] = lat_entero_nuevo(vm, lastIndexOf(lat_obtener_cadena(a), lat_obtener_cadena(b)));
}

void lat_rellenar_izquierda(lat_vm* vm){
  lat_object* c = lat_desapilar(vm);
  lat_object* b = lat_desapilar(vm);
  lat_object* a = lat_desapilar(vm);
  vm->regs[255] = lat_cadena_nueva(vm, padLeft(lat_obtener_cadena(a), lat_obtener_entero(b), lat_obtener_caracter(c)));
}

void lat_rellenar_derecha(lat_vm* vm){
  lat_object* c = lat_desapilar(vm);
  lat_object* b = lat_desapilar(vm);
  lat_object* a = lat_desapilar(vm);
  vm->regs[255] = lat_cadena_nueva(vm, padRight(lat_obtener_cadena(a), lat_obtener_entero(b), lat_obtener_caracter(c)));
}

void lat_eliminar(lat_vm* vm){
  lat_object* b = lat_desapilar(vm);
  lat_object* a = lat_desapilar(vm);
  vm->regs[255] = lat_cadena_nueva(vm, replace(lat_obtener_cadena(a), lat_obtener_cadena(b), (char*)""));
}

void lat_esta_vacia(lat_vm* vm){
  lat_object* a = lat_desapilar(vm);
  if (strcmp(lat_obtener_cadena(a), "") == 0){
    vm->regs[255] = vm->true_object;
  }
  else{
    vm->regs[255] = vm->false_object;
  }
}

void lat_longitud(lat_vm* vm){
  lat_object* a = lat_desapilar(vm);
  vm->regs[255] = lat_entero_nuevo(vm, strlen(lat_obtener_cadena(a)));
}

void lat_reemplazar(lat_vm* vm){
  lat_object* c = lat_desapilar(vm);
  lat_object* b = lat_desapilar(vm);
  lat_object* a = lat_desapilar(vm);
  vm->regs[255] = lat_cadena_nueva(vm, replace(lat_obtener_cadena(a), lat_obtener_cadena(b), lat_obtener_cadena(c)));
}

void lat_empieza_con(lat_vm* vm)
{
  lat_object* b = lat_desapilar(vm);
  lat_object* a = lat_desapilar(vm);
  if (startsWith(lat_obtener_cadena(a), lat_obtener_cadena(b))){
    vm->regs[255] = vm->true_object;
  }
  else{
    vm->regs[255] = vm->false_object;
  }
}

void lat_restarcadena(lat_vm* vm){
  lat_object* c = lat_desapilar(vm);
  lat_object* b = lat_desapilar(vm);
  lat_object* a = lat_desapilar(vm);
  vm->regs[255] = lat_cadena_nueva(vm, substring(lat_obtener_cadena(a), lat_obtener_entero(b), lat_obtener_entero(c)));
}

void lat_minusculas(lat_vm* vm){
  lat_object* a = lat_desapilar(vm);
  vm->regs[255] = lat_cadena_nueva(vm, toLower(lat_obtener_cadena(a)));
}

void lat_mayusculas(lat_vm* vm){
  lat_object* a = lat_desapilar(vm);
  vm->regs[255] = lat_cadena_nueva(vm, toUpper(lat_obtener_cadena(a)));
}

void lat_quitar_espacios(lat_vm* vm){
  lat_object* a = lat_desapilar(vm);
  vm->regs[255] = lat_cadena_nueva(vm, trim(lat_obtener_cadena(a)));
}
