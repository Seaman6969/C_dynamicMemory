#ifndef _VECTOR_H
#define _VECTOR_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct vector
{
  unsigned char initialized;
  size_t size;
  size_t len;
  void *ptr;
  //
  unsigned char (*set)(struct vector *self, void *ptr, size_t len);
  unsigned char (*init)(void *self);
  unsigned char (*free)(void *self);
  unsigned char (*equals)(void *elem, void *cmp);
  unsigned char (*bigger)(void *elem, void *cmp);
  unsigned char (*smaller)(void *elem, void *cmp);
  unsigned char (*clone)(void *dest, void *src);
  unsigned char (*push)(struct vector *self, void *value);
  unsigned char (*pop)(struct vector *self, size_t index);
  unsigned char (*subvector)(struct vector *self, struct vector *receive, size_t index1, size_t index2);
  unsigned char (*add)(struct vector *self, void *added, size_t added_len);
  unsigned char (*prependElement)(struct vector *self, void *value);
  unsigned char (*prepend)(struct vector *self, void *added, size_t len);
  unsigned char (*insertElement)(struct vector *self, void *value, size_t index);
  unsigned char (*insert)(struct vector *self, void *value, size_t value_len, size_t index);
  unsigned char (*replace)(struct vector *self, void *pattern, size_t pattern_len, void *substitute, size_t substitute_len);
  unsigned char (*replaceAll)(struct vector *self, void *pattern, size_t pattern_len, void *substitute, size_t substitute_len);
  unsigned char (*indexOf)(struct vector *self, void *pattern, size_t pattern_len, size_t *index);
  unsigned char (*replaceNth)(struct vector *self, void *pattern, size_t pattern_len, void *substitute, size_t substitute_len, size_t n);
  unsigned char (*sort)(struct vector *self, unsigned char ascending);
  unsigned char (*contains)(struct vector *self, void *pattern, size_t pattern_len, unsigned char *res, size_t *where, size_t from);
  unsigned char (*bricks)(struct vector *self, void *pattern, size_t pattern_len, void ***result, size_t *xLen, size_t **yLens, size_t sizePtr);
} vector;
// verified
unsigned char vector_clone(vector *dest, vector *src)
{
  if (dest == NULL || src == NULL)
  {
    return 1;
  }
  *dest = *src;
  dest->ptr = malloc(src->size * src->len);
  if (dest->ptr == NULL)
  {
    return 2;
  }
  if (src->clone == NULL)
  {
    memcpy(dest->ptr, src->ptr, src->size * src->len);
  }
  else
  {
    size_t i;
    for (i = 0; i < src->len; i++)
    {
      if (src->clone(dest->ptr + i * dest->size, src->ptr + i * src->size))
      {
        return 3;
      }
    }
  }
  return 0;
}
// verified
unsigned char vector_free(vector *ptr)
{
  if (ptr == NULL)
  {
    return 1;
  }
  if (ptr->free)
  {
    for (size_t i = 0; i < ptr->len; i++)
    {
      if (ptr->free(ptr->ptr + i * ptr->size))
      {
        return 2;
      }
    }
  }
  free(ptr->ptr);
  free(ptr);
  ptr = NULL;
  return 0;
}
void vector_assignFunctions(vector *self);
// verified
vector *vector_new(size_t size, unsigned char (*init)(void *self),
                   unsigned char (*clone)(void *dest, void *src),
                   unsigned char (*free)(void *ptr),
                   unsigned char (*equals)(void *elem, void *cmp),
                   unsigned char (*bigger)(void *elem, void *cmp),
                   unsigned char (*smaller)(void *elem, void *cmp))
{
  vector *res = (vector *)malloc(sizeof(vector));
  if (res == NULL)
  {
    return NULL;
  }
  res->initialized = 0;
  res->len = 0;
  res->ptr = malloc(size);
  res->size = size;
  if (res->ptr == NULL)
  {
    return NULL;
  }
  res->init = init;
  res->clone = clone;
  res->free = free;
  res->equals = equals;
  res->bigger = bigger;
  res->smaller = smaller;
  vector_assignFunctions(res);
  return res;
}
// verified
unsigned char vector_set(vector *self, void *ptr, size_t len)
{
  if (!self || !ptr)
  {
    return 1;
  }
  if (!self->initialized)
  {
    return 2;
  }
  void *aux;
  aux = realloc(self->ptr, self->size * len);
  if (!aux)
  {
    return 3;
  }
  self->ptr = aux;
  memcpy(self->ptr, aux, self->size * len);
  self->len = len;
  if (self->init)
  {
    size_t i;
    for (i = 0; i < self->len; i++)
    {
      if (self->init(self->ptr + self->size * i))
      {
        return 4;
      }
    }
  }
  return 0;
}
// verified
unsigned char vector_push(vector *self, void *value)
{
  if (self == NULL || value == NULL)
  {
    return 1;
  }
  void *new_ptr;
  if (self->initialized)
  {
    new_ptr = realloc(self->ptr, self->size * (self->len + 1));
    if (new_ptr == NULL)
    {
      return 2;
    }
    else
    {
      self->ptr = new_ptr;
    }
    if (self->clone != NULL)
    {
      if (self->clone(self->ptr + self->len * self->size, value))
      {
        return 3;
      }
    }
    else
    {
      memcpy(self->ptr + self->len * self->size, value, self->size);
    }
    if (self->init != NULL)
    {
      if (self->init(self->ptr + self->len * self->size))
      {
        return 4;
      }
    }
    self->len++;
  }
  else
  {
    if (self->clone != NULL)
    {
      if (self->clone(self->ptr, value))
      {
        return 5;
      }
    }
    else
    {
      memcpy(self->ptr, value, self->size);
    }
    self->initialized = 1;
    self->len = 1;
    if (self->init != NULL)
    {
      if (self->init(self->ptr))
      {
        return 6;
      }
    }
  }
  return 0;
}
// verified
unsigned char vector_pop(vector *self, size_t index)
{
  if (self == NULL)
  {
    return 1;
  }
  if (!self->initialized || self->len <= index)
  {
    return 2;
  }
  if (self->free != NULL)
  {
    if (self->free(self->ptr + index * self->size))
    {
      return 3;
    }
  }
  void *new_ptr;
  if (self->len == 1 && index == 0)
  {
    self->len = 0;
    self->initialized = 0;
    return 0;
  }
  if (self->len - 1 != index)
  {
    memmove(self->ptr + index * self->size,
            self->ptr + (index + 1) * self->size,
            (self->len - index - 1) * self->size);
    new_ptr = realloc(self->ptr, (self->len - 1) * self->size);
    if (new_ptr == NULL)
    {
      return 4;
    }
    else
    {
      self->ptr = new_ptr;
    }
  }
  else
  {
    new_ptr = realloc(self->ptr, (self->len - 1) * self->size);
    if (new_ptr == NULL)
    {
      return 5;
    }
    else
    {
      self->ptr = new_ptr;
    }
  }
  self->len--;
  return 0;
}
// verified
unsigned char vector_subvector(vector *self, vector *receive, size_t index1,
                               size_t index2)
{
  if (self == NULL || receive == NULL)
  {
    return 1;
  }
  vector *aux =
      vector_new(receive->size, receive->init, receive->clone, receive->free,
                 receive->equals, receive->bigger, receive->smaller);
  if (aux == NULL)
  {
    return 2;
  }
  if (vector_clone(receive, aux))
  {
    return 3;
  }
  if (vector_free(aux))
  {
    return 4;
  }
  unsigned char value[self->size];
  size_t i;
  for (i = index1; i < index2; i++)
  {
    if (self->clone != NULL)
    {
      if (self->clone(value, self->ptr + i * self->size))
      {
        return 5;
      }
    }
    else
    {
      memcpy(value, self->ptr + i * self->size, self->size);
    }
    if (receive->push(receive, value))
    {
      return 6;
    }
  }
  return 0;
}
// verified
unsigned char vector_add(vector *self, void *added, size_t added_len)
{
  if (self == NULL || added == NULL)
  {
    return 1;
  }
  size_t i;
  for (i = 0; i < added_len; i++)
  {
    if (vector_push(self, added + i * self->size))
    {
      return 2;
    }
  }
  return 0;
}
// verified
unsigned char vector_prependElement(vector *self, void *value)
{
  if (self == NULL || value == NULL)
  {
    return 1;
  }
  void *new_ptr;
  if (self->initialized)
  {
    new_ptr = realloc(self->ptr, (self->len + 1) * self->size);
    if (new_ptr == NULL)
    {
      return 2;
    }
    else
    {
      self->ptr = new_ptr;
    }
    memmove(self->ptr + self->size, self->ptr, self->size * self->len);
    if (self->init != NULL)
    {
      if (self->init(self->ptr))
      {
        return 3;
      }
    }
    if (self->clone != NULL)
    {
      if (self->clone(self->ptr, value))
      {
        return 4;
      }
    }
    else
    {
      memcpy(self->ptr, value, self->size);
    }
    self->len++;
    return 0;
  }
  else
  {
    if (self->clone != NULL)
    {
      if (self->clone(self->ptr, value))
      {
        return 5;
      }
    }
    else
    {
      memcpy(self->ptr, value, self->size);
    }
    self->initialized = 1;
    self->len = 1;
    if (self->init != NULL)
    {
      if (self->init(self->ptr))
      {
        return 6;
      }
    }
    return 0;
  }
}
// verified
unsigned char vector_prepend(vector *self, void *added, size_t len)
{
  if (self == NULL || added == NULL)
  {
    return 1;
  }
  void *new_ptr;
  size_t i;
  if (self->initialized)
  {
    new_ptr = realloc(self->ptr, (self->len + len) * self->size);
    if (new_ptr == NULL)
    {
      return 2;
    }
    else
    {
      self->ptr = new_ptr;
    }
    memmove(self->ptr + len * self->size, self->ptr, self->len * self->size);
    if (self->init != NULL)
    {
      for (i = 0; i < len; i++)
      {
        if (self->init(self->ptr + i * self->size))
        {
          return 3;
        }
      }
    }
    if (self->clone != NULL)
    {
      for (i = 0; i < len; i++)
      {
        if (self->clone(self->ptr + i * self->size, added + i * self->size))
        {
          return 4;
        }
      }
    }
    else
    {
      memcpy(self->ptr, added, len * self->size);
    }
    self->len += len;
    return 0;
  }
  else
  {
    new_ptr = realloc(self->ptr, len * self->size);
    if (new_ptr == NULL)
    {
      return 5;
    }
    else
    {
      self->ptr = new_ptr;
    }
    if (self->clone != NULL)
    {
      for (i = 0; i < len; i++)
      {
        if (self->clone(self->ptr + i * self->size, added + i * self->size))
        {
          return 6;
        }
      }
    }
    else
    {
      memcpy(self->ptr, added, len * self->size);
    }
    if (self->init != NULL)
    {
      for (i = 0; i < len; i++)
      {
        if (self->init(self->ptr + i * self->size))
        {
          return 7;
        }
      }
    }
    self->len += len;
    return 0;
  }
}
// verified
unsigned char vector_insertElement(vector *self, void *value, size_t index)
{
  if (self == NULL || value == NULL)
  {
    return 1;
  }
  if (index >= self->len || !self->initialized)
  {
    return 2;
  }
  void *ptr;
  if (self->initialized)
  {
    ptr = realloc(self->ptr, (self->len + 1) * self->size);
    if (ptr == NULL)
    {
      return 3;
    }
    self->ptr = ptr;
    memmove(self->ptr + (index + 1) * self->size,
            self->ptr + index * self->size,
            self->size * (self->len - index - 1));
    if (self->clone != NULL)
    {
      self->clone(self->ptr + index * self->size, value);
    }
    else
    {
      memcpy(self->ptr + index * self->size, value, self->size);
    }
    if (self->init != NULL)
    {
      self->init(self->ptr + index * self->size);
    }
    self->len++;
  }
  else
  {
    if (self->clone != NULL)
    {
      if (self->clone(self->ptr, value))
      {
        return 4;
      }
    }
    else
    {
      memcpy(self->ptr, value, self->size);
    }
    self->initialized = 1;
    self->len = 1;
    if (self->init != NULL)
    {
      if (self->init(self->ptr))
      {
        return 5;
      }
    }
  }
  return 0;
}
// verified
unsigned char vector_insert(vector *self, void *value, size_t value_len, size_t index)
{
  if (self == NULL || value == NULL)
  {
    return 1;
  }
  if (index > self->len || !self->initialized)
  {
    return 2;
  }
  vector *backup = vector_new(self->size, self->init, self->clone, self->free,
                              self->equals, self->bigger, self->smaller);
  if (!backup)
  {
    return 3;
  }
  size_t i;
  unsigned char ok = 1;
  for (i = 0; i < index && ok; i++)
  {
    ok = !backup->push(backup, self->ptr + i * self->size);
  }
  if (!ok)
  {
    return 4;
  }
  ok = 1;
  for (i = 0; i < value_len && ok; i++)
  {
    ok = !backup->push(backup, value + i * self->size);
  }
  if (!ok)
  {
    return 5;
  }
  ok = 1;
  for (i = 0; i < self->len - index && ok; i++)
  {
    ok = !backup->push(backup, self->ptr + (index + i) * self->size);
  }
  if (!ok)
  {
    return 6;
  }
  if (vector_clone(self, backup))
  {
    return 7;
  }
  if (vector_free(backup))
  {
    return 8;
  }
  return 0;
}
// verified
unsigned char vector_replace(vector *self, void *pattern, size_t pattern_len,
                             void *substitute, size_t substitute_len)
{
  if (!self || !pattern || !substitute)
  {
    return 1;
  }
  if (self->equals)
  {
    return 2;
  }
  if (self->len < pattern_len)
  {
    return 3;
  }
  if (pattern_len == 0)
  {
    return 0;
  }
  size_t i, place;
  size_t j;
  unsigned char isPresent;
  for (i = 0; i < self->len; i++)
  {
    isPresent = 1;
    for (j = 0; j < pattern_len && isPresent && i + j < self->len; j++)
    {
      isPresent &= self->equals(pattern + j * self->size,
                                self->ptr + (i + j) * self->size);
    }
    if (isPresent)
    {
      place = i;
      break;
    }
  }
  if (isPresent)
  {
    unsigned char ok = 1;
    for (i = 0; i < pattern_len && ok; i++)
    {
      ok = !self->pop(self, place);
    }
    if (!ok)
    {
      return 4;
    }
    ok = !self->insert(self, substitute, substitute_len, place);
    if (!ok)
    {
      return 5;
    }
  }
  return 0;
}
// verified
unsigned char vector_replaceAll(vector *self, void *pattern, size_t pattern_len,
                                void *substitute, size_t substitute_len)
{
  if (!self || !pattern || !substitute)
  {
    return 1;
  }
  if (!self->equals)
  {
    return 2;
  }
  if (self->len < pattern_len)
  {
    return 3;
  }
  if (pattern_len == 0)
  {
    return 0;
  }
  vector *places =
      vector_new(sizeof(size_t), NULL, NULL, NULL, NULL, NULL, NULL);
  if (!places)
  {
    return 4;
  }
  size_t i;
  size_t j;
  {
    unsigned char isPresent, ok;
    for (i = 0; i < self->len; i++)
    {
      isPresent = 1;
      for (j = 0; j < pattern_len && isPresent && i + j < self->len; j++)
      {
        isPresent &= self->equals(pattern + j * self->size,
                                  self->ptr + (i + j) * self->size);
      }
      if (isPresent && j == pattern_len)
      {
        ok = !places->push(places, &i);
        if (!ok)
        {
          return 5;
        }
      }
    }
  }
  unsigned char okPlaces = 1, okPattern;
  for (i = 0; i < places->len && okPlaces; i++)
  {
    okPattern = 1;
    for (j = 0; j < pattern_len && okPattern; j++)
    {
      okPattern =
          !self->pop(self, ((size_t *)places->ptr)[places->len - i - 1]);
    }
    if (!okPattern)
    {
      return 6;
    }
    okPlaces = !self->insert(self, substitute, substitute_len,
                             ((size_t *)places->ptr)[places->len - i - 1]);
  }
  if (!okPlaces)
  {
    return 7;
  }
  if (vector_free(places))
  {
    return 8;
  }
  return 0;
}
// verified
unsigned char vector_indexOf(vector *self, void *pattern, size_t pattern_len,
                             size_t *index)
{
  if (!self || !pattern)
  {
    return 1;
  }
  if (!self->equals)
  {
    return 2;
  }
  if (self->len < pattern_len)
  {
    return 3;
  }
  if (pattern_len == 0)
  {
    return 0;
  }
  size_t i, j;
  unsigned char present;
  for (i = 0; i < self->len; i++)
  {
    present = 1;
    for (j = 0; j < pattern_len && present; j++)
    {
      present &= self->equals(self->ptr + (i + j) * self->size,
                              pattern + j * self->size);
    }
    if (present && j == pattern_len)
    {
      *index = i;
      return 0;
    }
  }
  return 0;
}
// verified
unsigned char vector_replaceNth(vector *self, void *pattern, size_t pattern_len,
                                void *substitute, size_t substitute_len,
                                size_t n)
{
  if (!self || !pattern || !substitute)
  {
    return 1;
  }
  if (!self->equals)
  {
    return 2;
  }
  if (self->len < pattern_len || n == 0)
  {
    return 3;
  }
  if (pattern_len == 0)
  {
    return 0;
  }
  size_t i, place = 0;
  size_t j, matches = 0;
  unsigned char isPresent;
beg:
  for (i = place; i < self->len; i++)
  {
    isPresent = 1;
    for (j = 0; j < pattern_len && isPresent && i + j < self->len; j++)
    {
      isPresent &= self->equals(pattern + j * self->size,
                                self->ptr + (i + j) * self->size);
    }
    if (isPresent && j == pattern_len)
    {
      matches++;
      place = i;
      break;
    }
  }

  if (isPresent)
  {
    if (matches == n)
    {
      unsigned char ok = 1;
      for (i = 0; i < pattern_len && ok; i++)
      {
        ok = !self->pop(self, place);
      }
      if (!ok)
      {
        return 4;
      }
      ok = !self->insert(self, substitute, substitute_len, place);
      if (!ok)
      {
        return 5;
      }
    }
    else
    {
      isPresent = 0;
      place++;
      goto beg;
    }
  }
  return 0;
}
unsigned char merge(vector *arr, size_t left, size_t mid, size_t right,
                    unsigned char ascending)
{
  size_t i, j, k;
  size_t n1 = mid - left + 1;
  size_t n2 = right - mid;
  unsigned char L[n1 * arr->size];
  unsigned char R[n2 * arr->size];
  for (i = 0; i < n1; i++)
  {
    if (arr->clone(L + i * arr->size, arr->ptr + (left + i) * arr->size))
    {
      return 0;
    }
  }
  for (j = 0; j < n2; j++)
  {
    if (arr->clone(R + j * arr->size, arr->ptr + (mid + 1 + j) * arr->size))
    {
      return 0;
    }
  }
  i = 0;
  j = 0;
  k = left;

  while (i < n1 && j < n2)
  {
    if (ascending ? arr->smaller(&L[i * arr->size], &R[j * arr->size]) ||
                        arr->equals(&L[i * arr->size], &R[j * arr->size])
                  : arr->bigger(&L[i * arr->size], &R[j * arr->size]))
    {
      if (arr->clone(arr->ptr + k * arr->size, L + i * arr->size))
      {
        return 0;
      }
      i++;
    }
    else
    {
      if (arr->clone(arr->ptr + k * arr->size, R + j * arr->size))
      {
        return 0;
      }
      j++;
    }
    k++;
  }
  while (i < n1)
  {
    if (arr->clone(arr->ptr + k * arr->size, L + i * arr->size))
    {
      return 0;
    }
    i++;
    k++;
  }
  while (j < n2)
  {
    if (arr->clone(arr->ptr + k * arr->size, R + j * arr->size))
    {
      return 0;
    }
    j++;
    k++;
  }
}
unsigned char mergeSort(vector *arr, size_t left, size_t right,
                        unsigned char ascending)
{
  if (left < right)
  {
    int mid = left + (right - left) / 2;
    mergeSort(arr, left, mid, ascending);
    mergeSort(arr, mid + 1, right, ascending);
    if (!merge(arr, left, mid, right, ascending))
    {
      return 0;
    }
  }
  return 1;
}
unsigned char vector_sort(vector *self, unsigned char ascending)
{
  if (!self)
  {
    return 1;
  }
  if (!self->initialized)
  {
    return 2;
  }
  if (self->len - 1 <= 0)
  {
    return 3;
  }
  if (ascending ? !self->smaller || !self->equals : !self->bigger)
  {
    return 4;
  }
  if (!self->clone)
  {
    return 5;
  }
  return !mergeSort(self, 0, self->len - 1, ascending);
}
// verified
unsigned char vector_contains(vector *self, void *pattern, size_t pattern_len, size_t *isPresent, size_t *where, size_t from)
{
  if (!self)
  {
    return 1;
  }
  if (!self->initialized || !self->equals)
  {
    return 2;
  }
  size_t i, j;
  for (i = from; i < self->len; i++, *where = i)
  {
    *isPresent = 1;
    for (j = 0; j < pattern_len && *isPresent && i + j < self->len; j++)
    {
      (*isPresent) &= self->equals(pattern + j * self->size,
                                   self->ptr + (i + j) * self->size);
    }
  }
  return 0;
}
// verified
unsigned char vector_bricks(vector *self, void *pattern, size_t pattern_len, void ***result, size_t *xLen, size_t **yLens, size_t sizePtr)
{
  if (!self)
  {
    return 1;
  }
  if (!self->initialized || !self->equals)
  {
    return 2;
  }
  unsigned char buffer[pattern_len / self->size];
  size_t i;
  for (i = 0; i < sizeof(buffer); i++)
  {
    buffer[i] = 1;
  }
  vector *aux = vector_new(sizeof(unsigned char), NULL, NULL, NULL, NULL, NULL, NULL);
  if (!aux)
  {
    return 3;
  }
  size_t j;
  unsigned char zero = 0, error;
  for (i = 0; i < self->len; i++)
  {
    error = aux->push(aux, &aux);
    if (error)
    {
      return 4;
    }
  }
  if (self->equals)
  {
    unsigned char res = 1;
    size_t where, from = 0;
    for (i = 0; i < self->len; i++, from += where + sizeof(buffer))
    {
      error = self->contains(self, pattern, pattern_len, &res, &where, from);
      if (error)
      {
        return 5;
      }
      error = res ? aux->insert(aux, buffer, sizeof(buffer), &where) : 0;
    }
    vector *tree = vector_new(sizeof(vector), vector_assignFunctions, vector_clone, vector_free, NULL, NULL, NULL);
    if (!tree)
    {
      return 6;
    }
    else
    {
      vector *type = vector_new(self->size, self->init, self->clone, self->free, self->equals, self->bigger, self->smaller);
      if (!type)
      {
        return 7;
      }
      unsigned char sub[] = {};
      for (i = 0; i < aux->len; i += res ? i + 1 : i, error = (res) ? aux->replace(aux, buffer, sizeof(buffer), sub, sizeof(sub)) : 0)
      {
        error = aux->contains(aux, buffer, sizeof(buffer), &res, &where);
        if (error)
        {
          return 8;
        }
      }
      for (j = 0; j < i; j++)
      {
        error = tree->push(tree, type);
        if (error)
        {
          return 9;
        }
      }
    }
  }
  else
  {
    return 10;
  }
  return 0;
}
void vector_assignFunctions(vector *self)
{
  self->push = vector_push;
  self->pop = vector_pop;
  self->subvector = vector_subvector;
  self->add = vector_add;
  self->prependElement = vector_prependElement;
  self->insertElement = vector_insertElement;
  self->insert = vector_insert;
  self->replace = vector_replace;
  self->replaceAll = vector_replaceAll;
  self->indexOf = vector_indexOf;
  self->replaceNth = vector_replaceNth;
  self->sort = vector_sort;
  self->contains = vector_contains;
  self->bricks = vector_bricks;
  self->set = vector_set;
}
#endif