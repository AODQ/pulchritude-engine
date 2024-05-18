function makeprg_release()
  vim.o.makeprg = 'puledit run'
  vim.cmd("make")
end
