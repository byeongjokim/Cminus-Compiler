/****************************************************/
/* File: symtab.c                                   */
/* Symbol table implementation for the TINY compiler*/
/* (allows only one symbol table)                   */
/* Symbol table is implemented as a chained         */
/* hash table                                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "symtab.h"


#define SHIFT 4
char *typeString[] = {"void", "int", "int[]"};

static int hash ( char * key )
{ int temp = 0;
  int i = 0;
  while (key[i] != '\0')
  { temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}

static ScopeList totalScope[1000];
static int ntotalScope = 0;
static ScopeList scopeStack[1000];
static int nScopeStack = 0;
static int location[1000];

ScopeList scope_top(){
  return scopeStack[nScopeStack - 1];
}

ScopeList scope_create(char *name){
  ScopeList new;

  new = (ScopeList) malloc(sizeof(struct ScopeListRec));
  new->name = name;
  new-> parent = scope_top();

  totalScope[ntotalScope++] = new;
  return new;
}

void scope_pop(){
  nScopeStack = nScopeStack-1;
}

void scope_push(ScopeList scope){
  scopeStack[nScopeStack] = scope;
  location[nScopeStack++] = 0;
}

int addLocation(){
  return location[nScopeStack - 1]++;
}


BucketList st_lookup (char * name){
  int h = hash(name);
  ScopeList sc = scope_top();

  while(sc){
      BucketList l = sc->bucket[h];
      while(l!=NULL){
        if(strcmp(l->name, name) == 0) return l;
        l = l->next;
      }
    
    sc = sc->parent;
  }
  
  return NULL;
}

BucketList st_lookup_excluding_parent ( char * scope, char * name){
  int h = hash(name);
  ScopeList sc = scope_top();
  
  if(strcmp(sc->name, scope)){
    BucketList l = sc->bucket[h];
    while(l!=NULL){
      if(strcmp(l->name, name) == 0) return l;
      l = l->next;
    }  
  }

  return NULL;
}

void st_insert(char *scope, char * name, ExpType type, int lineno, int loc, TreeNode *t)
{ 
  int h = hash(name);
  ScopeList sc = scope_top();
  

  while(sc){
    if(strcmp(sc->name, scope) == 0){
      break;
    }

    sc = sc->parent;
  }
  BucketList l =  sc->bucket[h];

  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;

  if (l == NULL) /* variable not yet in table */
  { 

    l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->name = name;
    
    l->lines = (LineList) malloc(sizeof(struct LineListRec));
    l->lines->lineno = lineno;
    l->lines->next = NULL;
    
    l->type = type;
    l->memloc = loc;
    l->next = sc->bucket[h];
    l->treenode = t;
    sc->bucket[h] = l;
  }
  
  else /* found in table, so just add line number */
  {
    LineList t = l->lines;

    while (t->next != NULL) t = t->next;
    t->next = (LineList) malloc(sizeof(struct LineListRec));
    t->next->lineno = lineno;
    t->next->next = NULL;
  }
} /* st_insert */

void just_add_line(char * name, int lineno){

  BucketList l = st_lookup(name);
  
  LineList t = l->lines;

  while (t->next != NULL) t = t->next;
  t->next = (LineList) malloc(sizeof(struct LineListRec));
  t->next->lineno = lineno;
  t->next->next = NULL;
}

void printSymTab(FILE * listing){
  int i;
  int j;
  fprintf(listing,"Variable Name\tType\tLocation\tScope\t\tLine Numbers\n");
  fprintf(listing,"-------------\t-----\t---------\t------\t\t-------------\n");
  
  for(j = 0 ; j < ntotalScope; j++){
    ScopeList sc = totalScope[j];

    for (i=0;i<SIZE;++i)
    { if (sc->bucket[i] != NULL)
      {
        BucketList l = sc->bucket[i];
        while (l != NULL)
        { LineList t = l->lines;
          fprintf(listing,"%s\t\t",l->name);
          fprintf(listing,"%s\t\t", typeString[l->type]);
          fprintf(listing,"%d\t",l->memloc);
          fprintf(listing,"%s\t\t",sc->name);
          while (t != NULL)
          { fprintf(listing,"%d,",t->lineno);
            t = t->next;
          }
          fprintf(listing,"\n");
          l = l->next;
        }
      }
    }
  }
}
/* printSymTab */
