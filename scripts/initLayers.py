#!/usr/bin/python -B

import json

def main():
  out = open( 'src/regal/RegalInitLayers.cpp', 'w' )
  f = open( 'src/regal/layers.json', 'r' )

  j = json.load(f)

  out.write( '''

/*
  file generated by scripts/initLayers.py, edit at your peril
*/
#include "pch.h" /* For MS precompiled header support */

#include "RegalUtil.h"

#if ! REGAL_SYS_WGL
#include <dlfcn.h>
#else
#error "Implement me!"
#endif

REGAL_GLOBAL_BEGIN

#include <string>
using namespace std;

#include "RegalLog.h"
#include "RegalContext.h"
#include "RegalDispatch.h"

extern "C" {

''' )

  for e in j:
    c = e['constructor']
    if c and len(c) == 0:
      print e
      print "must have a constructor member, exiting"
      exit(1)

    if e['module'] == "builtin":
      out.write( "Regal::Layer * %s(Regal::RegalContext *);\n" % c )

  out.write( '''
}

REGAL_GLOBAL_END

REGAL_NAMESPACE_BEGIN

typedef Layer *(*Constructor)( RegalContext * ctx );

void * mod = NULL;
Constructor GetConstructor( const char * constructorName ) {
  if( mod == NULL ) {
     mod = dlopen( NULL, RTLD_LAZY );
  }
  Constructor c = reinterpret_cast<Constructor>( dlsym( mod, constructorName ) );
  return c;
}

void InitLayers( RegalContext * ctx ) {
  Constructor constr = NULL;
  Layer * layer = NULL;
''' )

  for e in j:
    c = e['constructor']
    if c and len(c) == 0:
      print e
      print "must have a constructor member, exiting"
      exit(1)
    instInf = ''
    if 'instanceInfo' in e: 
     instInf = e['instanceInfo'] 

    if e['module'] == "builtin":
      out.write( "  constr = %s;\n" % c )
    out.write( "  if( constr ) {\n" )
    out.write( "    layer = constr( ctx );\n" )
    out.write( "    bool success = layer->Initialize( \"%s\" );\n" % instInf )
    out.write( "    if( success ) {\n" )
    out.write( "      Info(\"%s initialization succeeded.\");\n" % c )
    out.write( "      ctx->layer.push_back( layer );\n" )
    out.write( "    } else {\n" )
    out.write( "      Info(\"%s initialization failed.\");\n" % c )
    out.write( "      delete layer;\n" )
    out.write( "    }\n" )
    out.write( "  }\n" )

  out.write( '''
}

REGAL_NAMESPACE_END

''' )

  f.close()
  out.close()

if __name__ == '__main__':
  main()

