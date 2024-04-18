# TODO: This function could be better isolated better

function(_td_generate_clangd_config directory)
string(REPLACE "C:" "" _TD_DIR_EXTERNAL_CLANGD ${TD_DIR_EXTERNAL})
file(CONFIGURE OUTPUT ${directory}/.clangd CONTENT
    [[
CompileFlags:
    Add:      [ 
        -ferror-limit=200,
        -march=mips1,
        -isystem, ${_TD_DIR_EXTERNAL_CLANGD}/psn00bsdk/mipsel-none-elf/include/c++/12.3.0,
        -isystem, ${_TD_DIR_EXTERNAL_CLANGD}/psn00bsdk/mipsel-none-elf/include/c++/12.3.0/mipsel-none-elf/soft-float,
        -isystem, ${_TD_DIR_EXTERNAL_CLANGD}/psn00bsdk/lib/gcc/mipsel-none-elf/12.3.0/include,
        -isystem, ${_TD_DIR_EXTERNAL_CLANGD}/psn00bsdk/lib/gcc/mipsel-none-elf/12.3.0/include-fixed,
        -isystem, ${_TD_DIR_EXTERNAL_CLANGD}/psn00bsdk/lib/gcc/mipsel-none-elf/c++/12.3.0/include-fixed,
        -isystem, ${_TD_DIR_EXTERNAL_CLANGD}/psn00bsdk/mipsel-none-elf/include
        
    ]
    Remove:   [ -march, -mno-llsc, -mdivide-breaks ]

Diagnostics:
    Suppress: "asm_unknown_register_name"
]]
)
endfunction()