function makeprg_release()
  vim.o.makeprg = 'ninja -C ../build-pulchritude-engine/ install'
  vim.cmd("make")
end
