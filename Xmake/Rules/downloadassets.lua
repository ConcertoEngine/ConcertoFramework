rule('download.assets')
    on_config(function (target)
        import('net.http')
        import('utils.archive')

        local binaryPath = '$(buildir)/$(plat)/$(arch)/$(mode)'
        os.mkdir(binaryPath .. '/assets')

        if (not os.exists('assets/sponza')) then
            print('Downloading assets...')
            http.download('https://casual-effects.com/g3d/data10/common/model/crytek_sponza/sponza.zip', 'assets/sponza.zip')
            print('Downloading assets... Done !')

            print('Extracting assets...')
                archive.extract('assets/sponza.zip', 'assets/sponza')
                os.rm('assets/sponza.zip')
            print('Extracting assets... Done !')
        else
            print('Assets already downloaded !')
        end

        print('Copying assets to build directory...')
        os.cp('assets/**',  binaryPath .. '/assets')
        print('Copying assets to build directory... Done !')
    end)
