function makeprg_release()
  vim.o.makeprg = './scripts/compile-engine-shaders.py --dir $PWD && ninja -C ../build-pulchritude-engine/ install'
  vim.cmd("make")
end
