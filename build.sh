#!/usr/bin/env sh
#if ! ./scripts/compile-engine-shaders.py --dir $PWD ; then
#  exit
#fi
#pushd util/binding-generator/
## TODO this always returns 0, so we need to check the output
#if ! puledit run --error-segfaults ; then 
#  popd
#  exit
#fi
#popd
ninja -C ../build/ install
