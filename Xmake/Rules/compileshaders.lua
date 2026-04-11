rule('compile.shaders')
    add_deps('glslang')
    after_build(function (target)
        local binaryPath = '$(buildir)/$(plat)/$(arch)/$(mode)'
        os.mkdir(binaryPath .. '/Shaders')
        print('Copying shaders to build directory...')
        os.cp('Shaders/**',  binaryPath .. '/Shaders')
        print('Copying shaders to build directory... Done !')
    end)