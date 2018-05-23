set expandtab
set tabstop=4
set softtabstop=4
set shiftwidth=4
set number

set colorcolumn=110
highlight ColorColumn ctermbg=darkgray
augroup project
    autocmd!
    autocmd BufRead,BufNewFile *.h,*.c set filetype=c.doxygen
augroup END
let &path.="../../../include,../../../osif/cortexm4/SPL_inc"