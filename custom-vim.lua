function makeprg_release()
  vim.o.makeprg = './build.sh'
  vim.cmd("make")
end
